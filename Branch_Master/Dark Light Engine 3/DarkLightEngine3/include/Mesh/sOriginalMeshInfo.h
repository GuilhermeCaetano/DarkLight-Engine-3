#ifndef _sOriginalMeshInfo_HG_
#define _sOriginalMeshInfo_HG_

#include <string>

// Information about the vertex
struct sMeshVertex
{
	float x, y, z; // Position
	float nx, ny, nz; // Normals
	float u, v;
};

// Information about the triangle
struct sMeshTriangle
{
	int vertex1;
	int vertex2;
	int vertex3;
};

struct sOriginalMeshInfo
{
public:
	sOriginalMeshInfo();
	~sOriginalMeshInfo();

	// Dynamic arrays
	sMeshVertex* pVerticesFromFile;
	sMeshTriangle* pTrianglesFromFile;

	std::string meshFileName;

	// Original extents
	float oMaxX, oMaxY, oMaxZ;
	float oMinX, oMinY, oMinZ;
	float oExtentX, oExtentY, oExtentZ;
	float oMaxExtent;
	float oScaleForUnitBox;

	void CalculateExtents();
	void CalculateNormals();
};



#endif // !_sOriginalMeshInfo_HG_

