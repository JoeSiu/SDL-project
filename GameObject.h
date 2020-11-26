#pragma once
#include "Global.h"
#include "Graphics.h"

class gameObject {
public:
	int size;
	double rotation;

	//game object position
	float px;
	float py;

	//game object render offset
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