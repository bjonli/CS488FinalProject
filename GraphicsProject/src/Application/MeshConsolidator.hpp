#pragma once

#include "BatchInfo.hpp"

#include "../OpenGLImport.hpp"
#include <vector>
#include <unordered_map>
#include <string>

// String identifier for a mesh.
typedef std::string  MeshId;

// File path to a .obj file.
typedef std::string ObjFilePath;


// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
// object. Each BatchInfo object contains an index offset and the number of indices
// required to render the mesh with identifier MeshId.
typedef std::unordered_map<MeshId, BatchInfo>  BatchInfoMap;


/*
* Class for consolidating all vertex data within a list of .obj files.
*/
class MeshConsolidator {
public:
	MeshConsolidator();

	MeshConsolidator(std::vector<ObjFilePath>  objFileList);

	~MeshConsolidator();

	const float * getVertexPositionDataPtr() const;

	const float * getVertexNormalDataPtr() const;

	const float * getUVCoordDataPtr() const;

	size_t getNumVertexPositionBytes() const;

	size_t getNumVertexNormalBytes() const;

	size_t getNumUVCoordBytes() const;

	void getBatchInfoMap(BatchInfoMap & batchInfoMap) const;


private:
	std::vector<glm::vec3> m_vertexPositionData;
	std::vector<glm::vec3> m_vertexNormalData;
	std::vector<glm::vec2> m_vertexTextureData;

	BatchInfoMap m_batchInfoMap;
};


