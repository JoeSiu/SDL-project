#pragma once
#include "Include/Global.h"
#include "Include/Timer.h"

extern LTimer systemTimer;

class popUpText {
public:
	float duration;
	float interval;
	int currentLine;
	int currentPart;

	void showText();
	void hideText();
	bool check();
	void tick();
	bool onScreen();
	void setFlag(bool f);

	//set duration and interval
	popUpText(float d, float i);

private:
	float onScreenTimer;
	bool show;
	bool flag;
	float startTime;
};