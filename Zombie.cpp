#include "Zombie.h"

zombie::zombie()
{
	vx = 0;
	vy = 0;

	size = SCREEN_HEIGHT / 7;
	type = 0;
	attackTimer = 0;
}

void zombie::init()
{
	//set random position
	bool ok = false;
	int randomX = 0;
	int randomY = 0;

	while (!ok)
	{
		randomX = GetRandomInt(size, LEVEL_WIDTH - size, 1);
		randomY = GetRandomInt(size, LEVEL_HEIGHT - size, 1);
		setPosition(randomX, randomY);
		if (calDistance(myPlayer) > SCREEN_WIDTH) //to avoid zombie spawning within the camera view
		{
			ok = true;
		}
	}
	setRenderPosition(px, py);

	//set random type
	int randomType = GetRandomInt(0, 8, 1);
	//randomType = 0; //temp
	if (randomType <= NORMAL_ZOMBIE_CHANCE) //normal zombie
	{
		type = 0;
		health = NORMAL_ZOMBIE_HEALTH;
		speed = NORMAL_ZOMBIE_SPEED;
		damage = NORMAL_ZOMBIE_DAMAGE;
		attackRange = NORMAL_ZOMBIE_ATTACK_RANGE;
		attackSpeed = NORMAL_ZOMBIE_ATTACK_SPEED;
	}
	else if (randomType <= NORMAL_ZOMBIE_CHANCE + FAST_ZOMBIE_CHANCE) //fast zombie
	{
		type = 1;
		health = FAST_ZOMBIE_HEALTH;
		speed = FAST_ZOMBIE_SPEED;
		damage = FAST_ZOMBIE_DAMAGE;
		attackRange = FAST_ZOMBIE_ATTACK_RANGE;
		attackSpeed = FAST_ZOMBIE_ATTACK_SPEED;
	}
	else if (randomType <= NORMAL_ZOMBIE_CHANCE + FAST_ZOMBIE_CHANCE + TANK_ZOMBIE_CHANCE) //tank zombie
	{
		type = 2;
		health = TANK_ZOMBIE_HEALTH;
		speed = TANK_ZOMBIE_SPEED;
		damage = TANK_ZOMBIE_DAMAGE;
		attackRange = TANK_ZOMBIE_ATTACK_RANGE;
		attackSpeed = TANK_ZOMBIE_ATTACK_SPEED;
	}
}

void zombie::move(gameObject target)
{
	currentState = zombieState::WALK;

	calRotation(target.px, target.py);

	float dirX = 0;
	float dirY = 0;

	dirX = -cos(rotation * M_PI / 180.0);
	dirY = -sin(rotation * M_PI / 180.0);

	vx = dirX * speed * systemTimer.getDeltaTime();
	vy = dirY * speed * systemTimer.getDeltaTime();

	px += vx;
	py += vy;

	setRenderPosition(px, py);
}

bool zombie::attack(player& target)
{
	currentState = zombieState::ATTACK;

	attackTimer += systemTimer.getDeltaTime();
	if (attackTimer > attackSpeed)
	{
		attackTimer = 0;
		target.health -= damage;
		return true;
	}
	return false;
}

void zombie::hurt()
{
	health -= myPlayer.myWeapon[myPlayer.currentWeapon].getDamage();
}

void zombie::setAnimation(LTexture& targetTexture, SDL_Rect& targetClip)
{
	currentTexture = &targetTexture;
	currentClip = &targetClip;
}

void zombie::render(SDL_Rect& camera)
{
	//change color based on the type
	switch (type)
	{
	case 0:
		currentTexture->setColor(255, 255, 255);
		break;
	case 1:
		currentTexture->setColor(100, 255, 100);
		break;
	case 2:
		currentTexture->setColor(100, 100, 255);
		break;
	}
	currentTexture -> render(rx - camera.x, ry - camera.y, size, size, currentClip, 180 + rotation);
}