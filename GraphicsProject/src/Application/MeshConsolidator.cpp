#include "MeshConsolidator.hpp"
#include "Exception.hpp"
#include "ObjFileDecoder.hpp"
#include <iostream>
#include <algorithm>

using namespace glm;
using namespace std;

//----------------------------------------------------------------------------------------
// Default constructor
MeshConsolidator::MeshConsolidator()
{

}

//----------------------------------------------------------------------------------------
// Destructor
MeshConsolidator::~MeshConsolidator()
{

}

//----------------------------------------------------------------------------------------
template <typename T>
static void appendVector (
		std::vector<T> & dest,
		const std::vector<T> & source
) {
	// Increase capacity to hold source.size() more elements
	dest.reserve(dest.size() + source.size());

	dest.insert(dest.end(), source.begin(), source.end());
}


//----------------------------------------------------------------------------------------
MeshConsolidator::MeshConsolidator(
		std::vector<ObjFilePath> objFileList
) {

	MeshId meshId;
	vector<vec3> positions;
	vector<vec3> normals;
	vector<vec2> uvCoords;
	BatchInfo batchInfo;
	unsigned long indexOffset(0);


    for(const ObjFilePath & objFile : objFileList) {
	    ObjFileDecoder::decode(objFile.c_str(), meshId, positions, normals, uvCoords);

		uint numIndices = positions.size();

		std::cout << "Parsed Object: " << meshId << std::endl;
		std::cout << "Number of Vertices " << numIndices << std::endl;
		std::cout << "AABB: " << numIndices << std::endl;

		// print the AABB box initially
		float minx=100000, miny=100000, minz=100000;
		float maxx=-100000, maxy=-100000, maxz=-100000;
		for (int i=0; i<positions.size(); i++) {
			maxx = std::max(maxx, positions[i].x);
			maxy = std::max(maxy, positions[i].y);
			maxz = std::max(maxz, positions[i].z);
			minx = std::min(minx, positions[i].x);
			miny = std::min(miny, positions[i].y);
			minz = std::min(minz, positions[i].z);
		}
		std::cout << "X range: " << minx << " " << maxx << std::endl;
		std::cout << "Y range: " << miny << " " << maxy << std::endl;
		std::cout << "Z range: " << minz << " " << maxz << std::endl;

	    if (numIndices != normals.size()) {
		    throw Exception("Error within MeshConsolidator: "
					"positions.size() != normals.size()\n");
	    }

		std::cout << std::endl;

	    batchInfo.startIndex = indexOffset;
	    batchInfo.numIndices = numIndices;
		batchInfo.aabb = AABB(minx, maxx, miny, maxy, minz, maxz);

	    m_batchInfoMap[meshId] = batchInfo;

	    appendVector(m_vertexPositionData, positions);
	    appendVector(m_vertexNormalData, normals);
		appendVector(m_vertexTextureData, uvCoords);

	    indexOffset += numIndices;
    }

}

//----------------------------------------------------------------------------------------
void MeshConsolidator::getBatchInfoMap (
		BatchInfoMap & batchInfoMap
) const {
	batchInfoMap = m_batchInfoMap;
}

//----------------------------------------------------------------------------------------
// Returns the starting memory location for vertex position data.
const float * MeshConsolidator::getVertexPositionDataPtr() const {
	return &(m_vertexPositionData[0].x);
}

//----------------------------------------------------------------------------------------
// Returns the starting memory location for vertex normal data.
const float * MeshConsolidator::getVertexNormalDataPtr() const {
    return &(m_vertexNormalData[0].x);
}

const float* MeshConsolidator::getUVCoordDataPtr() const {
	return &(m_vertexTextureData[0].x);
}

//----------------------------------------------------------------------------------------
// Returns the total number of bytes of all vertex position data.
size_t MeshConsolidator::getNumVertexPositionBytes() const {
	return m_vertexPositionData.size() * sizeof(vec3);
}

//----------------------------------------------------------------------------------------
// Returns the total number of bytes of all vertex normal data.
size_t MeshConsolidator::getNumVertexNormalBytes() const {
	return m_vertexNormalData.size() * sizeof(vec3);
}

size_t MeshConsolidator::getNumUVCoordBytes() const {
	return m_vertexTextureData.size() * sizeof(vec2);
}