#include "Weapon.h"

bullet::bullet(SDL_Rect& camera, gameObject source, int targetX, int targetY)
{
	size = BULLET_SIZE;

	px = source.px;
	py = source.py;

	setRenderPosition(px, py);

	speed = BULLET_SPEED * systemTimer.getDeltaTime();

	rotation = source.rotation; 

	float screenX = calOnScreenXPosition(camera, px);
	float screenY = calOnScreenYPosition(camera, py);

	float tempX = targetX - screenX;
	float tempY = targetY - screenY;

	float dirX = 0;
	float dirY = 0;

	dirX = cos(rotation * M_PI / 180.0);
	dirY = sin(rotation * M_PI / 180.0);

	vx = dirX * speed;
	vy = dirY * speed;
}

weapon::weapon()
{
	name = "";
	damage = 0;
	rateOfFire = 0;
	ammo = 0;
	clipSize = 0;
	rateOfFireTimer = 0;
}

void weapon::init(std::string n, float d, float r, int a, int c, float t)
{
	name = n;
	damage = d;
	rateOfFire = r;
	ammo = a;
	clipSize = c;
	reloadTime = t;
	reloadFlag = false;
	rateOfFireTimer = 0;
	reloadTimer = 0;
}

std::string weapon::getName()
{
	return name;
}

int weapon::getDamage()
{
	return damage;
}

float weapon::getRateOfFire()
{
	return rateOfFire;
}

int weapon::getClipSize()
{
	return clipSize;
}

bool weapon::checkRateOfFire()
{
	rateOfFireTimer += systemTimer.getDeltaTime();
	if (rateOfFireTimer > rateOfFire)
	{
		rateOfFireTimer = 0;
		return true;
	}
	else
	{
		return false;
	}
}

bool weapon::checkAmmo()
{
	if (ammo <= 0)
	{
		ammo = 0;
	}
	else
	{
		if (!reloadFlag)
		{
		return true;
		}
	}
		return false;
}

bool weapon::checkReload()
{
	if (reloadFlag)
	{
		if (reloadTimer <= 0)
		{
			reloadTimer = 0;
			reloadFlag = false;
			ammo = clipSize;
		} 
		else
		{
			reloadTimer -= systemTimer.getDeltaTime();
			//printf("reload timer = %f\n", reloadTimer);
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool weapon::getReloadFlag()
{
	return reloadFlag;
}

bool weapon::fire()
{
	if (checkRateOfFire() && checkAmmo())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void weapon::reload()
{
	if (!reloadFlag)
	{
		reloadFlag = true;
		reloadTimer = reloadTime;
	}

}

void weapon::stopReload()
{
	reloadFlag = false;
	reloadTimer = 0;
}

void weapon::resetTimer()
{
	rateOfFireTimer = rateOfFire;
}

