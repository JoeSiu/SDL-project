#pragma once
#include "Include/Global.h"
#include "Include/GameObject.h"
#include "Include/Graphics.h"

enum class buttonState
{
	DEFAULT, HOVERED, PRESSED
};

class button: public gameObject
{
public:
	std::string text;
	buttonState currentState;

	void init(int x, int y, int s, std::string t, TTF_Font* f);
	bool checkInside(int x, int y);
	void checkButton(Uint32 mouses, int x, int y);
	void setText(std::string t);
	void render(SDL_Renderer* target);

private:
	int w;
	int h;
	TTF_Font* font;
};