#pragma once
#include "Include/GameObject.h"
#include "Include/Weapon.h"

enum class playerState
{
	IDLE, WALK, FIRE, RELOAD, DEAD
};

class player : public  gameObject 
{
public:
	//player stat
	float health;
	int speed;
	int vx;
	int vy;
	
	playerState currentState;
	playerState previousState;

	//animation related
	LTexture* currentTexture;
	SDL_Rect* currentClip;
	int currentFrame;
	int currentTotalFrame;

	//weapon related
	int currentWeapon;
	weapon myWeapon[PLAYER_WEAPON_SLOT];


	player();
	void initPlayer();
	void cycleWeapon();
	void setAnimation(LTexture& targetTexture, SDL_Rect& targetClip);
	void render(SDL_Rect& camera);
	void calRotation(SDL_Rect& camera, int x, int y);
};
