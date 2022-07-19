#include "GeometryNode.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & meshId,
	const std::string & name,
	AABB aabb
) : SceneNode(name),
	meshId(meshId),
	objType(ObjectType::Basic),
	textureId(0),
	materialType(MaterialType::Plain),
	baseAABB(aabb)
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::setMaterial(Material m) {
	material = m;
	materialType = MaterialType::Plain;
}

void GeometryNode::setTexture(GLuint id) {
	textureId = id;
	materialType = MaterialType::Texture;
}

ObjectType GeometryNode::getObjectType() { return objType; }

void GeometryNode::updateGlobalPos(glm::mat4 curM) {
	transformedAABB = baseAABB.transform(curM*trans);
	SceneNode::updateGlobalPos(curM);
}

GeometryNode* GeometryNode::checkIntersect(AABB& other) {
	if (transformedAABB.intersect(other)) { return this; }
	return SceneNode::checkIntersect(other);
}