#include "Project.hpp"

#include "Application/MathUtils.hpp"
#include "Application/GlErrorCheck.hpp"
#include "Objects/Lantern.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <algorithm>

using namespace glm;

// fix game ticks per second
const float TICKS_PER_SECOND = 60;
const float cycleRate = 1/TICKS_PER_SECOND;

Project::Project():
    primary_shader(nullptr), 
    picking_shader(nullptr),
	shape_shader(nullptr),
    particle_shader(nullptr),
    text_shader(nullptr),
    shadow_shader(nullptr),
    skybox_shader(nullptr),
	sprite_shader(nullptr),
	hud(nullptr),
	flameManager(nullptr),
	soundManager(nullptr),
	textureManager(nullptr) {}

Project::~Project() {
	// CR-someday: getting a malformed heap exception when deleting primary shader
	// should fix. Not too urgent as this is the end, but indicates some problem 
	if (scene != nullptr) { delete scene; }
	if (primary_shader != nullptr) { delete primary_shader; } 
    if (picking_shader != nullptr) { delete picking_shader; }
	if (shape_shader != nullptr) { delete shape_shader;  }
    if (particle_shader != nullptr) { delete particle_shader; } 
    if (text_shader != nullptr) { delete text_shader; } 
    if (shadow_shader != nullptr) { delete shadow_shader; } 
    if (skybox_shader != nullptr) { delete skybox_shader; } 
	if (sprite_shader != nullptr) { delete sprite_shader; }
	if (hud != nullptr) { delete hud; }
	if (flameManager != nullptr) { delete flameManager; } 
    if (soundManager != nullptr) { delete soundManager; }
	if (textureManager != nullptr) { delete textureManager;  }
}

//----------------------------------------------
void Project::init() {
    glClearColor(0.7, 0.7, 0.7, 1.0);

    // view and perspective matrix
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);

    soundManager = new SoundManager(true);
	textureManager = new TextureManager();

    // load all the meshes
	std::vector<std::string> objects = {
		"same_side_cube.obj", 
		"sphere.obj", 
		"suzanne.obj", 
		"lantern1.obj",
		"table.obj",
		"chair.obj",
		"monkey.obj"
	};
	for (int i = 0; i < objects.size(); i++) {
		objects[i] = getAssetFilePath(("Objects/" + objects[i]).c_str());
	}
    MeshConsolidator meshConsolidator(objects);
    meshConsolidator.getBatchInfoMap(m_batchInfoMap);

    flameManager = new FlameManager(textureManager);
    selectedFlame = flameManager -> getFlameById(0);

	// setup shaders for the hud
    text_shader = new TextShader("Roboto-Black.ttf", m_windowHeight, m_windowWidth);
    text_shader -> initData();
	sprite_shader = new SpriteShader(m_windowHeight, m_windowWidth);
	sprite_shader->initData();
	shape_shader = new ShapeShader(m_windowHeight, m_windowWidth);
	shape_shader->initData();
	hud = new HUD(m_windowHeight, m_windowWidth, text_shader, shape_shader, sprite_shader);

    // init shaders and load meshes to shaders
	shouldDrawShadows = true;
	transparencyEnabled = true;
	shadow_shader = new ShadowShader(&m_batchInfoMap, m_windowWidth, m_windowHeight, shouldDrawShadows, transparencyEnabled);
    shadow_shader -> initMeshData(meshConsolidator);
    
    primary_shader = new ClassicShader(&m_batchInfoMap, shadow_shader, true, transparencyEnabled);
    primary_shader -> initMeshData(meshConsolidator);
    primary_shader -> loadUniforms(m_perpsective, shouldDrawShadows);

    quad_shader = new QuadShader(shadow_shader);
    
    picking_shader = new PickingShader(&m_batchInfoMap);
    picking_shader -> initMeshData(meshConsolidator);
    picking_shader -> loadUniforms(m_perpsective);

    particle_shader = new ParticleShader(10000);
    particle_shader -> initData();
    particle_shader -> loadUniforms(m_perpsective);

    skybox_shader = new SkyboxShader("Skybox/mountain", "png");
    skybox_shader -> loadUniforms(m_perpsective);

    // update scene & player
    scene = new Scene(particle_shader);
    scene -> generateScene(textureManager, m_batchInfoMap, soundManager);
    player = scene -> getPlayer();

    // set key bindings
    // CR-someday: maybe data-structure, command pattern
    keyForward = GLFW_KEY_W;
    keyBackward = GLFW_KEY_S;
    keyLeft = GLFW_KEY_A;
    keyRight = GLFW_KEY_D;
    keyUp = GLFW_KEY_R;
    keyDown = GLFW_KEY_F;
    keyToggleHUD = GLFW_KEY_T;
    keyToggleFlameRight = GLFW_KEY_E;
    keyToggleFlameLeft = GLFW_KEY_Q;
	keyToggleParticles = GLFW_KEY_5;
	keyToggleSkybox = GLFW_KEY_9;
	keyToggleShadow = GLFW_KEY_8;
	keyToggleTextures = GLFW_KEY_7;
	keyToggleTransparency = GLFW_KEY_0;
	keyToggleSound = GLFW_KEY_6;
	keyTogglePlayerMode = GLFW_KEY_2;
	keyJump = GLFW_KEY_SPACE;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

//----------------------------------------------
void Project::appLogic(float elapsedTime){
	// tick multiple times so it is fluent
	while (elapsedTime > 0) {
		float deltaT = std::min((float)elapsedTime, (float)cycleRate);

		// tick everything first
		scene->tick(deltaT);
		particle_shader->tick(deltaT);
		hud->tick(deltaT);
		player->tick(deltaT);

		scene->updateGlobalPos();

		// lastly, move the player
		parsePlayerInput(deltaT);

		elapsedTime -= cycleRate;
	}
	
	// then update picking
	pick();
}

//----------------------------------------------
void Project::guiLogic() {}

//----------------------------------------------
void Project::draw(){
    glm::mat4 V = player->getViewMatrix();
    glm::vec3 viewPos = player->getViewPos();
   
	// generate shadow map first
    shadow_shader -> drawScene(scene, viewPos);

	// draw the scene
	skybox_shader->draw(V, viewPos);
	primary_shader -> drawScene(scene, V, viewPos);
    particle_shader -> drawScene(V, viewPos);

    // then draw the hud
    hud->draw(selectedObj, selectedFlame);
	
	// for debugging
	// quad_shader -> draw();
}

//----------------------------------------------
void Project::cleanup(){}

void Project::parsePlayerInput(float deltaT) {
	// left right
	float deltaSide = 0;
	bool leftPressed = glfwGetKey(m_window, keyLeft) == GLFW_PRESS;
	bool rightPressed = glfwGetKey(m_window, keyRight) == GLFW_PRESS;
	if (leftPressed && !rightPressed) { deltaSide = -1; }
	else if (!leftPressed && rightPressed) { deltaSide = 1; }

	// forward backward
	float deltaForward = 0;
	bool forwardPressed = glfwGetKey(m_window, keyForward) == GLFW_PRESS;
	bool backwardPressed = glfwGetKey(m_window, keyBackward) == GLFW_PRESS;
	if (forwardPressed && !backwardPressed) { deltaForward = 1; }
	else if (!forwardPressed && backwardPressed) { deltaForward = -1; }

	// up down
	float deltaUp = 0;
	if (player->getPlayerMode() != PlayerMode::WALK) {
		// if flying, check the flying keys
		bool upPressed = glfwGetKey(m_window, keyUp) == GLFW_PRESS;
		bool downPressed = glfwGetKey(m_window, keyDown) == GLFW_PRESS;
		if (upPressed && !downPressed) { deltaUp = 1; }
		else if (!upPressed && downPressed) { deltaUp = -1; }
		player->setVelocityDescrete(deltaForward, deltaSide, deltaUp);
	}
	else {
		// only look at jumping key if were are in walking mode
		player->setVelocityDescrete(deltaForward, deltaSide);
		if (glfwGetKey(m_window, keyJump) == GLFW_PRESS) { player->jump(); }
	}
	player->tryMove(deltaT, scene);
}

bool Project::mouseMoveEvent (
		double xPos,
		double yPos
) {
    if (prevX < 0 || prevY < 0) {   // init case
        prevX = xPos;
        prevY = yPos;
        return true;
    }

    // move camera and move cursor back to the center, like a FPS
    double centerX =  m_windowWidth*0.5;
    double centerY = m_windowHeight*0.5;
    double deltaY = centerY - yPos;
    double deltaX = centerX - xPos;
    // ignore too large deltas (prob when you initially focus)
    if (abs(deltaY) < 200 && abs(deltaX) < 200) {
        player->turn(deltaX, deltaY);
    }
    glfwSetCursorPos(m_window, centerX, centerY);

    prevX = xPos;
    prevY = yPos;
	return true;
}

void Project::pick() {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // draw the scene using the picking shader
    glm::mat4 V = player->getViewMatrix();
    picking_shader->drawScene(scene, V, player->getViewPos());

	// A bit ugly -- don't want to swap the just-drawn false colours
	// to the screen, so read from the back buffer.
	glReadBuffer( GL_BACK );

	// read pixel in the center of the screen
	double centerX = m_windowWidth * 0.5;
	double centerY = m_windowHeight * 0.5;
	GLubyte buffer[4] = { 0, 0, 0, 0 };
	glReadPixels( int(centerX), int(centerY), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
	CHECK_GL_ERRORS;
    
	// find the geometry node whos id matches the pixel colour we extracted
    unsigned int id = picking_shader->colourToId(buffer);
    SceneNode* lookingAt = scene -> getNodeWithId(id);
    if (lookingAt == nullptr || lookingAt -> m_nodeType != NodeType::GeometryNode) {
        selectedObj = nullptr;
        return;
    }
    GeometryNode* geometryNode = static_cast<GeometryNode *>(lookingAt);
    selectedObj = geometryNode;
}

bool Project::mouseButtonInputEvent(int button, int actions, int mods) {
    bool eventHandled = false;
    if (actions == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            // try toggling a lantern
            if (selectedObj != nullptr && 
                selectedObj -> getObjectType() == ObjectType::Lantern) {
                Lantern* lantern = static_cast<Lantern*>(selectedObj);
                if (lantern -> getIsActivated()) {
                    lantern -> extinguish(soundManager);
                } else {
                    lantern -> light(selectedFlame, soundManager, player->getViewPos());
                }
                eventHandled = true;
            }
        }
    } 
    
    return eventHandled;
}

bool Project::keyInputEvent (int key, int action, int mods) {
    bool eventHandled = false;
	// yea, a comman pattern would be really nice here
	if (action == GLFW_PRESS) {
		if (key == keyToggleHUD) {
			hud->setShouldDraw(!(hud->getShouldDraw()));
			return true;
		}
		if (key == keyToggleFlameLeft) {
			selectedFlame = flameManager->getPrevFlame(selectedFlame);
			return true;
		}
		if (key == keyToggleFlameRight) {
			selectedFlame = flameManager->getNextFlame(selectedFlame);
			return true;
		}
		if (key == keyToggleParticles) {
			if (particle_shader->getIsEnabled()) {
				hud->displayMessage("Particles Disabled");
				particle_shader->setIsEnabled(false);
			}
			else {
				hud->displayMessage("Particles Enabled");
				particle_shader->setIsEnabled(true);
			}
			return true;
		}
		if (key == keyToggleSkybox) {
			if (skybox_shader->getIsEnabled()) {
				hud->displayMessage("Skybox Disabled");
				skybox_shader->setIsEnabled(false);
			}
			else {
				hud->displayMessage("Skybox Enabled");
				skybox_shader->setIsEnabled(true);
			}
			return true;
		}
		if (key == keyToggleShadow) {
			if (shouldDrawShadows) {
				hud->displayMessage("Shadows Disabled");
			}
			else {
				hud->displayMessage("Shadows Enabled");
			}
			shouldDrawShadows = !shouldDrawShadows;
			primary_shader->setShadowsEnabled(shouldDrawShadows);
			shadow_shader->setIsEnabled(shouldDrawShadows);
			return true;
		}
		if (key == keyToggleTextures) {
			if (primary_shader->getTexturesEnabled()) {
				hud->displayMessage("Textures Disabled");
				primary_shader->setTexturesEnabled(false);
			}
			else {
				hud->displayMessage("Textures Enabled");
				primary_shader->setTexturesEnabled(true);
			}
			return true; 
		}
		if (key == keyToggleTransparency) {
			if (transparencyEnabled) {
				hud->displayMessage("Transparency Disabled");
			}
			else {
				hud->displayMessage("Transparency Enabled");
			}				
			transparencyEnabled = !transparencyEnabled;
			shadow_shader->setTransparencyEnabled(transparencyEnabled);
			primary_shader->setTransparencyEnabled(transparencyEnabled);
			return true;
		}
		if (key == keyToggleSound) {
			if (soundManager->getIsEnabled()){
				hud->displayMessage("Sound Disabled");
				soundManager->setIsEnabled(false);
			}
			else {
				hud->displayMessage("Sound Enabled");
				soundManager->setIsEnabled(true);
			}
			return true;
		}
		if (key == keyTogglePlayerMode) {
			switch (player ->getPlayerMode()) {
			case PlayerMode::FLY:
				hud->displayMessage("Player Mode: Ghost");
				player->togglePlayerMode(PlayerMode::GHOST);
				break;
			case PlayerMode::GHOST:
				hud->displayMessage("Player Mode: Walk");
				player->togglePlayerMode(PlayerMode::WALK);
				break;
			case PlayerMode::WALK:
				hud->displayMessage("Player Mode: Fly");
				player->togglePlayerMode(PlayerMode::FLY);
				break;
			}
			return true;
		}
    }
    return eventHandled;
}
