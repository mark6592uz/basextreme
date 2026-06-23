
#include <assert.h>

#define _BOOL
#include <maya/MDagPathArray.h>
#include <maya/MFloatArray.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MPointArray.h>
#include <maya/MVector.h>

#include <maya/MItDependencyNodes.h>
#include <maya/MItGeometry.h>
#include <maya/MItMeshPolygon.h>

#include <maya/MFnLambertShader.h>
#include <maya/MFnMesh.h>
#include <maya/MFnSkinCluster.h>

#include "mayamesh.h"
#include "mayamaterial.h"
#include "profiler.h"

#define LOG(...) { printf(__VA_ARGS__); fflush(stdout); }

/*
//bool GetWeights(MFnSkinCluster &rkSkinCluster, std::vector<MayaVertexWeights*> &rvpkWeights);
bool GetWeights(MFnSkinCluster &rkSkinCluster, std::vector<MayaVertexWeights*> &rvpkWeights)
{
	PROFILE(SceneExporter::GetWeights);

	MStatus kStatus;

	LOG("Extracting weights from skin cluster: %s\n", rkSkinCluster.name().asChar());

	// Get the list of influences for skin cluster
	MDagPathArray kInfluences;
	unsigned int uiInfluences = rkSkinCluster.influenceObjects(kInfluences, &kStatus);
	if(kStatus != MStatus::kSuccess) {
		LOG("ERROR: Unable to get influences for skin cluster: %s\n", rkSkinCluster.name().asChar());
		return false;
	}
	if (uiInfluences == 0) {
		LOG("ERROR: No influences for skin cluster: %s\n", rkSkinCluster.name().asChar());
		return false;
	}

	// Export influences for all geometries affected by skin cluster
	unsigned int uiGeometries = rkSkinCluster.numOutputConnections();
	unsigned int i;
	for (i = 0; i < uiGeometries; i++) {
		// Get geometry index
		unsigned int uiGeometryIndex = rkSkinCluster.indexForOutputConnection(i, &kStatus);
		if(kStatus != MStatus::kSuccess) {
			LOG("ERROR: Unable to get geometry index\n");
			return false;
		}

		// Get path to geometry
		MDagPath kSkinPath;
		kStatus = rkSkinCluster.getPathAtIndex(uiGeometryIndex, kSkinPath);
		if(kStatus != MStatus::kSuccess) {
			LOG("ERROR: Unable to get geometry path\n");
			return false;
		}

		MItGeometry kVertexIter(kSkinPath);

		// Iterate through vertices
		for (; !kVertexIter.isDone(); kVertexIter.next() ) {
			MObject kVertex = kVertexIter.component(&kStatus);
			if(kStatus != MStatus::kSuccess) {
				LOG("ERROR: Unable to get vertex for geometry: %s\n", kSkinPath.partialPathName().asChar());
				return false;
			} 

			// Get the weights for this vertex
			MFloatArray kafWeights;
			unsigned int uiVertexInfluences;
			kStatus = rkSkinCluster.getWeights(kSkinPath, kVertex, kafWeights, uiVertexInfluences);
			if(kStatus != MStatus::kSuccess) {
				LOG("WARNING: Unable to get weights for vertex: %d. Skipping!\n", kVertexIter.index());
				return false;
			} 
			if (uiVertexInfluences == 0) {
				LOG("ERROR: No weights for vertex: %d\n", kVertexIter.index());
				return false;
			}

			MPoint kPoint = kVertexIter.position(MSpace::kWorld);

			// Output the weight data for this vertex
			unsigned int i;
			MayaVertexWeights *pkWeights = new MayaVertexWeights();
			std::string strInfluencer;
			MayaJoint *pkJoint;
			for (i = 0; i < uiVertexInfluences; i++) {
				// TODO: option
				if (kafWeights[i] < 0.01f)
					continue;
				MayaVertexWeight kWeight;
				kWeight.m_uiBone = i;
				kWeight.m_fWeight = kafWeights[i];

				// Find the bone, so we can calculate vertex position in bone space
				unsigned int uiBone;
				strInfluencer = kInfluences[i].partialPathName().asChar();
				pkJoint = NULL;
				for (uiBone = m_vpkJoints.size(); uiBone--;) {
					if (m_vpkJoints[uiBone]->m_strName == strInfluencer) {
						pkJoint = m_vpkJoints[uiBone];
						break;
					}
				}

				if (pkJoint) {
					Vector3x kPos(kPoint.x, kPoint.y, kPoint.z);
					kPos -= pkJoint->m_kWorldTranslation;
					Quaternionx kInvWorldRot = pkJoint->m_kWorldRotation;
					kInvWorldRot.Inverse();
					kPos = kInvWorldRot * kPos;
					kWeight.x = kPos.x;
					kWeight.y = kPos.y;
					kWeight.z = kPos.z;
				} else {
					LOG("ERROR: can't find joint for influencer: %s\n", strInfluencer.c_str());
					continue;
				}

				pkWeights->m_vkWeights.push_back(kWeight);
			}
			rvpkWeights.push_back(pkWeights);
		}
	}

	return true;
}
*/

// Get skin cluster for mesh
//MObject				GetSkinCluster(MDagPath& rkDagPath);
MObject GetSkinCluster(MDagPath& rkDagPath)
{
	PROFILE(SceneExporter::GetSkinCluster);

	MStatus kStatus;

	LOG("... Looking for skin cluster...\n");

	MItDependencyNodes kNodeIter(MFn::kSkinClusterFilter);
	for(; !kNodeIter.isDone(); kNodeIter.next()) {
		MObject kObject = kNodeIter.item();

		// Get skin cluster
		MFnSkinCluster kSkinCluster(kObject, &kStatus);
		if(kStatus != MStatus::kSuccess) {
			LOG("ERROR: Unable to get skin cluster object\n");
			continue;
		} 
		LOG("Skin cluster: %s\n", kSkinCluster.name().asChar());

		unsigned int uiConnections = kSkinCluster.numOutputConnections();
		unsigned int i;
		for (i = 0; i < uiConnections; i++) {
			unsigned int uiGeometryIndex = kSkinCluster.indexForOutputConnection(i, &kStatus);
			if(kStatus != MStatus::kSuccess) {
				LOG("ERROR: Unable to get geometry index\n");
				continue;
			}

			// Get path to geometry
			MDagPath kSkinPath;
			kStatus = kSkinCluster.getPathAtIndex(uiGeometryIndex, kSkinPath);
			if(kStatus != MStatus::kSuccess) {
				LOG("ERROR: Unable to get geometry path\n");
				continue;
			}

			LOG("1: %s\n", rkDagPath.fullPathName().asChar());
			LOG("2: %s\n", kSkinPath.fullPathName().asChar());
			if (rkDagPath == kSkinPath) {
				LOG("Found skin\n");
				return kObject;
			}
		}
	}

	return MObject::kNullObj;
}


void ExtractTriangle(MayaMesh* pcPolygonMesh, MFnMesh& rcMeshFn, MItMeshPolygon& ritPolygon, int iTriangleId)
{
	MPointArray kPoints;
	MIntArray kVerticeIdList;
	MayaTriangle kTriangle;
	MPoint kPoint;
	MVector kNormal;
	int iVertexId;

        pcPolygonMesh->mName = rcMeshFn.name();

	ritPolygon.getTriangle(iTriangleId, kPoints, kVerticeIdList, MSpace::kObject);

	if (kPoints.length() != 3) {
		LOG("WARNING: Triangle has %d points instead of 3. Skipping!\n", kPoints.length());
		return;
	}
	if (kVerticeIdList.length() != 3) {
		LOG("WARNING: Triangle has %d vertices instead of 3. Skipping!\n", kVerticeIdList.length());
		return;
	}

	unsigned int uiPoint;
	for (uiPoint = 0; uiPoint < 3; uiPoint++) {
		MayaVertex kVertex;

		iVertexId = kVerticeIdList[uiPoint];
		rcMeshFn.getPoint(iVertexId, kPoint, MSpace::kObject);

		// Get vertex pos
		kVertex.x = float(kPoint.x);
		kVertex.y = float(kPoint.y);
		kVertex.z = float(kPoint.z);

		kVertex.m_uiMayaVertexIndex = iVertexId;
//					LOG("vl: %d\n", kVerticeList[uiPoint]);
		// Get the local (face) vertex index for our current point
		// we need it to get UVs
		unsigned int uiLocalVertexId;
		for (uiLocalVertexId = 0; ritPolygon.polygonVertexCount(); uiLocalVertexId++) {
			if (ritPolygon.vertexIndex(uiLocalVertexId) == iVertexId) {
				break;
			}
		}

		// Get normal
		ritPolygon.getNormal(uiLocalVertexId, kNormal, MSpace::kObject);
		kVertex.nx = float(kNormal.x);
		kVertex.ny = float(kNormal.y);
		kVertex.nz = float(kNormal.z);

		// Get UVs
		int iUVIndex = -1;
		ritPolygon.getUVIndex(uiLocalVertexId, iUVIndex, kVertex.u, kVertex.v, NULL);
                kVertex.v = 1.0f - kVertex.v;

                // TODO: check this out if we need it
		// Transform UVs based on place 2d texture settings
//		kVertex.u *= pcPolygonMesh->m_pcMaterial->m_fRepeatU;
//		kVertex.v *= pcPolygonMesh->m_pcMaterial->m_fRepeatV;

		// Check if we have same vertex already added to the vertex list
		// We go backwards, since same vertices should be more often on the end of list
		int iVertexId = (int)pcPolygonMesh->m_acVertices.size();
		for(; iVertexId--;) {
			if(pcPolygonMesh->m_acVertices[iVertexId] == kVertex) {
//				++uiNumVerticesSaved;
				break;
			}
		}
		if(iVertexId == -1) {
			pcPolygonMesh->m_acVertices.push_back(kVertex); 
			iVertexId = pcPolygonMesh->m_acVertices.size() - 1;
		}

		kTriangle.v[uiPoint] = (unsigned int)iVertexId;
	}

	pcPolygonMesh->m_acTriangles.push_back(kTriangle);
}


MString GetShaderName(MObject& rcShader)
{
	if (!rcShader.hasFn(MFn::kShadingEngine)) {
		LOG("WARNING!: Not a MFn::kShaderEngine in shader: %s\n", rcShader.apiTypeStr());
	}

	MFnDependencyNode node(rcShader);
	MPlug plug = node.findPlug("surfaceShader");
	MPlugArray input;

	plug.connectedTo(input, true, false);
	if (input.length() != 1) {
		LOG("WARNING!: surfaceShader is not connected for: %s\n", rcShader.apiTypeStr());
		return "-invalid-";
	}

	MObject material = input[0].node();
	if (material.hasFn(MFn::kLambert)) {
		MFnLambertShader lambert(material);
		return lambert.name();
	} else {
		LOG("WARNING!: Unsupported shader: %s\n", material.apiTypeStr());
	}

	return "";
}


MayaMaterial* GetMaterial(std::vector<MayaMaterial*>* papcMayaMaterial, MString& rsName)
{
	unsigned int i;
	unsigned int n = papcMayaMaterial->size();

	for (i = 0; i < n; i++) {
		if ((*papcMayaMaterial)[i]->m_strName == rsName) {
			return (*papcMayaMaterial)[i];
		}
	}

	return NULL;
}


void ExtractTriangles(MayaModel* pcModel, MFnMesh& rkMeshFn, MObjectArray& racShaders, MIntArray& raiMapPolygonToShader, std::vector<MayaMaterial*>* papcMayaMaterial)
{
	PROFILE(ExtractTriangles);

	MayaMesh* pcMeshWithoutShader = NULL;
	MayaMesh** apcMeshes = new MayaMesh*[racShaders.length()];

	// Get or create meshes for each connected shader
	// TODO: check for meshes that already might exist in model
	{
		unsigned int i;
		for (i = 0; i < racShaders.length(); i++) {
			apcMeshes[i] = new MayaMesh();
//			apcMeshes[i]->m_strMaterial = GetShaderName(racShaders[i]);
			apcMeshes[i]->m_pcMaterial = GetMaterial(papcMayaMaterial, GetShaderName(racShaders[i]));
                        if (apcMeshes[i]->m_pcMaterial == 0) {
                                LOG(("Couldn't found shader: %s, for mesh: %s\n", GetShaderName(racShaders[i]).asChar(), apcMeshes[i]->mName.asChar()));
		        	assert(apcMeshes[i]->m_pcMaterial); // Materials are parsed before, and should be there!
                        }
		}
	}
	


	// Get polygon iterator for triangle extraction
	MDagPath path;
	MStatus status;
	rkMeshFn.getPath(path);
	MItMeshPolygon kPolygonIter(path, MObject::kNullObj, &status);
	if (status != MStatus::kSuccess) {
		LOG("ERROR: Unable to get polygon iterator\n");
		goto CLEANUP;
	}

	unsigned int uiNumTriangles = 0;
//	unsigned int uiNumVerticesSaved = 0;
	for (; !kPolygonIter.isDone(); kPolygonIter.next()) {
		if (!kPolygonIter.hasValidTriangulation()) {
			LOG("WARNING: polygon %d doesn't have valid triangulation! Skipped.\n", kPolygonIter.index());
			continue;
		}

		int iTriangles;
		kPolygonIter.numTriangles(iTriangles);
		uiNumTriangles += iTriangles;

		unsigned int uiPolygonId = kPolygonIter.index();
		int iShaderId = raiMapPolygonToShader[uiPolygonId];
		MayaMesh* pcPolygonMesh = NULL;

		// Select mesh by material for current polygon
		if (iShaderId == -1) {
			if (pcMeshWithoutShader == NULL) {
				pcMeshWithoutShader = new MayaMesh();
			}
			pcPolygonMesh = pcMeshWithoutShader;
		} else {
			pcPolygonMesh = apcMeshes[iShaderId];
		}

		int iTriangleId;
		for (iTriangleId = 0; iTriangleId < iTriangles; iTriangleId++) {
			ExtractTriangle(pcPolygonMesh, rkMeshFn, kPolygonIter, iTriangleId);
		}
	}

        // Add meshes that have triangles
	unsigned int i;
	for (i = 0; i < racShaders.length(); i++) {
                if(apcMeshes[i]->m_acTriangles.size() > 0) {
		        pcModel->m_apcMeshes.push_back(apcMeshes[i]);
                } else {
                        delete apcMeshes[i];
                }
	}

//	LOG("\tTriangles: %d, Optimized vertices: %d\n", uiNumTriangles, uiNumVerticesSaved);
	LOG("\tTriangles: %d\n", uiNumTriangles);

CLEANUP:
	delete[] apcMeshes;
}


bool ExtractMayaMesh(std::vector<MayaMaterial*>* papcMayaMaterial, MayaModel* pcModel, MDagPath& rkDagPath)
{
	PROFILE(ExtractMayaMesh);

	MStatus kStatus;
	bool bHasUVs = true;
	bool bHasSkin = false;
	std::vector<MayaVertex> vkVertices;
	std::vector<MayaTriangle> vkTriangles;
	std::vector<MayaVertexWeights*> vpkWeights;

	std::vector<MVector*> vpkNormals;

	// Get mesh from path
	MFnMesh kMeshFn(rkDagPath, &kStatus);

	// If it's intermediate object it's not a normal mesh. It's usually
	// used by deformers and similar stuff for their own calculations.
	if (kMeshFn.isIntermediateObject()) {
		return true;
	}

	// Finally we have a mesh to export...
	LOG("Exporting mesh: %s\n", kMeshFn.partialPathName().asChar());

	rkDagPath.extendToShape();
	unsigned int uiInstanceId = 0;
	if (rkDagPath.isInstanced()) {
		LOG("... Mesh is instanced\n");
		uiInstanceId = rkDagPath.instanceNumber();
	}

	// Check if we have a skin, then the mesh is skinned, otherwise it's
	// a static
	MObject kObject = GetSkinCluster(rkDagPath);
	MFnSkinCluster kSkinCluster(kObject, &kStatus);
	if(kStatus == MStatus::kSuccess) {
		LOG("... Found skin for mesh.\n");
		LOG("TODO SKELETONS ARE NOT EXPORTED.\n");
//		ExportSkeletons(kSkinCluster);
//		bHasSkin = true;
//		GetWeights(kSkinCluster, vpkWeights);
		
	} else {
		LOG("... Skin not found. Exporting as static.\n");
	}

	//if (!fMesh->getConnectedSetsAndMembers(instanceNum, fPolygonSets, fPolygonComponents, true)) {
	//	MGlobal::displayError("MFnMesh::getConnectedSetsAndMembers"); 
	//	return MStatus::kFailure;
	//}

	//MStringArray uvSetNames;
	//if (MStatus::kFailure == fMesh->getUVSetNames(uvSetNames)) {
	//	MGlobal::displayError("MFnMesh::getUVSetNames"); 
	//	return MStatus::kFailure;
	//}

	//unsigned int uvSetCount = uvSetNames.length();
	//unsigned int i;

	//UVSet* currUVSet = NULL;

	//for (i = 0; i < uvSetCount; i++ ) {
	//	if (0 == i) {
	//		currUVSet = new UVSet;
	//		fHeadUVSet = currUVSet;
	//	} else {
	//		currUVSet->next = new UVSet;
	//		currUVSet = currUVSet->next;
	//	}

	//	currUVSet->name = uvSetNames[i];
	//	currUVSet->next = NULL;

	//	// Retrieve the UV values
	//	//
	//	if (MStatus::kFailure == fMesh->getUVs(currUVSet->uArray, currUVSet->vArray, &currUVSet->name)) {
	//		return MStatus::kFailure;
	//	}
	//}

//	// Get normals
//	MFloatVectorArray akNormals;
//	kMeshFn.getNormals(akNormals, MSpace::kWorld);

	// Get UVs
	MFloatArray akUArray, akVArray;
	kMeshFn.getUVs(akUArray, akVArray, NULL); 


	// Extract mesh shaders and the polygon->shader map
	MObjectArray kShaders;
	MIntArray kMapPolygonToShader;
	unsigned int uiShader;
	kMeshFn.getConnectedShaders(uiInstanceId, kShaders, kMapPolygonToShader);


	ExtractMaterials(papcMayaMaterial, kShaders);
	ExtractTriangles(pcModel, kMeshFn, kShaders, kMapPolygonToShader, papcMayaMaterial);

	return true;
}
