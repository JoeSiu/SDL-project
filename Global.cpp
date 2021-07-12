#include "Include/Global.h"

int GetRandomInt(int min_value, int max_value, int step)
{
	max_value += step;
	int random_value = (rand() % ((++max_value - min_value) / step)) * step + min_value;
	return random_value;
}

float GetRandomFloat(float min_value, float max_value, float step)
{
	max_value += step;
	float random_value = (std::fmodf(rand(), ((++max_value - min_value) / step)) * step + min_value);
	return random_value;
}

int map(int x, int in_min, int in_max, int out_min, int out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


float calOnScreenXPosition(SDL_Rect& camera, int targetX)
{
	return ((targetX - camera.x) + (targetX - camera.x)) / 2;
}

float calOnScreenYPosition(SDL_Rect& camera, int targetY)
{
	return((targetY - camera.y) + (targetY - camera.y)) / 2;
}

float calDistance(float x1, float y1, float x2, float y2)
{
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}