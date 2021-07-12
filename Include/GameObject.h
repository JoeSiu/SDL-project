#pragma once
#include "Include/Global.h"
#include "Include/Graphics.h"

class gameObject
{
public:
	int size;
	double rotation;

	//game object's center position
	float px;
	float py;

	//game object render offset (top left corner)
	int rx;
	int ry;

	int type;

	void init(float x, float y, int s, double r, int t = -1);
	void setPosition(float x, float y);
	void setRenderPosition(float x, float y);
	void updateRenderPosition();
	void calRotation(int x, int y);
	float calDistance(gameObject target);
	bool checkCollision(gameObject target, float padding = 0);
	void resize(int s);
	void render(LTexture target);
	void render(LTexture target, SDL_Rect& camera);
};

void renderGameObject(SDL_Rect camera, LTexture& sourceTexture, std::vector<gameObject>& vectorList);
void renderGameObject(SDL_Rect camera, LTexture& sourceTexture, std::vector<gameObject>& vectorList, std::vector<SDL_Rect> clips);