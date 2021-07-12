#include "Include/Graphics.h"

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

bool LTexture::loadFromRenderedText(std::string textureText, TTF_Font* font, SDL_Color textColor)
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor);
	if (textSurface != NULL)
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}
	else
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}


	//Return success
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
{
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

SDL_Texture* LTexture::getSDLTexture()
{
	return mTexture;
}

void LTexture::render(int x, int y, int w, int h, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, w, h };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w * 0.5;
		renderQuad.h = clip->h * 0.5;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

void LTexture::render(SDL_Rect& camera, int x, int y, int w, int h, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x - camera.x, y - camera.y, w, h };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;// *0.5;
		renderQuad.h = clip->h;// *0.5;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

int drawText(int x, int y, TTF_Font* font, SDL_Color color, std::string text, int anchorMode)
{
	int textWidth;
	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text.c_str(), color);

	// render the text surface
	if (surfaceMessage == NULL)
	{
		printf("surfaceMessage could not be created! SDL Error: %s\n", TTF_GetError());
	}
	else
	{
		SDL_Texture* Message = SDL_CreateTextureFromSurface(gRenderer, surfaceMessage);
		if (Message == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else {
			SDL_Rect Message_rect;
			Message_rect.w = surfaceMessage->w;
			Message_rect.h = surfaceMessage->h;
			textWidth = Message_rect.w;

			//set a ancher point for the x and y
			switch (anchorMode)
			{
			case 0: //top left
				Message_rect.x = x;
				Message_rect.y = y;
				break;
			case 1: //top mid
				Message_rect.x = x - (Message_rect.w / 2);
				Message_rect.y = y;
				break;
			case 2: //top right
				Message_rect.x = x - Message_rect.w;
				Message_rect.y = y;
				break;
			}

			//draw text
			SDL_RenderCopy(gRenderer, Message, NULL, &Message_rect);

			//free surface and texture
			SDL_FreeSurface(surfaceMessage);
			SDL_DestroyTexture(Message);

			//return the width of the text
			return textWidth;
		}
	}
	return 0;
}