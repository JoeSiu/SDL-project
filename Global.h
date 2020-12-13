#pragma once

#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2_image.lib")
#pragma comment(lib, "SDL2_mixer.lib")
#pragma comment(lib, "SDL2_ttf.lib")

//include SDL stuff
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

//include ability to exit program
#include <stdio.h>
#include <stdlib.h>

//include other stuffs
#include <stack>
#include <string>
#include <ctime>
#include <cmath>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

//render window
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;

//frame rate
const int SCREEN_FPS = 60;
const int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS;

//screen size
static int SCREEN_WIDTH = 1280;
static int SCREEN_HEIGHT = 720;
const int V_BORDER = SCREEN_WIDTH / 50;
const int H_BORDER = SCREEN_HEIGHT / 50;

//dimensions of the level
const int GROUND_TILE_SIZE = 512;
const int LEVEL_SIZE = 25;
const int LEVEL_WIDTH = GROUND_TILE_SIZE * LEVEL_SIZE;
const int LEVEL_HEIGHT = GROUND_TILE_SIZE * LEVEL_SIZE;

//texture related
const int TREES_CLIP = 14;
const int CROSSHAIR_SIZE = SCREEN_HEIGHT / 35;

//level elements' properties
static int DIFFICULTY = 1; //difficulty will increase the maximum zombie numbers when spawning
const int DIFFICULTY_REQUIREMENT = 10; //the required zombies to kill in order to advance to next level
const int ZOMBIE_NUMBER_STEP = 5; //how many zombie added for each difficulty
const int COLLIDER_TOLERANCE = 2; //the collider size tolerance for the game objects
//trees
const int MAX_TREE_NUM = 10 * LEVEL_SIZE;
static int MIN_TREE_SIZE = SCREEN_HEIGHT / 3;
static int MAX_TREE_SIZE = SCREEN_HEIGHT / 2;
//harm zones
const int MAX_HARM_ZONE_NUM = 5 * LEVEL_SIZE;
const int MIN_HARM_ZONE_SIZE = SCREEN_HEIGHT / 2;
const int MAX_HARM_ZONE_SIZE = SCREEN_HEIGHT;
const int MAX_HARM_ZONE_DAMAGE = 20;
//blood pool
const int MAX_BLOOD_POOL_NUM = 50;

//objectives
const int TIME_LIMIT = 360; //total game time in second
const int TOTAL_OBJECTIVE = 5;
const int ZOMBIE_NEEDED_TO_KILL = 15; //for objective 4

//player properties
const int PLAYER_SIZE = 100;
const int PLAYER_STATE = 4;
const int PLAYER_SPEED = 250;
const int PLAYER_WEAPON_SLOT = 2;

//weapon properties
const int BULLET_SPEED = 2500;
const int BULLET_SIZE = 15;
//pistol
static std::string PISTOL_NAME = "pistol";
static float PISTOL_DAMAGE = 5.0f;
static float PISTOL_RATE_OF_FIRE = 0.75f;
static int PISTOL_CLIP_SIZE = 12;
static float PISTOL_RELOAD_TIME = 1.0f;
//rifle
static std::string RIFLE_NAME = "assault rifle";
static float RIFLE_DAMAGE = 3.5f;
static float RIFLE_RATE_OF_FIRE = 0.2f;
static int RIFLE_CLIP_SIZE = 30;
static float RIFLE_RELOAD_TIME = 2.0f;

//zombie properties
static int MAX_ZOMBIE_NUM = DIFFICULTY_REQUIREMENT;
//normal zombie
const int NORMAL_ZOMBIE_CHANCE = 5;
const float NORMAL_ZOMBIE_HEALTH = 20;
const float NORMAL_ZOMBIE_SPEED = PLAYER_SPEED * 1.1;
const float NORMAL_ZOMBIE_DAMAGE = 2.5f;
const float NORMAL_ZOMBIE_ATTACK_RANGE = 30.0f;
const float NORMAL_ZOMBIE_ATTACK_SPEED = 0.35f;
//fast zombie
const int FAST_ZOMBIE_CHANCE = 3;
const float FAST_ZOMBIE_HEALTH = 15;
const float FAST_ZOMBIE_SPEED = PLAYER_SPEED * 1.5;
const float FAST_ZOMBIE_DAMAGE = 1.5f;
const float FAST_ZOMBIE_ATTACK_RANGE = 20.0f;
const float FAST_ZOMBIE_ATTACK_SPEED = 0.15f;
//tank zombie
const int TANK_ZOMBIE_CHANCE = 2;
const float TANK_ZOMBIE_HEALTH = 50;
const float TANK_ZOMBIE_SPEED = PLAYER_SPEED * 0.75;
const float TANK_ZOMBIE_DAMAGE = 10.0f;
const float TANK_ZOMBIE_ATTACK_RANGE = 20.0f;
const float TANK_ZOMBIE_ATTACK_SPEED = 0.75f;

//animations properties
const float ANIMATION_INTERVAL = 0.05f; //animation interval until next frame in second
//player
//idle
const int PLAYER_PISTOL_IDLE_ANIMATION_FRAMES = 20;
const int PLAYER_RIFLE_IDLE_ANIMATION_FRAMES = 20;
//walk
const int PLAYER_PISTOL_WALK_ANIMATION_FRAMES = 20;
const int PLAYER_RIFLE_WALK_ANIMATION_FRAMES = 20;
//fire
const int PLAYER_PISTOL_FIRE_ANIMATION_FRAMES = 3;
const int PLAYER_RIFLE_FIRE_ANIMATION_FRAMES = 3;
//reload
const int PLAYER_PISTOL_RELOAD_ANIMATION_FRAMES = 15;
const int PLAYER_RIFLE_RELOAD_ANIMATION_FRAMES = 20;
//zombie
const int ZOMBIE_WALK_ANIMATION_FRAMES = 17;
const int ZOMBIE_ATTACK_ANIMATION_FRAMES = 9;

//settings
static bool setting_Music = true;

int GetRandomInt(int min_value, int max_value, int step);
float GetRandomFloat(float min_value, float max_value, float step);
int map(int x, int in_min, int in_max, int out_min, int out_max);
float calOnScreenXPosition(SDL_Rect& camera, int targetX);
float calOnScreenYPosition(SDL_Rect& camera, int targetY);
float calDistance(float x1, float y1, float x2, float y2);

struct StateStruct
{
	void (*StatePointer)();
};
