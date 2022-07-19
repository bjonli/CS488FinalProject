#include "Scene.hpp"
#include "GeometryNode.hpp"
#include "../Application/MathUtils.hpp"

#include <glm/glm.hpp>
#include <string>
#include <glm/gtc/matrix_transform.hpp>

Scene::Scene(ParticleShader* pm):
    root(nullptr), player(nullptr), particleManager(pm) {}

Scene::~Scene() {
    if (root != nullptr) {
        delete root;
        root = nullptr;
    }
}

// helper function, quickly creates a scenenode and geometry node for a mesh
SceneNode* Scene::createNodeRotateMesh(
    std::string meshId, std::string name, 
    glm::vec3 T, glm::vec3 S, float Ry = 0 
) {
    SceneNode* baseNode = new SceneNode(name);
    baseNode -> rotate('y', Ry);
    baseNode -> translate(T);
    GeometryNode* meshNode = new GeometryNode(
		meshId, 
		name+"Mesh",
		batchInfoMap[meshId].aabb
	);
    meshNode -> scale(S);
    baseNode -> add_child(meshNode);
    return baseNode;
}

void setTexture(SceneNode* baseNode, std::string textureName, TextureManager* manager) {
	GeometryNode* meshNode = static_cast<GeometryNode*>(baseNode->children.front());
	meshNode->setTexture(manager -> getTextureId(textureName));
}

void setMaterial(SceneNode* baseNode, Material m) {
	GeometryNode* meshNode = static_cast<GeometryNode*>(baseNode->children.front());
	meshNode->setMaterial(m);
}

// MATERIALS
Material lightGray(glm::vec4(0.85, 0.85, 0.85, 1), glm::vec3(0.3, 0.3, 0.3), 25.0);
Material ground(glm::vec4(0.3, 0.6, 0.3, 1), glm::vec3(0.1, 0.1, 0.1), 10.0);
Material red(glm::vec4(0.8, 0.2, 0.2, 1), glm::vec3(0.5, 0.5, 0.5), 10.0);
Material blue(glm::vec4(0.1, 0.1, 0.8, 1), glm::vec3(0.5, 0.5, 0.5), 10.0);
Material blueTint(glm::vec4(0.1, 0.1, 0.8, 0.3), glm::vec3(0.7, 0.7, 0.7), 10.0);
Material windowTint(glm::vec4(0.3, 1, 1, 0.15), glm::vec3(0.9, 0.9, 0.9), 25.0);
Material redTint(glm::vec4(0.8, 0.1, 0.1, 0.3), glm::vec3(0.7, 0.7, 0.7), 10.0);
Material brown(glm::vec4(0.8, 0.2, 0.2, 1), glm::vec3(0.5, 0.5, 0.5), 10.0);
Material greenTint(glm::vec4(0.1, 0.8, 0.1, 0.3), glm::vec3(0.7, 0.7, 0.7), 10.0);
Material almostTransparent(glm::vec4(0.8, 0.8, 1, 0.02), glm::vec3(0.7, 0.7, 0.7), 10.0);

SceneNode* Scene::generateShapeScene() {
	SceneNode* root = new SceneNode("shape_root");

	SceneNode* sphereNode = createNodeRotateMesh("sphere", "mySphere1",
		glm::vec3(0, 2, 2), glm::vec3(2, 2, 2), 0);
	setMaterial(sphereNode, red);
	root->add_child(sphereNode);

	SceneNode* boxNode = createNodeRotateMesh("same_side_cube", "mybox1",
		glm::vec3(-2, 0.5, -3), glm::vec3(3, 1, 4), 0);
	setMaterial(boxNode, blue);
	root->add_child(boxNode);

	SceneNode* crateNode = createNodeRotateMesh("same_side_cube", "mycrate",
		glm::vec3(-1, 1, -1), glm::vec3(1, 1, 1), 0);
	setTexture(crateNode, "Crate", textureManager);
	boxNode->add_child(crateNode);

	SceneNode* tNode = createNodeRotateMesh("same_side_cube", "tNode",
		glm::vec3(3, 1.01, -2), glm::vec3(1, 2, 1), 0);
	setMaterial(tNode, blueTint);
	root->add_child(tNode);

	SceneNode* trNode = createNodeRotateMesh("same_side_cube", "trNode",
		glm::vec3(-4, 1.01, -1), glm::vec3(1, 2, 1), 0);
	setMaterial(trNode, redTint);
	root->add_child(trNode);

	SceneNode* tgNode = createNodeRotateMesh("same_side_cube", "tgNode",
		glm::vec3(1, 1.01, -5), glm::vec3(1, 2, 1), 0);
	setMaterial(tgNode, greenTint);
	root->add_child(tgNode);

	// TODO: make helper function
	Lantern* lantern = new Lantern("mylantern", 5, batchInfoMap["mylantern"].aabb);
	lantern->translate(glm::vec3(0, 0, -1));
	root->add_child(lantern);
	lanterns.push_back(lantern);

	return root;
}


SceneNode* Scene::generateRoomScene() {
	SceneNode* root = new SceneNode("room_root");

	SceneNode* tableNode = createNodeRotateMesh("table", "table",
		glm::vec3(0.25, 0.45, 0), glm::vec3(2, 2, 2), 0);
	setTexture(tableNode, "TableWood", textureManager);
	root->add_child(tableNode);

	SceneNode* chairNode1 = createNodeRotateMesh("chair", "chair1",
		glm::vec3(-1.2, 0.7, 0), glm::vec3(1.4, 1.4, 1.4), 180);
	setTexture(chairNode1, "TableWood", textureManager);
	root->add_child(chairNode1);

	SceneNode* chairNode2 = createNodeRotateMesh("chair", "chair2",
		glm::vec3(-0.75, 0.7, -1.4), glm::vec3(1.4, 1.4, 1.4), 0);
	setTexture(chairNode2, "TableWood", textureManager);
	// root->add_child(chairNode2);

	SceneNode* floor = createNodeRotateMesh("same_side_cube", "roomfloor",
		glm::vec3(0.5, 0.02, 2), glm::vec3(4.5, 0.05, 6), 0);
	setTexture(floor, "FloorWood", textureManager);
	root->add_child(floor);

	SceneNode* crate1 = createNodeRotateMesh("same_side_cube", "roomcrate1",
		glm::vec3(1.65, 0.5, 3.9), glm::vec3(1, 1, 1), 45);
	setTexture(crate1, "Crate", textureManager);
	root->add_child(crate1);

	SceneNode* crate2 = createNodeRotateMesh("same_side_cube", "roomcrate2",
		glm::vec3(0.2, 0.9, 0.2), glm::vec3(0.8, 0.8, 0.8), 60);
	setTexture(crate2, "Crate", textureManager);
	crate1->add_child(crate2);

	SceneNode* leftWall = createNodeRotateMesh("same_side_cube", "roomLW",
		glm::vec3(-1.8, 1.75, 2), glm::vec3(0.2, 3.5, 6), 0);
	setTexture(leftWall, "WallWood", textureManager);
	root->add_child(leftWall);

	SceneNode* painting = createNodeRotateMesh("same_side_cube", "painting",
		glm::vec3(0.2, 0.4, 0), glm::vec3(0.2, 2, 1.5), 0);
	setTexture(painting, "Painting1", textureManager);
	leftWall->add_child(painting);

	SceneNode* rightWall = createNodeRotateMesh("same_side_cube", "roomRW",
		glm::vec3(2.7, 1.75, 2), glm::vec3(0.2, 3.5, 6), 0);
	setTexture(rightWall, "WallWood", textureManager);
	root->add_child(rightWall);

	SceneNode* a4 = createNodeRotateMesh("same_side_cube", "a4",
		glm::vec3(-0.15, 0.4, 0), glm::vec3(0.2, 2, 2), 0);
	setTexture(a4, "A4", textureManager);
	rightWall->add_child(a4);

	SceneNode* backWallL = createNodeRotateMesh("same_side_cube", "backWallL",
		glm::vec3(-1.25, 1.75, -1), glm::vec3(1, 3.5, 0.2), 0);
	setTexture(backWallL, "WallWood", textureManager);
	root->add_child(backWallL);

	SceneNode* monkeyNode = createNodeRotateMesh("monkey", "monkey",
		glm::vec3(0, 1, 0.3), glm::vec3(0.2, 0.2, 0.2), 0);
	setMaterial(monkeyNode, red);
	backWallL->add_child(monkeyNode);

	SceneNode* backWallR = createNodeRotateMesh("same_side_cube", "backWallR",
		glm::vec3(2.5, 1.75, -1), glm::vec3(0.5, 3.5, 0.2), 0);
	setTexture(backWallR, "WallWood", textureManager);
	root->add_child(backWallR);

	SceneNode* backWallB = createNodeRotateMesh("same_side_cube", "backWallB",
		glm::vec3(0.75, 0.5, -1), glm::vec3(3, 1, 0.2), 0);
	setTexture(backWallB, "WallWood", textureManager);
	root->add_child(backWallB);

	SceneNode* windowLedge = createNodeRotateMesh("same_side_cube", "windowLedge",
		glm::vec3(0, 0.6, 0), glm::vec3(3, 0.2, 0.35), 0);
	setMaterial(windowLedge, lightGray);
	backWallB->add_child(windowLedge);

	SceneNode* window = createNodeRotateMesh("same_side_cube", "window",
		glm::vec3(0, 1.01, 0), glm::vec3(3, 1.8, 0.15), 0);
	setMaterial(window, windowTint);
	windowLedge->add_child(window);

	SceneNode* backWallT = createNodeRotateMesh("same_side_cube", "backWallT",
		glm::vec3(0, 1.151, 0), glm::vec3(3, 0.5, 0.2), 0);
	setTexture(backWallT, "WallWood", textureManager);
	window->add_child(backWallT);

	SceneNode* frontWallL = createNodeRotateMesh("same_side_cube", "frontWallL",
		glm::vec3(-1.5, 1.75, 5), glm::vec3(0.5, 3.5, 0.2), 0);
	setTexture(frontWallL, "WallWood", textureManager);
	root->add_child(frontWallL);

	SceneNode* frontWallR = createNodeRotateMesh("same_side_cube", "frontWallR",
		glm::vec3(1.7, 1.75, 5), glm::vec3(2.2, 3.5, 0.2), 0);
	setTexture(frontWallR, "WallWood", textureManager);
	root->add_child(frontWallR);

	SceneNode* roof = createNodeRotateMesh("same_side_cube", "frontWallR",
		glm::vec3(0.5, 3.5, 2), glm::vec3(5, 0.2, 6.5), 0);
	setMaterial(roof, brown);
	root->add_child(roof);

	SceneNode* plat = createNodeRotateMesh("same_side_cube", "room plat",
		glm::vec3(6.5, 3.8, -0.5), glm::vec3(2, 0.2, 2), 0);
	setMaterial(plat, almostTransparent);
	root->add_child(plat);

	Lantern* lantern = new Lantern("roomlantern2", 5, batchInfoMap["mylantern"].aabb);
	lantern->translate(glm::vec3(2, 0, 0));
	root->add_child(lantern);
	lanterns.push_back(lantern);

	return root;
}

SceneNode* Scene::generateRocksScene() {
	SceneNode* root = new SceneNode("rocks_root");

	Lantern* lantern = new Lantern("rocksLantern", 7, batchInfoMap["mylantern"].aabb);
	lantern->translate(glm::vec3(0, 0.25, 0));
	root->add_child(lantern);
	lanterns.push_back(lantern);

	const float dist = 2;

	SceneNode* LFpillar = createNodeRotateMesh("same_side_cube", "LFpillar",
		glm::vec3(-dist, 1, dist), glm::vec3(1, 2, 1), 0);
	setTexture(LFpillar, "Stone", textureManager);
	root->add_child(LFpillar);

	SceneNode* LBpillar = createNodeRotateMesh("same_side_cube", "LBpillar",
		glm::vec3(-dist, 1, -dist), glm::vec3(1, 2, 1), 0);
	setTexture(LBpillar, "Stone", textureManager);
	root->add_child(LBpillar);

	SceneNode* RFpillar = createNodeRotateMesh("same_side_cube", "RFpillar",
		glm::vec3(dist, 1, dist), glm::vec3(1, 2, 1), 0);
	setTexture(RFpillar, "Stone", textureManager);
	root->add_child(RFpillar);

	SceneNode* RBpillar = createNodeRotateMesh("same_side_cube", "RBpillar",
		glm::vec3(dist, 1, -dist), glm::vec3(1, 2, 1), 0);
	setTexture(RBpillar, "Stone", textureManager);
	root->add_child(RBpillar);

	SceneNode* Lwall = createNodeRotateMesh("same_side_cube", "Lwall",
		glm::vec3(-dist, 0.5, 0), glm::vec3(0.7, 1, 3), 0);
	setTexture(Lwall, "Stone", textureManager);
	root->add_child(Lwall);

	SceneNode* Rwall = createNodeRotateMesh("same_side_cube", "Rwall",
		glm::vec3(dist, 0.5, 0), glm::vec3(0.7, 1, 3), 0);
	setTexture(Rwall, "Stone", textureManager);
	root->add_child(Rwall);

	SceneNode* Bwall = createNodeRotateMesh("same_side_cube", "Rwall",
		glm::vec3(0, 0.5, -dist), glm::vec3(3, 1, 0.7), 0);
	setTexture(Bwall, "Stone", textureManager);
	root->add_child(Bwall);

	std::vector<std::vector<bool>> ARROW = {
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 1, 0},
		{0, 1, 0, 0, 1, 1, 1, 0},
		{0, 1, 0, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 1, 0, 0, 0},
		{0, 1, 1, 1, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 1, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0}
	};

	// list of colours with a gray scale of 0.5
	std::vector<glm::vec3> rgbDark = {
		glm::vec3(0.15081960775957004 ,  0.6488479396137355 ,  0.6493964625142618),
		glm::vec3(0.3224105011532953 ,  0.6076099823646552 ,  0.4116859693606325),
		glm::vec3(0.9115924912486839 ,  0.31239795140554305 ,  0.3864583126455241),
		glm::vec3(0.278388057236434 ,  0.5234859131411064 ,  0.9603135076533055),
		glm::vec3(0.3627154164910924 ,  0.6131687968203202 ,  0.2773509362775037),
		glm::vec3(0.42237567326701864 ,  0.5660606681611152 ,  0.3634391358121651),
		glm::vec3(0.7935805196073782 ,  0.42967752585176955 ,  0.09209400844215114),
		glm::vec3(0.6673100699342197 ,  0.33348028728345136 ,  0.9186084250375649),
		glm::vec3(0.7433107761570519 ,  0.3552761840352492 ,  0.6070434903539493),
		glm::vec3(0.09813549533307109 ,  0.6276955329977064 ,  0.896493061629457),
		glm::vec3(0.9609918274613004 ,  0.2681991736548074 ,  0.4844783215236777),
		glm::vec3(0.8327362430894132 ,  0.31246617771201357 ,  0.5929317280641535),
		glm::vec3(0.6541434907530567 ,  0.4043041376912129 ,  0.5884611178955627),
		glm::vec3(0.6689828101657215 ,  0.38733009650600136 ,  0.636941869398478),
		glm::vec3(0.26394188254959405 ,  0.7115883515804186 ,  0.02964048017513735),
	};

	// list of colours with a gray scale of 0.6
	std::vector<glm::vec3> rgbLight = {
		glm::vec3(0.7556015435900337 ,  0.5545037031785043 ,  0.4261531991298065),
		glm::vec3(0.5471182420318671 ,  0.6296542486038641 ,  0.5860052780877497),
		glm::vec3(0.016846655403377042 ,  0.9579202667242342 ,  0.2865232760286381),
		glm::vec3(0.4530400700843564 ,  0.6016332107638868 ,  0.9770379326875079),
		glm::vec3(0.16250396897100916 ,  0.8576392011977554 ,  0.4208517734612793),
		glm::vec3(0.13469965953086804 ,  0.760975071023079 ,  0.9915125886817812),
		glm::vec3(0.5178026032127128 ,  0.6513841193517138 ,  0.5510047682451129),
		glm::vec3(0.044442894743673134 ,  0.9602227178914188 ,  0.202288062012096),
		glm::vec3(0.8319858920184069 ,  0.4962530648488396 ,  0.5257514843879607),
		glm::vec3(0.2763551169595625 ,  0.7605116990489688 ,  0.6223636200644403),
		glm::vec3(0.6408941501228909 ,  0.6063972734494644 ,  0.4598021894598254),
		glm::vec3(0.18912587718537255 ,  0.7532018603784665 ,  0.8887883392931033),
		glm::vec3(0.5493814996247264 ,  0.7231374477088186 ,  0.09871271760640589),
		glm::vec3(0.35496732067211323 ,  0.6839756936947375 ,  0.8102722712300635),
		glm::vec3(0.5654571931864191 ,  0.6053286077503803 ,  0.663161460419188)
	};

	glm::vec3 mosiacCenter = glm::vec3(0, 0, 0);
	float mosiacWidth = 3;
	float mosiacHeight = 3;
	int pixelHeight = ARROW.size();
	int pixelWidth = ARROW[0].size();
	float deltaW = mosiacWidth / pixelWidth;
	float deltaH = mosiacHeight / pixelHeight;
	glm::vec3 pixelDim = glm::vec3(deltaW, 0.2, deltaH);
	glm::vec3 topLeftCorner = mosiacCenter - glm::vec3(mosiacWidth*0.5, 0, mosiacHeight*0.5) 
		+ pixelDim*0.5f;
	for (int r = 0; r < pixelHeight; r++) {
		for (int c = 0; c < pixelWidth; c++) {
			SceneNode* pixel = createNodeRotateMesh("same_side_cube", 
				std::string("pixel"),
				topLeftCorner + glm::vec3(deltaW*c, 0, deltaH*r), 
				pixelDim, 0
			);
			Material m(glm::vec4(0.0f), glm::vec3(0.0f), 10);
			if (ARROW[r][c] == 1) {
				int x = randRangeInt(0, rgbDark.size());
				m.kd = glm::vec4(rgbDark[x], 1.0f);
			}
			else {
				int x = randRangeInt(0, rgbLight.size());
				m.kd = glm::vec4(rgbLight[x], 1.0f);
			}
			setMaterial(pixel, m);
			root->add_child(pixel);
		}
	}

	return root;
}

void Scene::generateScene(
	TextureManager* textureManager_, 
	BatchInfoMap& batchInfoMap_,
	SoundManager* soundManager
) {
	textureManager = textureManager_;
	batchInfoMap = batchInfoMap_;

	// TODO: have a function read from a file
    root = new SceneNode("root");

	// create the floor
    SceneNode* floorNode = createNodeRotateMesh("same_side_cube", "floor", 
        glm::vec3(0, -0.26, 0), glm::vec3(40, 0.5, 40), 0);
	setMaterial(floorNode, ground);
    root -> add_child(floorNode);
    
	SceneNode* shapesRoot = generateShapeScene();
	shapesRoot->translate(glm::vec3(5, 0.25, 5));
	floorNode->add_child(shapesRoot);

	SceneNode* roomRoot = generateRoomScene();
	roomRoot->translate(glm::vec3(-8, 0.25, -8));
	floorNode->add_child(roomRoot);

	SceneNode* rocksRoot = generateRocksScene();
	rocksRoot->translate(glm::vec3(5.5, 0.25, -12));
	floorNode->add_child(rocksRoot);

    // directional light from sky
    directionalLight = DirectionalLight(
        glm::normalize(glm::vec3(0.3, -1, 0.7)), 
        glm::vec3(1, 1, 1));

    player = new Player(glm::vec3(0, 1.2501, 10), 0.2, 0, PlayerMode::FLY, soundManager);
	
	// right now, the point light sources only come from the lanterns
	for (int i = 0; i < lanterns.size(); i++) {
		pointLights.push_back(lanterns[i]->getLightSource());
	}
}

Player* Scene::getPlayer() { return player; }

SceneNode* Scene::getRoot() { return root; }

std::vector<PointLight*> Scene::getPointLights() { return pointLights; }

void Scene::updateGlobalPos() {
    root->updateGlobalPos(glm::mat4(1));
}

DirectionalLight Scene::getDirectionalLight() {
    return directionalLight;
}

std::vector<Lantern*> Scene::getLanterns() { return lanterns; }

SceneNode* Scene::getNodeWithId(unsigned int id) {
    if (root == nullptr) { return nullptr; }
    return root -> getNodeWithId(id);
};

void Scene::tick(float elapsedTime) {
    // tick each lantern
    for (int i=0; i<lanterns.size(); i++) {
        lanterns[i] -> tick(elapsedTime, particleManager, player->getViewPos());
    }
}

GeometryNode* Scene::checkIntersect(AABB& other) {
	return root->checkIntersect(other);
}