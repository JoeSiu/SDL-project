#include "Include/PopUpText.h"

popUpText::popUpText(float d, float i) {
	duration = d;
	interval = i;
	onScreenTimer = 0;
	currentLine = 0;
	currentPart = 0;
	show = false;
	flag = false;
	startTime = 0;
}

void popUpText::showText()
{
	show = true;
	startTime = systemTimer.getCurrentTime();
	onScreenTimer = duration;
}

void popUpText::hideText()
{
	show = false;
	startTime = systemTimer.getCurrentTime();
}

bool popUpText::check()
{
	systemTimer.calDeltaTime();
	tick();

	if (!show && systemTimer.getCurrentTime() - startTime > interval)
	{
		showText();
		return true;
	}
	if (show && onScreenTimer < 0)
	{
		hideText();
		setFlag(false);
		return false;
	}

	if (show)
	{
		return true;
	}
	if (!show)
	{
		return false;
	}

}

void popUpText::tick()
{
	if (show && onScreenTimer > 0)
	{
		onScreenTimer -= systemTimer.getDeltaTime();
	}
}

bool popUpText::onScreen()
{
	if (flag)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void popUpText::setFlag(bool f)
{
	flag = f;
}