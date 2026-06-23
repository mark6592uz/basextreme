
#include "engine/asset.h"
#include "engine/bsp.h"
#include "ccor/Core.h"
#include "shared/engine.h"
#include "profiler.h"
#include "exporter.h"

#include <maya/MFnDagNode.h>
#include <maya/MMatrix.h>

#include "mayamaterial.h"
#include "mayalight.h"
#include "mayamesh.h"
#include "mayanode.h"


#define LOG(...) { printf(__VA_ARGS__); fflush(stdout); }


using namespace engine;


engine::IEngine* gEngine;


struct TriangleMark {
        int mMesh;
        int mTriangle;
        int mV[3];
};


class ExporterAsset: public Asset
{
	std::vector<MayaMaterial*>      mMaterials;

        MayaNode*               mRootNode;

        AssetObjectM            mAssetObjects;

        void                    TraverseMayaScene();
        bool                    TraverseMayaSceneRecurse(MayaNode* parentNode, MDagPath& parentPath);

        void                    ExportScene();
        void                    ExportSceneRecursively(MayaNode* parentNode, Clump* clump, Frame* parentFrame);
        bool                    ExportModelAsBSP(Clump* clump, Frame* frame, MayaModel* model);
        bool                    ExportModelAsBSPRecursively(BSPSector* sector, Clump* clump, Frame* frame, MayaModel* model, std::vector<TriangleMark>* triangles);
        bool                    ExportModel(Clump* clump, Frame* frame, MayaModel* model);
        void                    ExportMesh(Geometry* geometry, MayaMesh* mesh, int materialId, int* vertexOffset, int* triangleOffset);
//        Geometry*               ExportMesh(MayaMesh* mesh);
        bool                    ExportLight(Clump* clump, Frame* frame, MayaLight* mayaLight);
        Shader*                 ExportMayaMaterial(MayaMaterial* material);

public:
        virtual void __stdcall release(void) { delete this; };
        virtual void __stdcall serialize(void) { };

        ExporterAsset();
};


Matrix4f MayaToBaMatrix(MMatrix matrix) {
        Matrix4f m;

        int x, y;
        for (y = 0; y < 4; ++y) {
                for (x = 0; x < 4; ++x) {
                        m[x][y] = (float)matrix.matrix[x][y];
                }
        }

        return m;
}


void ExporterAsset::ExportScene()
{
        int i;
        int count = mRootNode->mChilds.size();
        for (i = 0; i < count; ++i) {
                MayaNode* node = mRootNode->mChilds[i];

                if (node->mModel) {
                        if (node->mModel->m_strName == "Terrain") {
                                assert(_bsps.size() == 0);
                                ExportModelAsBSP(0, 0, node->mModel);
                                continue;
                        }
                }


                Clump* clump = new Clump(node->mName.asChar());
                Frame* frame = new Frame(node->mName.asChar());

                Matrix4f transform(MayaToBaMatrix(node->mTransform));
                frame->setMatrix(transform);

                AssetObjectT assetObjectT((auid)frame, frame);
                mAssetObjects.insert(assetObjectT);

                clump->setFrame(frame);
                
                _clumps.push_back(clump);

                ExportSceneRecursively(node, clump, frame);
        }

        // Add dummy bsp
        if (_bsps.size() == 0) {
                AABB boundingBox( wrap( Vector3f(-1000000.0f, -1000000.0f, -1000000.0f) ), wrap( Vector3f(1000000.0f, 1000000.0f, 1000000.0f) ) );

                BSP* bsp = new BSP( "blabsp", boundingBox, 0 );
                _bsps.push_back(bsp);
                BSPSector* rootSector = new BSPSector( bsp, NULL, boundingBox, NULL );
        }
}

void ExporterAsset::ExportSceneRecursively(MayaNode* parentNode, Clump* clump, Frame* parentFrame) {
        int i;
        int count = parentNode->mChilds.size();
        for (i = 0; i < count; ++i) {
                MayaNode* node = parentNode->mChilds[i];

                Frame* frame = new Frame(node->mName.asChar());

                Matrix4f transform(MayaToBaMatrix(node->mTransform));
                frame->setMatrix(transform);

                AssetObjectT assetObjectT((auid)frame, frame);
                mAssetObjects.insert(assetObjectT);

                frame->setParent(parentFrame);

                ExportSceneRecursively(node, clump, frame);
        }

        if (parentNode->mModel) {
                ExportModel(clump, parentFrame, parentNode->mModel);
        }

        if (parentNode->mLight) {
                ExportLight(clump, parentFrame, parentNode->mLight);
        }
}


bool ExporterAsset::ExportModelAsBSP(Clump* clump, Frame* frame, MayaModel* model)
{
        LOG("Exporting model: %s as BSP\n", model->m_strName.asChar());

        AABB boundingBox(wrap( Vector3f(100000000.0f, 100000000.0f, 100000000.0f) ), wrap( Vector3f(-100000000.0f, -100000000.0f, -100000000.0f) ) );
        int numShaders = model->m_apcMeshes.size();
        std::vector<TriangleMark> triangles;

        LOG("  Gather triangles\n");
        int m;
        for (m = 0; m < model->m_apcMeshes.size(); ++m) {
                int count = model->m_apcMeshes[m]->m_acTriangles.size();
                int t;
                for (t = 0; t < count; ++t) {
                        TriangleMark triangle;
                        triangle.mMesh = m;
                        triangle.mTriangle = t;
                        triangles.push_back(triangle);
                }

                std::vector<MayaVertex>& vertices = model->m_apcMeshes[m]->m_acVertices;
                int v;
                count = vertices.size();
                for (v = 0; v < count; ++v) {
                        boundingBox.addPoint(Vector(vertices[v].x, vertices[v].y, vertices[v].z));
                }
        }

        BSP* bsp = new BSP("blabsp", boundingBox, numShaders);
        _bsps.push_back(bsp);
        BSPSector* rootSector = new BSPSector(bsp, NULL, boundingBox, NULL);
        BSPSector::subdivide(rootSector);


        LOG("  Start recursive sector export\n");
        ExportModelAsBSPRecursively(bsp->getRoot(), clump, frame, model, &triangles);

        // Setup shaders
        int i;
        for (i = 0; i < numShaders; ++i) {
                Shader* shader = ExportMayaMaterial(model->m_apcMeshes[i]->m_pcMaterial);
                bsp->setShader(i, shader);
        }

        return true;
}


bool ExporterAsset::ExportModelAsBSPRecursively(BSPSector* sector, Clump* clump, Frame* frame, MayaModel* model, std::vector<TriangleMark>* triangles)
{
        if (!sector->isLeaf()) {
                ExportModelAsBSPRecursively(sector->getLeftSector(), clump, frame, model, triangles);
                ExportModelAsBSPRecursively(sector->getRightSector(), clump, frame, model, triangles);
        }

        std::vector<TriangleMark> geometryTriangles;

        int i;
        for (i = 0; i < triangles->size();) {
                const TriangleMark& mark = (*triangles)[i];
                const MayaMesh* mesh = model->m_apcMeshes[mark.mMesh];
                const MayaTriangle& triangle = mesh->m_acTriangles[mark.mTriangle];
                const MayaVertex& v0 = mesh->m_acVertices[triangle.v[0]];
                const MayaVertex& v1 = mesh->m_acVertices[triangle.v[1]];
                const MayaVertex& v2 = mesh->m_acVertices[triangle.v[2]];

                bool isInside = 
                        sector->getBoundingBox()->isInside(Vector(v0.x, v0.y, v0.z)) ||
                        sector->getBoundingBox()->isInside(Vector(v1.x, v1.y, v1.z)) ||
                        sector->getBoundingBox()->isInside(Vector(v2.x, v2.y, v2.z));

                if (isInside) {
                        geometryTriangles.push_back(mark);

                        triangles->erase(triangles->begin() + i);
                } else {
                        ++i;
                }
        }

        if (geometryTriangles.size() == 0) {
                return true;
        }

        LOG("  Sector: %x, tris: %d\n", sector, geometryTriangles.size());
        assert(sector->isLeaf());

        std::vector<MayaVertex> geometryVertices;
        int shaders = model->m_apcMeshes.size();

        for (i = 0; i < geometryTriangles.size(); ++i) {
                TriangleMark& mark = geometryTriangles[i];
                const MayaMesh* mesh = model->m_apcMeshes[mark.mMesh];
                const MayaTriangle& triangle = mesh->m_acTriangles[mark.mTriangle];

                int vi;
                for (vi = 0; vi < 3; ++vi) {
                        const MayaVertex& v = mesh->m_acVertices[triangle.v[vi]];

                        int id;
                        for (id = 0; id < geometryVertices.size(); ++id) {
                                if (v == geometryVertices[id]) {
                                        break;
                                } 
                        }
                        if (id == geometryVertices.size()) {
                                geometryVertices.push_back(v);
                        }

                        mark.mV[vi] = id;
                }
        }

        Geometry* geometry = new Geometry(geometryVertices.size(), geometryTriangles.size(), 1, model->m_apcMeshes.size(), 0, true, model->m_strName.asChar());

        // Fill vertex data
        Vector* vertices = geometry->getVertices();
        Vector* normals = geometry->getNormals();
        Flector* uvs = geometry->getUVSet(0);
        for (i = 0; i < geometryVertices.size(); ++i) {
                vertices[i].x = geometryVertices[i].x;
                vertices[i].y = geometryVertices[i].y;
                vertices[i].z = geometryVertices[i].z;

                normals[i].x = geometryVertices[i].nx;
                normals[i].y = geometryVertices[i].ny;
                normals[i].z = geometryVertices[i].nz;

                uvs[i].x = geometryVertices[i].u;
                uvs[i].y = geometryVertices[i].v;
        }

        // Fill triangle data
        Triangle* gtriangles = geometry->getTriangles();
        for (i = 0; i < geometryTriangles.size(); i++) {
                const TriangleMark& triangle = geometryTriangles[i];
                gtriangles[i].set(
                        triangle.mV[0], 
                        triangle.mV[1], 
                        triangle.mV[2], 
                        triangle.mMesh
                );
        }

        // Setup shaders
        //for (i = 0; i < shaders; ++i) {
        //        Shader* shader = ExportMayaMaterial(model->m_apcMeshes[i]->m_pcMaterial);
        //        geometry->setShader(i, shader);
        //}
        geometry->setShaders(sector->bsp()->getShaders());

        sector->setGeometry(geometry);

        return true;
}


bool ExporterAsset::ExportModel(Clump* clump, Frame* frame, MayaModel* model)
{
        int vertexCount = 0;
        int triangleCount = 0;
        int uvCount = 1;
        int shaderCount = model->m_apcMeshes.size();

        int i;
        int meshCount = model->m_apcMeshes.size();
        for (i = 0; i < meshCount; ++i) {
                MayaMesh* mesh = model->m_apcMeshes[i];
                vertexCount += mesh->m_acVertices.size();
                triangleCount += mesh->m_acTriangles.size();
        }

        Geometry* geometry = new Geometry(vertexCount, triangleCount, uvCount, shaderCount, 0, false, model->m_strName.asChar());

        int vertexOffset = 0;
        int triangleOffset = 0;

        for (i = 0; i < meshCount; ++i) {
                MayaMesh* mesh = model->m_apcMeshes[i];

                ExportMesh(geometry, mesh, i, &vertexOffset, &triangleOffset);
        }

        Atomic* atomic = new Atomic();
        atomic->setFrame(frame);
        atomic->setGeometry(geometry);
        clump->add(atomic);

        return true;
}


void ExporterAsset::ExportMesh(Geometry* geometry, MayaMesh* mesh, int materialId, int* vertexOffset, int* triangleOffset)
{
        int vertexCount = mesh->m_acVertices.size();
        int triangleCount = mesh->m_acTriangles.size();

        // Fill vertex data
        int i;
        Vector* vertices = geometry->getVertices() + *vertexOffset;
        Vector* normals = geometry->getNormals() + *vertexOffset;
        Flector* uvs = geometry->getUVSet(0) + *vertexOffset;
        for (i = 0; i < vertexCount; ++i) {
                vertices[i].x = mesh->m_acVertices[i].x;
                vertices[i].y = mesh->m_acVertices[i].y;
                vertices[i].z = mesh->m_acVertices[i].z;

                normals[i].x = mesh->m_acVertices[i].nx;
                normals[i].y = mesh->m_acVertices[i].ny;
                normals[i].z = mesh->m_acVertices[i].nz;

                uvs[i].x = mesh->m_acVertices[i].u;
                uvs[i].y = mesh->m_acVertices[i].v;
        }

        // Fill triangle data
        Triangle* triangles = geometry->getTriangles() + *triangleOffset;
        for (i = 0; i < triangleCount; i++) {
                const MayaTriangle& triangle = mesh->m_acTriangles[i];
                triangles[i].set(
                        triangle.v[0] + *vertexOffset, 
                        triangle.v[1] + *vertexOffset, 
                        triangle.v[2] + *vertexOffset, 
                        materialId
                );
        }

        *vertexOffset = *vertexOffset + vertexCount;
        *triangleOffset = *triangleOffset + triangleCount;

        // Setup shaders
        Shader* shader = ExportMayaMaterial(mesh->m_pcMaterial);
        geometry->setShader(materialId, shader);
}


bool ExporterAsset::ExportLight(Clump* clump, Frame* frame, MayaLight* mayaLight)
{
        engine::LightType lightType;
        switch (mayaLight->mType) {
                case MayaLightType::Ambient:    lightType = engine::ltAmbient; break;
                case MayaLightType::Directional: lightType = engine::ltDirectional; break;
                case MayaLightType::Point:      lightType = engine::ltPoint; break;
                case MayaLightType::Spot:       lightType = engine::ltSpot; break;
        }

        Light* light = new Light(lightType);

        Vector3x diffuseLight(mayaLight->mRed, mayaLight->mGreen, mayaLight->mBlue);
        diffuseLight *= mayaLight->mIntensity;
        light->setDiffuseColor(Vector4f(diffuseLight.x, diffuseLight.y, diffuseLight.z, 1.0f));
        light->setAttenuation(Vector3f(mayaLight->mAttenuation[0], mayaLight->mAttenuation[1], mayaLight->mAttenuation[2]));
        light->setRange(10000000.0f);

        clump->add(light);
        light->setFrame(frame);

        return true;
}


void ExporterAsset::TraverseMayaScene()
{
	PROFILE(ExporterAsset::TraverseMayaScene);

	MStatus kStatus;

	printf("---------------------------------------------\n");
	printf("Traversing scene.\n");

        MItDag kDagIter(MItDag::kBreadthFirst, MFn::kInvalid, &kStatus);

	if (kStatus != MStatus::kSuccess) {
		printf("Error: Unable to get DAG iterator\n");
		return;
	}

        mRootNode = new MayaNode();

	// Iterate through all nodes
	for(; !kDagIter.isDone(); kDagIter.next()) {
		MDagPath path;
		if (kDagIter.getPath(path) != MStatus::kSuccess) {
			printf("Error: Unable to get path to mesh node\n");
			return;
		}

                MFnDagNode dagNode(path);

                if (dagNode.isIntermediateObject() || dagNode.isDefaultNode()) {
                        continue;
                }

                if (kDagIter.depth() != 1) {
                        continue;
                }

                bool exp = false;
                if (path.hasFn(MFn::kMesh)) exp = true;
                if (path.hasFn(MFn::kLight)) exp = true;

                TraverseMayaSceneRecurse(mRootNode, path);

                fflush(stdout);
	}
}


bool ExporterAsset::TraverseMayaSceneRecurse(MayaNode* parentNode, MDagPath& path) {
        printf("Checking: %s\n", path.fullPathName().asChar());

        bool hasSomething = false;

        if (path.hasFn(MFn::kTransform)) {
                MayaNode* node = new MayaNode();
                MFnTransform transform(path);
                node->mName = path.partialPathName();
                node->mTransform = transform.transformationMatrix();

                int i;
                for (i = 0; i < path.childCount(); ++i) {
                        MObject child = path.child(i);
                        path.push(child);
                        if (TraverseMayaSceneRecurse(node, path)) {
                                hasSomething = true;
                        }
                        path.pop();
                }

                if (hasSomething) {
                        printf("Node: %s\n", path.partialPathName().asChar());
                        parentNode->mChilds.push_back(node);
                } else {
                        delete node;
                }
        } else if (path.hasFn(MFn::kMesh)) {
                printf("Mesh: %s\n", path.partialPathName().asChar());
                hasSomething = true;

                MayaModel* model = new MayaModel();
                if (ExtractMayaMesh(&mMaterials, model, path)) {
                        model->m_strName = parentNode->mName;
                        parentNode->mModel = model;
                } else {
                        delete model;
                }
        } else if (path.hasFn(MFn::kLight)) {
                printf("Light: %s\n", path.partialPathName().asChar());
                hasSomething = true;

                MayaLight* light = new MayaLight();

                if (ExtractMayaLight(light, path)) {
		        light->mName = path.partialPathName().asChar();
                        parentNode->mLight = light;
                } else {
                        delete light;
                }
        }

        return hasSomething;
}




Shader* ExporterAsset::ExportMayaMaterial(MayaMaterial* material)
{
        Shader* shader = new Shader(material->mTextures.size(), material->m_strName.asChar());

        int i;
        int textureCount = material->mTextures.size();
        for (i = 0; i < textureCount; ++i) {
                ITexture* texture = gEngine->getTexture(Texture::getTextureNameFromFilePath(material->mTextures[i]->mName.asChar()).c_str());
                if (texture == 0) {
                        texture = gEngine->createTexture(material->mTextures[i]->mName.asChar(), false);
                        texture->setMagFilter(ftAnisotropic);
                        texture->setMinFilter(ftAnisotropic);
                }
                shader->setLayerTexture(i, texture);
        }

        return shader;
}


ExporterAsset::ExporterAsset()
{
        TraverseMayaScene();
        ExportScene();

        fflush(stdout);
}



void ExportBa(const char* fileName)
{
        printf("Starting export -------------------\n");

        int rcode = CoreInitEngine(0, 0, "", 0);
        if (rcode == 0) {
                if(askInterfaceT("Engine", &gEngine)) {
                        IAsset* asset = gEngine->createAsset(atBinary, fileName);

                        ExporterAsset exporterAsset;

                        asset->seize(&exporterAsset);

                        asset->serialize();
                        asset->release();
                } else {
                        printf("Couldn't get engine interface.\n");
                }
        }
        rcode = CoreShutdownEngine(rcode);
        gEngine = 0;

        printf("Export finished -------------------\n");
        fflush(stdout);
}

void main() {
        printf("Starting export -------------------");
        ExportBa("test.ba");
        printf("Export finished -------------------");
        fflush(stdout);
}
