#pragma once

// Force GLFW to include gl3.h core functionality instead of gl.h deprecated code.
#include "Application/CS488Window.hpp"
#include "Shaders/ShadersImport.hpp"
#include "SoundManager.hpp"
#include "TextureManager.hpp"
#include "FlameManager.hpp"
#include "HUD.hpp"
#include "Objects/Scene.hpp"

#include <string>
#include <memory>

/*
 * Singleton base class for creating a GLFW window and OpenGL context.
 * Call getInstance() in order to obtain the singleton instance of this class.
 */
class Project: public CS488Window {
	SoundManager* soundManager;
	TextureManager* textureManager;
	FlameManager* flameManager;
	HUD* hud;

    ClassicShader* primary_shader;
	PickingShader* picking_shader;
	ParticleShader* particle_shader;
	TextShader* text_shader;
	ShadowShader* shadow_shader;
	SkyboxShader* skybox_shader;
	SpriteShader* sprite_shader;
	ShapeShader* shape_shader;

	// CR-someday: toggle this with some preprocessing var or something
	QuadShader* quad_shader; 					// for debugging shadows

    BatchInfoMap m_batchInfoMap;
    Scene* scene;
	Player* player;
	Flame selectedFlame;

	GeometryNode* selectedObj;

	glm::mat4 m_perpsective;

	double prevX = -1;
	double prevY = -1;

	// all the key bindings
	unsigned int keyForward;
	unsigned int keyBackward;
	unsigned int keyLeft;
	unsigned int keyRight;
	unsigned int keyUp;
	unsigned int keyDown;
	unsigned int keyToggleHUD;
	unsigned int keyToggleFlameRight;
	unsigned int keyToggleFlameLeft;
	unsigned int keyToggleParticles;
	unsigned int keyToggleSkybox;
	unsigned int keyToggleShadow;
	unsigned int keyToggleTextures;
	unsigned int keyToggleTransparency;
	unsigned int keyToggleSound;
	unsigned int keyTogglePlayerMode;
	unsigned int keyJump;

	// flag variables, for objectives that involve multiple shaders
	bool shouldDrawShadows;
	bool transparencyEnabled;

protected:
	void pick();
	// for key inputs that are held, like player movement
	void parsePlayerInput(float elapsedTime);

	virtual void init() override;
	virtual void appLogic(float elapsedTime) override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;
public:
    virtual ~Project();

    Project(); // Prevent direct construction.
};
