// Termm-Fall 2020

#pragma once

#include "Material.hpp"
#include "../Application/AABB.hpp"

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <map>
#include <iostream>

enum class NodeType {
	SceneNode,
	GeometryNode
};

class GeometryNode;

class SceneNode {
public:
	// variables
	SceneNode* parent;
	std::list<SceneNode*> children;

	NodeType m_nodeType;
	std::string m_name;
	unsigned int m_nodeId;
	glm::mat4 trans;
	glm::vec3 globalPos;

	// methods
    SceneNode(const std::string & name);
	SceneNode(const SceneNode & other);
    virtual ~SceneNode();
	int totalSceneNodes() const;
    
    const glm::mat4& get_transform() const;
    void set_transform(const glm::mat4& m);
    
    void add_child(SceneNode* child);
    void remove_child(SceneNode* child);

	//-- Transformations:
    void rotate(char axis, float angle);
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);

	friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

    virtual void updateGlobalPos(glm::mat4 curM);
    glm::vec3 getGlobalPos();
	// recursively checks if any node intersects with the given AABB
	virtual GeometryNode* checkIntersect(AABB& other);
	// recursively finds node with id
    SceneNode* getNodeWithId(unsigned int id);
private:
	// The number of SceneNode instances.
	static unsigned int nodeInstanceCount;
};
