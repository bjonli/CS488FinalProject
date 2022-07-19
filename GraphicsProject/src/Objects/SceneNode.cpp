#include "SceneNode.hpp"
#include "GeometryNode.hpp"

#include <iostream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Application/MathUtils.hpp"


// Static class variable
unsigned int SceneNode::nodeInstanceCount = 0;

//---------------------------------------------------------------------------------------
SceneNode::SceneNode(const std::string& name)
  : m_name(name),
	m_nodeType(NodeType::SceneNode),
	trans(glm::mat4(1.0)),
	m_nodeId(nodeInstanceCount++)
{

}

//---------------------------------------------------------------------------------------
// Deep copy
SceneNode::SceneNode(const SceneNode & other)
	: m_nodeType(other.m_nodeType),
	  m_name(other.m_name),
	  trans(other.trans),
	  parent(nullptr)
{
	for(SceneNode * child : other.children) {
		this->children.push_front(new SceneNode(*child));
	}
}

//---------------------------------------------------------------------------------------
SceneNode::~SceneNode() {
	for(SceneNode * child : children) {
		delete child;
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::set_transform(const glm::mat4& m) {
	trans = m;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_transform() const {
	return trans;
}

//---------------------------------------------------------------------------------------
void SceneNode::add_child(SceneNode* child) {
	if (child) { child->parent = this; }
	children.push_back(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::remove_child(SceneNode* child) {
	if (child) { child->parent = nullptr; }
	children.remove(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::rotate(char axis, float angle) {
	glm::vec3 rot_axis;

	switch (axis) {
		case 'x':
			rot_axis = glm::vec3(1,0,0);
			break;
		case 'y':
			rot_axis = glm::vec3(0,1,0);
	        break;
		case 'z':
			rot_axis = glm::vec3(0,0,1);
	        break;
		default:
			break;
	}
	glm::mat4 rot_matrix = glm::rotate(degreesToRadians((float)angle), rot_axis);
	trans = rot_matrix * trans;
}

//---------------------------------------------------------------------------------------
void SceneNode::scale(const glm::vec3 & amount) {
	trans = glm::scale(amount) * trans;
}

//---------------------------------------------------------------------------------------
void SceneNode::translate(const glm::vec3& amount) {
	trans = glm::translate(amount) * trans;
}


//---------------------------------------------------------------------------------------
int SceneNode::totalSceneNodes() const {
	return nodeInstanceCount;
}

void SceneNode::updateGlobalPos(glm::mat4 curM) {
	glm::mat4 fullT = curM*trans;
	globalPos = glm::vec3(fullT[3]);
	for(SceneNode * child : children) {
		child -> updateGlobalPos(fullT);
	}
}

GeometryNode* SceneNode::checkIntersect(AABB& other) {
	for (SceneNode * child : children) {
		GeometryNode* possible = child->checkIntersect(other);
		if (possible != nullptr) { return possible; }
	}
	return nullptr;
}

glm::vec3 SceneNode::getGlobalPos() { return globalPos; }

SceneNode* SceneNode::getNodeWithId(unsigned int id) {
	if (m_nodeId == id) { return this; }
	for(SceneNode * child : children) {
		SceneNode* candidate = child -> getNodeWithId(id);
		if (candidate != nullptr) { return candidate; }
	}
	return nullptr;
}

//---------------------------------------------------------------------------------------
std::ostream & operator << (std::ostream & os, const SceneNode & node) {

	//os << "SceneNode:[NodeType: ___, name: ____, id: ____]"
	switch (node.m_nodeType) {
		case NodeType::SceneNode:
			os << "SceneNode";
			break;
		case NodeType::GeometryNode:
			os << "GeometryNode";
			break;
	}
	os << ":[";

	os << "name:" << node.m_name << ", ";
	os << "id:" << node.m_nodeId;
	os << "]";

	return os;
}
