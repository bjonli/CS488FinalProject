#pragma once

#include "SceneNode.hpp"
#include "../OpenGLImport.hpp"

// node is a lantern, player, or other
enum class ObjectType {
	Basic,
	Lantern,
	Player
};

// colour the mesh either with one colour or a texture
enum class MaterialType {
	Plain,
	Texture
};

class GeometryNode : public SceneNode {
	protected:
		ObjectType objType;

	public:
		GeometryNode(
			const std::string & meshId,
			const std::string & name,
			AABB aabb_
		);

		MaterialType materialType;
		Material material;
		GLuint textureId;

		AABB baseAABB;				// AABB of the underlying mesh
		AABB transformedAABB;		// AABB after all transformations

		// Mesh Identifier. This must correspond to an object name of
		// a loaded .obj file.
		std::string meshId;

		ObjectType getObjectType();

		void setMaterial(Material m);
		void setTexture(GLuint id);

		virtual GeometryNode* checkIntersect(AABB& other) override;
		virtual void updateGlobalPos(glm::mat4 curM) override;
};
