#pragma once
#include "Include/Global.h"
#include "Include/GameObject.h"
#include "Include/Timer.h"

extern LTimer deltaTimer;

class bullet : public gameObject
{
public:
	int speed;
	float vx;
	float vy;

	bullet(SDL_Rect& camera, gameObject source, int targetX, int targetY);
	//~bullet();
	/*void create(gameObject source, int targetX, int targetY);*/
};

class weapon
{
public:
	weapon();

	int ammo;
	int getDamage();
	float getRateOfFire();
	int getClipSize();

	void init(std::string n, float d, float r, int a, int c, float t); //name, damage, rate of fire, ammo, clip size, reload time
	std::string getName();
	bool checkRateOfFire();
	bool checkAmmo();
	bool checkReload();
	bool getReloadFlag();
	void stopReload();
	bool fire();
	void reload();
	void resetTimer();

private:
	std::string name;
	float damage;
	float rateOfFire;
	int clipSize;
	float reloadTime;
	bool reloadFlag;

	float rateOfFireTimer;
	float reloadTimer;
};

