#define _USE_MATH_DEFINES
#include <cmath>
//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

// contains new std::shuffle definition
#include <algorithm>
#include <random>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

// Model geometric includes
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
#include "Headers/ThirdPersonCamera.h"

// Font rendering
#include "Headers/FontTypeRendering.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/Texture.h"

// Include loader Model class
#include "Headers/Model.h"

// Include Terrain
#include "Headers/Terrain.h"

#include "Headers/AnimationUtils.h"

// Include Colision headers functions
#include "Headers/Colisiones.h"

// ShadowBox include
#include "Headers/ShadowBox.h"

// OpenAL include
#include <AL/alut.h>

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

int screenWidth;
int screenHeight;

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

//Random seed for key location
std::random_device device;
std::mt19937 rng(device());
std::uniform_int_distribution<std::mt19937::result_type> dist5(1, 5);

std::string blendMapTexture = "../Textures/Proyecto/blendMap.png";
std::string heightMapTexture = "../Textures/Proyecto/heightMap.png";
std::string pathTexture = "../Textures/Proyecto/environment/Path/path.jpg";
std::string envTextures[3] = {
	"../Textures/Proyecto/environment/Path/path.jpg",
	"../Textures/Proyecto/environment/Path/concretoDanado.jpg",
	"../Textures/Proyecto/environment/Path/concretoDanadoVerdoso.jpg"
};
std::string wallObjDir[6] = {
	"../models/Walls/WallA.fbx",
	"../models/Walls/WallB.fbx",
	"../models/Walls/WallC.fbx",
	"../models/Walls/WallD.fbx",
	"../models/Walls/WallE.fbx",
	"../models/Walls/WallF.fbx"
};
std::string HAobjDir[3] = {
	"../models/Houses/HA/HA-front.fbx",
	"../models/Houses/HA/HA-left.fbx",
	"../models/Houses/HA/HA-top.fbx"
};
std::string HBobjDir[5] = {
	"../models/Houses/HB/HB-back.fbx",
	"../models/Houses/HB/HB-front.fbx",
	"../models/Houses/HB/HB-left1.fbx",
	"../models/Houses/HB/HB-left2.fbx",
	"../models/Houses/HB/HB-top.fbx"
};
std::string HCobjDir[3] = {
	"../models/Houses/HC/HC-front.fbx",
	"../models/Houses/HC/HC-left.fbx",
	"../models/Houses/HC/HC-top.fbx"
};
std::string HDobjDir[4] = {
	"../models/Houses/HD/HD-back.fbx",
	"../models/Houses/HD/HD-left.fbx",
	"../models/Houses/HD/HD-right.fbx",
	"../models/Houses/HD/HD-top.fbx"
};
std::string HEobjDir[4] = {
	"../models/Houses/HE/HE-back.fbx",
	"../models/Houses/HE/HE-left.fbx",
	"../models/Houses/HE/HE-right.fbx",
	"../models/Houses/HE/HE-top.fbx"
};

GLFWwindow *window;

Shader shader;
//Shader con skybox
Shader shaderSkybox;
//Shader con multiples luces
Shader shaderMulLighting;
//Shader para el terreno
Shader shaderTerrain;
//Shader para las particulas de fountain
Shader shaderParticlesFountain;
//Shader para las particulas de fuego
Shader shaderParticlesFire;
//Shader para visualizar el buffer de profundidad
Shader shaderViewDepth;
//Shader para dibujar el buffer de profunidad
Shader shaderDepth;

std::shared_ptr<Camera> camera(new ThirdPersonCamera());
float distanceFromTarget = 15.0f;

Sphere skyboxSphere(20, 20);
Box boxCollider;
Sphere sphereCollider(10, 10);
Box boxViewDepth;
Box boxLightViewBox;

//Rayos
Cylinder cylinderShoot(10, 10, 0.05, 0.05);

ShadowBox *shadowBox;

// Models complex instances
// Lamps
Model modelLamp1;
Model modelLamp2;
Model modelLampPost2;
glm::vec3 escalamientoLampara = glm::vec3(0.5*7);
// Hierba
Model modelGrass;
// Fountain
Model modelFountain;

//Walls
Model modelWallA, modelWallB, modelWallC, modelWallD, modelWallE, modelWallF;
Model modelHAleft, modelHAfront, modelHAtop;	//3
Model modelHBleft1, modelHBleft2, modelHBtop, modelHBfront, modelHBback;	//5
Model modelHCleft, modelHCfront, modelHCtop;	//3
Model modelHDleft, modelHDright, modelHDtop, modelHDback;	//4
Model modelHEleft, modelHEright, modelHEtop, modelHEback;	//4
// Model animate instance
Model playerModelAnimate;
Model zombieModelAnimate1;
Model zombieModelAnimate2;
Model zombieModelAnimate3;
Model zombieModelAnimate4;
Model zombieModelAnimate5;
Model zombieModelAnimate6;
Model zombieModelAnimate7;

//Keys
Model keyModelAnimateA;
Model keyModelAnimateB;

Model modelRedDoor;
// Terrain model instance
Terrain terrain(-1, -1, 200, 12, heightMapTexture);

GLuint textureCespedID;
GLuint textureTerrainBackgroundID, textureTerrainRID, textureTerrainGID,
		textureTerrainBID, textureTerrainBlendMapID;
GLuint textureParticleFountainID, textureParticleFireID, texId;
GLuint skyboxTextureID;

//MAIN MENU TEXTURES
GLuint textureInit1ID, textureInit2ID, textureActivaID;
Shader shaderTexture;
Box boxIntro;
bool iniciaPartida = false, presionarOpcion = false;
bool isTryingToEscape = false;

//UI Textures
GLuint textureKeysCompleteID, textureKeyAFoundID, textureKeysEmptyID, textureKeysActivaID;


// Modelo para el redener de texto
FontTypeRendering::FontTypeRendering *modelText;
FontTypeRendering::FontTypeRendering *modelText2;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

std::string fileNames[6] = { "../Textures/mp_bloodvalley/blood-valley_ft.tga",
		"../Textures/mp_bloodvalley/blood-valley_bk.tga",
		"../Textures/mp_bloodvalley/blood-valley_up.tga",
		"../Textures/mp_bloodvalley/blood-valley_dn.tga",
		"../Textures/mp_bloodvalley/blood-valley_rt.tga",
		"../Textures/mp_bloodvalley/blood-valley_lf.tga" };



bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

float moveX = 0.0f, moveZ = 0.0f;

// Model matrix definitions
//eliminate ? matrixHeli
glm::mat4 modelMatrixHeli = glm::mat4(1.0f);
glm::mat4 modelMatrixFountain = glm::mat4(1.0f);
glm::mat4 modelMatrixWallA = glm::mat4(1.0f);
glm::mat4 modelMatrixWallB = glm::mat4(1.0f);
glm::mat4 modelMatrixWallC = glm::mat4(1.0f);
glm::mat4 modelMatrixWallD = glm::mat4(1.0f);
glm::mat4 modelMatrixWallE = glm::mat4(1.0f);
glm::mat4 modelMatrixWallF = glm::mat4(1.0f);
//Houses
std::vector<glm::mat4> houseAMatrix = { glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f) };
std::vector<glm::mat4> houseBMatrix = { glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f) };
std::vector<glm::mat4> houseCMatrix = { glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f) };
std::vector<glm::mat4> houseDMatrix = { glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f) };
std::vector<glm::mat4> houseEMatrix = { glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f) };

glm::mat4 modelMatrixPlayer = glm::mat4(1.0f);
glm::vec3 escalamientoPlayer = glm::vec3(0.08f);
glm::vec3 escalamientoWalls = glm::vec3(8.0f);
//Zombies
glm::mat4 modelMatrixZombie1 = glm::mat4(1.0f);
glm::mat4 modelMatrixZombie2 = glm::mat4(1.0f);
glm::mat4 modelMatrixZombie3 = glm::mat4(1.0f);
glm::mat4 modelMatrixZombie4 = glm::mat4(1.0f);
glm::mat4 modelMatrixZombie5 = glm::mat4(1.0f);
glm::mat4 modelMatrixZombie6 = glm::mat4(1.0f);
glm::mat4 modelMatrixZombie7 = glm::mat4(1.0f);
glm::vec3 scaleZombie = glm::vec3(0.08f);

//Keys
glm::mat4 modelMatrixKeyA = glm::mat4(1.0f);
glm::mat4 modelMatrixKeyB = glm::mat4(1.0f);
glm::vec3 scaleKey = glm::vec3(0.08f);
//glm::vec3 scaleKey = glm::vec3(1.0f);

glm::mat4 modelMatrixRedDoor = glm::mat4(1.0f);
glm::vec3 scaleRedDoor = glm::vec3(8.0f);

/*
* Animations Player
* 0 Idle ?
* 1 Walk
* 2 WalkBackwards
* 3 InjuredWalking
* 4 LeftSideWalk
* 5 RightSideWalk
* 6 Shoot
* 
* Animations Zombie
* 0 Idle
* 1 Walk
* 2 Die1
* 3 Die2
* 4 Reaction Shot
* 5 Bite Neck
* 6 ZombieDieDownPose1
* 7 ZombieDieDownPose2
*/

int animationIndexPlayer = 0;
int animationIndexZombie1 = 0;
int animationIndexZombie2 = 0;
int animationIndexZombie3 = 0;
int animationIndexZombie4 = 0;
int animationIndexZombie5 = 0;
int animationIndexZombie6 = 0;
int animationIndexZombie7 = 0;
int modelSelected = 1;
bool enableCountSelected = true;
bool renderEverything = true;

// Lamps positions
std::vector<glm::vec3> lamp1Position = { glm::vec3(-7.03, 0, -19.14), glm::vec3(
		24.41, 0, -34.57), glm::vec3(-10.15, 0, -54.10) };
std::vector<float> lamp1Orientation = { -17.0, -82.67, 23.70 };
//std::vector<glm::vec3> lamp2Position = { glm::vec3(-36.52, 0, -23.24), glm::vec3(-52.73, 0, -3.90) };
std::vector<glm::vec3> lamp2Position = { glm::vec3(-82.4, 0, 0), glm::vec3(15.92, 0, -31.44), glm::vec3(-55.2, 0, 72.16), glm::vec3(13.68, 0, -55.28) };
std::vector<float> lamp2Orientation = { 21.37 + 90, -65.0 + 90, 23.5 + 90, -60.0 + 90 };

// Blending model unsorted
std::map<std::string, glm::vec3> blendingUnsorted = { { "aircraft", glm::vec3(
		10.0, 0.0, -17.5) }, { "lambo", glm::vec3(23.0, 0.0, 0.0) }, { "heli",
		glm::vec3(5.0, 10.0, -5.0) },
		{ "fountain", glm::vec3(5.0, 0.0, -40.0) }, { "fire", glm::vec3(0.0,
				0.0, 7.0) } };

double deltaTime;
double currTime, lastTime;

// Jump variables
bool isJumpPlayer = false;
float GRAVITY = 1.81;
double tmv = 0;
double startTimeJump = 0;

//Shoot variables
bool isShooting = false;
double deltaTimeShoot = 0.0f, currTimeShoot = 0.0f, lastTimeShoot = 0.0f;

//Zombie variables

double deltaTimeShooted = 0.0f, deltaTimeDying = 0.0f;
double currTimeShooted = 0.0f, lastTimeShooted = 0.0f;
double currTimeDying = 0.0f, lastTimeDying = 0.0f;
double currTimeKeys = 0.0f, lastTimeKeys = 0.0f, deltaTimeKeys = 0.0f;

int zombieLife[7] = {3,3,3,3,3,3,3};
bool isZombieShooted[7] = {false, false, false, false, false, false, false};
bool isZombieDying[7] = { false, false, false, false, false, false, false };
bool isZombieAlive[7] = { true, true, true, true, true, true, true };
bool isZombieInRange[7] = { false, false, false, false, false, false, false };

//Player Variables
int vidasPlayer = 3;


//Keys Locations
glm::vec3 KeyAPos = glm::vec3(46.96f , 2.0f, -77.36f);
glm::vec3 KeyBPos = glm::vec3(82.8f	 , 2.0f,  62.48f);
glm::vec3 KeyCPos = glm::vec3(-14.88f, 2.0f, -64.48f);
glm::vec3 KeyDPos = glm::vec3(-13.76f, 2.0f, -14.0f );
glm::vec3 KeyEPos = glm::vec3(-23.52f, 2.0f,  39.84f);
int keySlotSelected = 0;
int keySlotSelectedB = 0;
bool isKeyCollected[2] = { false, false };
int keysFound = 0;

//Rays
glm::vec3 rayShootDirection;
glm::vec3 rayShootOri;
glm::vec3 rayShootTar;


// Definition for the particle system
GLuint initVel, startTime;
GLuint VAOParticles;
GLuint nParticles = 8000;
double currTimeParticlesAnimation, lastTimeParticlesAnimation;

// Definition for the particle system fire
GLuint initVelFire, startTimeFire;
GLuint VAOParticlesFire;
GLuint nParticlesFire = 2000;
GLuint posBuf[2], velBuf[2], age[2];
GLuint particleArray[2];
GLuint feedback[2];
GLuint drawBuf = 1;
float particleSize = 0.5, particleLifetime = 3.0;
double currTimeParticlesAnimationFire, lastTimeParticlesAnimationFire;

// Colliders
std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > collidersOBB;
std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> > collidersSBB;

// Framesbuffers
GLuint depthMap, depthMapFBO;

/**********************
 * OpenAL config
 * Sources:
 * 0	-	Fire
 * 1	-	Gunshot
 * 2	-	ZombieNormal
 * 3	-	ZombieDying
 * 4	-	Collect Key
 * 5	-	Player Walk
 */

// OpenAL Defines
#define NUM_BUFFERS 6
#define NUM_SOURCES 19
#define NUM_ENVIRONMENTS 1
// Listener
ALfloat listenerPos[] = { 0.0, 0.0, 4.0 };
ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat listenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
// Source 0 - Fire
ALfloat source0Pos[] = { -2.0, 0.0, 0.0 };
ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };
// Source 1 - Gunshot
ALfloat sourceGunPos[] = { 2.0, 0.0, 0.0 };
ALfloat sourceGunVel[] = { 0.0, 0.0, 0.0 };

// Source 2 - Zombie 1
ALfloat sourceZ1NPos[] = { 2.0, 0.0,0.0 };
ALfloat sourceZ1NVel[] = { 0.0, 0.0,0.0 };
// Source 3 - Zombie 2
ALfloat sourceZ2NPos[] = { 2.0, 0.0,0.0 };
ALfloat sourceZ2NVel[] = { 0.0, 0.0,0.0 };
// Source 4 - Zombie 3
ALfloat sourceZ3NPos[] = { 2.0, 0.0,0.0 };
ALfloat sourceZ3NVel[] = { 0.0, 0.0,0.0 };
// Source 3 - Zombie 4
ALfloat sourceZ4NPos[] = { 2.0, 0.0,0.0 };
ALfloat sourceZ4NVel[] = { 0.0, 0.0,0.0 };
// Source 3 - Zombie 5
ALfloat sourceZ5NPos[] = { 2.0, 0.0,0.0 };
ALfloat sourceZ5NVel[] = { 0.0, 0.0,0.0 };
// Source 3 - Zombie 6
ALfloat sourceZ6NPos[] = { 2.0, 0.0,0.0 };
ALfloat sourceZ6NVel[] = { 0.0, 0.0,0.0 };
// Source 3 - Zombie 7
ALfloat sourceZ7NPos[] = { 2.0, 0.0,0.0 };
ALfloat sourceZ7NVel[] = { 0.0, 0.0,0.0 };

//Source ZombieDie - Zombie 1
ALfloat sourceZ1DPos[] = { 2.0, 0.0,0.0 };
ALfloat sourceZ1DVel[] = { 0.0, 0.0,0.0 };
//Source ZombieDie - Zombie 2
ALfloat sourceZ2DPos[] = { 2.0, 0.0,0.0 };
ALfloat sourceZ2DVel[] = { 0.0, 0.0,0.0 };
//Source ZombieDie - Zombie 3
ALfloat sourceZ3DPos[] = { 2.0, 0.0,0.0 };
ALfloat sourceZ3DVel[] = { 0.0, 0.0,0.0 };
//Source ZombieDie - Zombie 4
ALfloat sourceZ4DPos[] = { 2.0, 0.0,0.0 };
ALfloat sourceZ4DVel[] = { 0.0, 0.0,0.0 };
//Source ZombieDie - Zombie 5
ALfloat sourceZ5DPos[] = { 2.0, 0.0,0.0 };
ALfloat sourceZ5DVel[] = { 0.0, 0.0,0.0 };
//Source ZombieDie - Zombie 6
ALfloat sourceZ6DPos[] = { 2.0, 0.0,0.0 };
ALfloat sourceZ6DVel[] = { 0.0, 0.0,0.0 };
//Source ZombieDie - Zombie 7
ALfloat sourceZ7DPos[] = { 2.0, 0.0,0.0 };
ALfloat sourceZ7DVel[] = { 0.0, 0.0,0.0 };

// Source Collect Key A
ALfloat sourceKeyAPos[] = { 2.0, 0.0, 0.0 };
ALfloat sourceKeyAVel[] = { 0.0, 0.0, 0.0 };
// Source Collect Key B
ALfloat sourceKeyBPos[] = { 2.0, 0.0, 0.0 };
ALfloat sourceKeyBVel[] = { 0.0, 0.0, 0.0 };
// Source Walk
ALfloat sourceWalkPos[] = { 2.0, 0.0, 0.0 };
ALfloat sourceWalkVel[] = { 0.0, 0.0, 0.0 };
// Buffers
ALuint buffer[NUM_BUFFERS];
ALuint source[NUM_SOURCES];
ALuint environment[NUM_ENVIRONMENTS];
// Configs
ALsizei size, freq;
ALenum format;
ALvoid *data;
int ch;
ALboolean loop;
//								{  0		1	  2	      3       4       5       6       7       8        9       10      11      12      13      14      15     16       17    18 };
//								{ fire,gunshot,	 z1n,	 z2n,    z3n,    z4n,    z5n,    z6n,    z7n,     z1d,    z2d,    z3d,    z4d,    z5d,    z6d,    z7d,   keyA,   keyB,  Walk};
std::vector<bool> sourcesPlay = { true,	false,	true,	true,	true,	true,	true,	true,	true,	false,	false,	false,	false,	false,	false,	false,	false,	false,	false };

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow *window, int key, int scancode, int action,
		int mode);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
void initParticleBuffers();
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);
void prepareScene();
void prepareDepthScene();
void renderScene(bool renderParticles = true);
//Funciones agregadas para seccionar el desastre
void modelMovementXboxPlayer();
void playerMovementKeyboard();
void stateZombieGeneral(int zN);
void configAudioListeners();
void openALSoundData();


void openALSoundData() {
	// Listener for the Third person camera
	listenerPos[0] = modelMatrixPlayer[3].x;
	listenerPos[1] = modelMatrixPlayer[3].y;
	listenerPos[2] = modelMatrixPlayer[3].z;
	alListenerfv(AL_POSITION, listenerPos);

	glm::vec3 upModel = glm::normalize(modelMatrixPlayer[1]);
	glm::vec3 frontModel = glm::normalize(modelMatrixPlayer[2]);

	listenerOri[0] = frontModel.x;
	listenerOri[1] = frontModel.y;
	listenerOri[2] = frontModel.z;
	listenerOri[3] = upModel.x;
	listenerOri[4] = upModel.y;
	listenerOri[5] = upModel.z;

	// Listener for the First person camera
	/*listenerPos[0] = camera->getPosition().x;
	 listenerPos[1] = camera->getPosition().y;
	 listenerPos[2] = camera->getPosition().z;
	 alListenerfv(AL_POSITION, listenerPos);
	 listenerOri[0] = camera->getFront().x;
	 listenerOri[1] = camera->getFront().y;
	 listenerOri[2] = camera->getFront().z;
	 listenerOri[3] = camera->getUp().x;
	 listenerOri[4] = camera->getUp().y;
	 listenerOri[5] = camera->getUp().z;*/
	alListenerfv(AL_ORIENTATION, listenerOri);

	for (unsigned int i = 0; i < sourcesPlay.size(); i++) {
		if (sourcesPlay[i]) {
			sourcesPlay[i] = false;
			alSourcePlay(source[i]);
		}
	}
}

void configAudioListeners() {
	alutInit(0, nullptr);
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
	alGetError(); // clear any error messages
	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating buffers !!\n");
		exit(1);
	}
	else {
		printf("init() - No errors yet.");
	}
	// Config source 0
	/* Sources:
		* 0		- Fire
		* 1		- Gunshot
		* 2-8	- ZombieNormal
		* 9-15	- ZombieDying
		* 16-17 - Collect Key
		* 18	- Player Walk
	*/
	// Generate buffers, or else no sound will happen!
	alGenBuffers(NUM_BUFFERS, buffer);
	buffer[0] = alutCreateBufferFromFile("../sounds/fire.wav");
	buffer[1] = alutCreateBufferFromFile("../sounds/P226-9mm-Close-Single-Gunshot-C.wav");
	buffer[2] = alutCreateBufferFromFile("../sounds/Zombie-Long-Attack-A5.wav");
	buffer[3] = alutCreateBufferFromFile("../sounds/Zombie-Dying-and-Choking-A1.wav");
	buffer[4] = alutCreateBufferFromFile("../sounds/Fast-Collection-of-coins.wav");
	//buffer[5] = alutCreateBufferFromFile("../sounds/sneaker-shoe-on-concrete-floor-slow-pace-1.wav");
	buffer[5] = alutCreateBufferFromFile("../sounds/sneaker-shoe-on-concrete-floor-medium-pace-1.wav");
	int errorAlut = alutGetError();
	if (errorAlut != ALUT_ERROR_NO_ERROR) {
		printf("- Error open files with alut %d !!\n", errorAlut);
		exit(2);
	}

	alGetError(); /* clear error */
	alGenSources(NUM_SOURCES, source);

	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating sources !!\n");
		exit(2);
	}
	else {
		printf("init - no errors after alGenSources\n");
	}
	//Fire
	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 3.0f);
	alSourcefv(source[0], AL_POSITION, source0Pos);
	alSourcefv(source[0], AL_VELOCITY, source0Vel);
	alSourcei(source[0], AL_BUFFER, buffer[0]);
	alSourcei(source[0], AL_LOOPING, AL_TRUE);
	alSourcef(source[0], AL_MAX_DISTANCE, 2000);
	//Gunshot
	alSourcef(source[1], AL_PITCH, 1.0f);
	alSourcef(source[1], AL_GAIN, 3.0f);
	alSourcefv(source[1], AL_POSITION, sourceGunPos);
	alSourcefv(source[1], AL_VELOCITY, sourceGunVel);
	alSourcei(source[1], AL_BUFFER, buffer[1]);
	alSourcei(source[1], AL_LOOPING, AL_FALSE);
	alSourcef(source[1], AL_MAX_DISTANCE, 200);

	//ZombieNormal - 1
	alSourcef(source[2], AL_PITCH, 1.0f);
	alSourcef(source[2], AL_GAIN, 0.3f);
	alSourcefv(source[2], AL_POSITION, sourceZ1NPos);
	alSourcefv(source[2], AL_VELOCITY, sourceZ1NVel);
	alSourcei(source[2], AL_BUFFER, buffer[2]);
	alSourcei(source[2], AL_LOOPING, AL_TRUE);
	alSourcef(source[2], AL_MAX_DISTANCE, 1000);
	//ZombieNormal - 2
	alSourcef(source[3], AL_PITCH, 1.0f);
	alSourcef(source[3], AL_GAIN, 0.3f);
	alSourcefv(source[3], AL_POSITION, sourceZ2NPos);
	alSourcefv(source[3], AL_VELOCITY, sourceZ2NVel);
	alSourcei(source[3], AL_BUFFER, buffer[2]);
	alSourcei(source[3], AL_LOOPING, AL_TRUE);
	alSourcef(source[3], AL_MAX_DISTANCE, 1000);
	//ZombieNormal - 3
	alSourcef(source[4], AL_PITCH, 1.0f);
	alSourcef(source[4], AL_GAIN, 0.3f);
	alSourcefv(source[4], AL_POSITION, sourceZ3NPos);
	alSourcefv(source[4], AL_VELOCITY, sourceZ3NVel);
	alSourcei(source[4], AL_BUFFER, buffer[2]);
	alSourcei(source[4], AL_LOOPING, AL_TRUE);
	alSourcef(source[4], AL_MAX_DISTANCE, 1000);
	//ZombieNormal - 4
	alSourcef(source[5], AL_PITCH, 1.0f);
	alSourcef(source[5], AL_GAIN, 0.3f);
	alSourcefv(source[5], AL_POSITION, sourceZ4NPos);
	alSourcefv(source[5], AL_VELOCITY, sourceZ4NVel);
	alSourcei(source[5], AL_BUFFER, buffer[2]);
	alSourcei(source[5], AL_LOOPING, AL_TRUE);
	alSourcef(source[5], AL_MAX_DISTANCE, 1000);
	//ZombieNormal - 5
	alSourcef(source[6], AL_PITCH, 1.0f);
	alSourcef(source[6], AL_GAIN, 0.3f);
	alSourcefv(source[6], AL_POSITION, sourceZ5NPos);
	alSourcefv(source[6], AL_VELOCITY, sourceZ5NVel);
	alSourcei(source[6], AL_BUFFER, buffer[2]);
	alSourcei(source[6], AL_LOOPING, AL_TRUE);
	alSourcef(source[6], AL_MAX_DISTANCE, 1000);
	//ZombieNormal - 6
	alSourcef(source[7], AL_PITCH, 1.0f);
	alSourcef(source[7], AL_GAIN, 0.3f);
	alSourcefv(source[7], AL_POSITION, sourceZ6NPos);
	alSourcefv(source[7], AL_VELOCITY, sourceZ6NVel);
	alSourcei(source[7], AL_BUFFER, buffer[2]);
	alSourcei(source[7], AL_LOOPING, AL_TRUE);
	alSourcef(source[7], AL_MAX_DISTANCE, 1000);
	//ZombieNormal - 7
	alSourcef(source[8], AL_PITCH, 1.0f);
	alSourcef(source[8], AL_GAIN, 0.3f);
	alSourcefv(source[8], AL_POSITION, sourceZ7NPos);
	alSourcefv(source[8], AL_VELOCITY, sourceZ7NVel);
	alSourcei(source[8], AL_BUFFER, buffer[2]);
	alSourcei(source[8], AL_LOOPING, AL_TRUE);
	alSourcef(source[8], AL_MAX_DISTANCE, 1000);
	//ZombieDie - 1
	alSourcef(source[9], AL_PITCH, 1.0f);
	alSourcef(source[9], AL_GAIN, 0.3f);
	alSourcefv(source[9], AL_POSITION, sourceZ1DPos);
	alSourcefv(source[9], AL_VELOCITY, sourceZ1DVel);
	alSourcei(source[9], AL_BUFFER, buffer[3]);
	alSourcei(source[9], AL_LOOPING, AL_FALSE);
	alSourcef(source[9], AL_MAX_DISTANCE, 1100);
	//ZombieDie - 2
	alSourcef(source[10], AL_PITCH, 1.0f);
	alSourcef(source[10], AL_GAIN, 0.3f);
	alSourcefv(source[10], AL_POSITION, sourceZ2DPos);
	alSourcefv(source[10], AL_VELOCITY, sourceZ2DVel);
	alSourcei(source[10], AL_BUFFER, buffer[3]);
	alSourcei(source[10], AL_LOOPING, AL_FALSE);
	alSourcef(source[10], AL_MAX_DISTANCE, 1100);
	//ZombieDie - 3
	alSourcef(source[11], AL_PITCH, 1.0f);
	alSourcef(source[11], AL_GAIN, 0.3f);
	alSourcefv(source[11], AL_POSITION, sourceZ3DPos);
	alSourcefv(source[11], AL_VELOCITY, sourceZ3DVel);
	alSourcei(source[11], AL_BUFFER, buffer[3]);
	alSourcei(source[11], AL_LOOPING, AL_FALSE);
	alSourcef(source[11], AL_MAX_DISTANCE, 1100);
	//ZombieDie - 4
	alSourcef(source[12], AL_PITCH, 1.0f);
	alSourcef(source[12], AL_GAIN, 0.3f);
	alSourcefv(source[12], AL_POSITION, sourceZ4DPos);
	alSourcefv(source[12], AL_VELOCITY, sourceZ4DVel);
	alSourcei(source[12], AL_BUFFER, buffer[3]);
	alSourcei(source[12], AL_LOOPING, AL_FALSE);
	alSourcef(source[12], AL_MAX_DISTANCE, 1100);
	//ZombieDie - 5
	alSourcef(source[13], AL_PITCH, 1.0f);
	alSourcef(source[13], AL_GAIN, 0.3f);
	alSourcefv(source[13], AL_POSITION, sourceZ5DPos);
	alSourcefv(source[13], AL_VELOCITY, sourceZ5DVel);
	alSourcei(source[13], AL_BUFFER, buffer[3]);
	alSourcei(source[13], AL_LOOPING, AL_FALSE);
	alSourcef(source[13], AL_MAX_DISTANCE, 1100);
	//ZombieDie - 6
	alSourcef(source[14], AL_PITCH, 1.0f);
	alSourcef(source[14], AL_GAIN, 0.3f);
	alSourcefv(source[14], AL_POSITION, sourceZ6DPos);
	alSourcefv(source[14], AL_VELOCITY, sourceZ6DVel);
	alSourcei(source[14], AL_BUFFER, buffer[3]);
	alSourcei(source[14], AL_LOOPING, AL_FALSE);
	alSourcef(source[14], AL_MAX_DISTANCE, 1100);
	//ZombieDie - 7
	alSourcef(source[15], AL_PITCH, 1.0f);
	alSourcef(source[15], AL_GAIN, 0.3f);
	alSourcefv(source[15], AL_POSITION, sourceZ7DPos);
	alSourcefv(source[15], AL_VELOCITY, sourceZ7DVel);
	alSourcei(source[15], AL_BUFFER, buffer[3]);
	alSourcei(source[15], AL_LOOPING, AL_FALSE);
	alSourcef(source[15], AL_MAX_DISTANCE, 1100);
	//Collect Key A
	alSourcef(source[16], AL_PITCH, 1.0f);
	alSourcef(source[16], AL_GAIN, 0.3f);
	alSourcefv(source[16], AL_POSITION, sourceKeyAPos);
	alSourcefv(source[16], AL_VELOCITY, sourceKeyAVel);
	alSourcei(source[16], AL_BUFFER, buffer[4]);
	alSourcei(source[16], AL_LOOPING, AL_FALSE);
	alSourcef(source[16], AL_MAX_DISTANCE, 1100);
	//Collect Key B
	alSourcef(source[17], AL_PITCH, 1.0f);
	alSourcef(source[17], AL_GAIN, 0.3f);
	alSourcefv(source[17], AL_POSITION, sourceKeyBPos);
	alSourcefv(source[17], AL_VELOCITY, sourceKeyBVel);
	alSourcei(source[17], AL_BUFFER, buffer[4]);
	alSourcei(source[17], AL_LOOPING, AL_FALSE);
	alSourcef(source[17], AL_MAX_DISTANCE, 1100);
	//Walk
	alSourcef(source[18], AL_PITCH, 1.0f);
	alSourcef(source[18], AL_GAIN, 0.5f);
	alSourcefv(source[18], AL_POSITION, sourceWalkPos);
	alSourcefv(source[18], AL_VELOCITY, sourceWalkVel);
	alSourcei(source[18], AL_BUFFER, buffer[5]);
	alSourcei(source[18], AL_LOOPING, AL_TRUE);
	alSourcef(source[18], AL_MAX_DISTANCE, 300);
}

void initParticleBuffers() {
	// Generate the buffers
	glGenBuffers(1, &initVel);   // Initial velocity buffer
	glGenBuffers(1, &startTime); // Start time buffer

	// Allocate space for all buffers
	int size = nParticles * 3 * sizeof(float);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL,
			GL_STATIC_DRAW);

	// Fill the first velocity buffer with random velocities
	glm::vec3 v(0.0f);
	float velocity, theta, phi;
	GLfloat *data = new GLfloat[nParticles * 3];
	for (unsigned int i = 0; i < nParticles; i++) {

		theta = glm::mix(0.0f, glm::pi<float>() / 6.0f,
				((float) rand() / RAND_MAX));
		phi = glm::mix(0.0f, glm::two_pi<float>(), ((float) rand() / RAND_MAX));

		v.x = sinf(theta) * cosf(phi);
		v.y = cosf(theta);
		v.z = sinf(theta) * sinf(phi);

		velocity = glm::mix(0.6f, 0.8f, ((float) rand() / RAND_MAX));
		v = glm::normalize(v) * velocity;

		data[3 * i] = v.x;
		data[3 * i + 1] = v.y;
		data[3 * i + 2] = v.z;
	}
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

	// Fill the start time buffer
	delete[] data;
	data = new GLfloat[nParticles];
	float time = 0.0f;
	float rate = 0.00075f;
	for (unsigned int i = 0; i < nParticles; i++) {
		data[i] = time;
		time += rate;
	}
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] data;

	glGenVertexArrays(1, &VAOParticles);
	glBindVertexArray(VAOParticles);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void initParticleBuffersFire() {
	// Generate the buffers
	glGenBuffers(2, posBuf);    // position buffers
	glGenBuffers(2, velBuf);    // velocity buffers
	glGenBuffers(2, age);       // age buffers

	// Allocate space for all buffers
	int size = nParticlesFire * sizeof(GLfloat);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, age[1]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);

	// Fill the first age buffer
	std::vector<GLfloat> initialAge(nParticlesFire);
	float rate = particleLifetime / nParticlesFire;
	for (unsigned int i = 0; i < nParticlesFire; i++) {
		int index = i - nParticlesFire;
		float result = rate * index;
		initialAge[i] = result;
	}
	// Shuffle them for better looking results
	//Random::shuffle(initialAge);
	auto rng = std::default_random_engine { };
	std::shuffle(initialAge.begin(), initialAge.end(), rng);
	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, initialAge.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create vertex arrays for each set of buffers
	glGenVertexArrays(2, particleArray);

	// Set up particle array 0
	glBindVertexArray(particleArray[0]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// Set up particle array 1
	glBindVertexArray(particleArray[1]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, age[1]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// Setup the feedback objects
	glGenTransformFeedbacks(2, feedback);

	// Transform feedback 0
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[0]);

	// Transform feedback 1
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[1]);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
				glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
				nullptr);

	if (window == nullptr) {
		std::cerr
				<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
				<< std::endl;
		destroy();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Inicialización de los shaders
	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderSkybox.initialize("../Shaders/skyBox.vs", "../Shaders/skyBox_fog.fs");
	shaderMulLighting.initialize(
			"../Shaders/iluminacion_textura_animation_shadow.vs",
			"../Shaders/multipleLights_shadow.fs");
	shaderTerrain.initialize("../Shaders/terrain_shadow.vs",
			"../Shaders/terrain_shadow.fs");
	shaderParticlesFountain.initialize("../Shaders/particlesFountain.vs",
			"../Shaders/particlesFountain.fs");
	shaderParticlesFire.initialize("../Shaders/particlesFire.vs",
			"../Shaders/particlesFire.fs", { "Position", "Velocity", "Age" });
	shaderViewDepth.initialize("../Shaders/texturizado.vs",
			"../Shaders/texturizado_depth_view.fs");
	shaderDepth.initialize("../Shaders/shadow_mapping_depth.vs",
			"../Shaders/shadow_mapping_depth.fs");
	shaderTexture.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado.fs");

	// Inicializacion de los objetos.
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(100.0f, 100.0f, 100.0f));

	boxCollider.init();
	boxCollider.setShader(&shader);
	boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	sphereCollider.init();
	sphereCollider.setShader(&shader);
	sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	boxViewDepth.init();
	boxViewDepth.setShader(&shaderViewDepth);

	boxLightViewBox.init();
	boxLightViewBox.setShader(&shaderViewDepth);

	boxIntro.init();
	boxIntro.setShader(&shaderTexture);
	boxIntro.setScale(glm::vec3(2.0, 2.0, 1.0));

	terrain.init();
	terrain.setShader(&shaderTerrain);
	terrain.setPosition(glm::vec3(100, 0, 100));

	//Rayos
	cylinderShoot.init();
	cylinderShoot.setShader(&shader);
	cylinderShoot.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	//Lamp models
	modelLamp1.loadModel("../models/Street-Lamp-Black/objLamp.obj");
	modelLamp1.setShader(&shaderMulLighting);
	modelLamp2.loadModel("../models/Street_Light/Lamp.obj");
	modelLamp2.setShader(&shaderMulLighting);
	modelLampPost2.loadModel("../models/Street_Light/LampPost.obj");
	modelLampPost2.setShader(&shaderMulLighting);

	//Grass
	modelGrass.loadModel("../models/grass/grassModel.obj");
	modelGrass.setShader(&shaderMulLighting);

	//Fountain
	modelFountain.loadModel("../models/fountain/fountain.obj");
	modelFountain.setShader(&shaderMulLighting);

	//Walls
	modelWallA.loadModel(wallObjDir[0]);
	modelWallA.setShader(&shaderMulLighting);
	modelWallB.loadModel(wallObjDir[1]);
	modelWallB.setShader(&shaderMulLighting);
	modelWallC.loadModel(wallObjDir[2]);
	modelWallC.setShader(&shaderMulLighting);
	modelWallD.loadModel(wallObjDir[3]);
	modelWallD.setShader(&shaderMulLighting);
	modelWallE.loadModel(wallObjDir[4]);
	modelWallE.setShader(&shaderMulLighting);
	modelWallF.loadModel(wallObjDir[5]);
	modelWallF.setShader(&shaderMulLighting);

	//Houses
	modelHAfront.loadModel(HAobjDir[0]);
	modelHAfront.setShader(&shaderMulLighting);
	modelHAleft.loadModel(HAobjDir[1]);
	modelHAleft.setShader(&shaderMulLighting);
	modelHAtop.loadModel(HAobjDir[2]);
	modelHAtop.setShader(&shaderMulLighting);

	modelHBback.loadModel(HBobjDir[0]);
	modelHBback.setShader(&shaderMulLighting);
	modelHBfront.loadModel(HBobjDir[1]);
	modelHBfront.setShader(&shaderMulLighting);
	modelHBleft1.loadModel(HBobjDir[2]);
	modelHBleft1.setShader(&shaderMulLighting);
	modelHBleft2.loadModel(HBobjDir[3]);
	modelHBleft2.setShader(&shaderMulLighting);
	modelHBtop.loadModel(HBobjDir[4]);
	modelHBtop.setShader(&shaderMulLighting);

	modelHCfront.loadModel(HCobjDir[0]);
	modelHCfront.setShader(&shaderMulLighting);
	modelHCleft.loadModel(HCobjDir[1]);
	modelHCleft.setShader(&shaderMulLighting);
	modelHCtop.loadModel(HCobjDir[2]);
	modelHCtop.setShader(&shaderMulLighting);

	modelHDback.loadModel(HDobjDir[0]);
	modelHDback.setShader(&shaderMulLighting);
	modelHDleft.loadModel(HDobjDir[1]);
	modelHDleft.setShader(&shaderMulLighting);
	modelHDright.loadModel(HDobjDir[2]);
	modelHDright.setShader(&shaderMulLighting);
	modelHDtop.loadModel(HDobjDir[3]);
	modelHDtop.setShader(&shaderMulLighting);

	modelHEback.loadModel(HEobjDir[0]);
	modelHEback.setShader(&shaderMulLighting);
	modelHEleft.loadModel(HEobjDir[1]);
	modelHEleft.setShader(&shaderMulLighting);
	modelHEright.loadModel(HEobjDir[2]);
	modelHEright.setShader(&shaderMulLighting);
	modelHEtop.loadModel(HEobjDir[3]);
	modelHEtop.setShader(&shaderMulLighting);

	//Player
	playerModelAnimate.loadModel("../models/Player/PlayerMixamo.fbx");
	playerModelAnimate.setShader(&shaderMulLighting);

	//Zombies
	zombieModelAnimate1.loadModel("../models/ZombieGirl/ZombieGirl.fbx");
	zombieModelAnimate1.setShader(&shaderMulLighting);
	zombieModelAnimate2.loadModel("../models/ZombieGirl/ZombieGirl.fbx");
	zombieModelAnimate2.setShader(&shaderMulLighting);
	zombieModelAnimate3.loadModel("../models/ZombieGirl/ZombieGirl.fbx");
	zombieModelAnimate3.setShader(&shaderMulLighting);
	zombieModelAnimate4.loadModel("../models/ZombieGirl/ZombieGirl.fbx");
	zombieModelAnimate4.setShader(&shaderMulLighting);
	zombieModelAnimate5.loadModel("../models/ZombieGirl/ZombieGirl.fbx");
	zombieModelAnimate5.setShader(&shaderMulLighting);
	zombieModelAnimate6.loadModel("../models/ZombieGirl/ZombieGirl.fbx");
	zombieModelAnimate6.setShader(&shaderMulLighting);
	zombieModelAnimate7.loadModel("../models/ZombieGirl/ZombieGirl.fbx");
	zombieModelAnimate7.setShader(&shaderMulLighting);

	//Keys
	keyModelAnimateA.loadModel("../models/Key/Key.fbx");
	keyModelAnimateA.setShader(&shaderMulLighting);
	keyModelAnimateA.setAnimationIndex(0);

	keyModelAnimateB.loadModel("../models/Key/Key.fbx");
	keyModelAnimateB.setShader(&shaderMulLighting);
	keyModelAnimateB.setAnimationIndex(0);

	//Door
	modelRedDoor.loadModel("../models/Door/door.fbx");
	modelRedDoor.setShader(&shaderMulLighting);

	camera->setPosition(glm::vec3(0.0, 0.0, 10.0));
	camera->setDistanceFromTarget(distanceFromTarget);
	camera->setSensitivity(1.0);

	// Definimos el tamanio de la imagen
	int imageWidth, imageHeight;
	FIBITMAP *bitmap;
	unsigned char *data;

	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		skyboxTexture = Texture(fileNames[i]);
		FIBITMAP *bitmap = skyboxTexture.loadImage(true);
		unsigned char *data = skyboxTexture.convertToData(bitmap, imageWidth,
				imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		} else
			std::cout << "Failed to load texture" << std::endl;
		skyboxTexture.freeImage(bitmap);
	}

	// Definiendo la textura a utilizar
	Texture textureCesped("../Textures/cesped.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureCesped.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureCesped.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureCespedID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureCespedID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureCesped.freeImage(bitmap);

	
	// Definiendo la textura a utilizar
	Texture textureTerrainBackground("../Textures/grassy2.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBackground.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBackground.convertToData(bitmap, imageWidth,
			imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBackgroundID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBackground.freeImage(bitmap);

	// Definiendo la textura a utilizar
	//Texture textureTerrainR("../Textures/mud.png");
	Texture textureTerrainR(envTextures[1]);
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainR.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainR.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainRID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainR.freeImage(bitmap);

	// Definiendo la textura a utilizar
	//Texture textureTerrainG("../Textures/grassFlowers.png");
	Texture textureTerrainG(envTextures[2]);
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainG.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainG.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainGID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainG.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainB(pathTexture);
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainB.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainB.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainB.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainBlendMap(blendMapTexture);
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBlendMap.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBlendMap.convertToData(bitmap, imageWidth,
			imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBlendMapID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBlendMap.freeImage(bitmap);

	Texture textureParticlesFountain("../Textures/bluewater.png");
	bitmap = textureParticlesFountain.loadImage();
	data = textureParticlesFountain.convertToData(bitmap, imageWidth,
			imageHeight);
	glGenTextures(1, &textureParticleFountainID);
	glBindTexture(GL_TEXTURE_2D, textureParticleFountainID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	textureParticlesFountain.freeImage(bitmap);

	Texture textureParticleFire("../Textures/fire.png");
	bitmap = textureParticleFire.loadImage();
	data = textureParticleFire.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureParticleFireID);
	glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	textureParticleFire.freeImage(bitmap);

	//Texturas Main Screen
	Texture textureIntro1("../Textures/mainMenuIniciar.png");
	bitmap = textureIntro1.loadImage();
	data = textureIntro1.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureInit1ID);
	glBindTexture(GL_TEXTURE_2D, textureInit1ID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	textureIntro1.freeImage(bitmap);

	Texture textureIntro2("../Textures/mainMenuSalir.png");
	bitmap = textureIntro2.loadImage();
	data = textureIntro2.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureInit2ID);
	glBindTexture(GL_TEXTURE_2D, textureInit2ID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	textureIntro2.freeImage(bitmap);

	Texture textureKeyEmpty("../Textures/UI_empty.png");
	bitmap = textureKeyEmpty.loadImage();
	data = textureKeyEmpty.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureKeysEmptyID);
	glBindTexture(GL_TEXTURE_2D, textureKeysEmptyID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	textureKeyEmpty.freeImage(bitmap);

	Texture textureKeyComplete("../Textures/UI_complete.png");
	bitmap = textureKeyComplete.loadImage();
	data = textureKeyComplete.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureKeysCompleteID);
	glBindTexture(GL_TEXTURE_2D, textureKeysCompleteID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	textureKeyComplete.freeImage(bitmap);

	Texture textureKeyAFound("../Textures/UI_Afound.png");
	bitmap = textureKeyAFound.loadImage();
	data = textureKeyAFound.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureKeyAFoundID);
	glBindTexture(GL_TEXTURE_2D, textureKeyAFoundID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	textureKeyAFound.freeImage(bitmap);

	std::uniform_real_distribution<float> distr01 =
			std::uniform_real_distribution<float>(0.0f, 1.0f);
	std::mt19937 generator;
	std::random_device rd;
	generator.seed(rd());
	int size = nParticlesFire * 2;
	std::vector<GLfloat> randData(size);
	for (int i = 0; i < randData.size(); i++) {
		randData[i] = distr01(generator);
	}

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_1D, texId);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, size);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, size, GL_RED, GL_FLOAT,
			randData.data());
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	shaderParticlesFire.setInt("Pass", 1);
	shaderParticlesFire.setInt("ParticleTex", 0);
	shaderParticlesFire.setInt("RandomTex", 1);
	shaderParticlesFire.setFloat("ParticleLifetime", particleLifetime);
	shaderParticlesFire.setFloat("ParticleSize", particleSize);
	shaderParticlesFire.setVectorFloat3("Accel",
			glm::value_ptr(glm::vec3(0.0f, 0.1f, 0.0f)));
	shaderParticlesFire.setVectorFloat3("Emitter",
			glm::value_ptr(glm::vec3(0.0f)));

	glm::mat3 basis;
	glm::vec3 u, v, n;
	v = glm::vec3(0, 1, 0);
	n = glm::cross(glm::vec3(1, 0, 0), v);
	if (glm::length(n) < 0.00001f) {
		n = glm::cross(glm::vec3(0, 1, 0), v);
	}
	u = glm::cross(v, n);
	basis[0] = glm::normalize(u);
	basis[1] = glm::normalize(v);
	basis[2] = glm::normalize(n);
	shaderParticlesFire.setMatrix3("EmitterBasis", 1, false,
			glm::value_ptr(basis));

	/*******************************************
	 * Inicializacion de los buffers de la fuente
	 *******************************************/
	initParticleBuffers();

	/*******************************************
	 * Inicializacion de los buffers del fuego
	 *******************************************/
	initParticleBuffersFire();

	/*******************************************
	 * Inicializacion del framebuffer para
	 * almacenar el buffer de profunidadad
	 *******************************************/
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
			SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
			depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*******************************************
	 * OpenAL init
	 *******************************************/
	configAudioListeners();

	// Se inicializa el modelo de texeles.
	modelText = new FontTypeRendering::FontTypeRendering(screenWidth,
			screenHeight);
	modelText->Initialize();
	modelText2 = new FontTypeRendering::FontTypeRendering(screenWidth,
		screenHeight);
	modelText2->Initialize();
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Shaders Delete
	shader.destroy();
	shaderMulLighting.destroy();
	shaderSkybox.destroy();
	shaderTerrain.destroy();
	shaderParticlesFountain.destroy();
	shaderParticlesFire.destroy();

	// Basic objects Delete
	skyboxSphere.destroy();
	boxCollider.destroy();
	sphereCollider.destroy();
	boxViewDepth.destroy();
	boxLightViewBox.destroy();

	cylinderShoot.destroy();

	// Terrains objects Delete
	terrain.destroy();

	// Custom objects Delete
	modelLamp1.destroy();
	modelLamp2.destroy();
	modelLampPost2.destroy();
	modelGrass.destroy();
	modelFountain.destroy();
	modelWallA.destroy();
	modelWallB.destroy();
	modelWallC.destroy();
	modelWallD.destroy();
	modelWallE.destroy();
	modelWallF.destroy();

	modelHAfront.destroy();
	modelHAleft.destroy();
	modelHAtop.destroy();
	modelHBback.destroy();
	modelHBfront.destroy();
	modelHBleft1.destroy();
	modelHBleft2.destroy();
	modelHBtop.destroy();
	modelHCfront.destroy();
	modelHCleft.destroy();
	modelHCtop.destroy();
	modelHDback.destroy();
	modelHDleft.destroy();
	modelHDright.destroy();
	modelHDtop.destroy();
	modelHEback.destroy();
	modelHEleft.destroy();
	modelHEright.destroy();
	modelHEtop.destroy();

	// Custom objects animate
	playerModelAnimate.destroy();
	//Zombies
	zombieModelAnimate1.destroy();
	zombieModelAnimate2.destroy();
	zombieModelAnimate3.destroy();
	zombieModelAnimate4.destroy();
	zombieModelAnimate5.destroy();
	zombieModelAnimate6.destroy();
	zombieModelAnimate7.destroy();

	keyModelAnimateA.destroy();
	keyModelAnimateB.destroy();
	modelRedDoor.destroy();

	// Textures Delete
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureCespedID);
	glDeleteTextures(1, &textureTerrainBackgroundID);
	glDeleteTextures(1, &textureTerrainRID);
	glDeleteTextures(1, &textureTerrainGID);
	glDeleteTextures(1, &textureTerrainBID);
	glDeleteTextures(1, &textureTerrainBlendMapID);
	glDeleteTextures(1, &textureParticleFountainID);
	glDeleteTextures(1, &textureParticleFireID);

	// Cube Maps Delete
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDeleteTextures(1, &skyboxTextureID);

	// Remove the buffer of the fountain particles
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &initVel);
	glDeleteBuffers(1, &startTime);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOParticles);

	// Remove the buffer of the fire particles
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(2, posBuf);
	glDeleteBuffers(2, velBuf);
	glDeleteBuffers(2, age);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glDeleteTransformFeedbacks(2, feedback);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOParticlesFire);
}

void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
		int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	distanceFromTarget -= yoffset;
	camera->setDistanceFromTarget(distanceFromTarget);
}

void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

void modelMovementXboxPlayer() {
	/*
		* Animations
		* 0 Idle ?
		* 1 Walk
		* 2 WalkBackwards
		* 3 InjuredWalking
		* 4 LeftSideWalk
		* 5 RightSideWalk
		* 6 Shoot
		* 7 Jump
		*
	*/
	if (glfwJoystickPresent(GLFW_JOYSTICK_1) == GL_TRUE) {
		int axesCout, buttonCount;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCout);
		const unsigned char* botones = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
		float moveSpeed = 0.3f, rotationSpeed = 0.8f;
		
		if (!iniciaPartida) {
			if (textureActivaID == textureInit1ID && botones[7] == GLFW_PRESS) {
				iniciaPartida = true;
			}
			if (textureActivaID == textureInit2ID && botones[7] == GLFW_PRESS) {
				exitApp = true;
			}

			if (!presionarOpcion && (botones[10] == GLFW_PRESS || botones[12] == GLFW_PRESS)) {
				presionarOpcion = true;
				if (textureActivaID == textureInit1ID)
					textureActivaID = textureInit2ID;
				else if (textureActivaID == textureInit2ID)
					textureActivaID = textureInit1ID;
			}
			else if (botones[10] == GLFW_RELEASE && botones[12] == GLFW_RELEASE) {
				presionarOpcion = false;
			}
		}
		else {
			//Xbox 360 

			//0.2 es un umbral de movimiento 
			// CONTROL MOVIMIENTO LS 
			if (fabs(axes[1]) > 0.2) {
				//en el codigo del profe, es -axes[1] pero en ese caso, estan invertidos los controles del joystick. Presionar hacia arriba en el joystick es ir hacia atrás, etc. 
				modelMatrixPlayer = glm::translate(modelMatrixPlayer, glm::vec3(0.0, 0.0, -axes[1] * moveSpeed));
			}
			if (axes[1] > 0.2) {
				animationIndexPlayer = 1;
			}
			else if (axes[1] < -0.2)
				animationIndexPlayer = 2;

			if (fabs(axes[0]) > 0.2) {
				modelMatrixPlayer = glm::rotate(modelMatrixPlayer, glm::radians(-axes[0] * rotationSpeed), glm::vec3(0.0, 1.0, 0.0));
				animationIndexPlayer = 1;
			}

			// CONTROL CAMARA RS 
			if (fabs(axes[2]) > 0.2) {
				camera->mouseMoveCamera(axes[2], 0, deltaTime);
			}
			if (fabs(axes[3]) > 0.2) {
				camera->mouseMoveCamera(0, axes[3], deltaTime);
			}

			//Shooting
			if (!isShooting) {
				if (axes[5] > 0.5) {
					isShooting = true;
					animationIndexPlayer = 6;
					//glfwSetTime(0.0f);
					currTimeShoot = glfwGetTime();
					deltaTimeShoot = 0.0f;
					lastTimeShoot = glfwGetTime();
				}
			}
			else{
				//Gunshot
				sourceGunPos[0] = modelMatrixPlayer[3].x;
				sourceGunPos[1] = modelMatrixPlayer[3].y;
				sourceGunPos[2] = modelMatrixPlayer[3].z;
				alSourcefv(source[1], AL_POSITION, sourceGunPos);
				currTimeShoot = glfwGetTime();
				deltaTimeShoot += currTimeShoot - lastTimeShoot;
				lastTimeShoot = currTimeShoot;
				if (deltaTimeShoot > 0.7) {
					isShooting = false;
					currTimeShoot = glfwGetTime();
					deltaTimeShoot = 0.0f;
					lastTimeShoot = glfwGetTime();
				}
			}
			/*std::cout << "L2 : " << axes[4] << std::endl;
			std::cout << "R2 : " << axes[5] << std::endl;*/
			// CONTROL BOTONES 
			/*
			* XBOX 360
			* 0		->		A
			* 1		->		B
			* 2		->		X
			* 3		->		Y
			* 4		->		L1
			* 5		->		R1
			* 6		->		Select
			* 7		->		Start
			* 8		->		L3
			* 9		->		R3
			* 10	->		PadUp
			* 11	->		PadR
			* 12	->		PadDown
			* 13	->		PadL
			*/
		

			if (botones[0] == GLFW_PRESS && !isJumpPlayer) {
				std::cout << "Se presiona A y saltamos" << buttonCount << std::endl;
				isJumpPlayer = true;
				startTimeJump = currTime;
				tmv = 0;
			}

			if (botones[6] == GLFW_PRESS) {
				std::cout << "Se presiono boton 6 " << buttonCount << std::endl;
			}
			if (botones[7] == GLFW_PRESS) {
				std::cout << "Se presiono boton 7 " << buttonCount << std::endl;
			}
			if (botones[8] == GLFW_PRESS) {
				std::cout << "Se presiono boton 8" << buttonCount << std::endl;
			}
			if (botones[9] == GLFW_PRESS) {
				std::cout << "Se presiono boton 9" << buttonCount << std::endl;
			}
			if (botones[10] == GLFW_PRESS) {
				std::cout << "Se presiono boton 10" << buttonCount << std::endl;
			}
			if (botones[11] == GLFW_PRESS) {
				std::cout << "Se presiono boton 11" << buttonCount << std::endl;
			}
			if (botones[12] == GLFW_PRESS) {
				std::cout << "Se presiono boton 12" << buttonCount << std::endl;
			}
			if (botones[13] == GLFW_PRESS) {
				std::cout << "Se presiono boton 13" << buttonCount << std::endl;
			}
			//select + start + L1 + R2 => salir de la app
			if (botones[6] == GLFW_PRESS && botones[7] == GLFW_PRESS && botones[4] == GLFW_PRESS && botones[5] == GLFW_PRESS)
				exitApp = true;
		}
	}
}

void playerMovementKeyboard() {
	//Rotate left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		modelMatrixPlayer = glm::rotate(modelMatrixPlayer, glm::radians(1.0f * 0.8f), glm::vec3(0, 1, 0));
		animationIndexPlayer = 0;
	}
	//Rotate right
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		modelMatrixPlayer = glm::rotate(modelMatrixPlayer, glm::radians(-1.0f * 0.8f), glm::vec3(0, 1, 0));
		animationIndexPlayer = 0;
	}
	//Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ) {
		modelMatrixPlayer = glm::translate(modelMatrixPlayer, glm::vec3(0, 0, -0.3));
		animationIndexPlayer = 1;
	}
	//Move backwards
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		modelMatrixPlayer = glm::translate(modelMatrixPlayer, glm::vec3(0, 0, 0.3));
		animationIndexPlayer = 2;
	}
	//Shoot
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		if (!isShooting) {
			isShooting = true;
			animationIndexPlayer = 6;
			glfwSetTime(0.0f);
		}
		else {
			std::cout << "Time !: " << glfwGetTime() << std::endl;
			if (glfwGetTime() > 0.7)
				isShooting = false;
		}
	}
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}

	if (!iniciaPartida) {
		bool presionarEnter = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS;
		if (textureActivaID == textureInit1ID && presionarEnter) {
			iniciaPartida = true;
		}
		if (textureActivaID == textureInit2ID && presionarEnter) {
			exitApp = true;
		}
		if (!presionarOpcion && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
			presionarOpcion = true;
			if (textureActivaID == textureInit1ID)
				textureActivaID = textureInit2ID;
			else if (textureActivaID == textureInit2ID)
				textureActivaID = textureInit1ID;
		}
		else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
			presionarOpcion = false;
		}
	}

	bool isJoystickPresent = false;

	if (glfwJoystickPresent(GLFW_JOYSTICK_1) == GL_TRUE) {
		//std::cout << "Esta presente el joystick" << std::endl;
		isJoystickPresent = true;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, 0.0, deltaTime);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		camera->mouseMoveCamera(0.0, offsetY, deltaTime);
	offsetX = 0;
	offsetY = 0;

	// Seleccionar modelo
	//if (enableCountSelected && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
	//	enableCountSelected = false;
	//	modelSelected++;
	//	if (modelSelected > 2)
	//		modelSelected = 0;
	//	if (modelSelected == 1)
	//		//fileName = "../animaciones/animation_dart_joints.txt";
	//	if (modelSelected == 2)
	//		//fileName = "../animaciones/animation_dart.txt";
	//	std::cout << "modelSelected:" << modelSelected << std::endl;
	//} else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
	//	enableCountSelected = true;
	
	bool keySpaceStatus = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
	if(!isJumpPlayer && keySpaceStatus){
		isJumpPlayer = true;
		startTimeJump = currTime;
		tmv = 0;
	}

	playerMovementKeyboard();
	if(isJoystickPresent)
		modelMovementXboxPlayer();

	glfwPollEvents();
	return continueApplication;
}

void stateZombieGeneral(int zN) {
	//zN zombieNumber
	//Caminar si rayo colisiona con player
	float zombieSpeed[7] = {1,1,1,1,1,1,1};
	if (isZombieInRange[zN] && isZombieAlive[zN] && !isZombieShooted[zN]) {
		switch (zN) {
		case 0: animationIndexZombie1 = 1;
			break;
		case 1: animationIndexZombie2 = 1;
			break;
		case 2: animationIndexZombie3 = 1;
			break;
		case 3: animationIndexZombie4 = 1;
			break;
		case 4: animationIndexZombie5 = 1;
			break;
		case 5: animationIndexZombie6 = 1;
			break;
		case 6: animationIndexZombie7 = 1;
			break;
		}

		if (zombieLife[zN] == 3)
			zombieSpeed[zN] = 1;
		else if (zombieLife[zN] == 2)
			zombieSpeed[zN] = 2.0;
		else
			zombieSpeed[zN] = 3.5;
		if (zombieLife[zN] == 0) {
			switch (zN) {
			case 1:
				modelMatrixZombie1 = glm::translate(modelMatrixZombie1, glm::vec3(0, 0, 0));
				break;
			case 2: 
				modelMatrixZombie2 = glm::translate(modelMatrixZombie2, glm::vec3(0, 0, 0));
				break;
			case 3: 
				modelMatrixZombie3 = glm::translate(modelMatrixZombie3, glm::vec3(0, 0, 0));
				break;
			case 4: 
				modelMatrixZombie4 = glm::translate(modelMatrixZombie4, glm::vec3(0, 0, 0));
				break;
			case 5: 
				modelMatrixZombie5 = glm::translate(modelMatrixZombie5, glm::vec3(0, 0, 0));
				break;
			case 6: 
				modelMatrixZombie6 = glm::translate(modelMatrixZombie6, glm::vec3(0, 0, 0));
				break;
			case 7: 
				modelMatrixZombie7 = glm::translate(modelMatrixZombie7, glm::vec3(0, 0, 0));
				break;
			}


		}
		else {
			switch (zN) {
			case 0: modelMatrixZombie1 = glm::translate(modelMatrixZombie1, glm::vec3(0, 0, 0.1f * zombieSpeed[zN]));
				break;
			case 1: modelMatrixZombie2 = glm::translate(modelMatrixZombie2, glm::vec3(-0.1f * zombieSpeed[zN], 0, 0));
				break;
			case 2: modelMatrixZombie3 = glm::translate(modelMatrixZombie3, glm::vec3(0, 0, -0.1f * zombieSpeed[zN]));
				break;
			case 3: modelMatrixZombie4 = glm::translate(modelMatrixZombie4, glm::vec3(0, 0, 0.1f * zombieSpeed[zN]));
				break;
			case 4: modelMatrixZombie5 = glm::translate(modelMatrixZombie5, glm::vec3(-0.1f * zombieSpeed[zN], 0, 0.1f * zombieSpeed[zN]));
				break;
			case 5: modelMatrixZombie6 = glm::translate(modelMatrixZombie6, glm::vec3(0, 0, 0.1f * zombieSpeed[zN]));
				break;
			case 6: modelMatrixZombie7 = glm::translate(modelMatrixZombie7, glm::vec3(0, 0, 0.1f * zombieSpeed[zN]));
				break;
			}
		}
			
	}

	//Animacion si el zombie ha sido disparado
	if (isZombieShooted[zN] == true && isZombieAlive[zN] == true && zombieLife[zN] > 0) {
		currTimeShooted = glfwGetTime();
		deltaTimeShooted += currTimeShooted - lastTimeShooted;
		lastTimeShooted = currTimeShooted;
		//Se supone que es 1 segundo por cantidad de frames que dura en blender / 60 fps
		if (deltaTimeShooted < 1 * (62.0 / 60.0)) {
			switch (zN) {
			case 0: animationIndexZombie1 = 4;
				break;
			case 1: animationIndexZombie2 = 4;
				break;
			case 2: animationIndexZombie3 = 4;
				break;
			case 3: animationIndexZombie4 = 4;
				break;
			case 4: animationIndexZombie5 = 4;
				break;
			case 5: animationIndexZombie6 = 4;
				break;
			case 6: animationIndexZombie7 = 4;
				break;
			}
		}
		else {
			isZombieShooted[zN] = false;
			zombieLife[zN]--;
			currTimeShooted = glfwGetTime();
			lastTimeShooted = glfwGetTime();
			deltaTimeShooted = 0;
			if (zombieLife[zN] <= 0) {
				currTimeDying = glfwGetTime();
				deltaTimeDying = 0;
				lastTimeDying = glfwGetTime();
				isZombieDying[zN] = true;
			}
		}
	}

	//Animacion si zombie ya no tiene vida
	if (isZombieDying[zN] == true && isZombieAlive[zN] == true) {
		zombieSpeed[zN] = 0;

		currTimeDying = glfwGetTime();
		deltaTimeDying += currTimeDying - lastTimeDying;
		lastTimeDying = currTimeDying;
		if (deltaTimeDying <= 1 * (101.0 / 60.0)) {
			switch (zN) {
			case 0: animationIndexZombie1 = 2;
				break;
			case 1: animationIndexZombie2 = 2;
				break;
			case 2: animationIndexZombie3 = 2;
				break;
			case 3: animationIndexZombie4 = 2;
				break;
			case 4: animationIndexZombie5 = 2;
				break;
			case 5: animationIndexZombie6 = 2;
				break;
			case 6: animationIndexZombie7 = 2;
				break;
			}
		}
		else {
			isZombieDying[zN] = false;
			isZombieAlive[zN] = false;
			currTimeDying = glfwGetTime();
			deltaTimeDying = 0;
			lastTimeDying = glfwGetTime();

			switch (zN) {
			case 0: animationIndexZombie1 = 6;
				std::cout << "Zombie1 eliminated" << std::endl;
				collidersOBB.erase("Zombie1");
				break;
			case 1: animationIndexZombie2 = 6;
				std::cout << "Zombie2 eliminated" << std::endl;
				collidersOBB.erase("Zombie2");
				break;
			case 2: animationIndexZombie3 = 6;
				std::cout << "Zombie3 eliminated" << std::endl;
				collidersOBB.erase("Zombie3");
				break;
			case 3: animationIndexZombie4 = 6;
				std::cout << "Zombie4 eliminated" << std::endl;
				collidersOBB.erase("Zombie4");
				break;
			case 4: animationIndexZombie5 = 6;
				std::cout << "Zombie5 eliminated" << std::endl;
				collidersOBB.erase("Zombie5");
				break;
			case 5: animationIndexZombie6 = 6;
				std::cout << "Zombie6 eliminated" << std::endl;
				collidersOBB.erase("Zombie6");
				break;
			case 6: animationIndexZombie7 = 6;
				std::cout << "Zombie7 eliminated" << std::endl;
				collidersOBB.erase("Zombie7");
				break;
			}
		}
	}
}


void applicationLoop() {
	bool psi = true;

	glm::mat4 view;
	glm::vec3 axis;
	glm::vec3 target;
	glm::vec3 positionPlayer;
	float angleTarget;

	//Posicion Inicial
	modelMatrixPlayer = glm::translate(modelMatrixPlayer, glm::vec3(-90.0f, 0.0f, 85.0f));
	//Posicion custom centro del mapa
	//modelMatrixPlayer = glm::translate(modelMatrixPlayer, glm::vec3(0.0f, 0.0f, 0.0f));

	// Walls ModelMatrixWall
	//Translate into correct place
	modelMatrixWallA = glm::scale(modelMatrixWallA, escalamientoWalls);
	modelMatrixWallA = glm::translate(modelMatrixWallA, glm::vec3(-12.655f, 0.0f, 0.0f));
	modelMatrixWallA[3][1] = terrain.getHeightTerrain(modelMatrixWallA[3][0], modelMatrixWallA[3][2]);

	modelMatrixWallB = glm::scale(modelMatrixWallB, escalamientoWalls);
	modelMatrixWallB = glm::translate(modelMatrixWallB, glm::vec3(0.0f, 0.0f, -12.655f));
	modelMatrixWallB[3][1] = terrain.getHeightTerrain(modelMatrixWallB[3][0], modelMatrixWallB[3][2]);

	modelMatrixWallC = glm::scale(modelMatrixWallC, escalamientoWalls);
	modelMatrixWallC = glm::translate(modelMatrixWallC, glm::vec3(12.655f, 0.0f, 0.0f));
	modelMatrixWallC[3][1] = terrain.getHeightTerrain(modelMatrixWallC[3][0], modelMatrixWallC[3][2]);

	modelMatrixWallD = glm::scale(modelMatrixWallD, escalamientoWalls);
	modelMatrixWallD = glm::translate(modelMatrixWallD, glm::vec3(0.0f, 0.0f, 12.655f));
	modelMatrixWallD[3][1] = terrain.getHeightTerrain(modelMatrixWallD[3][0], modelMatrixWallD[3][2]);

	modelMatrixWallE = glm::scale(modelMatrixWallE, escalamientoWalls);
	modelMatrixWallE = glm::translate(modelMatrixWallE, glm::vec3(-8.49f, 0.0f, 1.26f));
	modelMatrixWallE[3][1] = terrain.getHeightTerrain(modelMatrixWallE[3][0], modelMatrixWallE[3][2]);

	modelMatrixWallF = glm::scale(modelMatrixWallF, escalamientoWalls);
	modelMatrixWallF = glm::translate(modelMatrixWallF, glm::vec3(6.64f, 0.0f, -5.08f));
	modelMatrixWallF[3][1] = terrain.getHeightTerrain(modelMatrixWallF[3][0], modelMatrixWallF[3][2]);

	//Posicionamiento de Houses
	//House A houseAMatrix = { front, left, top };
	houseAMatrix[0] = glm::scale(houseAMatrix[0], escalamientoWalls);
	houseAMatrix[0] = glm::translate(houseAMatrix[0], glm::vec3(7.46f, 0, -7.46f));
	houseAMatrix[0][3][1] = terrain.getHeightTerrain(houseAMatrix[0][3][0], houseAMatrix[0][3][2]);

	houseAMatrix[1] = glm::scale(houseAMatrix[1], escalamientoWalls);
	houseAMatrix[1] = glm::translate(houseAMatrix[1], glm::vec3(4.48f, 0, -9.83f));
	houseAMatrix[1][3][1] = terrain.getHeightTerrain(houseAMatrix[1][3][0], houseAMatrix[1][3][2]);

	houseAMatrix[2] = glm::scale(houseAMatrix[2], escalamientoWalls);
	houseAMatrix[2] = glm::translate(houseAMatrix[2], glm::vec3(8.32f, 4, -9.84f));
	houseAMatrix[2] = glm::rotate(houseAMatrix[2], glm::radians(-90.0f), glm::vec3(1, 0, 0));

	//HouseB houseBMatrix = { back, front, left1, left2, top };
	houseBMatrix[0] = glm::scale(houseBMatrix[0], escalamientoWalls);
	houseBMatrix[0] = glm::translate(houseBMatrix[0], glm::vec3(8.17f, 0, -2.28f));
	houseBMatrix[0][3][1] = terrain.getHeightTerrain(houseBMatrix[0][3][0], houseBMatrix[0][3][2]);

	houseBMatrix[1] = glm::scale(houseBMatrix[1], escalamientoWalls);
	houseBMatrix[1] = glm::translate(houseBMatrix[1], glm::vec3(8.17f, 0, 9.06f));
	houseBMatrix[1][3][1] = terrain.getHeightTerrain(houseBMatrix[1][3][0], houseBMatrix[1][3][2]);

	houseBMatrix[2] = glm::scale(houseBMatrix[2], escalamientoWalls);
	houseBMatrix[2] = glm::translate(houseBMatrix[2], glm::vec3(4.17f, 0.0f, 0.75f));
	houseBMatrix[2][3][1] = terrain.getHeightTerrain(houseBMatrix[2][3][0], houseBMatrix[2][3][2]);

	houseBMatrix[3] = glm::scale(houseBMatrix[3], escalamientoWalls);
	houseBMatrix[3] = glm::translate(houseBMatrix[3], glm::vec3(4.17f, 0.0f, 7.146f));
	houseBMatrix[3][3][1] = terrain.getHeightTerrain(houseBMatrix[3][3][0], houseBMatrix[3][3][2]);

	houseBMatrix[4] = glm::scale(houseBMatrix[4], escalamientoWalls);
	houseBMatrix[4] = glm::translate(houseBMatrix[4], glm::vec3(8.1637f, 4, 3.4089f));
	houseBMatrix[4] = glm::rotate(houseBMatrix[4], glm::radians(-90.0f), glm::vec3(1, 0, 0));

	//HouseC 	houseCMatrix = { front, left, top };
	houseCMatrix[0] = glm::scale(houseCMatrix[0], escalamientoWalls);
	houseCMatrix[0] = glm::translate(houseCMatrix[0], glm::vec3(-1.3614f, 0, -7.3674f));
	houseCMatrix[0][3][1] = terrain.getHeightTerrain(houseCMatrix[0][3][0], houseCMatrix[0][3][2]);

	houseCMatrix[1] = glm::scale(houseCMatrix[1], escalamientoWalls);
	houseCMatrix[1] = glm::translate(houseCMatrix[1], glm::vec3(-3.9375f, 0, -9.7709f));
	houseCMatrix[1][3][1] = terrain.getHeightTerrain(houseCMatrix[1][3][0], houseCMatrix[1][3][2]);

	houseCMatrix[2] = glm::scale(houseCMatrix[2], escalamientoWalls);
	houseCMatrix[2] = glm::translate(houseCMatrix[2], glm::vec3(-1.6393f, 4, -9.7861f));
	houseCMatrix[2] = glm::rotate(houseCMatrix[2], glm::radians(-90.0f), glm::vec3(1, 0, 0));

	//HouseD	houseDMatrix = { back, left, right, top };
	houseDMatrix[0] = glm::scale(houseDMatrix[0], escalamientoWalls);
	houseDMatrix[0] = glm::translate(houseDMatrix[0], glm::vec3(-1.626f, 0, -5.1438f));
	houseDMatrix[0][3][1] = terrain.getHeightTerrain(houseDMatrix[0][3][0], houseDMatrix[0][3][2]);
	
	houseDMatrix[1] = glm::scale(houseDMatrix[1], escalamientoWalls);
	houseDMatrix[1] = glm::translate(houseDMatrix[1], glm::vec3(-3.724f, 0, -2.1339f));
	houseDMatrix[1][3][1] = terrain.getHeightTerrain(houseDMatrix[1][3][0], houseDMatrix[1][3][2]);

	houseDMatrix[2] = glm::scale(houseDMatrix[2], escalamientoWalls);
	houseDMatrix[2] = glm::translate(houseDMatrix[2], glm::vec3(0.48886f, 0, -2.1339f));
	houseDMatrix[2][3][1] = terrain.getHeightTerrain(houseDMatrix[2][3][0], houseDMatrix[2][3][2]);

	houseDMatrix[3] = glm::scale(houseDMatrix[3], escalamientoWalls);
	houseDMatrix[3] = glm::translate(houseDMatrix[3], glm::vec3(-1.6109f, 4.0f, -2.4316f));
	houseDMatrix[3] = glm::rotate(houseDMatrix[3], glm::radians(-90.0f), glm::vec3(1, 0, 0));

	//HouseE	houseEMatrix = { back, left, right, top };
	houseEMatrix[0] = glm::scale(houseEMatrix[0], escalamientoWalls);
	houseEMatrix[0] = glm::translate(houseEMatrix[0], glm::vec3(-1.5f, 0, 3.6823f));
	houseEMatrix[0][3][1] = terrain.getHeightTerrain(houseEMatrix[0][3][0], houseEMatrix[0][3][2]);

	houseEMatrix[1] = glm::scale(houseEMatrix[1], escalamientoWalls);
	houseEMatrix[1] = glm::translate(houseEMatrix[1], glm::vec3(-3.6023f, 0, 7.4836f));
	houseEMatrix[1][3][1] = terrain.getHeightTerrain(houseEMatrix[1][3][0], houseEMatrix[1][3][2]);

	houseEMatrix[2] = glm::scale(houseEMatrix[2], escalamientoWalls);
	houseEMatrix[2] = glm::translate(houseEMatrix[2], glm::vec3(0.6229f, 0, 8.242f));
	houseEMatrix[2][3][1] = terrain.getHeightTerrain(houseEMatrix[2][3][0], houseEMatrix[2][3][2]);

	houseEMatrix[3] = glm::scale(houseEMatrix[3], escalamientoWalls);
	houseEMatrix[3] = glm::translate(houseEMatrix[3], glm::vec3(-1.4875f, 4, 7.9188f));
	houseEMatrix[3] = glm::rotate(houseEMatrix[3], glm::radians(-90.0f), glm::vec3(1, 0, 0));
	//Fountain
	modelMatrixFountain = glm::translate(modelMatrixFountain,
			glm::vec3(5.0, 0.0, -40.0));
	modelMatrixFountain[3][1] = terrain.getHeightTerrain(
			modelMatrixFountain[3][0], modelMatrixFountain[3][2]) + 0.2;
	modelMatrixFountain = glm::scale(modelMatrixFountain,
			glm::vec3(10.0f, 10.0f, 10.0f));

	modelMatrixZombie1 = glm::translate(modelMatrixZombie1, glm::vec3(-90.0f, 0.0f, -75.0f));
	modelMatrixZombie2 = glm::translate(modelMatrixZombie2, glm::vec3(-8.0f, 0.0f, -49.28f));
	modelMatrixZombie3 = glm::translate(modelMatrixZombie3, glm::vec3(-44.96f, 0.0f, 92.32f));
	modelMatrixZombie4 = glm::translate(modelMatrixZombie4, glm::vec3(-12.0f, 0.0f, 36.88f));
	modelMatrixZombie5 = glm::translate(modelMatrixZombie5, glm::vec3(87.12f, 0.0f, -5.36f));
	modelMatrixZombie6 = glm::translate(modelMatrixZombie6, glm::vec3(20.48f, 0.0f, -92.16f));
	modelMatrixZombie7 = glm::translate(modelMatrixZombie7, glm::vec3(89.07f, 0.0f, -92.9f));

	//Keys random location of 5
	keySlotSelected = dist5(rng);
	switch (keySlotSelected) {
	case 1:
		modelMatrixKeyA = glm::translate(modelMatrixKeyA, KeyAPos);
		break;
	case 2:
		modelMatrixKeyA = glm::translate(modelMatrixKeyA, KeyBPos);
		break;
	case 3:
		modelMatrixKeyA = glm::translate(modelMatrixKeyA, KeyCPos);
		break;
	case 4:
		modelMatrixKeyA = glm::translate(modelMatrixKeyA, KeyDPos);
		break;
	case 5:
		modelMatrixKeyA = glm::translate(modelMatrixKeyA, KeyEPos);
		break;
	}
	keySlotSelectedB = dist5(rng);
	while (keySlotSelectedB == keySlotSelected) {
		keySlotSelectedB = dist5(rng);
	}

	switch (keySlotSelectedB) {
	case 1:
		modelMatrixKeyB = glm::translate(modelMatrixKeyB, KeyAPos);
		break;
	case 2:
		modelMatrixKeyB = glm::translate(modelMatrixKeyB, KeyBPos);
		break;
	case 3:
		modelMatrixKeyB = glm::translate(modelMatrixKeyB, KeyCPos);
		break;
	case 4:
		modelMatrixKeyB = glm::translate(modelMatrixKeyB, KeyDPos);
		break;
	case 5:
		modelMatrixKeyB = glm::translate(modelMatrixKeyB, KeyEPos);
		break;
	}

	modelMatrixKeyA = glm::scale(modelMatrixKeyA, scaleKey);
	modelMatrixKeyB = glm::scale(modelMatrixKeyB, scaleKey);

	//Red Door
	modelMatrixRedDoor = glm::scale(modelMatrixRedDoor, scaleRedDoor);
	modelMatrixRedDoor = glm::translate(modelMatrixRedDoor, glm::vec3(11.527f, 0, 11.0f));
	//modelMatrixRedDoor = glm::rotate(modelMatrixRedDoor, glm::radians(180.0f), glm::vec3(0, 1, 0));

	lastTime = TimeManager::Instance().GetTime();

	// Time for the particles animation
	currTimeParticlesAnimation = lastTime;
	lastTimeParticlesAnimation = lastTime;

	currTimeParticlesAnimationFire = lastTime;
	lastTimeParticlesAnimationFire = lastTime;

	glm::vec3 lightPos = glm::vec3(10.0, 10.0, 0.0);

	shadowBox = new ShadowBox(-lightPos, camera.get(), 100.0f, 0.1f, 60.0f);

	textureActivaID = textureInit1ID;
	textureKeysActivaID = textureKeysEmptyID;

	while (psi) {
		currTime = TimeManager::Instance().GetTime();
		if (currTime - lastTime < 0.016666667) {
			glfwPollEvents();
			continue;
		}
		lastTime = currTime;
		TimeManager::Instance().CalculateFrameRate(true);
		deltaTime = TimeManager::Instance().DeltaTime;
		psi = processInput(true);

		std::map<std::string, bool> collisionDetection;

		
		glm::mat4 projection = glm::perspective(glm::radians(60.0f),
				(float) screenWidth / (float) screenHeight, 0.1f, 300.0f);

		
		shadowBox->update(screenWidth, screenHeight);
		glm::vec3 centerBox = shadowBox->getCenter();

		// Projection light shadow mapping
		glm::mat4 lightProjection = glm::mat4(1.0f), lightView = glm::mat4(
				1.0f);
		glm::mat4 lightSpaceMatrix;

		lightProjection[0][0] = 2.0f / shadowBox->getWidth();
		lightProjection[1][1] = 2.0f / shadowBox->getHeight();
		lightProjection[2][2] = -2.0f / shadowBox->getLength();
		lightProjection[3][3] = 1.0f;

		lightView = glm::lookAt(centerBox,
				centerBox + glm::normalize(-lightPos),
				glm::vec3(0.0, 1.0, 0.0));

		lightSpaceMatrix = lightProjection * lightView;
		shaderDepth.setMatrix4("lightSpaceMatrix", 1, false,
				glm::value_ptr(lightSpaceMatrix));

		// Settea la matriz de vista y projection al shader con solo color
		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con skybox
		shaderSkybox.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderSkybox.setMatrix4("view", 1, false,
				glm::value_ptr(glm::mat4(glm::mat3(view))));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderMulLighting.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderMulLighting.setMatrix4("view", 1, false, glm::value_ptr(view));
		shaderMulLighting.setMatrix4("lightSpaceMatrix", 1, false,
				glm::value_ptr(lightSpaceMatrix));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderTerrain.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderTerrain.setMatrix4("view", 1, false, glm::value_ptr(view));
		shaderTerrain.setMatrix4("lightSpaceMatrix", 1, false,
				glm::value_ptr(lightSpaceMatrix));
		// Settea la matriz de vista y projection al shader para el fountain
		shaderParticlesFountain.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderParticlesFountain.setMatrix4("view", 1, false,
				glm::value_ptr(view));
		// Settea la matriz de vista y projection al shader para el fuego
		shaderParticlesFire.setInt("Pass", 2);
		shaderParticlesFire.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderParticlesFire.setMatrix4("view", 1, false, glm::value_ptr(view));

		/*******************************************
		 * Propiedades de neblina
		 *******************************************/
		shaderMulLighting.setVectorFloat3("fogColor",
				glm::value_ptr(glm::vec3(0.4, 0.4, 0.3)));
		/*shaderTerrain.setVectorFloat3("fogColor",
				glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));*/
		shaderTerrain.setVectorFloat3("fogColor",
				glm::value_ptr(glm::vec3(0.3, 0.3, 0.2)));
		shaderSkybox.setVectorFloat3("fogColor",
				glm::value_ptr(glm::vec3(0.7, 0.7, 0.6)));

		/*******************************************
		 * Propiedades Luz direccional
		 *******************************************/
		shaderMulLighting.setVectorFloat3("viewPos",
				glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient",
				glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse",
				glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular",
				glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction",
				glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		/*******************************************
		 * Propiedades Luz direccional Terrain
		 *******************************************/
		shaderTerrain.setVectorFloat3("viewPos",
				glm::value_ptr(camera->getPosition()));
		shaderTerrain.setVectorFloat3("directionalLight.light.ambient",
				glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderTerrain.setVectorFloat3("directionalLight.light.diffuse",
				glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderTerrain.setVectorFloat3("directionalLight.light.specular",
				glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderTerrain.setVectorFloat3("directionalLight.direction",
				glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		/*******************************************
		 * Propiedades SpotLights
		 *******************************************/
		glm::vec3 spotPosition = glm::vec3( modelMatrixHeli * glm::vec4(0.32437, 0.226053, 1.79149, 1.0));
		shaderMulLighting.setInt("spotLightCount", 1);
		shaderTerrain.setInt("spotLightCount", 1);
		shaderMulLighting.setVectorFloat3("spotLights[0].light.ambient",
				glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.diffuse",
				glm::value_ptr(glm::vec3(0.2, 0.3, 0.2)));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.specular",
				glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderMulLighting.setVectorFloat3("spotLights[0].position",glm::value_ptr(spotPosition));
		shaderMulLighting.setVectorFloat3("spotLights[0].direction",
				glm::value_ptr(glm::vec3(0, -1, 0)));
		shaderMulLighting.setFloat("spotLights[0].constant", 1.0);
		shaderMulLighting.setFloat("spotLights[0].linear", 0.074);
		shaderMulLighting.setFloat("spotLights[0].quadratic", 0.03);
		shaderMulLighting.setFloat("spotLights[0].cutOff",
				cos(glm::radians(12.5f)));
		shaderMulLighting.setFloat("spotLights[0].outerCutOff",
				cos(glm::radians(15.0f)));
		shaderTerrain.setVectorFloat3("spotLights[0].light.ambient",
				glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		shaderTerrain.setVectorFloat3("spotLights[0].light.diffuse",
				glm::value_ptr(glm::vec3(0.2, 0.3, 0.2)));
		shaderTerrain.setVectorFloat3("spotLights[0].light.specular",
				glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderTerrain.setVectorFloat3("spotLights[0].position",
				glm::value_ptr(spotPosition));
		shaderTerrain.setVectorFloat3("spotLights[0].direction",
				glm::value_ptr(glm::vec3(0, -1, 0)));
		shaderTerrain.setFloat("spotLights[0].constant", 1.0);
		shaderTerrain.setFloat("spotLights[0].linear", 0.074);
		shaderTerrain.setFloat("spotLights[0].quadratic", 0.03);
		shaderTerrain.setFloat("spotLights[0].cutOff",
				cos(glm::radians(12.5f)));
		shaderTerrain.setFloat("spotLights[0].outerCutOff",
				cos(glm::radians(15.0f)));

		/*******************************************
		 * Propiedades PointLights
		 *******************************************/
		shaderMulLighting.setInt("pointLightCount",
				lamp1Position.size() + lamp2Orientation.size());
		shaderTerrain.setInt("pointLightCount",
				lamp1Position.size() + lamp2Orientation.size());
		for (int i = 0; i < lamp1Position.size(); i++) {
			glm::mat4 matrixAdjustLamp = glm::mat4(1.0f);
			matrixAdjustLamp = glm::translate(matrixAdjustLamp,
					lamp1Position[i]);
			matrixAdjustLamp = glm::rotate(matrixAdjustLamp,
					glm::radians(lamp1Orientation[i]), glm::vec3(0, 1, 0));
			matrixAdjustLamp = glm::scale(matrixAdjustLamp,
					glm::vec3(0.5, 0.5, 0.5));
			matrixAdjustLamp = glm::translate(matrixAdjustLamp,
					glm::vec3(0, 10.3585, 0));
			glm::vec3 lampPosition = glm::vec3(matrixAdjustLamp[3]);
			shaderMulLighting.setVectorFloat3(
					"pointLights[" + std::to_string(i) + "].light.ambient",
					glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderMulLighting.setVectorFloat3(
					"pointLights[" + std::to_string(i) + "].light.diffuse",
					glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
			shaderMulLighting.setVectorFloat3(
					"pointLights[" + std::to_string(i) + "].light.specular",
					glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderMulLighting.setVectorFloat3(
					"pointLights[" + std::to_string(i) + "].position",
					glm::value_ptr(lampPosition));
			shaderMulLighting.setFloat(
					"pointLights[" + std::to_string(i) + "].constant", 1.0);
			shaderMulLighting.setFloat(
					"pointLights[" + std::to_string(i) + "].linear", 0.09);
			shaderMulLighting.setFloat(
					"pointLights[" + std::to_string(i) + "].quadratic", 0.01);
			shaderTerrain.setVectorFloat3(
					"pointLights[" + std::to_string(i) + "].light.ambient",
					glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderTerrain.setVectorFloat3(
					"pointLights[" + std::to_string(i) + "].light.diffuse",
					glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
			shaderTerrain.setVectorFloat3(
					"pointLights[" + std::to_string(i) + "].light.specular",
					glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderTerrain.setVectorFloat3(
					"pointLights[" + std::to_string(i) + "].position",
					glm::value_ptr(lampPosition));
			shaderTerrain.setFloat(
					"pointLights[" + std::to_string(i) + "].constant", 1.0);
			shaderTerrain.setFloat(
					"pointLights[" + std::to_string(i) + "].linear", 0.09);
			shaderTerrain.setFloat(
					"pointLights[" + std::to_string(i) + "].quadratic", 0.02);
		}
		for (int i = 0; i < lamp2Position.size(); i++) {
			glm::mat4 matrixAdjustLamp = glm::mat4(1.0f);
			matrixAdjustLamp = glm::translate(matrixAdjustLamp, lamp2Position[i]);
			matrixAdjustLamp = glm::rotate(matrixAdjustLamp, glm::radians(lamp2Orientation[i]), glm::vec3(0, 1, 0));
			matrixAdjustLamp = glm::scale(matrixAdjustLamp, glm::vec3(1.0, 1.0, 1.0));
			matrixAdjustLamp = glm::translate(matrixAdjustLamp, glm::vec3(0.759521, 5.00174, 0));
			glm::vec3 lampPosition = glm::vec3(matrixAdjustLamp[3]);
			shaderMulLighting.setVectorFloat3(
					"pointLights[" + std::to_string(lamp1Position.size() + i)
							+ "].light.ambient",
					glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderMulLighting.setVectorFloat3(
					"pointLights[" + std::to_string(lamp1Position.size() + i)
							+ "].light.diffuse",
					glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
			shaderMulLighting.setVectorFloat3(
					"pointLights[" + std::to_string(lamp1Position.size() + i)
							+ "].light.specular",
					glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderMulLighting.setVectorFloat3(
					"pointLights[" + std::to_string(lamp1Position.size() + i)
							+ "].position", glm::value_ptr(lampPosition));
			shaderMulLighting.setFloat(
					"pointLights[" + std::to_string(lamp1Position.size() + i)
							+ "].constant", 1.0);
			shaderMulLighting.setFloat(
					"pointLights[" + std::to_string(lamp1Position.size() + i)
							+ "].linear", 0.09);
			shaderMulLighting.setFloat(
					"pointLights[" + std::to_string(lamp1Position.size() + i)
							+ "].quadratic", 0.01);
			shaderTerrain.setVectorFloat3(
					"pointLights[" + std::to_string(lamp1Position.size() + i)
							+ "].light.ambient",
					glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderTerrain.setVectorFloat3(
					"pointLights[" + std::to_string(lamp1Position.size() + i)
							+ "].light.diffuse",
					glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
			shaderTerrain.setVectorFloat3(
					"pointLights[" + std::to_string(lamp1Position.size() + i)
							+ "].light.specular",
					glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderTerrain.setVectorFloat3(
					"pointLights[" + std::to_string(lamp1Position.size() + i)
							+ "].position", glm::value_ptr(lampPosition));
			shaderTerrain.setFloat(
					"pointLights[" + std::to_string(lamp1Position.size() + i)
							+ "].constant", 1.0);
			shaderTerrain.setFloat(
					"pointLights[" + std::to_string(lamp1Position.size() + i)
							+ "].linear", 0.09);
			shaderTerrain.setFloat(
					"pointLights[" + std::to_string(lamp1Position.size() + i)
							+ "].quadratic", 0.02);
		}

		if (!iniciaPartida) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0, 0, screenWidth, screenHeight);
			shaderTexture.setMatrix4("projection", 1, false, glm::value_ptr(glm::mat4(1.0)));
			shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(1.0)));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureActivaID);
			shaderTexture.setInt("outTexture", 0);
			boxIntro.render();
			glfwSwapBuffers(window);
			continue;
		}

		/*******************************************
		 * 1.- We render the depth buffer
		 *******************************************/
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// render scene from light's point of view
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glCullFace(GL_FRONT);
		prepareDepthScene();
		renderScene(false);
		//glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		/*******************************************
		 * Debug to view the texture view map
		 *******************************************/
		// reset viewport
		/*glViewport(0, 0, screenWidth, screenHeight);
		 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		 // render Depth map to quad for visual debugging
		 shaderViewDepth.setMatrix4("projection", 1, false, glm::value_ptr(glm::mat4(1.0)));
		 shaderViewDepth.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(1.0)));
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, depthMap);
		 boxViewDepth.setScale(glm::vec3(2.0, 2.0, 1.0));
		 boxViewDepth.render();*/

		/*******************************************
		 * 2.- We render the normal objects
		 *******************************************/
		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		prepareScene();
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		shaderMulLighting.setInt("shadowMap", 10);
		shaderTerrain.setInt("shadowMap", 10);
		/*******************************************
		 * Skybox
		 *******************************************/
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderSkybox.setFloat("skybox", 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		glActiveTexture(GL_TEXTURE0);
		skyboxSphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);
		renderScene();

		//Render UI
		shaderTexture.setMatrix4("projection", 1, false, glm::value_ptr(glm::mat4(1.0)));
		shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(1.0)));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureKeysActivaID);
		shaderTexture.setInt("outTexture", 0);
		boxIntro.render();
		glBindTexture(GL_TEXTURE_2D, 0);

		/*******************************************
		 * Debug to box light box
		 *******************************************/
		/*glm::vec3 front = glm::normalize(-lightPos);
		 glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), front));
		 glm::vec3 up = glm::normalize(glm::cross(front, right));
		 glDisable(GL_CULL_FACE);
		 glm::mat4 boxViewTransform = glm::mat4(1.0f);
		 boxViewTransform = glm::translate(boxViewTransform, centerBox);
		 boxViewTransform[0] = glm::vec4(right, 0.0);
		 boxViewTransform[1] = glm::vec4(up, 0.0);
		 boxViewTransform[2] = glm::vec4(front, 0.0);
		 boxViewTransform = glm::scale(boxViewTransform, glm::vec3(shadowBox->getWidth(), shadowBox->getHeight(), shadowBox->getLength()));
		 boxLightViewBox.enableWireMode();
		 boxLightViewBox.render(boxViewTransform);
		 glEnable(GL_CULL_FACE);*/

		/*******************************************
		 * Creacion de colliders
		 * IMPORTANT do this before interpolations
		 *******************************************/
		//Collider de Walls
		glm::mat4 modelMatrixColliderWallA = glm::mat4(modelMatrixWallA);
		AbstractModel::OBB wallACollider;
		wallACollider.u = glm::quat_cast(modelMatrixWallA);
		modelMatrixColliderWallA = glm::scale(modelMatrixColliderWallA, escalamientoWalls);
		modelMatrixColliderWallA = glm::translate(modelMatrixColliderWallA, modelWallA.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		wallACollider.c = glm::vec3(modelMatrixColliderWallA[3]);
		wallACollider.e = modelWallA.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "wallA", wallACollider, modelMatrixWallA);

		glm::mat4 modelMatrixColliderWallB = glm::mat4(modelMatrixWallB);
		AbstractModel::OBB wallBCollider;
		wallBCollider.u = glm::quat_cast(modelMatrixWallB);
		modelMatrixColliderWallB = glm::scale(modelMatrixColliderWallB, escalamientoWalls);
		modelMatrixColliderWallB = glm::translate(modelMatrixColliderWallB, modelWallB.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		wallBCollider.c = glm::vec3(modelMatrixColliderWallB[3]);
		wallBCollider.e = modelWallB.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "wallB", wallBCollider, modelMatrixWallB);

		glm::mat4 modelMatrixColliderWallC = glm::mat4(modelMatrixWallC);
		AbstractModel::OBB wallCCollider;
		wallCCollider.u = glm::quat_cast(modelMatrixWallC);
		modelMatrixColliderWallC = glm::scale(modelMatrixColliderWallC, escalamientoWalls);
		modelMatrixColliderWallC = glm::translate(modelMatrixColliderWallC, modelWallC.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		wallCCollider.c = glm::vec3(modelMatrixColliderWallC[3]);
		wallCCollider.e = modelWallC.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "wallC", wallCCollider, modelMatrixWallC);

		glm::mat4 modelMatrixColliderWallD = glm::mat4(modelMatrixWallD);
		AbstractModel::OBB wallDCollider;
		wallDCollider.u = glm::quat_cast(modelMatrixWallD);
		modelMatrixColliderWallD = glm::scale(modelMatrixColliderWallD, escalamientoWalls);
		modelMatrixColliderWallD = glm::translate(modelMatrixColliderWallD, modelWallD.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		wallDCollider.c = glm::vec3(modelMatrixColliderWallD[3]);
		wallDCollider.e = modelWallD.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "wallD", wallDCollider, modelMatrixWallD);

		glm::mat4 modelMatrixColliderWallE = glm::mat4(modelMatrixWallE);
		AbstractModel::OBB wallECollider;
		wallECollider.u = glm::quat_cast(modelMatrixWallE);
		modelMatrixColliderWallE = glm::scale(modelMatrixColliderWallE, escalamientoWalls);
		modelMatrixColliderWallE = glm::translate(modelMatrixColliderWallE, modelWallE.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		wallECollider.c = glm::vec3(modelMatrixColliderWallE[3]);
		wallECollider.e = modelWallE.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "wallE", wallECollider, modelMatrixWallE);

		glm::mat4 modelMatrixColliderWallF = glm::mat4(modelMatrixWallF);
		AbstractModel::OBB wallFCollider;
		wallFCollider.u = glm::quat_cast(modelMatrixWallF);
		modelMatrixColliderWallF = glm::scale(modelMatrixColliderWallF, escalamientoWalls);
		modelMatrixColliderWallF = glm::translate(modelMatrixColliderWallF, modelWallF.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		wallFCollider.c = glm::vec3(modelMatrixColliderWallF[3]);
		wallFCollider.e = modelWallF.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "wallF", wallFCollider, modelMatrixWallF);

		//Colliders Houses
		//House A houseAMatrix = { front, left, top };
		/*
			houseAMatrix[0] = glm::rotate(houseAMatrix[0], glm::radians(-90.0f), glm::vec3(0, 1, 0));
			houseAMatrix[0] = glm::rotate(houseAMatrix[0], glm::radians(270.0f), glm::vec3(1, 0, 0));
			houseAMatrix[1] = glm::rotate(houseAMatrix[1], glm::radians(-90.0f), glm::vec3(1, 0, 0));
		*/
		glm::mat4 modelMatrixColliderHAfront = glm::mat4(houseAMatrix[0]);
		AbstractModel::OBB HAfrontCollider;
		HAfrontCollider.u = glm::quat_cast(houseAMatrix[0]);
		modelMatrixColliderHAfront = glm::scale(modelMatrixColliderHAfront, escalamientoWalls);
		modelMatrixColliderHAfront = glm::translate(modelMatrixColliderHAfront, modelHAfront.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		HAfrontCollider.c = glm::vec3(modelMatrixColliderHAfront[3]);
		HAfrontCollider.e = modelHAfront.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "HA-front", HAfrontCollider, houseAMatrix[0]);

		glm::mat4 modelMatrixColliderHAleft = glm::mat4(houseAMatrix[1]);
		AbstractModel::OBB HAleftCollider;
		HAleftCollider.u = glm::quat_cast(houseAMatrix[1]);
		modelMatrixColliderHAleft = glm::scale(modelMatrixColliderHAleft, escalamientoWalls);
		modelMatrixColliderHAleft = glm::translate(modelMatrixColliderHAleft, modelHAleft.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		modelMatrixColliderHAleft = glm::rotate(modelMatrixColliderHAleft, glm::radians(-90.0f), glm::vec3(1,0,0));
		HAleftCollider.c = glm::vec3(modelMatrixColliderHAleft[3]);
		HAleftCollider.e = modelHAleft.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "HA-left", HAleftCollider, houseAMatrix[1]);
		//Top no necesario
		
		//HouseB houseBMatrix = { back, front, left1, left2, top };
		glm::mat4 modelMatrixColliderHBback = glm::mat4(houseBMatrix[0]);
		AbstractModel::OBB HBbackCollider;
		HBbackCollider.u = glm::quat_cast(houseBMatrix[0]);
		modelMatrixColliderHBback = glm::scale(modelMatrixColliderHBback, escalamientoWalls);
		modelMatrixColliderHBback = glm::translate(modelMatrixColliderHBback, modelHBback.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		HBbackCollider.c = glm::vec3(modelMatrixColliderHBback[3]);
		HBbackCollider.e = modelHBback.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "HB-back", HBbackCollider, houseBMatrix[0]);

		glm::mat4 modelMatrixColliderHBfront = glm::mat4(houseBMatrix[1]);
		AbstractModel::OBB HBfrontCollider;
		HBfrontCollider.u = glm::quat_cast(houseBMatrix[1]);
		modelMatrixColliderHBfront = glm::scale(modelMatrixColliderHBfront, escalamientoWalls);
		modelMatrixColliderHBfront = glm::translate(modelMatrixColliderHBfront, modelHBfront.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		HBfrontCollider.c = glm::vec3(modelMatrixColliderHBfront[3]);
		HBfrontCollider.e = modelHBfront.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "HB-front", HBfrontCollider, houseBMatrix[1]);

		glm::mat4 modelMatrixColliderHBleft1 = glm::mat4(houseBMatrix[2]);
		AbstractModel::OBB HBleft1Collider;
		HBleft1Collider.u = glm::quat_cast(houseBMatrix[2]);
		modelMatrixColliderHBleft1 = glm::scale(modelMatrixColliderHBleft1, escalamientoWalls);
		modelMatrixColliderHBleft1 = glm::translate(modelMatrixColliderHBleft1, modelHBleft1.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		HBleft1Collider.c = glm::vec3(modelMatrixColliderHBleft1[3]);
		HBleft1Collider.e = modelHBleft1.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "HB-left1", HBleft1Collider, houseBMatrix[2]);

		glm::mat4 modelMatrixColliderHBleft2 = glm::mat4(houseBMatrix[3]);
		AbstractModel::OBB HBleft2Collider;
		HBleft2Collider.u = glm::quat_cast(houseBMatrix[3]);
		modelMatrixColliderHBleft2 = glm::scale(modelMatrixColliderHBleft2, escalamientoWalls);
		modelMatrixColliderHBleft2 = glm::translate(modelMatrixColliderHBleft2, modelHBleft2.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		HBleft2Collider.c = glm::vec3(modelMatrixColliderHBleft2[3]);
		HBleft2Collider.e = modelHBleft2.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "HB-left2", HBleft2Collider, houseBMatrix[3]);

		//HouseC 	houseCMatrix = { front, left, top };
		glm::mat4 modelMatrixColliderHCfront = glm::mat4(houseCMatrix[0]);
		AbstractModel::OBB HCfrontCollider;
		HCfrontCollider.u = glm::quat_cast(houseCMatrix[0]);
		modelMatrixColliderHCfront = glm::scale(modelMatrixColliderHCfront, escalamientoWalls);
		modelMatrixColliderHCfront = glm::translate(modelMatrixColliderHCfront, modelHCfront.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		HCfrontCollider.c = glm::vec3(modelMatrixColliderHCfront[3]);
		HCfrontCollider.e = modelHCfront.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "HC-front", HCfrontCollider, houseCMatrix[0]);

		glm::mat4 modelMatrixColliderHCleft = glm::mat4(houseCMatrix[1]);
		AbstractModel::OBB HCleftCollider;
		HCleftCollider.u = glm::quat_cast(houseCMatrix[1]);
		modelMatrixColliderHCleft = glm::scale(modelMatrixColliderHCleft, escalamientoWalls);
		modelMatrixColliderHCleft = glm::translate(modelMatrixColliderHCleft, modelHCleft.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		HCleftCollider.c = glm::vec3(modelMatrixColliderHCleft[3]);
		HCleftCollider.e = modelHCleft.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "HC-left", HCleftCollider, houseCMatrix[1]);
		//HouseD	houseDMatrix = { back, left, right, top };
		glm::mat4 modelMatrixColliderHDback = glm::mat4(houseDMatrix[0]);
		AbstractModel::OBB HDbackCollider;
		HDbackCollider.u = glm::quat_cast(houseDMatrix[0]);
		modelMatrixColliderHDback = glm::scale(modelMatrixColliderHDback, escalamientoWalls);
		modelMatrixColliderHDback = glm::translate(modelMatrixColliderHDback, modelHDback.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		HDbackCollider.c = glm::vec3(modelMatrixColliderHDback[3]);
		HDbackCollider.e = modelHDback.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "HD-back", HDbackCollider, houseDMatrix[0]);

		glm::mat4 modelMatrixColliderHDleft = glm::mat4(houseDMatrix[1]);
		AbstractModel::OBB HDleftCollider;
		HDleftCollider.u = glm::quat_cast(houseDMatrix[1]);
		modelMatrixColliderHDleft = glm::scale(modelMatrixColliderHDleft, escalamientoWalls);
		modelMatrixColliderHDleft = glm::translate(modelMatrixColliderHDleft, modelHDleft.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		HDleftCollider.c = glm::vec3(modelMatrixColliderHDleft[3]);
		HDleftCollider.e = modelHDleft.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "HD-left", HDleftCollider, houseDMatrix[1]);

		glm::mat4 modelMatrixColliderHDright = glm::mat4(houseDMatrix[2]);
		AbstractModel::OBB HDrightCollider;
		HDrightCollider.u = glm::quat_cast(houseDMatrix[2]);
		modelMatrixColliderHDright = glm::scale(modelMatrixColliderHDright, escalamientoWalls);
		modelMatrixColliderHDright = glm::translate(modelMatrixColliderHDright, modelHDright.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		HDrightCollider.c = glm::vec3(modelMatrixColliderHDright[3]);
		HDrightCollider.e = modelHDright.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "HD-right", HDrightCollider, houseDMatrix[2]);
		//HouseE	houseEMatrix = { back, left, right, top };
		glm::mat4 modelMatrixColliderHEback = glm::mat4(houseEMatrix[0]);
		AbstractModel::OBB HEbackCollider;
		HEbackCollider.u = glm::quat_cast(houseEMatrix[0]);
		modelMatrixColliderHEback = glm::scale(modelMatrixColliderHEback, escalamientoWalls);
		modelMatrixColliderHEback = glm::translate(modelMatrixColliderHEback, modelHEback.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		HEbackCollider.c = glm::vec3(modelMatrixColliderHEback[3]);
		HEbackCollider.e = modelHEback.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "HE-back", HEbackCollider, houseEMatrix[0]);

		glm::mat4 modelMatrixColliderHEleft = glm::mat4(houseEMatrix[1]);
		AbstractModel::OBB HEleftCollider;
		HEleftCollider.u = glm::quat_cast(houseEMatrix[1]);
		modelMatrixColliderHEleft = glm::scale(modelMatrixColliderHEleft, escalamientoWalls);
		modelMatrixColliderHEleft = glm::translate(modelMatrixColliderHEleft, modelHEleft.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		HEleftCollider.c = glm::vec3(modelMatrixColliderHEleft[3]);
		HEleftCollider.e = modelHEleft.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "HE-left", HEleftCollider, houseEMatrix[1]);

		glm::mat4 modelMatrixColliderHEright = glm::mat4(houseEMatrix[2]);
		AbstractModel::OBB HErightCollider;
		HErightCollider.u = glm::quat_cast(houseEMatrix[2]);
		modelMatrixColliderHEright = glm::scale(modelMatrixColliderHEright, escalamientoWalls);
		modelMatrixColliderHEright = glm::translate(modelMatrixColliderHEright, modelHEright.getObb().c / glm::vec3(1.0f, escalamientoWalls.y, 1.0f));
		HErightCollider.c = glm::vec3(modelMatrixColliderHEright[3]);
		HErightCollider.e = modelHEright.getObb().e * escalamientoWalls;
		addOrUpdateColliders(collidersOBB, "HE-right", HErightCollider, houseEMatrix[2]);

		glm::mat4 modelMatrixColliderDoor = glm::mat4(modelMatrixRedDoor);
		AbstractModel::OBB redDoorCollider;
		redDoorCollider.u = glm::quat_cast(modelMatrixRedDoor);
		modelMatrixColliderDoor = glm::scale(modelMatrixColliderDoor, glm::vec3(8.0f));
		modelMatrixColliderDoor = glm::translate(modelMatrixColliderDoor, modelRedDoor.getObb().c / glm::vec3(1.0f, scaleRedDoor.y, 8.0f));
		redDoorCollider.c = glm::vec3(modelMatrixColliderDoor[3]);
		redDoorCollider.e = modelRedDoor.getObb().e * scaleRedDoor * glm::vec3(1.0f);
		addOrUpdateColliders(collidersOBB, "RedDoor", redDoorCollider, modelMatrixRedDoor);


		// Lamps1 colliders
		/*for (int i = 0; i < lamp1Position.size(); i++) {
			AbstractModel::OBB lampCollider;
			glm::mat4 modelMatrixColliderLamp = glm::mat4(1.0);
			modelMatrixColliderLamp = glm::translate(modelMatrixColliderLamp,
					lamp1Position[i]);
			modelMatrixColliderLamp = glm::rotate(modelMatrixColliderLamp,
					glm::radians(lamp1Orientation[i]), glm::vec3(0, 1, 0));
			addOrUpdateColliders(collidersOBB, "lamp1-" + std::to_string(i),
					lampCollider, modelMatrixColliderLamp);
			// Set the orientation of collider before doing the scale
			lampCollider.u = glm::quat_cast(modelMatrixColliderLamp);
			modelMatrixColliderLamp = glm::scale(modelMatrixColliderLamp,
					glm::vec3(0.5, 0.5, 0.5));
			modelMatrixColliderLamp = glm::translate(modelMatrixColliderLamp,
					modelLamp1.getObb().c);
			lampCollider.c = glm::vec3(modelMatrixColliderLamp[3]);
			//lampCollider.e = modelLamp1.getObb().e * glm::vec3(0.5, 0.5, 0.5);
			lampCollider.e = modelLamp1.getObb().e * escalamientoLampara;
			std::get<0>(collidersOBB.find("lamp1-" + std::to_string(i))->second) =
					lampCollider;
		}*/

		// Lamps2 colliders
		for (int i = 0; i < lamp2Position.size(); i++) {
			AbstractModel::OBB lampCollider;
			glm::mat4 modelMatrixColliderLamp = glm::mat4(1.0);
			modelMatrixColliderLamp = glm::translate(modelMatrixColliderLamp,
					lamp2Position[i]);
			modelMatrixColliderLamp = glm::rotate(modelMatrixColliderLamp,
					glm::radians(lamp2Orientation[i]), glm::vec3(0, 1, 0));
			addOrUpdateColliders(collidersOBB, "lamp2-" + std::to_string(i),
					lampCollider, modelMatrixColliderLamp);
			// Set the orientation of collider before doing the scale
			lampCollider.u = glm::quat_cast(modelMatrixColliderLamp);
			modelMatrixColliderLamp = glm::scale(modelMatrixColliderLamp,
					glm::vec3(1.0, 1.0, 1.0));
			modelMatrixColliderLamp = glm::translate(modelMatrixColliderLamp,
					modelLampPost2.getObb().c);
			lampCollider.c = glm::vec3(modelMatrixColliderLamp[3]);
			//lampCollider.e = modelLampPost2.getObb().e * glm::vec3(1.0, 1.0, 1.0);
			lampCollider.e = modelLampPost2.getObb().e * escalamientoLampara;
			std::get<0>(collidersOBB.find("lamp2-" + std::to_string(i))->second) =
					lampCollider;
		}

		//Collider de Player
		AbstractModel::OBB playerCollider;
		glm::mat4 modelMatrixColliderPlayer = glm::mat4(modelMatrixPlayer);
		playerCollider.u = glm::quat_cast(modelMatrixPlayer);
		modelMatrixColliderPlayer = glm::scale(modelMatrixColliderPlayer, escalamientoPlayer);
		modelMatrixColliderPlayer = glm::translate(modelMatrixColliderPlayer, playerModelAnimate.getObb().c);
		playerCollider.c = glm::vec3(modelMatrixColliderPlayer[3]);
		playerCollider.e = playerModelAnimate.getObb().e * escalamientoPlayer * glm::vec3(0.4f, 1.0f, 1.5f);
		addOrUpdateColliders(collidersOBB, "Player", playerCollider, modelMatrixPlayer);

		//Collider de Zombies
		//Z1
		AbstractModel::OBB zombieCollider1;
		glm::mat4 modelMatrixColliderZombie1 = glm::mat4(modelMatrixZombie1);
		if (isZombieAlive[0]) {
			zombieCollider1.u = glm::quat_cast(modelMatrixZombie1);
			modelMatrixColliderZombie1 = glm::scale(modelMatrixColliderZombie1, scaleZombie);
			modelMatrixColliderZombie1 = glm::translate(modelMatrixColliderZombie1, zombieModelAnimate1.getObb().c);
			zombieCollider1.c = glm::vec3(modelMatrixColliderZombie1[3]);
			zombieCollider1.e = zombieModelAnimate1.getObb().e * scaleZombie * glm::vec3(0.4f, 1.0f, 1.5f);
			addOrUpdateColliders(collidersOBB, "Zombie1", zombieCollider1, modelMatrixZombie1);
		}

		//Z2
		AbstractModel::OBB zombieCollider2;
		glm::mat4 modelMatrixColliderZombie2 = glm::mat4(modelMatrixZombie2);

		if (isZombieAlive[1]) {
			zombieCollider2.u = glm::quat_cast(modelMatrixZombie2);
			modelMatrixColliderZombie2 = glm::scale(modelMatrixColliderZombie2, scaleZombie);
			modelMatrixColliderZombie2 = glm::translate(modelMatrixColliderZombie2, zombieModelAnimate2.getObb().c);
			zombieCollider2.c = glm::vec3(modelMatrixColliderZombie2[3]);
			zombieCollider2.e = zombieModelAnimate2.getObb().e * scaleZombie * glm::vec3(0.4f, 1.0f, 1.5f);
			addOrUpdateColliders(collidersOBB, "Zombie2", zombieCollider2, modelMatrixZombie2);
		}

		//Z3
		AbstractModel::OBB zombieCollider3;
		glm::mat4 modelMatrixColliderZombie3 = glm::mat4(modelMatrixZombie3);

		if (isZombieAlive[2]) {
			zombieCollider3.u = glm::quat_cast(modelMatrixZombie3);
			modelMatrixColliderZombie3 = glm::scale(modelMatrixColliderZombie3, scaleZombie);
			modelMatrixColliderZombie3 = glm::translate(modelMatrixColliderZombie3, zombieModelAnimate3.getObb().c);
			zombieCollider3.c = glm::vec3(modelMatrixColliderZombie3[3]);
			zombieCollider3.e = zombieModelAnimate3.getObb().e * scaleZombie * glm::vec3(0.4f, 1.0f, 1.5f);
			addOrUpdateColliders(collidersOBB, "Zombie3", zombieCollider3, modelMatrixZombie3);
		}

		//Z4
		AbstractModel::OBB zombieCollider4;
		glm::mat4 modelMatrixColliderZombie4 = glm::mat4(modelMatrixZombie4);

		if (isZombieAlive[3]) {
			zombieCollider4.u = glm::quat_cast(modelMatrixZombie4);
			modelMatrixColliderZombie4 = glm::scale(modelMatrixColliderZombie4, scaleZombie);
			modelMatrixColliderZombie4 = glm::translate(modelMatrixColliderZombie4, zombieModelAnimate4.getObb().c);
			zombieCollider4.c = glm::vec3(modelMatrixColliderZombie4[3]);
			zombieCollider4.e = zombieModelAnimate4.getObb().e * scaleZombie * glm::vec3(0.4f, 1.0f, 1.5f);
			addOrUpdateColliders(collidersOBB, "Zombie4", zombieCollider4, modelMatrixZombie4);
		}


		//Z5
		AbstractModel::OBB zombieCollider5;
		glm::mat4 modelMatrixColliderZombie5 = glm::mat4(modelMatrixZombie5);
		if (isZombieAlive[4]) {
			zombieCollider5.u = glm::quat_cast(modelMatrixZombie5);
			modelMatrixColliderZombie5 = glm::scale(modelMatrixColliderZombie5, scaleZombie);
			modelMatrixColliderZombie5 = glm::translate(modelMatrixColliderZombie5, zombieModelAnimate5.getObb().c);
			zombieCollider5.c = glm::vec3(modelMatrixColliderZombie5[3]);
			zombieCollider5.e = zombieModelAnimate5.getObb().e * scaleZombie * glm::vec3(0.4f, 1.0f, 1.5f);
			addOrUpdateColliders(collidersOBB, "Zombie5", zombieCollider5, modelMatrixZombie5);
		}

		//Z6
		AbstractModel::OBB zombieCollider6;
		glm::mat4 modelMatrixColliderZombie6 = glm::mat4(modelMatrixZombie6);
		if (isZombieAlive[5]) {
			zombieCollider6.u = glm::quat_cast(modelMatrixZombie6);
			modelMatrixColliderZombie6 = glm::scale(modelMatrixColliderZombie6, scaleZombie);
			modelMatrixColliderZombie6 = glm::translate(modelMatrixColliderZombie6, zombieModelAnimate6.getObb().c);
			zombieCollider6.c = glm::vec3(modelMatrixColliderZombie6[3]);
			zombieCollider6.e = zombieModelAnimate6.getObb().e * scaleZombie * glm::vec3(0.4f, 1.0f, 1.5f);
			addOrUpdateColliders(collidersOBB, "Zombie6", zombieCollider6, modelMatrixZombie6);
		}

		//Z7
		AbstractModel::OBB zombieCollider7;
		glm::mat4 modelMatrixColliderZombie7 = glm::mat4(modelMatrixZombie7);
		if (isZombieAlive[6]) {
			zombieCollider7.u = glm::quat_cast(modelMatrixZombie7);
			modelMatrixColliderZombie7 = glm::scale(modelMatrixColliderZombie7, scaleZombie);
			modelMatrixColliderZombie7 = glm::translate(modelMatrixColliderZombie7, zombieModelAnimate7.getObb().c);
			zombieCollider7.c = glm::vec3(modelMatrixColliderZombie7[3]);
			zombieCollider7.e = zombieModelAnimate7.getObb().e * scaleZombie * glm::vec3(0.4f, 1.0f, 1.5f);
			addOrUpdateColliders(collidersOBB, "Zombie7", zombieCollider7, modelMatrixZombie7);
		}
				
		//Keys
		AbstractModel::OBB keyColliderA;
		glm::mat4 modelMatrixColliderKeyA = glm::mat4(modelMatrixKeyA);
		if (isKeyCollected[0] == false) {
			keyColliderA.u = glm::quat_cast(modelMatrixKeyA);
			modelMatrixColliderKeyA = glm::scale(modelMatrixColliderKeyA, scaleKey);
			modelMatrixColliderKeyA = glm::translate(modelMatrixColliderKeyA, keyModelAnimateA.getObb().c);
			keyColliderA.c = glm::vec3(modelMatrixColliderKeyA[3]);
			keyColliderA.e = keyModelAnimateA.getObb().e * scaleKey * glm::vec3(100.0f);
			addOrUpdateColliders(collidersOBB, "KeyA", keyColliderA, modelMatrixKeyA);
		}

		AbstractModel::OBB keyColliderB;
		glm::mat4 modelMatrixColliderKeyB = glm::mat4(modelMatrixKeyB);
		if (isKeyCollected[1] == false) {
			keyColliderB.u = glm::quat_cast(modelMatrixKeyB);
			modelMatrixColliderKeyB = glm::scale(modelMatrixColliderKeyB, scaleKey);
			modelMatrixColliderKeyB = glm::translate(modelMatrixColliderKeyB, keyModelAnimateB.getObb().c);
			keyColliderB.c = glm::vec3(modelMatrixColliderKeyB[3]);
			keyColliderB.e = keyModelAnimateB.getObb().e * scaleKey * glm::vec3(100.0f);
			addOrUpdateColliders(collidersOBB, "KeyB", keyColliderB, modelMatrixKeyB);
		}
		/*******************************************
		 * Render de colliders
		 *******************************************/
		if (renderEverything) {
			for (std::map<std::string,
					std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
					collidersOBB.begin(); it != collidersOBB.end(); it++) {
				glm::mat4 matrixCollider = glm::mat4(1.0);
				matrixCollider = glm::translate(matrixCollider,
						std::get<0>(it->second).c);
				matrixCollider = matrixCollider
						* glm::mat4(std::get<0>(it->second).u);
				matrixCollider = glm::scale(matrixCollider,
						std::get<0>(it->second).e * 2.0f);
				boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
				boxCollider.enableWireMode();
				boxCollider.render(matrixCollider);
			}

			for (std::map<std::string,
					std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
					collidersSBB.begin(); it != collidersSBB.end(); it++) {
				glm::mat4 matrixCollider = glm::mat4(1.0);
				matrixCollider = glm::translate(matrixCollider,
						std::get<0>(it->second).c);
				matrixCollider = glm::scale(matrixCollider,
						glm::vec3(std::get<0>(it->second).ratio * 2.0f));
				sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
				sphereCollider.enableWireMode();
				sphereCollider.render(matrixCollider);
			}
		}

		/*******************************************
		 * Test Colisions
		 *******************************************/
		for (std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
				collidersOBB.begin(); it != collidersOBB.end(); it++) {
			bool isCollision = false;
			for (std::map<std::string,
					std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
					collidersOBB.begin(); jt != collidersOBB.end(); jt++) {
				if (it != jt && testOBBOBB(std::get<0>(it->second),std::get<0>(jt->second))) {
					std::cout << "Colision " << it->first << " with "
							<< jt->first << std::endl;
					isCollision = true;

					if (it->first.compare("Player") == 0) {
						if (jt->first.compare("KeyA") == 0) {
							isKeyCollected[0] = true;
							keysFound++;
							collidersOBB.erase("KeyA");
							if (keysFound == 1)
								textureKeysActivaID = textureKeyAFoundID;
							else if (keysFound == 2)
								textureKeysActivaID = textureKeysCompleteID;
						}
						if (jt->first.compare("KeyB") == 0) {
							isKeyCollected[1] = true;
							keysFound++;
							collidersOBB.erase("KeyB");
							if (keysFound == 1)
								textureKeysActivaID = textureKeyAFoundID;
							else if (keysFound == 2)
								textureKeysActivaID = textureKeysCompleteID;
						}

						/*if (jt->first.compare("Zombie1") == 0 || jt->first.compare("Zombie2") == 0 || jt->first.compare("Zombie3") == 0 || jt->first.compare("Zombie4") == 0 || jt->first.compare("Zombie5") == 0 || jt->first.compare("Zombie6") == 0 || jt->first.compare("Zombie7") == 0) {
							if (vidasPlayer == 3)
								vidasPlayer = 2;
							else if (vidasPlayer == 2)
								vidasPlayer = 1;
							else
								vidasPlayer = 0;
						}*/
						if (jt->first.compare("RedDoor") == 0) {
							isTryingToEscape = true;
							currTimeKeys = glfwGetTime();
							lastTimeKeys = glfwGetTime();
						}
						else if (keysFound < 2)
							isTryingToEscape = false;
					}
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		}

		for (std::map<std::string,
				std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
				collidersSBB.begin(); it != collidersSBB.end(); it++) {
			bool isCollision = false;
			for (std::map<std::string,
					std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator jt =
					collidersSBB.begin(); jt != collidersSBB.end(); jt++) {
				if (it != jt
						&& testSphereSphereIntersection(std::get<0>(it->second),
								std::get<0>(jt->second))) {
					std::cout << "Colision " << it->first << " with "
							<< jt->first << std::endl;
					isCollision = true;
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first,
					isCollision);
		}

		for (std::map<std::string,
				std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
				collidersSBB.begin(); it != collidersSBB.end(); it++) {
			bool isCollision = false;
			std::map<std::string,
					std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
					collidersOBB.begin();
			for (; jt != collidersOBB.end(); jt++) {
				if (testSphereOBox(std::get<0>(it->second),
						std::get<0>(jt->second))) {
					std::cout << "Colision " << it->first << " with "
							<< jt->first << std::endl;
					isCollision = true;
					addOrUpdateCollisionDetection(collisionDetection, jt->first,
							isCollision);
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first,
					isCollision);
		}

		std::map<std::string, bool>::iterator colIt;
		for (colIt = collisionDetection.begin();
				colIt != collisionDetection.end(); colIt++) {
			std::map<std::string,
					std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
					collidersSBB.find(colIt->first);
			std::map<std::string,
					std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
					collidersOBB.find(colIt->first);
			if (it != collidersSBB.end()) {
				if (!colIt->second)
					addOrUpdateColliders(collidersSBB, it->first);
			}
			if (jt != collidersOBB.end()) {
				if (!colIt->second)
					addOrUpdateColliders(collidersOBB, jt->first);
				else {
					//Esto hace que no puedas atravesar con el collider
					if (jt->first.compare("Player") == 0)
						modelMatrixPlayer = std::get<1>(jt->second);
				}
			}
		}

		/*******************************************
		* Ray Test Colisions
		*******************************************/

		for (std::map < std::string, std::tuple < AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it = collidersOBB.begin(); it != collidersOBB.end(); it++) {
			//Movimiento con ShootRay
			if (intersectRayOBB(rayShootOri, rayShootTar, rayShootDirection, std::get<0>(it->second))) {
				
				if (it->first.compare("Zombie1") == 0) {
					//std::cout << "Collision " << it->first << " with ShootRay" << std::endl;
					isZombieInRange[0] = true;
					if (isShooting) { 
						//Atacar a player?
						isZombieShooted[0] = true;
						currTimeShooted = glfwGetTime();
						lastTimeShooted = currTimeShooted;
						deltaTimeShooted = 0.0f;
					}
				}
				if (it->first.compare("Zombie2") == 0) {
					isZombieInRange[1] = true;
					if (isShooting) {
						//Atacar a player?
						isZombieShooted[1] = true;
						currTimeShooted = glfwGetTime();
						lastTimeShooted = currTimeShooted;
						deltaTimeShooted = 0.0f;
					}
				}
				if (it->first.compare("Zombie3") == 0) {
					isZombieInRange[2] = true;
					if (isShooting) {
						//Atacar a player?
						isZombieShooted[2] = true;
						currTimeShooted = glfwGetTime();
						lastTimeShooted = currTimeShooted;
						deltaTimeShooted = 0.0f;
					}
				}
				if (it->first.compare("Zombie4") == 0) {
					isZombieInRange[3] = true;
					if (isShooting) {
						//Atacar a player?
						isZombieShooted[3] = true;
						currTimeShooted = glfwGetTime();
						lastTimeShooted = currTimeShooted;
						deltaTimeShooted = 0.0f;
					}
				}
				if (it->first.compare("Zombie5") == 0) {
					isZombieInRange[4] = true;
					if (isShooting) {
						//Atacar a player?
						isZombieShooted[4] = true;
						currTimeShooted = glfwGetTime();
						lastTimeShooted = currTimeShooted;
						deltaTimeShooted = 0.0f;
					}
				}
				if (it->first.compare("Zombie6") == 0) {
					isZombieInRange[5] = true;
					if (isShooting) {
						//Atacar a player?
						isZombieShooted[5] = true;
						currTimeShooted = glfwGetTime();
						lastTimeShooted = currTimeShooted;
						deltaTimeShooted = 0.0f;
					}
				}
				if (it->first.compare("Zombie7") == 0) {
					isZombieInRange[6] = true;
					if (isShooting) {
						//Atacar a player?
						isZombieShooted[6] = true;
						currTimeShooted = glfwGetTime();
						lastTimeShooted = currTimeShooted;
						deltaTimeShooted = 0.0f;
					}
				}
			}
		}


		/*******************************************
		 * Constantes de animaciones
		 *******************************************/
		if(!isShooting)
			animationIndexPlayer = 0;
		//Z1
		if(!isZombieShooted[0] && !isZombieDying[0] && isZombieAlive[0])
			animationIndexZombie1 = 0;
		else if (!isZombieAlive[0])
			animationIndexZombie1 = 6;
		//Z2
		if (!isZombieShooted[1] && !isZombieDying[1] && isZombieAlive[1])
			animationIndexZombie2 = 0;
		else if (!isZombieAlive[1])
			animationIndexZombie2 = 6;
		//Z3
		if (!isZombieShooted[2] && !isZombieDying[2] && isZombieAlive[2])
			animationIndexZombie3 = 0;
		else if (!isZombieAlive[2])
			animationIndexZombie3 = 6;
		//Z4
		if (!isZombieShooted[3] && !isZombieDying[3] && isZombieAlive[3])
			animationIndexZombie4 = 0;
		else if (!isZombieAlive[3])
			animationIndexZombie4 = 6;
		//Z5
		if (!isZombieShooted[4] && !isZombieDying[4] && isZombieAlive[4])
			animationIndexZombie5 = 0;
		else if (!isZombieAlive[4])
			animationIndexZombie5 = 6;
		//Z6
		if (!isZombieShooted[5] && !isZombieDying[5] && isZombieAlive[5])
			animationIndexZombie6 = 0;
		else if (!isZombieAlive[5])
			animationIndexZombie6 = 6;
		//Z7
		if (!isZombieShooted[6] && !isZombieDying[6] && isZombieAlive[6])
			animationIndexZombie7 = 0;
		else if (!isZombieAlive[6])
			animationIndexZombie7 = 6;

		//Triggers para audio
		if (animationIndexPlayer == 1) {
			sourcesPlay[18] = true;
			sourcesPlay[1] = false;
		}
		else if (animationIndexPlayer == 6) {
			sourcesPlay[18] = false;
			sourcesPlay[1] = true;
		}
		else {
			sourcesPlay[1] = false;
			sourcesPlay[2] = false;
		}
//{  0		1		2	  3     4     5     6     7     8     9      10     11      12     13     14     15     16     17    18 };
//{ fire, gunshot, z1n,  z2n,  z3n,  z4n,  z5n,  z6n,  z7n,  z1d,   z2d,   z3d,    z4d,   z5d,   z6d,   z7d,   keyA,  keyB, Walk};
		for (int i = 0; i <= 6; i++) {
			if (isZombieAlive[i]) {
				sourcesPlay[i + 2] = true;
				//sourcesPlay[i + 9] = false;
			}
			else {
				//sourcesPlay[i + 2] = false;	//Ya no vive el zombie normal
				alSourceStop(sourcesPlay[i + 2]);
				sourcesPlay[i + 9] = true;	//Sonido de muerte de zombie
			}
		}
		/*if (isZombieAlive[0]) {
			sourcesPlay[2] = true;
			std::cout << "Zombie Alive" << std::endl;
			std::cout << "Sonido ZombieN = " << sourcesPlay[2] << "\nSonido ZombieD = " << sourcesPlay[9] << std::endl;
		}
		else {
			sourcesPlay[2] = false;
			sourcesPlay[9] = true;
			std::cout << "Zombie Dead" << std::endl;
			std::cout << "Sonido ZombieN = " << sourcesPlay[2] << "\nSonido ZombieD = " << sourcesPlay[9] << std::endl;
		}*/

		if (isKeyCollected[0])
			sourcesPlay[16] = true;
		else
			sourcesPlay[16] = false;
		if (isKeyCollected[1])
			sourcesPlay[17] = true;
		else
			sourcesPlay[17] = false;

		/*******************************************
		 * State machines
		 *******************************************/
		stateZombieGeneral(0);
		stateZombieGeneral(1);
		stateZombieGeneral(2);
		stateZombieGeneral(3);
		stateZombieGeneral(4);
		stateZombieGeneral(5);
		stateZombieGeneral(6);

		/*******************************************
		 * Text Rendering
		 *******************************************/
		std::string keysFoundStr = std::to_string(keysFound);
		//std::string showText = "Llaves encontradas: " + keysFoundStr + " - 2";
		//modelText->render(showText, 0.6, 0.9);
		if (isTryingToEscape == true && keysFound < 2) {
			currTimeKeys = glfwGetTime();
			deltaTimeKeys += currTimeKeys - lastTimeKeys;
			lastTimeKeys = currTimeKeys;
			if (deltaTimeKeys < 2.0f) {
				modelText2->render("Aun no puedes salir, necesitas 2 llaves", -0.2, 0);
			}
			else {
				modelText2->render("", -0.2, 0);
				currTimeKeys = 0.0f;
				deltaTimeKeys = 0.0f;
				lastTimeKeys = 0.0f;
			}

		}
		else if (keysFound == 2) {
			modelText2->render("Felicidades! Has escapado. Presiona ESC.", -0.2, 0);
		}
		glfwSwapBuffers(window);

		/*******************************************
		 * Camera Update
		 *******************************************/

		//if (modelSelected == 1) {
			axis = glm::axis(glm::quat_cast(modelMatrixPlayer));
			angleTarget = glm::angle(glm::quat_cast(modelMatrixPlayer));
			target = playerCollider.c;
			target.y += playerCollider.e.y;
		//}

		if (std::isnan(angleTarget))
			angleTarget = 0.0;
		if (axis.y < 0)
			angleTarget = -angleTarget;
		//Posiciona la camara detras de Player, hacia enfrente.
		//if (modelSelected == 1)
			angleTarget -= glm::radians(180.0f);
		camera->setCameraTarget(target);
		camera->setAngleTarget(angleTarget);
		camera->updateCamera();
		view = camera->getViewMatrix();


		/*******************************************
		* OpenAL sound data
		*******************************************/
		openALSoundData();
	}
}

void prepareScene() {

	skyboxSphere.setShader(&shaderSkybox);
	terrain.setShader(&shaderTerrain);

	//Lamp models
	modelLamp1.setShader(&shaderMulLighting);
	modelLamp2.setShader(&shaderMulLighting);
	modelLampPost2.setShader(&shaderMulLighting);

	//Grass
	modelGrass.setShader(&shaderMulLighting);

	//Player
	playerModelAnimate.setShader(&shaderMulLighting);
	//Zombies
	zombieModelAnimate1.setShader(&shaderMulLighting);
	zombieModelAnimate2.setShader(&shaderMulLighting);
	zombieModelAnimate3.setShader(&shaderMulLighting);
	zombieModelAnimate4.setShader(&shaderMulLighting);
	zombieModelAnimate5.setShader(&shaderMulLighting);
	zombieModelAnimate6.setShader(&shaderMulLighting);
	zombieModelAnimate7.setShader(&shaderMulLighting);

	keyModelAnimateA.setShader(&shaderMulLighting);
	keyModelAnimateB.setShader(&shaderMulLighting);

	//Walls
	modelWallA.setShader(&shaderMulLighting);
	modelWallB.setShader(&shaderMulLighting);
	modelWallC.setShader(&shaderMulLighting);
	modelWallD.setShader(&shaderMulLighting);
	modelWallE.setShader(&shaderMulLighting);
	//Houses
	modelHAfront.setShader(&shaderMulLighting);
	modelHAleft.setShader(&shaderMulLighting);
	modelHAtop.setShader(&shaderMulLighting);
	modelHBback.setShader(&shaderMulLighting);
	modelHBfront.setShader(&shaderMulLighting);
	modelHBleft1.setShader(&shaderMulLighting);
	modelHBleft2.setShader(&shaderMulLighting);
	modelHBtop.setShader(&shaderMulLighting);
	modelHCfront.setShader(&shaderMulLighting);
	modelHCleft.setShader(&shaderMulLighting);
	modelHCtop.setShader(&shaderMulLighting);
	modelHDback.setShader(&shaderMulLighting);
	modelHDleft.setShader(&shaderMulLighting);
	modelHDright.setShader(&shaderMulLighting);
	modelHDtop.setShader(&shaderMulLighting);
	modelHEback.setShader(&shaderMulLighting);
	modelHEleft.setShader(&shaderMulLighting);
	modelHEright.setShader(&shaderMulLighting);
	modelHEtop.setShader(&shaderMulLighting);
	//Door
	modelRedDoor.setShader(&shaderMulLighting);
}

void prepareDepthScene() {

	skyboxSphere.setShader(&shaderDepth);
	terrain.setShader(&shaderDepth);

	//Lamp models
	modelLamp1.setShader(&shaderDepth);
	modelLamp2.setShader(&shaderDepth);
	modelLampPost2.setShader(&shaderDepth);

	//Grass
	modelGrass.setShader(&shaderDepth);

	//Player
	playerModelAnimate.setShader(&shaderDepth);
	//Zombies
	zombieModelAnimate1.setShader(&shaderDepth);
	zombieModelAnimate2.setShader(&shaderDepth);
	zombieModelAnimate3.setShader(&shaderDepth);
	zombieModelAnimate4.setShader(&shaderDepth);
	zombieModelAnimate5.setShader(&shaderDepth);
	zombieModelAnimate6.setShader(&shaderDepth);
	zombieModelAnimate7.setShader(&shaderDepth);

	keyModelAnimateA.setShader(&shaderDepth);
	keyModelAnimateB.setShader(&shaderDepth);

	//Walls
	/*modelWallA.setShader(&shaderDepth);
	modelWallB.setShader(&shaderDepth);
	modelWallC.setShader(&shaderDepth);
	modelWallD.setShader(&shaderDepth);
	modelWallE.setShader(&shaderDepth);*/
	//Houses
	modelHAfront.setShader(&shaderDepth);
	modelHAleft.setShader(&shaderDepth);
	modelHAtop.setShader(&shaderDepth);
	modelHBback.setShader(&shaderDepth);
	modelHBfront.setShader(&shaderDepth);
	modelHBleft1.setShader(&shaderDepth);
	modelHBleft2.setShader(&shaderDepth);
	modelHBtop.setShader(&shaderDepth);
	modelHCfront.setShader(&shaderDepth);
	modelHCleft.setShader(&shaderDepth);
	modelHCtop.setShader(&shaderDepth);
	modelHDback.setShader(&shaderDepth);
	modelHDleft.setShader(&shaderDepth);
	modelHDright.setShader(&shaderDepth);
	modelHDtop.setShader(&shaderDepth);
	modelHEback.setShader(&shaderDepth);
	modelHEleft.setShader(&shaderDepth);
	modelHEright.setShader(&shaderDepth);
	modelHEtop.setShader(&shaderDepth);
	//Door
	modelRedDoor.setShader(&shaderDepth);
}

void renderScene(bool renderParticles) {
	/*******************************************
	 * Terrain Cesped
	 *******************************************/
	glm::mat4 modelCesped = glm::mat4(1.0);
	modelCesped = glm::translate(modelCesped, glm::vec3(0.0, 0.0, 0.0));
	modelCesped = glm::scale(modelCesped, glm::vec3(200.0, 0.001, 200.0));
	// Se activa la textura del background
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	shaderTerrain.setInt("backgroundTexture", 0);
	// Se activa la textura de tierra
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	shaderTerrain.setInt("rTexture", 1);
	// Se activa la textura de hierba
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	shaderTerrain.setInt("gTexture", 2);
	// Se activa la textura del camino
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	shaderTerrain.setInt("bTexture", 3);
	// Se activa la textura del blend map
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	shaderTerrain.setInt("blendMapTexture", 4);
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40, 40)));
	terrain.render();
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
	glBindTexture(GL_TEXTURE_2D, 0);

	/*******************************************
	 * Custom objects obj
	 *******************************************/
	// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
	glActiveTexture(GL_TEXTURE0);

	// Render the lamps
	/*
	for (int i = 0; i < lamp1Position.size(); i++) {
		lamp1Position[i].y = terrain.getHeightTerrain(lamp1Position[i].x,
				lamp1Position[i].z);
		modelLamp1.setPosition(lamp1Position[i]);
		modelLamp1.setScale(escalamientoLampara);
		modelLamp1.setOrientation(glm::vec3(0, lamp1Orientation[i], 0));
		modelLamp1.render();
	}*/

	for (int i = 0; i < lamp2Position.size(); i++) {
		lamp2Position[i].y = terrain.getHeightTerrain(lamp2Position[i].x,
				lamp2Position[i].z);
		modelLamp2.setPosition(lamp2Position[i]);
		modelLamp2.setScale(escalamientoLampara);
		modelLamp2.setOrientation(glm::vec3(0, lamp2Orientation[i], 0));
		modelLamp2.render();
		modelLampPost2.setPosition(lamp2Position[i]);
		modelLampPost2.setScale(escalamientoLampara);
		modelLampPost2.setOrientation(glm::vec3(0, lamp2Orientation[i], 0));
		modelLampPost2.render();
	}

	// Grass
	glDisable(GL_CULL_FACE);
	glm::vec3 grassPosition = glm::vec3(0.0, 0.0, 0.0);
	grassPosition.y = terrain.getHeightTerrain(grassPosition.x,
			grassPosition.z);
	modelGrass.setPosition(grassPosition);
	modelGrass.render();
	glEnable(GL_CULL_FACE);

	// Fountain
	glDisable(GL_CULL_FACE);
	modelFountain.render(modelMatrixFountain);
	glEnable(GL_CULL_FACE);

	//Walls Render
	//modelMatrixWallA[3][1] = terrain.getHeightTerrain(modelMatrixWallA[3][0], modelMatrixWallA[3][2]);
	modelWallA.render(modelMatrixWallA);
	modelWallB.render(modelMatrixWallB);
	modelWallC.render(modelMatrixWallC);
	modelWallD.render(modelMatrixWallD);
	modelWallE.render(modelMatrixWallE);
	modelWallF.render(modelMatrixWallF);

	//House Render
	//HouseA	houseAMatrix = { front, left, top };
	//HouseB	houseBMatrix = { back, front, left1, left2, top };
	//HouseC 	houseCMatrix = { front, left, top };
	//HouseD	houseDMatrix = { back, left, right, top };
	//HouseE	houseEMatrix = { back, left, right, top };


	modelHAfront.render(houseAMatrix[0]);
	modelHAleft.render(houseAMatrix[1]);
	modelHAtop.render(houseAMatrix[2]);
	modelHBback.render(houseBMatrix[0]);
	modelHBfront.render(houseBMatrix[1]);
	modelHBleft1.render(houseBMatrix[2]);
	modelHBleft2.render(houseBMatrix[3]);
	modelHBtop.render(houseBMatrix[4]);
	modelHCfront.render(houseCMatrix[0]);
	modelHCleft.render(houseCMatrix[1]);
	modelHCtop.render(houseCMatrix[2]);
	modelHDback.render(houseDMatrix[0]);
	modelHDleft.render(houseDMatrix[1]);
	modelHDright.render(houseDMatrix[2]);
	modelHDtop.render(houseDMatrix[3]);
	modelHEback.render(houseEMatrix[0]);
	modelHEleft.render(houseEMatrix[1]);
	modelHEright.render(houseEMatrix[2]);
	modelHEtop.render(houseEMatrix[3]);
	
	if(isKeyCollected[0] == false)
		keyModelAnimateA.render(modelMatrixKeyA);
	if(isKeyCollected[1] == false)
		keyModelAnimateB.render(modelMatrixKeyB);

	modelRedDoor.render(modelMatrixRedDoor);
	/*******************************************
	 * Custom Anim objects obj
	 *******************************************/
	//Player
	modelMatrixPlayer[3][1] = -GRAVITY * tmv * tmv + 3.5 * tmv + terrain.getHeightTerrain(modelMatrixPlayer[3][0], modelMatrixPlayer[3][2]);
	tmv = currTime - startTimeJump;
	if (modelMatrixPlayer[3][1] < terrain.getHeightTerrain(modelMatrixPlayer[3][0], modelMatrixPlayer[3][2])) {
		isJumpPlayer = false;
		modelMatrixPlayer[3][1] = terrain.getHeightTerrain(modelMatrixPlayer[3][0], modelMatrixPlayer[3][2]);
	}

	glm::mat4 modelMatrixPlayerBody = glm::mat4(modelMatrixPlayer);
	modelMatrixPlayerBody = glm::scale(modelMatrixPlayerBody, escalamientoPlayer);
	playerModelAnimate.setAnimationIndex(animationIndexPlayer);
	playerModelAnimate.render(modelMatrixPlayerBody);

	//Zombies

	modelMatrixZombie1[3][1] = terrain.getHeightTerrain(modelMatrixZombie1[3][0], modelMatrixZombie1[3][2]);
	glm::mat4 modelMatrixZombieBody1 = glm::mat4(modelMatrixZombie1);
	modelMatrixZombieBody1 = glm::scale(modelMatrixZombieBody1, scaleZombie);
	zombieModelAnimate1.setAnimationIndex(animationIndexZombie1);
	zombieModelAnimate1.render(modelMatrixZombieBody1);

	modelMatrixZombie2[3][1] = terrain.getHeightTerrain(modelMatrixZombie2[3][0], modelMatrixZombie2[3][2]);
	glm::mat4 modelMatrixZombieBody2 = glm::mat4(modelMatrixZombie2);
	modelMatrixZombieBody2 = glm::scale(modelMatrixZombieBody2, scaleZombie);
	modelMatrixZombieBody2 = glm::rotate(modelMatrixZombieBody2, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	zombieModelAnimate2.setAnimationIndex(animationIndexZombie2);
	zombieModelAnimate2.render(modelMatrixZombieBody2);

	modelMatrixZombie3[3][1] = terrain.getHeightTerrain(modelMatrixZombie3[3][0], modelMatrixZombie3[3][2]);
	glm::mat4 modelMatrixZombieBody3 = glm::mat4(modelMatrixZombie3);
	modelMatrixZombieBody3 = glm::scale(modelMatrixZombieBody3, scaleZombie);
	modelMatrixZombieBody3 = glm::rotate(modelMatrixZombieBody3, glm::radians(180.0f), glm::vec3(0, 1, 0));
	zombieModelAnimate3.setAnimationIndex(animationIndexZombie3);
	zombieModelAnimate3.render(modelMatrixZombieBody3);

	modelMatrixZombie4[3][1] = terrain.getHeightTerrain(modelMatrixZombie4[3][0], modelMatrixZombie4[3][2]);
	glm::mat4 modelMatrixZombieBody4 = glm::mat4(modelMatrixZombie4);
	modelMatrixZombieBody4 = glm::scale(modelMatrixZombieBody4, scaleZombie);
	zombieModelAnimate4.setAnimationIndex(animationIndexZombie4);
	zombieModelAnimate4.render(modelMatrixZombieBody4);

	modelMatrixZombie5[3][1] = terrain.getHeightTerrain(modelMatrixZombie5[3][0], modelMatrixZombie5[3][2]);
	glm::mat4 modelMatrixZombieBody5 = glm::mat4(modelMatrixZombie5);
	modelMatrixZombieBody5 = glm::scale(modelMatrixZombieBody5, scaleZombie);
	modelMatrixZombieBody5 = glm::rotate(modelMatrixZombieBody5, glm::radians(-45.0f), glm::vec3(0, 1, 0));
	zombieModelAnimate5.setAnimationIndex(animationIndexZombie5);
	zombieModelAnimate5.render(modelMatrixZombieBody5);

	modelMatrixZombie6[3][1] = terrain.getHeightTerrain(modelMatrixZombie6[3][0], modelMatrixZombie6[3][2]);
	glm::mat4 modelMatrixZombieBody6 = glm::mat4(modelMatrixZombie6);
	modelMatrixZombieBody6 = glm::scale(modelMatrixZombieBody6, scaleZombie);
	zombieModelAnimate6.setAnimationIndex(animationIndexZombie6);
	zombieModelAnimate6.render(modelMatrixZombieBody6);

	modelMatrixZombie7[3][1] = terrain.getHeightTerrain(modelMatrixZombie7[3][0], modelMatrixZombie7[3][2]);
	glm::mat4 modelMatrixZombieBody7 = glm::mat4(modelMatrixZombie7);
	modelMatrixZombieBody7 = glm::scale(modelMatrixZombieBody7, scaleZombie);
	zombieModelAnimate7.setAnimationIndex(animationIndexZombie7);
	zombieModelAnimate7.render(modelMatrixZombieBody7);


	/*******************************************
	* Rayos
	*******************************************/
	//Ray in Player view direction
	glm::mat4 modelMatrixPlayerShootRay = glm::mat4(modelMatrixPlayer);
	modelMatrixPlayerShootRay = glm::translate(modelMatrixPlayerShootRay, glm::vec3(0, 10.0, 0));
	rayShootDirection = glm::normalize(glm::vec3(-modelMatrixPlayerShootRay[2]));
	rayShootOri = glm::vec3(modelMatrixPlayerShootRay[3]);
	rayShootTar = rayShootOri + 30.0f + rayShootDirection;
	glm::vec3 rayShootDmd = rayShootOri + 15.0f * rayShootDirection;
	modelMatrixPlayerShootRay[3] = glm::vec4(rayShootDmd, 1.0);
	modelMatrixPlayerShootRay = glm::rotate(modelMatrixPlayerShootRay, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
	//modelMatrixPlayerShootRay = glm::rotate(modelMatrixPlayerShootRay, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
	modelMatrixPlayerShootRay = glm::scale(modelMatrixPlayerShootRay, glm::vec3(1.0, 30.0, 1.0));
	if(renderEverything)
		cylinderShoot.render(modelMatrixPlayerShootRay);


	/*******************************************
	* Update the position with alpha objects
	*******************************************/
	// Update the aircraft
	//eliminate blendingUnsorted.find("aircraft")->second = glm::vec3(modelMatrixAircraft[3]);
	// Update the lambo
	//eliminate blendingUnsorted.find("lambo")->second = glm::vec3(modelMatrixLambo[3]);
	// Update the helicopter
	//eliminate blendingUnsorted.find("heli")->second = glm::vec3(modelMatrixHeli[3]);

	/*******************************************
	* Sorter with alpha objects
	*******************************************/
	std::map<float, std::pair<std::string, glm::vec3>> blendingSorted;
	std::map<std::string, glm::vec3>::iterator itblend;
	for (itblend = blendingUnsorted.begin(); itblend != blendingUnsorted.end();
			itblend++) {
		float distanceFromView = glm::length(
				camera->getPosition() - itblend->second);
		blendingSorted[distanceFromView] = std::make_pair(itblend->first,
				itblend->second);
	}

	/*******************************************
	* Render de las transparencias y particulas
	*******************************************/
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	for (std::map<float, std::pair<std::string, glm::vec3> >::reverse_iterator it =
			blendingSorted.rbegin(); it != blendingSorted.rend(); it++) {
		if (it->second.first.compare("aircraft") == 0) {
			
		} else if (it->second.first.compare("lambo") == 0) {
			
		} else if (it->second.first.compare("heli") == 0) {
			
		} else if (renderParticles && it->second.first.compare("fountain") == 0) {
			/**********
			 * Init Render particles systems
			 */
			glm::mat4 modelMatrixParticlesFountain = glm::mat4(1.0);
			modelMatrixParticlesFountain = glm::translate(
					modelMatrixParticlesFountain, it->second.second);
			modelMatrixParticlesFountain[3][1] = terrain.getHeightTerrain(
					modelMatrixParticlesFountain[3][0],
					modelMatrixParticlesFountain[3][2]) + 0.36 * 10.0;
			modelMatrixParticlesFountain = glm::scale(
					modelMatrixParticlesFountain, glm::vec3(3.0, 3.0, 3.0));
			currTimeParticlesAnimation = TimeManager::Instance().GetTime();
			if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
				lastTimeParticlesAnimation = currTimeParticlesAnimation;
			//glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			// Set the point size
			glPointSize(10.0f);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureParticleFountainID);
			shaderParticlesFountain.turnOn();
			shaderParticlesFountain.setFloat("Time",
					float(
							currTimeParticlesAnimation
									- lastTimeParticlesAnimation));
			shaderParticlesFountain.setFloat("ParticleLifetime", 3.5f);
			shaderParticlesFountain.setInt("ParticleTex", 0);
			shaderParticlesFountain.setVectorFloat3("Gravity",
					glm::value_ptr(glm::vec3(0.0f, -0.3f, 0.0f)));
			shaderParticlesFountain.setMatrix4("model", 1, false,
					glm::value_ptr(modelMatrixParticlesFountain));
			glBindVertexArray(VAOParticles);
			glDrawArrays(GL_POINTS, 0, nParticles);
			glDepthMask(GL_TRUE);
			//glEnable(GL_DEPTH_TEST);
			shaderParticlesFountain.turnOff();
			/**********
			 * End Render particles systems
			 */
		}
		else if (renderParticles && it->second.first.compare("fire") == 0) {
			/**********
			 * Init Render particles systems
			 */
			lastTimeParticlesAnimationFire = currTimeParticlesAnimationFire;
			currTimeParticlesAnimationFire = TimeManager::Instance().GetTime();

			shaderParticlesFire.setInt("Pass", 1);
			shaderParticlesFire.setFloat("Time",
					currTimeParticlesAnimationFire);
			shaderParticlesFire.setFloat("DeltaT",
					currTimeParticlesAnimationFire
							- lastTimeParticlesAnimationFire);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_1D, texId);
			glEnable(GL_RASTERIZER_DISCARD);
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
			glBeginTransformFeedback(GL_POINTS);
			glBindVertexArray(particleArray[1 - drawBuf]);
			glVertexAttribDivisor(0, 0);
			glVertexAttribDivisor(1, 0);
			glVertexAttribDivisor(2, 0);
			glDrawArrays(GL_POINTS, 0, nParticlesFire);
			glEndTransformFeedback();
			glDisable(GL_RASTERIZER_DISCARD);

			shaderParticlesFire.setInt("Pass", 2);
			glm::mat4 modelFireParticles = glm::mat4(1.0);
			modelFireParticles = glm::translate(modelFireParticles,
					it->second.second);
			modelFireParticles[3][1] = terrain.getHeightTerrain(
					modelFireParticles[3][0], modelFireParticles[3][2]);
			shaderParticlesFire.setMatrix4("model", 1, false,
					glm::value_ptr(modelFireParticles));

			shaderParticlesFire.turnOn();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
			glDepthMask(GL_FALSE);
			glBindVertexArray(particleArray[drawBuf]);
			glVertexAttribDivisor(0, 1);
			glVertexAttribDivisor(1, 1);
			glVertexAttribDivisor(2, 1);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesFire);
			glBindVertexArray(0);
			glDepthMask(GL_TRUE);
			drawBuf = 1 - drawBuf;
			shaderParticlesFire.turnOff();

			/****************************+
			 * Open AL sound data
			 */
			source0Pos[0] = modelFireParticles[3].x;
			source0Pos[1] = modelFireParticles[3].y;
			source0Pos[2] = modelFireParticles[3].z;
			alSourcefv(source[0], AL_POSITION, source0Pos);

			/**********
			 * End Render particles systems
			 */
		}

	}

	/****************************+
	* Open AL sound data
	*/

	//ZombieNormal 1
	sourceZ1NPos[0] = modelMatrixZombie1[3].x;
	sourceZ1NPos[1] = modelMatrixZombie1[3].y;
	sourceZ1NPos[2] = modelMatrixZombie1[3].z;
	alSourcefv(source[2], AL_POSITION, sourceZ1NPos);
	//ZombieNormal 2
	sourceZ2NPos[0] = modelMatrixZombie2[3].x;
	sourceZ2NPos[1] = modelMatrixZombie2[3].y;
	sourceZ2NPos[2] = modelMatrixZombie2[3].z;
	alSourcefv(source[3], AL_POSITION, sourceZ2NPos);
	//ZombieNormal 3
	sourceZ3NPos[0] = modelMatrixZombie3[3].x;
	sourceZ3NPos[1] = modelMatrixZombie3[3].y;
	sourceZ3NPos[2] = modelMatrixZombie3[3].z;
	alSourcefv(source[4], AL_POSITION, sourceZ3NPos);
	//ZombieNormal 4
	sourceZ4NPos[0] = modelMatrixZombie4[3].x;
	sourceZ4NPos[1] = modelMatrixZombie4[3].y;
	sourceZ4NPos[2] = modelMatrixZombie4[3].z;
	alSourcefv(source[5], AL_POSITION, sourceZ4NPos);
	//ZombieNormal 5
	sourceZ5NPos[0] = modelMatrixZombie5[3].x;
	sourceZ5NPos[1] = modelMatrixZombie5[3].y;
	sourceZ5NPos[2] = modelMatrixZombie5[3].z;
	alSourcefv(source[6], AL_POSITION, sourceZ5NPos);
	//ZombieNormal 6
	sourceZ6NPos[0] = modelMatrixZombie6[3].x;
	sourceZ6NPos[1] = modelMatrixZombie6[3].y;
	sourceZ6NPos[2] = modelMatrixZombie6[3].z;
	alSourcefv(source[7], AL_POSITION, sourceZ6NPos);
	//ZombieNormal 7
	sourceZ7NPos[0] = modelMatrixZombie7[3].x;
	sourceZ7NPos[1] = modelMatrixZombie7[3].y;
	sourceZ7NPos[2] = modelMatrixZombie7[3].z;
	alSourcefv(source[8], AL_POSITION, sourceZ7NPos);

	//ZombieDie 1
	sourceZ1DPos[0] = modelMatrixZombie1[3].x;
	sourceZ1DPos[1] = modelMatrixZombie1[3].y;
	sourceZ1DPos[2] = modelMatrixZombie1[3].z;
	alSourcefv(source[9], AL_POSITION, sourceZ1DPos);
	//ZombieDie 2
	sourceZ2DPos[0] = modelMatrixZombie2[3].x;
	sourceZ2DPos[1] = modelMatrixZombie2[3].y;
	sourceZ2DPos[2] = modelMatrixZombie2[3].z;
	alSourcefv(source[10], AL_POSITION, sourceZ2DPos);
	//ZombieDie 3
	sourceZ3DPos[0] = modelMatrixZombie3[3].x;
	sourceZ3DPos[1] = modelMatrixZombie3[3].y;
	sourceZ3DPos[2] = modelMatrixZombie3[3].z;
	alSourcefv(source[11], AL_POSITION, sourceZ3DPos);
	//ZombieDie 4
	sourceZ4DPos[0] = modelMatrixZombie4[3].x;
	sourceZ4DPos[1] = modelMatrixZombie4[3].y;
	sourceZ4DPos[2] = modelMatrixZombie4[3].z;
	alSourcefv(source[12], AL_POSITION, sourceZ4DPos);
	//ZombieDie 5
	sourceZ5DPos[0] = modelMatrixZombie5[3].x;
	sourceZ5DPos[1] = modelMatrixZombie5[3].y;
	sourceZ5DPos[2] = modelMatrixZombie5[3].z;
	alSourcefv(source[13], AL_POSITION, sourceZ5DPos);
	//ZombieDie 6
	sourceZ6DPos[0] = modelMatrixZombie6[3].x;
	sourceZ6DPos[1] = modelMatrixZombie6[3].y;
	sourceZ6DPos[2] = modelMatrixZombie6[3].z;
	alSourcefv(source[14], AL_POSITION, sourceZ6DPos);
	//ZombieDie 7
	sourceZ7DPos[0] = modelMatrixZombie7[3].x;
	sourceZ7DPos[1] = modelMatrixZombie7[3].y;
	sourceZ7DPos[2] = modelMatrixZombie7[3].z;
	alSourcefv(source[15], AL_POSITION, sourceZ7DPos);

	//CollectKeyA
	sourceKeyAPos[0] = modelMatrixKeyA[3].x;
	sourceKeyAPos[1] = modelMatrixKeyA[3].y;
	sourceKeyAPos[2] = modelMatrixKeyA[3].z;
	alSourcefv(source[16], AL_POSITION, sourceKeyAPos);
	//CollectKeyB
	sourceKeyBPos[0] = modelMatrixKeyB[3].x;
	sourceKeyBPos[1] = modelMatrixKeyB[3].y;
	sourceKeyBPos[2] = modelMatrixKeyB[3].z;
	alSourcefv(source[17], AL_POSITION, sourceKeyBPos);

	//CollectKeyA
	sourceWalkPos[0] = modelMatrixPlayer[3].x;
	sourceWalkPos[1] = modelMatrixPlayer[3].y;
	sourceWalkPos[2] = modelMatrixPlayer[3].z;
	alSourcefv(source[18], AL_POSITION, sourceWalkPos);
	glEnable(GL_CULL_FACE);
}

int main(int argc, char **argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}
