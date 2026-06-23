
#include <assert.h>
#include <vector>

#include "exporter.h"
#include "mayamesh.h"
#include "mayamaterial.h"
#include "profiler.h"
#include "timer.h"
#include "exportba.h"


// BUG: root bone has not -1 id!!!
// BUG: skeleton and maybe other global stuff is not freed after exporting

// TODO: bones
// TODO: skeletal anim

// DONE: uv's
// POSSIBLE BUG:
// if vertex get's split because of UVs, then skin has to take this into account too
// ..might be that this is not the case..

// BUG:
// let's say we have two meshes. spahe1 and shape2. shape1 is attached to skin.
// shape1 and shape2 is combined into shape3. shape3 doesn't have a skin cluster
// since it's being driven by shape1 and shape2.

// POSSIBLE BUG:
// two vertices on same possition assigned to two different bones could be joined
// by current code! vertices should be compared on influences too?
//
// BUG:
// weights for each combined mesh




//Exporting models
//Exporting mesh: pPlaneShape1
//... Looking for skin cluster...
//PROFILE SceneExporter::GetSkinCluster 0.000000
//... Skin not found. Exporting as static.
//PROFILE Get all vertices 259.750000
//PROFILE Export model meshes 270.390625
//PROFILE SceneExporter::ExportMesh 530.148438

//Exporting mesh: pPlaneShape2
//... Looking for skin cluster...
//PROFILE SceneExporter::GetSkinCluster 0.000000
//... Skin not found. Exporting as static.
//PROFILE Get all vertices 0.000000
//PROFILE Export model meshes 0.000000
//PROFILE SceneExporter::ExportMesh 0.000000
//PROFILE SceneExporter::ExportModels 530.156250

//Exporting models
//Exporting mesh: pPlaneShape1
//... Looking for skin cluster...
//PROFILE SceneExporter::GetSkinCluster 0.000000
//... Skin not found. Exporting as static.
//PROFILE Get all vertices 1413.781250
//PROFILE Export model meshes 182.109375
//PROFILE SceneExporter::ExportMesh 1595.914063
//Exporting mesh: pPlaneShape2
//... Looking for skin cluster...
//PROFILE SceneExporter::GetSkinCluster 0.000000
//... Skin not found. Exporting as static.
//PROFILE Get all vertices 0.000000
//PROFILE Export model meshes 0.000000
//PROFILE SceneExporter::ExportMesh 0.000000
//PROFILE SceneExporter::ExportModels 1595.914063


//Exporting models
//Exporting mesh: pPlaneShape1
//... Looking for skin cluster...
//PROFILE SceneExporter::GetSkinCluster 0.000000
//... Skin not found. Exporting as static.

//PROFILE Get all vertices 4.079102
//PROFILE Export model meshes 0.538086
//PROFILE SceneExporter::ExportMesh 4.618164
//PROFILE SceneExporter::ExportModels 4.618164




Timer g_cTimer;

// Main export function
MStatus SceneExporter::writer(const MFileObject &rkFile, const MString &rkOptions, FileAccessMode eAccessMode)
{
	PROFILE(SceneExporter::writer);

	g_cTimer.Reset();

	m_bHasSkeleton = false;

//	m_hFile = fopen(rkFile.fullName().asChar(), "wb");

/*
	fprintf(m_hFile, "%s", "HMSF!txt\n");

	ExportModels(rkFile.resolvedPath());
	if (m_bHasSkeleton)
		ExportAnimation();

	fclose(m_hFile);

	fflush(NULL);
*/

        ExportBa(rkFile.fullName().asChar());

	return MS::kSuccess;
}


bool SceneExporter::ExportModels(MString& rsPath)
{
	PROFILE(SceneExporter::ExportModels);

	MStatus kStatus;

	printf("---------------------------------------------\n");
	printf("Exporting models\n");

	// Get iterator for meshes
	MItDag kDagIter(MItDag::kDepthFirst, MFn::kMesh, &kStatus);

	if (kStatus != MStatus::kSuccess) {
		printf("Error: Unable to get DAG iterator\n");
		return false;
	}

	std::vector<MayaMaterial*> apcMaterials;
	std::vector<MayaModel*> apcModels;

	// Iterate through all meshes and extract their materials and meshes
	for(; !kDagIter.isDone(); kDagIter.next()) {
		MDagPath kDagPath;
		if (kDagIter.getPath(kDagPath) != MStatus::kSuccess) {
			printf("Error: Unable to get path to mesh node\n");
			return false;
		}

//		ExportMesh(kDagPath);
		MayaModel* pcModel = new MayaModel();
		pcModel->m_strName = kDagPath.partialPathName();

		ExtractMayaMesh(&apcMaterials, pcModel, kDagPath);
		apcModels.push_back(pcModel);
	}

	unsigned int i;
	unsigned int n;
	
	// Export materials
	n = apcMaterials.size();
	for (i = 0; i < n; i++) {
//		ExportMayaMaterialAsHMS(m_hFile, rsPath, *apcMaterials[i]);
	}

	// Export meshes
	n = apcModels.size();
	for (i = 0; i < n; i++) {
//		ExportMayaMeshAsHMS(m_hFile, *apcModels[i]);
	}

	return true;
}


/*
bool SceneExporter::ExportMesh(MDagPath& rkDagPath)
{
	PROFILE(SceneExporter::ExportMesh);

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
	printf("Exporting mesh: %s\n", kMeshFn.partialPathName().asChar());

	rkDagPath.extendToShape();
	unsigned int uiInstanceId = 0;
	if (rkDagPath.isInstanced()) {
		printf("... Mesh is instanced\n");
		uiInstanceId = rkDagPath.instanceNumber();
	}

	// Check if we have a skin, then the mesh is skinned, otherwise it's
	// a static
	MObject kObject = GetSkinCluster(rkDagPath);
	MFnSkinCluster kSkinCluster(kObject, &kStatus);
	if(kStatus == MStatus::kSuccess) {
		printf("... Found skin for mesh.\n");
		ExportSkeletons(kSkinCluster);
		bHasSkin = true;
		GetWeights(kSkinCluster, vpkWeights);
		
	} else {
		printf("... Skin not found. Exporting as static.\n");
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

	ExtractTriangles(kMeshFn, &vkTriangles, &vkVertices);


	MObjectArray kShaders;
	MIntArray kMapToShader;
	unsigned int uiShader;
	kMeshFn.getConnectedShaders(uiInstanceId, kShaders, kMapToShader);

	ExportShaders(kShaders);

	// Start exporting model
	if (bHasSkin) {
		fprintf(m_hFile,
			"skinnedmodel = {\n"
			"\tname = %s\n"
			"\tskeleton = unnamed\n",
			kMeshFn.name().asChar());
	} else {
		fprintf(m_hFile,
			"model = {\n"
			"\tname = %s\n",
			kMeshFn.partialPathName().asChar());
	}

	// Export model meshes
	{
		PROFILE(Export model meshes);
		//
		// We have one maya mesh, but it can have more than one shader. So we
		// loop through all shaders and export triangles affected by them as
		// separate meshes, who in turn make up a whole model.
		for (uiShader = 0; uiShader < kShaders.length(); uiShader++) {
			// First we need to build vertex and triangle buffers
			std::vector<MayaVertex> vkMeshVertices;
			std::vector<MayaTriangle> vkMeshTriangles;

			unsigned int uiTriangle;
			// Get all triangles that are affected by current shader
			for (uiTriangle = 0; uiTriangle < vkTriangles.size(); uiTriangle++) {
				if (kMapToShader[vkTriangles[uiTriangle].m_uiMayaPolygonIndex] == uiShader) {
					vkMeshTriangles.push_back(vkTriangles[uiTriangle]);
					unsigned int uiTriangleId = (unsigned int)vkMeshTriangles.size() - 1;

					// Find a vertex, or add if not found
					// And patch triangles indices
					unsigned int uiVertex;
					for (uiVertex = 0; uiVertex < 3; uiVertex++) {
						MayaVertex &kVertex = vkVertices[vkMeshTriangles[uiTriangleId].v[uiVertex]];
						unsigned int uiVertexId;
						for(uiVertexId = 0; uiVertexId < vkMeshVertices.size(); uiVertexId++)
							if(vkMeshVertices[uiVertexId] == kVertex)
								break;
						if(uiVertexId == vkMeshVertices.size())
							vkMeshVertices.push_back(kVertex); 
						vkMeshTriangles[uiTriangleId].v[uiVertex] = uiVertexId;
					}
				}
			}

			if ((vkMeshVertices.size() == 0) || (vkMeshTriangles.size() == 0))
				continue;

			if (bHasSkin) {
				fprintf(m_hFile, "\tskinnedmesh = {\n");
			} else {
				fprintf(m_hFile, "\tmesh = {\n");
			}
			
			// Export vertices
			unsigned int uiMeshWeights = 0;
			unsigned int uiSkinnedVertices = 0;
			unsigned int i;
			fprintf(m_hFile, 
				"\t\tmaterial = hugo\n"
				"\t\tvertexbuffer = {\n"
				"\t\t\tvertices = %d\n"
				"\t\t\tformat = {\n"
				"\t\t\t\tposition = float3\n"
				"\t\t\t\tnormal = float3\n",
				vkMeshVertices.size());
			if (bHasUVs)
				fprintf(m_hFile, "\t\t\t\ttexcoord = float2\n");
			fprintf(m_hFile, 
				"\t\t\t}\n"
				"\t\t\tdata = [\n"
				);
			for (i = 0; i < vkMeshVertices.size(); i++) {
				MayaVertex &kVertex = vkMeshVertices[i];
				if (bHasSkin) {
					assert(kVertex.m_uiMayaVertexIndex < vpkWeights.size());
					if (vpkWeights[kVertex.m_uiMayaVertexIndex]->m_vkWeights.size()) {
						vpkWeights[kVertex.m_uiMayaVertexIndex]->m_uiOffset = uiMeshWeights;
						uiMeshWeights += vpkWeights[kVertex.m_uiMayaVertexIndex]->m_vkWeights.size();
						uiSkinnedVertices++;
					}
				}

				fprintf(m_hFile, "\t\t\t\t%f %f %f", kVertex.x, kVertex.y, kVertex.z);
				fprintf(m_hFile, " %f %f %f", kVertex.nx, kVertex.ny, kVertex.nz);
				if (bHasUVs)
					fprintf(m_hFile, " %f %f", kVertex.u, kVertex.v);
				fprintf(m_hFile, "\n");
			}
			fprintf(m_hFile,
				"\t\t\t]\n"
				"\t\t}\n");

			// Export polygons
			fprintf(m_hFile, 
				"\t\tpolygonbuffer = {\n"
				"\t\t\ttype = triangles\n"
				"\t\t\tpolygons = %d\n"
				"\t\t\tdata = [\n",
				vkMeshTriangles.size());
			for (i = 0; i < vkMeshTriangles.size(); i++) {
				MayaTriangle &kTriangle = vkMeshTriangles[i];
				fprintf(m_hFile, "\t\t\t\t%d %d %d\n", kTriangle.v[0], kTriangle.v[1], kTriangle.v[2]);
			}
			fprintf(m_hFile,
				"\t\t\t]\n"
				"\t\t}\n");

			// Export skin
			if (bHasSkin) {
				fprintf(m_hFile, 
					"\t\tskin = {\n"
					"\t\t\tvertices = %d\n"
					"\t\t\tweights = %d\n", uiSkinnedVertices, uiMeshWeights);

				unsigned int v;
				fprintf(m_hFile, "\t\t\tvertexdata = [\n");
				for (v = 0; v < vkMeshVertices.size(); v++) {
					MayaVertex &kVertex = vkMeshVertices[v];
					if (vpkWeights[kVertex.m_uiMayaVertexIndex]->m_vkWeights.size()) {
						fprintf(m_hFile, "\t\t\t\t%d %d %d\n",
							v, // vertex index
							vpkWeights[kVertex.m_uiMayaVertexIndex]->m_vkWeights.size(), // num weights
							vpkWeights[kVertex.m_uiMayaVertexIndex]->m_uiOffset); // index
					}
				}
				fprintf(m_hFile, "\t\t\t]\n");

				fprintf(m_hFile, "\t\t\tweightdata = [\n");
				for (v = 0; v < vkMeshVertices.size(); v++) {
					MayaVertex &kVertex = vkMeshVertices[v];
					if (vpkWeights[kVertex.m_uiMayaVertexIndex]->m_vkWeights.size()) {
						unsigned int w;
						for (w = 0; w < vpkWeights[kVertex.m_uiMayaVertexIndex]->m_vkWeights.size(); w++) {
							fprintf(m_hFile, "\t\t\t\t%d %f %f %f %f\n", 
								vpkWeights[kVertex.m_uiMayaVertexIndex]->m_vkWeights[w].m_uiBone,
								vpkWeights[kVertex.m_uiMayaVertexIndex]->m_vkWeights[w].m_fWeight,
								vpkWeights[kVertex.m_uiMayaVertexIndex]->m_vkWeights[w].x,
								vpkWeights[kVertex.m_uiMayaVertexIndex]->m_vkWeights[w].y,
								vpkWeights[kVertex.m_uiMayaVertexIndex]->m_vkWeights[w].z);
						}
					}
				}
				fprintf(m_hFile, "\t\t\t]\n\t\t}\n");
			}
			fprintf(m_hFile, "\t}\n");
			
		}
	}
	fprintf(m_hFile, "}\n");

	return true;
}
*/


bool SceneExporter::ExportSkeletons(MFnSkinCluster& rkSkinCluster)
{
	PROFILE(SceneExporter::ExportSkeletons);

	MStatus kStatus;

	// Get influence objects
	MDagPathArray kPaths;
	rkSkinCluster.influenceObjects(kPaths, &kStatus);

	if (kStatus != MStatus::kSuccess) {
		printf("WARNING: can't get influence objects for skin cluster: %s\n", rkSkinCluster.name().asChar());
		return false;
	}

	m_bHasSkeleton = true;

	// Extract all joints
	unsigned int i;
	unsigned int uiNumJoints = 0;
	for (i = 0; i < kPaths.length(); i++) {
		if (!kPaths[i].hasFn(MFn::kJoint)) {
			printf("... path %s is not a joint... skipping\n", kPaths[i].partialPathName().asChar());
			continue;
		}

		MFnIkJoint kJointFn(kPaths[i]);
		
		// We got joint
		MayaJoint *pkJoint = new MayaJoint();
		m_vpkJoints.push_back(pkJoint);
		pkJoint->m_uiID = uiNumJoints++;
		pkJoint->m_strName = kJointFn.partialPathName().asChar();
		pkJoint->m_kDagPath = kPaths[i];

		printf("...... joint %s\n", kJointFn.partialPathName().asChar());

		// Find parent for our joint
		MObject kParentObj = kJointFn.parent(0);
		if (kParentObj.hasFn(MFn::kJoint)) {
			MFnIkJoint kParentJointFn(kParentObj);
			pkJoint->m_bHasParent = true;
			pkJoint->m_strParentName = kParentJointFn.partialPathName().asChar();
	
			printf("......... parent %s\n", kParentJointFn.partialPathName().asChar());
		} else {
			pkJoint->m_bHasParent = false;

			//Should be the root transform node
			if(kParentObj.hasFn(MFn::kTransform)) {
				MFnDagNode kDagNode( kParentObj );
				printf("......... root transform %s\n", kDagNode.partialPathName().asChar());
			} else {
				MFnDependencyNode kDepNode( kParentObj );
				printf("WARNING: parent is not root transform: %s (%s)\n", kDepNode.name().asChar(), kDepNode.typeName().asChar());
			} 
		}

		// Get bind pose
		MPlug kBindPosePlug = kJointFn.findPlug("bindPose");
		MObject kBindPoseObject;
		kStatus = kBindPosePlug.getValue(kBindPoseObject);
		if(kStatus != MStatus::kSuccess) {
			printf("ERROR: Can't get bind pose plug object");
			return 0;
		}  

		MFnMatrixData kMatrixFn(kBindPoseObject); 
		MMatrix kMatrix = kMatrixFn.matrix();

		pkJoint->m_kTranslation.Set(
			kMatrix[3][0],
			kMatrix[3][1],
			kMatrix[3][2]
		);
		MQuaternion kRotation;
		kRotation = kMatrix;
//		pkJoint->m_kRotation.Set(
//			kTransform.rotation().x,
//			kTransform.rotation().y,
//			kTransform.rotation().z,
//			kTransform.rotation().w
//		);
		pkJoint->m_kRotation.Set(
			kRotation.x,
			kRotation.y,
			kRotation.z,
			kRotation.w
		);
		pkJoint->m_kWorldTranslation = pkJoint->m_kTranslation;
		pkJoint->m_kWorldRotation = pkJoint->m_kRotation;
	}

	// Resolve parents, and translate to local space
	for (i = 0; i < m_vpkJoints.size(); i++) {
		// Find parent by name
		if (m_vpkJoints[i]->m_bHasParent) {
			unsigned int k;
			bool bFound = false;
			for (k = m_vpkJoints.size(); k--;) {
				if (m_vpkJoints[i]->m_strParentName == m_vpkJoints[k]->m_strName) {
					bFound = true;
					m_vpkJoints[i]->m_uiParentID = k;
					break;
				}
			}

			if (!bFound) {
				m_vpkJoints[i]->m_uiParentID = 0xFFFFFFFF;
				printf("ERROR: can't find parent for joint: %s\n", m_vpkJoints[i]->m_strName.c_str());
				continue;
			}
		} else {
			m_vpkJoints[i]->m_uiParentID = 0xFFFFFFFF;
		}
	}

	fprintf(m_hFile, "skeleton = {\n\tname = unnamed\n");
	for (i = 0; i < m_vpkJoints.size(); i++) {
		fprintf(m_hFile, "\tbone = {\n");
		fprintf(m_hFile, "\t\tname = %s\n\t\tid = %d\n\t\tparent = %d\n",
			m_vpkJoints[i]->m_strName.c_str(),
			m_vpkJoints[i]->m_uiID,
			m_vpkJoints[i]->m_uiParentID == 0xFFFFFFFF ? -1 : m_vpkJoints[i]->m_uiParentID
		);

		Vector3x kLocalTranslation;
		Quaternionx kLocalRotation;
		if (m_vpkJoints[i]->m_bHasParent) {
			Vector3x kTranslation = m_vpkJoints[i]->m_kTranslation;
			Quaternionx kRotation = m_vpkJoints[i]->m_kRotation;
			Vector3x kParentTranslation = m_vpkJoints[m_vpkJoints[i]->m_uiParentID]->m_kTranslation;
			Quaternionx kParentRotation = m_vpkJoints[m_vpkJoints[i]->m_uiParentID]->m_kRotation;
			Quaternionx kInvParentRotation(-kParentRotation.x, -kParentRotation.y, -kParentRotation.z, kParentRotation.w);
			kLocalTranslation = kInvParentRotation * (kTranslation - kParentTranslation);
			kLocalRotation = kInvParentRotation * kRotation;
		} else {
			Vector3x kLocalTranslation = m_vpkJoints[i]->m_kTranslation;
			Quaternionx kLocalRotation = m_vpkJoints[i]->m_kRotation;
		}
		fprintf(m_hFile, "\t\tpos = %f %f %f\n\t\trot = %f %f %f %f\n",
			kLocalTranslation.x,
			kLocalTranslation.y,
			kLocalTranslation.z,
			kLocalRotation.x,
			kLocalRotation.y,
			kLocalRotation.z,
			kLocalRotation.w
		);
		fprintf(m_hFile, "\t}\n");
	}
	fprintf(m_hFile, "}\n");
}


bool SceneExporter::ExportSkins()
{
	PROFILE(SceneExporter::ExportSkins);

	MStatus kStatus;

	printf("Exporting skin clusters\n");

	MItDependencyNodes kNodeIter(MFn::kSkinClusterFilter);
	for(; !kNodeIter.isDone(); kNodeIter.next()) {
		MObject kObject = kNodeIter.item();

		// Get skin cluster
		MFnSkinCluster kSkinCluster(kObject, &kStatus);
		if(kStatus != MStatus::kSuccess) {
			printf("ERROR: Unable to get skin cluster object\n");
			return false;
		} 
		printf("Skin cluster: %s\n", kSkinCluster.name().asChar());

		// Get the list of influences for skin cluster
		MDagPathArray kInfluences;
		unsigned int uiInfluences = kSkinCluster.influenceObjects(kInfluences, &kStatus);
		if(kStatus != MStatus::kSuccess) {
			printf("ERROR: Unable to get influences for skin cluster: %s\n", kSkinCluster.name().asChar());
			return false;
		}
		if (uiInfluences == 0) {
			printf("ERROR: No influences for skin cluster: %s\n", kSkinCluster.name().asChar());
			return false;
		}
		
		// Export influences for all geometries affected by skin cluster
		unsigned int uiGeometries = kSkinCluster.numOutputConnections();
		unsigned int i;
		for (i = 0; i < uiGeometries; i++) {
			// Get geometry index
			unsigned int uiGeometryIndex = kSkinCluster.indexForOutputConnection(i, &kStatus);
			if(kStatus != MStatus::kSuccess) {
				printf("ERROR: Unable to get geometry index\n");
				return false;
			}

			// Get path to geometry
			MDagPath kSkinPath;
			kStatus = kSkinCluster.getPathAtIndex(uiGeometryIndex, kSkinPath);
			if(kStatus != MStatus::kSuccess) {
				printf("ERROR: Unable to get geometry path\n");
				return false;
			}

			MItGeometry kVertexIter(kSkinPath);
			fprintf(m_hFile, "skin = {\n\tmesh = %s\n",
				kSkinPath.partialPathName().asChar(),
				kVertexIter.count()
				);
			// uiInfluences

//			// print out the influence objects
//			//
//			for (unsigned int kk = 0; kk < nInfs; ++kk) {
//				fprintf(file,"%s ",infs[kk].partialPathName().asChar());
//			}
//			fprintf(file,"\n");

			float		*pafWeights;
			unsigned int	*pauiBones;
			unsigned int	*pauiVertices;
			unsigned int	uiSavedWeights;
			unsigned int	uiSavedVertices;

			// weights
			pafWeights = new float[kVertexIter.count() * uiInfluences];
			// bone id for weigh
			pauiBones = new unsigned int[kVertexIter.count() * uiInfluences];
			// vertex id, weights, index into weight buffer
			pauiVertices = new unsigned int[kVertexIter.count() * 3];

			uiSavedWeights = 0;
			uiSavedVertices = 0;

			// Iterate through vertices
			for (; !kVertexIter.isDone(); kVertexIter.next() ) {
				MObject kVertex = kVertexIter.component(&kStatus);
				if(kStatus != MStatus::kSuccess) {
					printf("ERROR: Unable to get vertex for geometry: %s\n", kSkinPath.partialPathName().asChar());
					delete [] pafWeights;
					delete [] pauiBones;
					delete [] pauiVertices;
					return false;
				} 

				// Get the weights for this vertex
				MFloatArray kafWeights;
				unsigned int uiVertexInfluences;
				kStatus = kSkinCluster.getWeights(kSkinPath, kVertex, kafWeights, uiVertexInfluences);
				if(kStatus != MStatus::kSuccess) {
					printf("WARNING: Unable to get weights for vertex: %d. Skipping!\n", kVertexIter.index());
					continue;
//					delete [] pafWeights;
//					delete [] pauiBones;
//					delete [] pauiVertices;
//					return false;
				} 
				if (uiVertexInfluences == 0) {
					printf("ERROR: No weights for vertex: %d\n", kVertexIter.index());
					delete [] pafWeights;
					delete [] pauiBones;
					delete [] pauiVertices;
					return false;
				}

				// Output the weight data for this vertex
				unsigned int uiWeight;
				unsigned int uiVertexWeights;
				unsigned int uiWeightsIndex;
				uiWeightsIndex = uiSavedWeights;
				uiVertexWeights = 0;
				for (uiWeight = 0; uiWeight < uiVertexInfluences; uiWeight++ ) {
					// TODO: option
					if (kafWeights[uiWeight] < 0.01)
						continue;
					pauiBones[uiSavedWeights] = uiWeight;
					pafWeights[uiSavedWeights] = kafWeights[uiWeight];
					uiVertexWeights++;
					uiSavedWeights++;
				}
				// Save vertex id, number of weigths and index into weight array
				if (uiVertexWeights > 0) {
					pauiVertices[uiSavedVertices * 3 + 0] = kVertexIter.index();
					pauiVertices[uiSavedVertices * 3 + 1] = uiVertexWeights;
					pauiVertices[uiSavedVertices * 3 + 2] = uiWeightsIndex;
					uiSavedVertices++;
				}
			}
			fprintf(m_hFile, "\tvertices = %d\n\tweights = %d\n", uiSavedVertices, uiSavedWeights);

			unsigned int v;
			fprintf(m_hFile, "\tvertex_data = [\n");
			for (v = 0; v < uiSavedVertices; v++) {
				fprintf(m_hFile, "\t\t%d %d %d\n",
					pauiVertices[v*3+0],
					pauiVertices[v*3+1],
					pauiVertices[v*3+2]);
			}
			fprintf(m_hFile, "\t]\n");

			unsigned int w;
			fprintf(m_hFile, "\tweight_data = [\n");
			for (w = 0; w < uiSavedWeights; w++) {
				fprintf(m_hFile, "\t\t%d %f\n", pauiBones[w], pafWeights[w]);
			}
			// skin = {
			//   weights = [
			fprintf(m_hFile, "\t]\n}\n");

			delete [] pafWeights;
			delete [] pauiBones;
			delete [] pauiVertices;
		}
	}
	return true;
}


bool SceneExporter::ExportAnimation()
{
	PROFILE(SceneExporter::ExportAnimation);

	MStatus kStatus;

	MTime kTimeMin   = MAnimControl::minTime();
	MTime kTimeMax   = MAnimControl::maxTime();
	MTime kTimeTotal = kTimeMax - kTimeMin;
	float fLength   = (float)kTimeTotal.as(MTime::kSeconds);
	int iTimeMin    = (int)kTimeMin.value();
	int iTimeMax    = (int)kTimeMax.value();
	int iFrames     = (iTimeMax - iTimeMin) + 1;
	
	if(iFrames == 0) {
		printf("WARNING: animation has 0 length\n");
		return false;
	}

	if ((!m_bHasSkeleton) || (m_vpkJoints.size() == 0)) {
		printf("ERROR: there's no skeleton!\n");
		return false;
	}

	MAnimControl kAnimControl;

	printf("Exporting animation\n");
	printf("... Animation start: %d end: %d, length: %fs\n", iTimeMin, iTimeMax, fLength);

	MTime kFrame = kTimeMin;
	int iFrame;
	unsigned int i;
	for(iFrame = iTimeMin; iFrame <= iTimeMax; iFrame++) {
		kFrame++;
		kAnimControl.setCurrentTime(kFrame);

		for(i = 0; i < m_vpkJoints.size(); i++) {
			MVector kTranslation;
			MQuaternion kRotation;
			MMatrix kLocalMat = m_vpkJoints[i]->m_kDagPath.inclusiveMatrix();
			MMatrix kInvMat   = m_vpkJoints[i]->m_kDagPath.exclusiveMatrixInverse();

			if(m_vpkJoints[i]->m_bHasParent)
				kLocalMat *= kInvMat;

			kTranslation.x = (float)kLocalMat( 3, 0 );
			kTranslation.y = (float)kLocalMat( 3, 1 );
			kTranslation.z = (float)kLocalMat( 3, 2 );

			float aafMatrix[4][4];
			kLocalMat.get(aafMatrix);
			kLocalMat.transpose();
			kRotation = kLocalMat;

//			if (i == 0)
//				printf("===\n");
//			printf("%d - %f %f %f\n", i, kTranslation.x, kTranslation.y, kTranslation.z);
//			printf("     %f %f %f %f\n", kRotation.x, kRotation.y, kRotation.z, kRotation.w);

			MayaAnimKeyFrame *pkKeyFrame = new MayaAnimKeyFrame();
			pkKeyFrame->t = ((float)iFrame)/((float)iTimeMax);
			pkKeyFrame->x = kTranslation.x;
			pkKeyFrame->y = kTranslation.y;
			pkKeyFrame->z = kTranslation.z;
			pkKeyFrame->rx = kRotation.x;
			pkKeyFrame->ry = kRotation.y;
			pkKeyFrame->rz = kRotation.z;
			pkKeyFrame->rw = kRotation.w;
			m_vpkJoints[i]->m_vpkKeyFrames.push_back(pkKeyFrame);
		}
	}

	fprintf(m_hFile, "skeletalanim = {\n\tname = unnamed\n\tlength = %f\n", fLength);
	for (i = 0; i < m_vpkJoints.size(); i++) {
		MayaJoint *pkJoint = m_vpkJoints[i];
		fprintf(m_hFile, "\ttrack = {\n\t\tbone = %d\n", pkJoint->m_uiID);
		for (iFrame = 0; iFrame < pkJoint->m_vpkKeyFrames.size(); iFrame++) {
			MayaAnimKeyFrame *pkKeyFrame = pkJoint->m_vpkKeyFrames[iFrame];
			fprintf(m_hFile, "\t\tkey = {\n\t\t\ttime = %f\n", pkKeyFrame->t);
			fprintf(m_hFile, "\t\t\tpos = %f %f %f\n", pkKeyFrame->x, pkKeyFrame->y, pkKeyFrame->z);
			fprintf(m_hFile, "\t\t\trot = %f %f %f %f\n", pkKeyFrame->rx, pkKeyFrame->ry, pkKeyFrame->rz, pkKeyFrame->rw);
			fprintf(m_hFile, "\t\t}\n");
		}
		fprintf(m_hFile, "\t}\n");
	}
	fprintf(m_hFile, "}\n");

	return true;
}

