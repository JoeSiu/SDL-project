#include "Button.h"

void button::init(int x, int y, int s, std::string t, TTF_Font* f)
{
	px = float(x);
	py = float(y);
	size = s;
	text = t;
    font = f;
    h = size;
    w = h * 3.5;
    rx = px - w / 2;
    ry = py - h / 7.5;
    currentState = buttonState::DEFAULT;
}

bool button::checkInside(int x, int y)
{
    if (x > px - w/2 &&
        x < px + w/2 &&
        y > py - h/2 &&
        y < py + h) {
        return true;
    }
    else {
        return false;
    }
}

void button::checkButton(Uint32 mouses, int x, int y)
{
    currentState = buttonState::DEFAULT;
    //check mouse input
    //if mouse is on the button
    if (checkInside(x, y))
    {
        currentState = buttonState::HOVERED;
        //if mouse is pressing the button
        if (mouses & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            currentState = buttonState::PRESSED;
        }
    }
}

void button::setText(std::string t)
{
    text = t;
}


void button::render(SDL_Renderer* target)
{
    //TODO: fix render
    //printf("px = %f, py = %f, rx = %i, ry = %i\n", px, py, rx, ry);
    SDL_Rect buttonRect{ rx, ry, w, h };
    
    //draw button rect
    switch (currentState)
    {
    case buttonState::DEFAULT:
        SDL_SetRenderDrawColor(target, 255, 255, 255, 255);
        SDL_RenderDrawRect(target, &buttonRect);
        break;
    case buttonState::HOVERED:
        SDL_SetRenderDrawColor(target, 100, 100, 100, 100);
        SDL_RenderFillRect(target, &buttonRect);
        SDL_SetRenderDrawColor(target, 200, 200, 200, 255);
        SDL_RenderDrawRect(target, &buttonRect);
        break;
    case buttonState::PRESSED:
        SDL_SetRenderDrawColor(target, 50, 50, 50, 100);
        SDL_RenderFillRect(target, &buttonRect);
        SDL_SetRenderDrawColor(target, 150, 150, 150, 255);
        SDL_RenderDrawRect(target, &buttonRect);
        break;
    }
   
    //draw text
    SDL_Color color = { 255, 255, 255 };
    drawText(px, py, font, color, text, 1);
}