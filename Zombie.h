#pragma once
#include "Global.h"
#include "GameObject.h"
#include "Player.h"
#include "Timer.h"

extern LTimer systemTimer;
extern player myPlayer;

enum class zombieState
{
	WALK, ATTACK, DEAD
};

class zombie : public gameObject
{
public:
	zombieState currentState;

	int vx;
	int vy;

	float health;
	float speed;
	float damage;
	float attackRange;
	float attackSpeed; //attack speed in second

	//animation related
	LTexture* currentTexture;
	SDL_Rect* currentClip;
	int currentFrame;
	int currentTotalFrame;

	zombie();
	void init();
	void move(gameObject target);
	bool attack(player& target);
	void hurt();
	void setAnimation(LTexture& targetTexture, SDL_Rect& targetClip);
	void render(SDL_Rect& camera);

private:
	float attackTimer;
};