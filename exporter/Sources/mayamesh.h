
#include <stdio.h>
#include <vector>

#define _BOOL
#include <maya/MDagPath.h>
#include <maya/MString.h>


class MayaMaterial;


class MayaVertex
{
public:
	unsigned int			m_uiMayaVertexIndex;
	// pos
	float				x, y, z;

	// tex coords
	float				u, v;

	// normal
	float				nx, ny, nz;

	bool				operator == (const MayaVertex &rkOther) const
					{
						return
							(x == rkOther.x) &&
							(y == rkOther.y) &&
							(z == rkOther.z) &&
							(u == rkOther.u) &&
							(v == rkOther.v) &&
							(nx == rkOther.nx) &&
							(ny == rkOther.ny) &&
							(nz == rkOther.nz);
						//return
						//	(fabs(x - rkOther.x) < 0.0001) &&
						//	(fabs(y - rkOther.y) < 0.0001) &&
						//	(fabs(z - rkOther.z) < 0.0001) &&
						//	(fabs(u - rkOther.u) < 0.0001) &&
						//	(fabs(v - rkOther.v) < 0.0001) &&
						//	(fabs(nx - rkOther.nx) < 0.0001) &&
						//	(fabs(ny - rkOther.ny) < 0.0001) &&
						//	(fabs(nz - rkOther.nz) < 0.0001);
					};
};


class MayaTriangle
{
public:
	// Index to maya polygon
	// Needed when mapping to shaders
//	unsigned int			m_uiMayaPolygonIndex;

	// Vertices
	unsigned int			v[3];
};


class MayaVertexWeight
{
public:
	// Bone ID
	unsigned int			m_uiBone;

	float				m_fWeight;

	// Vertex position in bone space
	float				x, y, z;
};


class MayaVertexWeights
{
public:
	// temp offset used when exporting
	unsigned int			m_uiOffset;

	std::vector<MayaVertexWeight>	m_vkWeights;
/*
	unsigned int			*m_puiBones;

	float				*m_pfWeights;
*/
/*
	void				operator=(const MayaVertexWeights &rkOther)
					{
						m_uiOffset = rkOther.m_uiOffset;
						m_uiWeights = rkOther.m_uiWeights;
						m_puiBones = rkOther.m_puiBones;
						m_pfWeights = rkOther.m_pfWeights;
					};
*/
//					MayaVertexWeights()
//					{
//						m_puiBones = NULL;
//						m_pfWeights = NULL;
//					};
};


// One material mesh
class MayaMesh
{
public:
        MString                         mName;
	MayaMaterial*			m_pcMaterial;

	std::vector<MayaVertex>		m_acVertices;

	std::vector<MayaTriangle>	m_acTriangles;
};


class MayaModel
{
public:
	MString				m_strName;

	std::vector<MayaMesh*>		m_apcMeshes;
};


bool ExtractMayaMesh(std::vector<MayaMaterial*>* papcMayaMaterial, MayaModel* pcModel, MDagPath& rkDagPath);
