#include "HUD.hpp"
#include <algorithm>

// some HUD constants
const double CURSOR_RAD = 10;
const float DELTA_OPACITY = -0.25;
const glm::vec4 INTERACTABLE_COLOUR = glm::vec4(1, 1, 0.4, 1);
const glm::vec4 UNINTERACTABLE_COLOUR = glm::vec4(1, 1, 1, 1);
const glm::vec4 HUD_COLOUR = glm::vec4(0.8, 0.8, 0.8, 1);
const glm::vec4 HUD_BORDER_COLOUR = glm::vec4(0, 0, 0, 1);
const glm::vec4 HUD_ITEM_DISPLAY_COLOUR = glm::vec4(0.4, 0.4, 0.4, 1);
const glm::vec4 HUD_SELECTED_FLAME_COLOUR = glm::vec4(0, 0, 0, 1);
const glm::vec4 HUD_MESSAGE_COLOUR = glm::vec4(1, 1, 1, 1);

HUD::HUD(float wH, float wW, TextShader* textShader_, ShapeShader* shapeShader_, SpriteShader* spriteShader_)
	: windowH(wH), windowW(wW), shouldDraw(true), 
	 textShader(textShader_), shapeShader(shapeShader_), spriteShader(spriteShader_),
	messageOpacity(0) {}

void HUD::displayMessage(std::string m) {
	messageOpacity = 1;
	message = m;
}

void HUD::tick(float deltaT) {
	// slowly make the message fade
	messageOpacity = std::max((float)messageOpacity + DELTA_OPACITY * deltaT, (float)0.0);
}

void HUD::draw(GeometryNode* selectedObj, Flame selectedFlame) {
	if (!shouldDraw) { return;  }
	
	// draw the cursor
	// interactable iff we are looking at a lantern
	bool interactable = true;
	if (selectedObj == nullptr || selectedObj->getObjectType() != ObjectType::Lantern) {
		interactable = false;
	}
	shapeShader -> drawCircle(windowW*0.5, windowH*0.5, CURSOR_RAD,
		interactable ? INTERACTABLE_COLOUR : UNINTERACTABLE_COLOUR, false);

	// draw the equiped flame
	shapeShader->drawRect(100, 30, 310, 50, HUD_COLOUR, true);
	shapeShader->drawRect(100, 30, 310, 50, HUD_BORDER_COLOUR, false);
	shapeShader->drawCircle(100, 100, 90,
		HUD_COLOUR, true);
	shapeShader->drawCircle(100, 100, 90,
		HUD_BORDER_COLOUR, false);
	shapeShader->drawCircle(100, 100, 70,
		HUD_ITEM_DISPLAY_COLOUR, true);
	shapeShader->drawCircle(100, 100, 70,
		HUD_BORDER_COLOUR, false);

	// write the selected flame
	textShader->renderText(selectedFlame.name, 185, 43,
		0.7, HUD_SELECTED_FLAME_COLOUR);
	float s = 60;
	spriteShader->draw(100-s, 100-s, s*2, s*2, selectedFlame.spriteId);

	// message
	textShader->renderText(message, windowW / 3, windowH / 4,
		0.8, glm::vec4(glm::vec3(HUD_MESSAGE_COLOUR), messageOpacity));
}

bool HUD::getShouldDraw() { return shouldDraw; }

void HUD::setShouldDraw(bool sd) { shouldDraw = sd; }
