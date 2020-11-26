#include "Player.h"

player::player()
{
	init(LEVEL_WIDTH / 2, LEVEL_HEIGHT / 2, PLAYER_SIZE, -1);
	speed = PLAYER_SPEED;
	health = 100;
	vx = 0;
	vy = 0;
	currentFrame = 0;
	currentTotalFrame = 0;
	currentWeapon = 0;
	currentState = playerState::IDLE;
	previousState = playerState::IDLE;
	for (int i = 0; i < PLAYER_WEAPON_SLOT; i++)
	{
		myWeapon[i].init("temp", 0, 0, 0, 0, 0);
	}
}

void player::initPlayer()
{
	init(LEVEL_WIDTH / 2, LEVEL_HEIGHT / 2, SCREEN_HEIGHT / 7, -1);
	speed = PLAYER_SPEED;
	health = 100;
	vx = 0;
	vy = 0;
	currentFrame = 0;
	currentTotalFrame = 0;
	currentWeapon = 0;
	currentState = playerState::IDLE;
	previousState = playerState::IDLE;
}

void player::calRotation(SDL_Rect& camera, int x, int y)
{
	int deltaX;
	int deltaY;
	//If player reached the edge of the level
	if (camera.x <= 0 || camera.y <= 0 || camera.x >= LEVEL_WIDTH - camera.w || camera.y >= LEVEL_HEIGHT - camera.h)
	{
		deltaX = px - camera.x - x;
		deltaY = py - camera.y - y;
	}
	else
	{
		deltaX = (SCREEN_WIDTH / 2 - size / 2) - x;
		deltaY = (SCREEN_HEIGHT / 2 - size / 2) - y;
	}

	rotation = 180 + (atan2(deltaY, deltaX) * 180.0000) / M_PI;
}

void player::cycleWeapon()
{
	myWeapon[currentWeapon].stopReload();
	currentWeapon++;
	if (currentWeapon >= PLAYER_WEAPON_SLOT)
	{
		currentWeapon = 0;
	}
}

void player::setAnimation(LTexture& targetTexture, SDL_Rect& targetClip)
{
	currentTexture = &targetTexture;
	currentClip = &targetClip;
}

void player::render(SDL_Rect& camera)
{
	currentTexture->render(rx - camera.x, ry - camera.y, size, size, currentClip, rotation, NULL, SDL_FLIP_NONE);
}