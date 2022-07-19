#pragma once
#include "Shaders/ShadersImport.hpp"
#include "FlameManager.hpp"
#include "Objects/GeometryNode.hpp"

class HUD {
	float windowH;
	float windowW;
	bool shouldDraw;

	std::string message;
	float messageOpacity;

	TextShader* textShader;
	ShapeShader* shapeShader;
	SpriteShader* spriteShader;
	
public:
	HUD(float wH, float wW, TextShader* textShader_, ShapeShader* shapeShader_, SpriteShader* spriteShader_);

	// CR-soon: maybe pass the project and call method sof project
	void draw(GeometryNode* selectedObj, Flame selectedFlame);

	void displayMessage(std::string message);
	void tick(float deltaT);

	bool getShouldDraw();
	void setShouldDraw(bool sd);
};
