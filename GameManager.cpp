#include "Include/Global.h"
#include "Include/Graphics.h"
#include "Include/Player.h"
#include "Include/Zombie.h"
#include "Include/Weapon.h"
#include "Include/Timer.h"
#include "Include/Audio.h"
#include "Include/PopUpText.h"
#include "Include/Button.h"

#pragma region Create_Variables
#pragma region Rendering
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
#pragma endregion

#pragma region Menus
std::stack<StateStruct> g_StateStack;
std::stack<StateStruct> emptyStack; //for clearing stack

//menu screen
void Menu();
void handleMenuEvent(int& choice);

//game screen
void Game();
void handleGameEvent();
void handleGameInput();

//pause screen
void Pause();
bool paused = false; //flag
void handlePauseEvent();

//confirm screen
void Confirm();
enum class confirmState { FALSE, RETRY, QUIT, QUIT_TO_MENU };
confirmState confirmMode;
void showConfirmScreen(confirmState m);
void hideConfirmScreen();
bool confirmScreen = false; //flag
void handleConfirmEvent(int& choice);

//end game screen
void EndGame();
enum class endState { FALSE, WIN, LOSE, TIME_OVER };
endState endGameMode = endState::FALSE;
void showEndGamecreen(endState m);
void hideEndGameScreen();
bool endGameScreen = false; //flag
void handleEndGameEvent(int& choice);

//exit
void Exit();

void clearScreen();
#pragma endregion

#pragma region Game_Objects
player myPlayer;
zombie myZombie;
gameObject myTree;
gameObject myHarmZone;
gameObject mySignalZone;
gameObject myHealthPickup;
#pragma endregion

#pragma region Textures
//static textures
LTexture gGroundTexture;
LTexture gTreeTexture;
LTexture gBulletTexture;
LTexture gWhiteTexture;
LTexture gBloodPoolTexture;
//corsshair
LTexture gCrosshairTexture;
//screen effect textures
LTexture gVignetteTexture;
LTexture gLightTexture;
LTexture gLensDirtTexture;
LTexture gBloodOverlayTexture;
//UI textures
LTexture gHealthIconTexture;
LTexture gPistolIconTexture;
LTexture gRifleIconTexture;
//health pickup texture
LTexture gHealthPickUpTexture;
//background texture for menu
LTexture gMenuTexture;
//backdrop texture used for pause screen
SDL_Texture* backdrop;
#pragma endregion

#pragma region Colors
SDL_Color whiteColor = { 255, 255, 255 };
SDL_Color blackColor = { 0, 0, 0 };
SDL_Color UIColor = whiteColor;
#pragma endregion

#pragma region Animations
double animationTimeCounter = 0;
//player
//animations: pistol
std::map<playerState, LTexture> gPlayerPistolTexture;
std::map<playerState, std::vector <SDL_Rect>> gPlayerPistolClips;
//animations: rifle
std::map<playerState, LTexture> gPlayerRifleTexture;
std::map<playerState, std::vector <SDL_Rect>> gPlayerRifleClips;
//zombie
std::map<zombieState, LTexture> gZombieTexture;
std::map<zombieState, std::vector <SDL_Rect>> gZombieClips;
#pragma endregion

#pragma region Trees
std::vector <SDL_Rect> gTreeClips;
#pragma endregion

#pragma region Audio
audioManager myAudio;
#pragma endregion

#pragma region Fonts
const int fontSize = SCREEN_HEIGHT / 30;
const int fontSizeSmall = SCREEN_HEIGHT / 40;
const int fontSizeLarge = SCREEN_HEIGHT / 20;
const int fontSizeTitle = SCREEN_HEIGHT / 7.5;
TTF_Font* boldFont;
TTF_Font* regularFont;
TTF_Font* boldFontSmall;
TTF_Font* boldFontLarge;
TTF_Font* boldFontTitle;
TTF_Font* regularFontSmall;
#pragma endregion

#pragma region Objectives
int totalZombieKilled = 0;
bool tutorial = true;
int currentObjective = -1;
bool objective[TOTAL_OBJECTIVE];
std::string objectiveText;
int timeLeft = TIME_LIMIT;
//objective goals
int obj_zombieKilled = 0;
bool obj_keyPressed[4] = { false, false, false, false }; //whether w, a, s, d have been pressed
int obj_zones;
#pragma endregion

#pragma region Dialogues
std::vector <std::string> dialogueLine; //dialogue texts
std::vector <std::string> tipsLine; //tips texts
popUpText dialogue(3.0f, 1.0f);
popUpText dialogueTips(3.0f, 2.0f);
#pragma endregion

#pragma region Framerate
LTimer systemTimer; //The frames per second timer
LTimer deltaTimer; //The frames per second cap timer
int countedFrames = 0; //total frames
#pragma endregion

#pragma region Input
int mouseX;
int mouseY;
const Uint8* keys;
Uint32 mouses;
#pragma endregion

#pragma region Flags
bool allowSpawnZombie = false; //flag for zombie spawning
bool initedLevel = false;
bool quit = false;
bool cheat = false;
#pragma endregion

#pragma region Event_Handler
SDL_Event event;
#pragma endregion

#pragma region Camera
SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
#pragma endregion

#pragma region Vectors
std::vector<gameObject> trees;
std::vector<gameObject> harmZones;
std::vector<zombie> zombies;
std::vector<gameObject> bloodpools;
std::vector<bullet> bullets;
std::vector<gameObject> signalZones;
std::vector<gameObject> healthPickUps;
#pragma endregion

#pragma region Button
button myButton;
std::vector<button> buttons;
#pragma endregion
#pragma endregion

#pragma region Init_And_Load_Media
bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Initialize SDL_ttf
		if (TTF_Init() == -1)
		{
			printf("Unable to init TTF: %s\n", SDL_GetError());
			success = false;
		}

		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("SM2603 Project", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN); //SDL_WINDOW_RESIZABLE
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				//Initialize SDL_mixer
				//new code for the sound here==================
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) < 0)
				{
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
					success = false;
				}
				Mix_AllocateChannels(48); //allocate audio channels
			}
		}
	}

	//set base scaling
	SDL_RenderSetLogicalSize(gRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);

	StateStruct state;
	//add menu 
	state.StatePointer = Menu;
	g_StateStack.push(state);
	return success;
}

void loadSpritesheet(enum playerState state, std::map<playerState, LTexture>& spritesheet,
	std::map<playerState, std::vector <SDL_Rect>>& spritesheetClip, int totalFrame)
{
	int w = spritesheet[state].getWidth() / totalFrame;
	int h = spritesheet[state].getHeight();
	for (int i = 0; i < totalFrame; i++)
	{
		spritesheetClip[state].push_back({ i * w, 0, w , h });
	}
}

void loadSpritesheet(enum zombieState state, std::map<zombieState, LTexture>& spritesheet,
	std::map<zombieState, std::vector <SDL_Rect>>& spritesheetClip, int totalFrame)
{
	int w = spritesheet[state].getWidth() / totalFrame;
	int h = spritesheet[state].getHeight();
	for (int i = 0; i < totalFrame; i++)
	{
		spritesheetClip[state].push_back({ i * w, 0, w , h });
	}
}

void loadSpritesheet(LTexture& spritesheet, std::vector <SDL_Rect>& spritesheetClip, int totalFrame)
{
	int w = spritesheet.getWidth() / totalFrame;
	int h = spritesheet.getHeight();
	for (int i = 0; i < totalFrame; i++)
	{
		spritesheetClip.push_back({ i * w, 0, w , h });
	}
}

void loadClips(LTexture& spritesheet, std::vector<SDL_Rect>& spritesheetClip, int totalClip)
{
	int w = spritesheet.getWidth() / totalClip;
	int h = spritesheet.getHeight();
	for (int i = 0; i < totalClip; i++)
	{
		spritesheetClip.push_back({ i * w, 0, w , h });
	}
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

#pragma region Load_static_textures
	//Load static texture
	if (!gGroundTexture.loadFromFile("Assets/Texture/ground.png"))
	{
		printf("Failed to load player texture!\n");
		success = false;
	}

	if (!gTreeTexture.loadFromFile("Assets/Texture/trees.png"))
	{
		printf("Failed to load tree texture!\n");
		success = false;
	}
	else
	{
		loadClips(gTreeTexture, gTreeClips, TREES_CLIP);
	}

	if (!gCrosshairTexture.loadFromFile("Assets/Texture/crosshair.png"))
	{
		printf("Failed to load player texture!\n");
		success = false;
	}

	if (!gBulletTexture.loadFromFile("Assets/Texture/bullet.png"))
	{
		printf("Failed to load bullet texture!\n");
		success = false;
	}

	if (!gWhiteTexture.loadFromFile("Assets/Texture/white.png"))
	{
		printf("Failed to load white texture!\n");
		success = false;
	}

	if (!gBloodPoolTexture.loadFromFile("Assets/Texture/blood pool.png"))
	{
		printf("Failed to load white texture!\n");
		success = false;
	}

	if (!gHealthPickUpTexture.loadFromFile("Assets/Texture/health pickup.png"))
	{
		printf("Failed to load health pickup texture!\n");
		success = false;
	}

	if (!gMenuTexture.loadFromFile("Assets/Texture/menu background.jpg"))
	{
		printf("Failed to load menu background texture!\n");
		success = false;
	}
#pragma endregion

#pragma region Load_animations_textures
	//Load animation textures
	//player
	//pistol
	//idle
	if (!gPlayerPistolTexture[playerState::IDLE].loadFromFile("Assets/Texture/spritesheets/player/pistol/pistol idle.png"))
	{
		printf("Failed to load player pistol idle texture!\n");
		success = false;
	}
	else
	{
		loadSpritesheet(playerState::IDLE, gPlayerPistolTexture, gPlayerPistolClips, PLAYER_PISTOL_IDLE_ANIMATION_FRAMES);
	}
	//walk
	if (!gPlayerPistolTexture[playerState::WALK].loadFromFile("Assets/Texture/spritesheets/player/pistol/pistol walk.png"))
	{
		printf("Failed to load player pistol walk texture!\n");
		success = false;
	}
	else
	{
		loadSpritesheet(playerState::WALK, gPlayerPistolTexture, gPlayerPistolClips, PLAYER_PISTOL_WALK_ANIMATION_FRAMES);
	}
	//fire
	if (!gPlayerPistolTexture[playerState::FIRE].loadFromFile("Assets/Texture/spritesheets/player/pistol/pistol fire.png"))
	{
		printf("Failed to load player pistol fire texture!\n");
		success = false;
	}
	else
	{
		loadSpritesheet(playerState::FIRE, gPlayerPistolTexture, gPlayerPistolClips, PLAYER_PISTOL_FIRE_ANIMATION_FRAMES);
	}
	//reload
	if (!gPlayerPistolTexture[playerState::RELOAD].loadFromFile("Assets/Texture/spritesheets/player/pistol/pistol reload.png"))
	{
		printf("Failed to load player pistol reload texture!\n");
		success = false;
	}
	else
	{
		loadSpritesheet(playerState::RELOAD, gPlayerPistolTexture, gPlayerPistolClips, PLAYER_PISTOL_RELOAD_ANIMATION_FRAMES);
	}
	//rifle
	//idle
	if (!gPlayerRifleTexture[playerState::IDLE].loadFromFile("Assets/Texture/spritesheets/player/rifle/rifle idle.png"))
	{
		printf("Failed to load player rifle idle texture!\n");
		success = false;
	}
	else
	{
		loadSpritesheet(playerState::IDLE, gPlayerRifleTexture, gPlayerRifleClips, PLAYER_RIFLE_IDLE_ANIMATION_FRAMES);
	}
	//walk
	if (!gPlayerRifleTexture[playerState::WALK].loadFromFile("Assets/Texture/spritesheets/player/rifle/rifle walk.png"))
	{
		printf("Failed to load player rifle walk texture!\n");
		success = false;
	}
	else
	{
		loadSpritesheet(playerState::WALK, gPlayerRifleTexture, gPlayerRifleClips, PLAYER_RIFLE_WALK_ANIMATION_FRAMES);
	}
	//fire
	if (!gPlayerRifleTexture[playerState::FIRE].loadFromFile("Assets/Texture/spritesheets/player/rifle/rifle fire.png"))
	{
		printf("Failed to load player rifle fire texture!\n");
		success = false;
	}
	else
	{
		loadSpritesheet(playerState::FIRE, gPlayerRifleTexture, gPlayerRifleClips, PLAYER_RIFLE_FIRE_ANIMATION_FRAMES);
	}
	//reload
	if (!gPlayerRifleTexture[playerState::RELOAD].loadFromFile("Assets/Texture/spritesheets/player/rifle/rifle reload.png"))
	{
		printf("Failed to load player rifle reload texture!\n");
		success = false;
	}
	else
	{
		loadSpritesheet(playerState::RELOAD, gPlayerRifleTexture, gPlayerRifleClips, PLAYER_RIFLE_RELOAD_ANIMATION_FRAMES);
	}

	//zombie
	//walk
	if (!gZombieTexture[zombieState::WALK].loadFromFile("Assets/Texture/spritesheets/zombie/zombie walk.png"))
	{
		printf("Failed to load zombie walk texture!\n");
		success = false;
	}
	else
	{
		loadSpritesheet(zombieState::WALK, gZombieTexture, gZombieClips, ZOMBIE_WALK_ANIMATION_FRAMES);
	}
	//attack
	if (!gZombieTexture[zombieState::ATTACK].loadFromFile("Assets/Texture/spritesheets/zombie/zombie attack.png")) //TODO: change to zombie attack
	{
		printf("Failed to load zombie attack texture!\n");
		success = false;
	}
	else
	{
		loadSpritesheet(zombieState::ATTACK, gZombieTexture, gZombieClips, ZOMBIE_ATTACK_ANIMATION_FRAMES);
	}
#pragma endregion

#pragma region Load_screen_effect_textures
	//Load screen effect texture
	if (!gVignetteTexture.loadFromFile("Assets/Texture/vignette.png"))
	{
		printf("Failed to load vignette texture!\n");
		success = false;
	}

	if (!gLightTexture.loadFromFile("Assets/Texture/light halo.png"))
	{
		printf("Failed to load light halo texture!\n");
		success = false;
	}

	if (!gLensDirtTexture.loadFromFile("Assets/Texture/lens dirt.png"))
	{
		printf("Failed to load len dirt texture!\n");
		success = false;
	}

	if (!gBloodOverlayTexture.loadFromFile("Assets/Texture/blood overlay.png"))
	{
		printf("Failed to load blood overlay texture!\n");
		success = false;
	}
#pragma endregion

#pragma region Load_UI_textures
	//Load UI texture
	if (!gHealthIconTexture.loadFromFile("Assets/Texture/health icon.png"))
	{
		printf("Failed to load health icon texture!\n");
		success = false;
	}

	if (!gPistolIconTexture.loadFromFile("Assets/Texture/pistol icon.png"))
	{
		printf("Failed to load pistol icon texture!\n");
		success = false;
	}

	if (!gRifleIconTexture.loadFromFile("Assets/Texture/rifle icon.png"))
	{
		printf("Failed to load rifle icon texture!\n");
		success = false;
	}
#pragma endregion

#pragma region Load_audio
	//Load audio
	myAudio.loadAudio();
#pragma endregion

#pragma region Load_text_font
	//Load text font
	boldFont = TTF_OpenFont("Assets/Font/OpenSans-Bold.ttf", fontSize);
	if (boldFont == NULL) {
		printf("Failed to load bold font! TTF Error: %s\n", TTF_GetError());
		success = false;
	}

	boldFontSmall = TTF_OpenFont("Assets/Font/OpenSans-Bold.ttf", fontSizeSmall);
	if (boldFontSmall == NULL) {
		printf("Failed to load bold font small! TTF Error: %s\n", TTF_GetError());
		success = false;
	}

	boldFontLarge = TTF_OpenFont("Assets/Font/OpenSans-Bold.ttf", fontSizeLarge);
	if (boldFontLarge == NULL) {
		printf("Failed to load bold font large! TTF Error: %s\n", TTF_GetError());
		success = false;
	}

	boldFontTitle = TTF_OpenFont("Assets/Font/OpenSans-Bold.ttf", fontSizeTitle);
	if (boldFontTitle == NULL) {
		printf("Failed to load bold font title! TTF Error: %s\n", TTF_GetError());
		success = false;
	}

	regularFont = TTF_OpenFont("Assets/Font/OpenSans-Regular.ttf", fontSize);
	if (regularFont == NULL) {
		printf("Failed to load regular font! TTF Error: %s\n", TTF_GetError());
		success = false;
	}

	regularFontSmall = TTF_OpenFont("Assets/Font/OpenSans-Regular.ttf", fontSizeSmall);
	if (regularFontSmall == NULL) {
		printf("Failed to load regular font small! TTF Error: %s\n", TTF_GetError());
		success = false;
	}
#pragma endregion

#pragma region Load_text_line
	//Load dialogues line
	std::string str;
	std::ifstream dialoguesFile("Assets/dialogues.txt");
	if (dialoguesFile.fail()) {
		printf("Failed to load dialogue file!\n");
		success = false;
	}
	else {
		while (std::getline(dialoguesFile, str))
		{
			dialogueLine.push_back(str);
		}
	}

	std::ifstream tipsFile("Assets/tips.txt");
	if (tipsFile.fail()) {
		printf("Failed to load dialogue file!\n");
		success = false;
	}
	else {
		while (std::getline(tipsFile, str))
		{
			tipsLine.push_back(str);
		}
	}
#pragma endregion

	return success;
}

void createGameObjectRandom(gameObject source, std::vector<gameObject>& vectorList, int total, int minSize, int maxSize, int rotation = 360, int maxType = -1)
{
	for (int i = 0; i < total; i++)
	{
		bool ok = false;
		int randomSize = 0;
		int randomX = 0;
		int randomY = 0;

		while (!ok)
		{
			randomSize = GetRandomInt(minSize, maxSize, 1);
			randomX = GetRandomInt(randomSize, LEVEL_WIDTH - randomSize, 1);
			randomY = GetRandomInt(randomSize, LEVEL_HEIGHT - randomSize, 1);
			if (calDistance(randomX, randomY, myPlayer.px, myPlayer.py) > randomSize + myPlayer.size)
			{
				ok = true;
			}
		}

		int randomRotation = 0;
		if (maxType != -1)
		{
			randomRotation = GetRandomInt(0, rotation, 1);
		}
		source.init(randomX, randomY, randomSize, randomRotation);
		if (maxType != -1)
		{
			source.type = GetRandomInt(0, maxType - 2, 1);
		}
		vectorList.push_back(source);
	}
}
#pragma endregion

#pragma region Common_Function
void frameCap()
{
	//Calculate and correct fps
	float avgFPS = countedFrames / (systemTimer.getTicks() / 1000.f);
	if (avgFPS > 2000000)
	{
		avgFPS = 0;
	}
	++countedFrames;

	//If frame finished early
	int frameTicks = deltaTimer.getTicks();
	if (frameTicks < SCREEN_TICK_PER_FRAME)
	{
		//Wait remaining time
		SDL_Delay(SCREEN_TICK_PER_FRAME - frameTicks);
	}

	std::string title = "SM2603 Project [avg fps: " + std::to_string(int(avgFPS)) + "] " +
		" [X:" + std::to_string(int(myPlayer.px)) + ", Y:" + std::to_string(int(myPlayer.py)) + "]";
	SDL_SetWindowTitle(gWindow, title.c_str());
}

void clearScreen()
{
	//Clear screen
	SDL_SetRenderDrawColor(gRenderer, 100, 100, 100, 0);
	SDL_RenderClear(gRenderer);
}
#pragma endregion

#pragma region Menu_Screen
void handleMenuEvent(int& choice)
{
	//Poll events
	while (SDL_PollEvent(&event))
	{
		//check events
		switch (event.type)
		{
		case SDL_QUIT: //User hit the X
			choice = 2;
			break;
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				//resize window
				SDL_SetWindowSize(gWindow, event.window.data1, event.window.data2);
				//SCREEN_WIDTH = event.window.data1;
				//SCREEN_HEIGHT = event.window.data2;
			}
			break;
		case SDL_KEYDOWN:
			break;
		case SDL_MOUSEBUTTONUP:
			//start button
			if (buttons[0].checkInside(mouseX, mouseY))
			{
				choice = 0;
			}
			//toggle music button
			if (buttons[1].checkInside(mouseX, mouseY))
			{
				choice = 1;
			}
			//quit
			if (buttons[2].checkInside(mouseX, mouseY))
			{
				choice = 2;
			}
			break;
		}
	}

	//play or pause music
	if (setting_Music)
	{
		Mix_ResumeMusic();
	}
	else if (!setting_Music)
	{
		Mix_PauseMusic();
	}
}

void Menu()
{
	//play background music
	myAudio.playMainMusic();

	myAudio.stopBackgroundLoop();

	//show back the cursor
	SDL_ShowCursor(SDL_ENABLE);
	//SDL_WarpMouseInWindow(gWindow, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2); //set the cursor to center of the window

	//set text positions
	int textOffset = SCREEN_HEIGHT / 4;
	int textX = SCREEN_WIDTH / 2;
	int textY = SCREEN_HEIGHT / 2 - textOffset;

	//add buttons
	//start button
	int buttonpy = textY + SCREEN_HEIGHT / 7.5 + 75;
	myButton.init(SCREEN_WIDTH / 2, buttonpy, 50, "Start", regularFont);
	buttons.push_back(myButton);
	//toggle music button
	buttonpy += 75;
	myButton.init(SCREEN_WIDTH / 2, buttonpy, 50, "Toggle music", regularFont);
	buttons.push_back(myButton);
	//quit button
	buttonpy += 75;
	myButton.init(SCREEN_WIDTH / 2, buttonpy, 50, "Quit", regularFont);
	buttons.push_back(myButton);

	int choice = -1; //0 for yes, 1 for no

	while (choice == -1)
	{
		deltaTimer.tick();
		mouses = SDL_GetMouseState(&mouseX, &mouseY);
		handleMenuEvent(choice);

		//Clear screen
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
		SDL_RenderClear(gRenderer);

		//Render black overlay 
		gMenuTexture.render(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		//Render title
		drawText(textX, textY, boldFontTitle, UIColor, "Dark Zone", 1);

		//set toggle music text
		if (setting_Music)
		{
			buttons[1].setText("Music On");
		}
		else if (!setting_Music)
		{
			buttons[1].setText("Music Off");
		}
		//Render buttons
		for (int i = 0; i < buttons.size(); i++)
		{
			buttons[i].checkButton(mouses, mouseX, mouseY);
			buttons[i].render(gRenderer);
		}

		//Update screen
		SDL_RenderPresent(gRenderer);

		frameCap();
	}

	//remove all buttons
	buttons.clear();

	clearScreen();

	StateStruct temp;
	switch (choice)
	{
	case 0: //start
		initedLevel = false;
		temp.StatePointer = Game;
		g_StateStack.push(temp);
		break;
	case 1: //toggle music
		setting_Music = !setting_Music;
		break;
	case 2: //quit
		showConfirmScreen(confirmState::QUIT);
		temp.StatePointer = Confirm;
		g_StateStack.push(temp);
	}

	//get backdrop
	backdrop = gMenuTexture.getSDLTexture();
}
#pragma endregion

#pragma region Game_Screen
void initLevel()
{
	//reset state and flags
	endGameMode = endState::FALSE;
	confirmMode = confirmState::FALSE;
	paused = false;
	confirmScreen = false;
	endGameScreen = false;
	allowSpawnZombie = false;

	//clear level objects
	trees.clear();
	harmZones.clear();
	zombies.clear();
	bloodpools.clear();
	bullets.clear();
	signalZones.clear();
	healthPickUps.clear();

	//reset difficulty
	DIFFICULTY = 1;
	totalZombieKilled = 0;

	//reset timer
	timeLeft = TIME_LIMIT;

	//reset sound effects
	Mix_HaltChannel(-1);

	//if tutorial have been finished
	if (currentObjective >= 2)
	{
		//reset objectives
		currentObjective = 3;
		objective[0] = true;
		objective[1] = true;
		objective[2] = true;
		tutorial = false;
		for (int i = 3; i < TOTAL_OBJECTIVE; i++)
		{
			objective[i] = false;
		}
		obj_zombieKilled = 0;
		allowSpawnZombie = true;
		//reset dialogues
		dialogue.currentLine = 16;
		dialogue.currentPart = 4;
	}
	else
	{
		//reset objectives
		currentObjective = -1;
		for (int i = 0; i < TOTAL_OBJECTIVE; i++)
		{
			objective[i] = false;
		}
		for (int i = 0; i < 3; i++)
		{
			obj_keyPressed[i] = false;
		}
		tutorial = true;
		//reset dialogues
		dialogue.currentLine = 0;
		dialogue.currentPart = 0;
	}

	//init player
	myPlayer.initPlayer();

	//create weapon
	myPlayer.myWeapon[0].init(PISTOL_NAME, PISTOL_DAMAGE, PISTOL_RATE_OF_FIRE, PISTOL_CLIP_SIZE, PISTOL_CLIP_SIZE, PISTOL_RELOAD_TIME);
	myPlayer.myWeapon[1].init(RIFLE_NAME, RIFLE_DAMAGE, RIFLE_RATE_OF_FIRE, RIFLE_CLIP_SIZE, RIFLE_CLIP_SIZE, RIFLE_RELOAD_TIME);

	//create trees
	createGameObjectRandom(myTree, trees, MAX_TREE_NUM, MIN_TREE_SIZE, MAX_TREE_SIZE, -1, TREES_CLIP);

	//create harmzone
	createGameObjectRandom(myHarmZone, harmZones, MAX_HARM_ZONE_NUM * DIFFICULTY, MIN_HARM_ZONE_SIZE, MAX_HARM_ZONE_SIZE);

	//create health pickups
	createGameObjectRandom(myHealthPickup, healthPickUps, MAX_HEALTH_PICKUP_NUM, HEALTH_PICKUP_SIZE, HEALTH_PICKUP_SIZE);

	//create objective zones for objective 5 (find the random signals)
	createGameObjectRandom(mySignalZone, signalZones, TOTAL_SIGNAL_ZONE, 500, 500, 0); //random objective zone
	obj_zones = signalZones.size();
	for (int i = 0; i < signalZones.size(); i++)
	{
		printf("signalZones[%i] px = %f, py = %f\n", i, signalZones[i].px, signalZones[i].py);
	}
	printf("---inited level---\n");
}

void checkPreObjective()
{
	if (dialogue.currentPart == 1)
	{
		currentObjective++;
	}
}

void checkObjective0(int key)
{
	if (currentObjective == 0)
	{

		obj_keyPressed[key] = true;
		if (obj_keyPressed[0] && obj_keyPressed[1] && obj_keyPressed[2] && obj_keyPressed[3])
		{
			objective[0] = true;
		}
	}
}

void checkObjective1()
{
	if (currentObjective == 1)
	{
		objective[1] = true;
	}
}

void checkObjective2()
{
	if (currentObjective == 2)
	{
		objective[2] = true;
		tutorial = false;
	}
}

void checkObjective3()
{
	if (currentObjective == 3)
	{

		obj_zombieKilled++;

		if (obj_zombieKilled >= ZOMBIE_NEEDED_TO_KILL) //killed required amount of zombies
		{
			objective[3] = true;
		}
	}
}

void checkObjective4()
{
	if (currentObjective == 4)
	{

		int i = 0;
		while (i < signalZones.size())
		{
			if (signalZones[i].checkCollision(myPlayer))
			{
				signalZones.erase(signalZones.begin() + i);
				myAudio.playCollectObject();
				obj_zones--;
			}
			else
			{
				int size1 = signalZones[i].size * 10;
				int size2 = signalZones[i].size * 5;
				int size3 = signalZones[i].size * 3;
				gLightTexture.setColor(0, 0, 255, 25);
				gLightTexture.render(camera, signalZones[i].px - size1 / 2, signalZones[i].py - size1 / 2, size1, size1);
				gLightTexture.setColor(0, 0, 255, 50);
				gLightTexture.render(camera, signalZones[i].px - size2 / 2, signalZones[i].py - size2 / 2, size2, size2);
				gLightTexture.setColor(0, 0, 255, 100);
				gLightTexture.render(camera, signalZones[i].px - size3 / 2, signalZones[i].py - size3 / 2, size3, size3);
				gLightTexture.setColor(0, 0, 255, 200);
				gLightTexture.render(camera, signalZones[i].rx, signalZones[i].ry, signalZones[i].size, signalZones[i].size);
			}
			i++;
		}

	}
	//renderGameObject(camera, gLightTexture, signalZones);

	if (obj_zones <= 0)
	{
		objective[4] = true;
	}
}

void checkEndGame()
{
	if (timeLeft <= 0)
	{
		if (objective[TOTAL_OBJECTIVE - 1])
		{
			showEndGamecreen(endState::WIN);
		}
		else
		{
			showEndGamecreen(endState::TIME_OVER);
		}
	}
	if (myPlayer.health <= 0)
	{
		showEndGamecreen(endState::LOSE);
	}
}

void updateObjective()
{
	static int spawned = false;

	switch (currentObjective)
	{
	case -1: //waiting for first dialogue to finish
		objectiveText = "waiting for objective";
		checkPreObjective();
		break;
	case 0: //objective 1: (tutorial): press w,a,s,d
		objectiveText = "move around the area by pressing W, A, S, D";
		break;
	case 1: //objective 2: (tutorial): shoot gun
		objectiveText = "shoot your gun by pressing Left Mouse Button";
		break;
	case 2: //objective 3: (tutorial): reload gun
		objectiveText = "reload your gun by pressing R";
		break;
	case 3: //objective 4: kill required amount of zombies (tutorial ended, zombie start spawning now)
		objectiveText = "kill " + std::to_string(ZOMBIE_NEEDED_TO_KILL) + " zombies, " + std::to_string(ZOMBIE_NEEDED_TO_KILL - obj_zombieKilled) + "/" + std::to_string(ZOMBIE_NEEDED_TO_KILL) + " left";
		//spawn zombie
		if (!spawned)
		{
			allowSpawnZombie = true;
			spawned = true;
		}
		break;
	case 4: //objective 5: find 5 missing signels
		objectiveText = "find the missing signel, " + std::to_string(signalZones.size()) + "/" + std::to_string(TOTAL_SIGNAL_ZONE) + " left";
		break;

	case TOTAL_OBJECTIVE: //objective 5: All objective finished
		objectiveText = "Survive!";
		break;
	}

	//if dialogue haven't finish
	if (objective[currentObjective] && dialogue.currentPart == currentObjective + 1 && currentObjective < TOTAL_OBJECTIVE)
	{
		objectiveText = "waiting for objective";
	}
}

void drawObjective()
{
	updateObjective();

	//draw the text "Objective"
	int renderedTextWidth;
	int objectiveTextX = V_BORDER;
	int objectiveTextY = H_BORDER;
	renderedTextWidth = drawText(objectiveTextX, objectiveTextY, boldFont, UIColor, "Objective: ", 0);

	//draw objectives
	int objectiveX = int(V_BORDER + renderedTextWidth * 1.05);
	int objectiveY = objectiveTextY;
	drawText(objectiveX, objectiveY, regularFont, UIColor, objectiveText, 0);
}

void drawTimer()
{
	static float timeCounter = 0;
	//calulate countdown time
	if (!systemTimer.isPaused() && !tutorial && timeLeft > 0)
	{
		timeCounter += systemTimer.getDeltaTime();
	}

	if (timeCounter >= 1)
	{
		timeLeft -= 1;
		timeCounter = 0;
	}

	//format time
	int m;
	int s;
	s = timeLeft % 60;
	m = timeLeft / 60;
	if (s == 60)
	{
		s = 0;
	}

	char timerText[20] = "tutorial section";
	if (!tutorial)
	{
		sprintf_s(timerText, "%02i:%02i", m, s);
	}

	//draw timer
	int renderedTextWidth;
	int timerX = SCREEN_WIDTH - V_BORDER;
	int timerY = H_BORDER;
	renderedTextWidth = drawText(timerX, timerY, regularFont, UIColor, timerText, 2); //TODO: change to actual timer with format

	//draw the text "Time Left"
	int timerTextX = int(SCREEN_WIDTH - V_BORDER - renderedTextWidth * 1.1);
	int timerTextY = timerY;
	drawText(timerTextX, timerTextY, boldFont, UIColor, "Time Left: ", 2);


}

void drawHealth() {
	//set health to 0 if it drop below 0
	if (myPlayer.health < 0)
	{
		myPlayer.health = 0;
	}

	//size of icons
	int healthBarWidth = map(myPlayer.health, 0, 100, 0, SCREEN_WIDTH / 5); //map player's current health to the health bar width
	int healthBarHeight = SCREEN_HEIGHT / 100;
	int healthIconSize = SCREEN_HEIGHT / 20;

	//position of icons
	int healthBarOffset = SCREEN_HEIGHT / 20;
	int healthBarX = V_BORDER;
	int healthBarY = SCREEN_HEIGHT - H_BORDER - healthBarOffset;
	int healthIconX = int(V_BORDER * 1.25);
	int healthIconY = healthBarY - healthBarHeight - healthIconSize;
	int healthX = int(healthIconX + healthIconSize * 1.5);
	int healthY = healthIconY;

	//draw heath icon
	gHealthIconTexture.render(healthIconX, healthIconY, healthIconSize, healthIconSize);

	//draw heath
	drawText(healthX, healthY, boldFont, UIColor, std::to_string(int(myPlayer.health)), 0);

	//draw heath bar
	gWhiteTexture.setColor(255, 255, 255, 255);
	gWhiteTexture.render(healthBarX, healthBarY, healthBarWidth, healthBarHeight);
}

void drawDialogue()
{
	static std::string fullDialogue;
	if (dialogue.check())
	{
		if (!dialogue.onScreen())
		{
			dialogue.setFlag(true);
			fullDialogue = " ";

			//check if current line reached end of part of the dialogue
			if (dialogue.currentLine < dialogueLine.size() &&
				dialogueLine[dialogue.currentLine] == "//end part//")
			{
				dialogue.currentLine += 1;
				dialogue.currentPart++;
			}

			if (currentObjective != TOTAL_OBJECTIVE)
			{
				//dialogue part for next object when current objective is finished (or when the game just start and dialogue 1 havent be finished)
				if (objective[currentObjective] && dialogue.currentPart == currentObjective + 1 || currentObjective == -1 && dialogue.currentPart == 0)
				{
					if (dialogue.currentLine < dialogueLine.size())
					{
						fullDialogue = "Radio: " + dialogueLine[dialogue.currentLine];
						dialogue.currentLine++;
						myAudio.playRadio();
					}
				}
				else if (objective[currentObjective] && dialogue.currentPart == currentObjective + 2)
				{
					currentObjective++;
				}
			}
		}

		/*//last objective is finished
		if (objective[TOTAL_OBJECTIVE -1] && dialogue.currentPart == TOTAL_OBJECTIVE + 1)
		{
				printf("yes");
			if (dialogueTips.check())
			{
				if (!dialogueTips.onScreen())
				{
					printf("2");
					fullDialogue = " ";
					dialogueTips.setFlag(true);

					//random interval and dialogue line
					dialogueTips.interval = GetRandomFloat(10.0f, 20.0f, 0.1f);
					dialogueTips.currentLine = GetRandomInt(0, tipsLine.size() - 2, 1);
					printf("interval = %f\n", dialogueTips.interval);
					//std::cout << "dialogueTips interval = " << dialogueTips.interval << std::endl; //temp
					//printf("dialogueTips current line = %i\n", dialogueTips.currentLine); //temp
					fullDialogue = "Radio: " + tipsLine[dialogueTips.currentLine];
				}
			}
		}*/

		//draw dialogue
		int dialogueOffset = SCREEN_HEIGHT / 10;
		int dialogueX = SCREEN_WIDTH / 2;
		int dialogueY = SCREEN_HEIGHT - H_BORDER - dialogueOffset;
		drawText(dialogueX, dialogueY, regularFont, UIColor, fullDialogue, 1);
	}
}

void drawWeapon()
{
	//draw weapon icon
	int weaponIconSize = SCREEN_HEIGHT / 5; //TODO: make a switch statement for current weapon here
	int weaponIconX = SCREEN_WIDTH - V_BORDER - weaponIconSize;
	int weaponIconY = SCREEN_HEIGHT - H_BORDER - weaponIconSize;

	switch (myPlayer.currentWeapon)
	{
	case 0:
		gPistolIconTexture.render(weaponIconX, weaponIconY, weaponIconSize, weaponIconSize);
		break;
	case 1:
		gRifleIconTexture.render(weaponIconX, weaponIconY, weaponIconSize, weaponIconSize);
		break;
	}

	//weaponIconTexture.free();

	//draw current weapon's clip size
	int renderedTextWidth;
	int clipSizeX = SCREEN_WIDTH - V_BORDER - weaponIconSize;
	int clipSizeY = int(SCREEN_HEIGHT - H_BORDER - weaponIconSize / 1.5);
	int clipSize = myPlayer.myWeapon[myPlayer.currentWeapon].getClipSize();
	renderedTextWidth = drawText(clipSizeX, clipSizeY, regularFont, { 128,128,128 }, std::to_string(clipSize), 2);

	//draw current ammo
	int currentAmmoX = int(SCREEN_WIDTH - V_BORDER - weaponIconSize - renderedTextWidth * 1.1);
	int currentAmmoY = int(SCREEN_HEIGHT - H_BORDER - weaponIconSize / 1.5);
	int currentAmmo = myPlayer.myWeapon[myPlayer.currentWeapon].ammo;
	drawText(currentAmmoX, currentAmmoY, boldFontLarge, UIColor, std::to_string(currentAmmo), 2);
}

void drawUI()
{
	drawObjective();
	drawTimer();
	drawHealth();
	drawWeapon();
	drawDialogue();

	if (cheat)
	{
		drawText(SCREEN_WIDTH / 2, H_BORDER, boldFontSmall, UIColor, "cheat mode: on", 1);
	}
}

void setPlayerAnimation()
{
	if (myPlayer.myWeapon[myPlayer.currentWeapon].checkReload())
	{
		myPlayer.currentState = playerState::RELOAD;
	}
	else if (myPlayer.previousState == playerState::RELOAD)
	{
		myPlayer.currentState = playerState::IDLE;
	}
	if (myPlayer.currentState == playerState::FIRE && myPlayer.currentFrame >= myPlayer.currentTotalFrame - 1)
	{
		myPlayer.currentState = playerState::IDLE;
		myPlayer.currentFrame = 0;
	}

	switch (myPlayer.currentState)
	{
	case playerState::IDLE:
		myAudio.pauseWalk();
		if (myPlayer.myWeapon[myPlayer.currentWeapon].getName() == "pistol")
		{
			myPlayer.currentTotalFrame = PLAYER_PISTOL_IDLE_ANIMATION_FRAMES;
		}
		else if (myPlayer.myWeapon[myPlayer.currentWeapon].getName() == "assault rifle")
		{
			myPlayer.currentTotalFrame = PLAYER_RIFLE_IDLE_ANIMATION_FRAMES;
		}
		break;
	case playerState::WALK:
		myAudio.resumeWalk(myPlayer);
		if (myPlayer.myWeapon[myPlayer.currentWeapon].getName() == "pistol")
		{
			myPlayer.currentTotalFrame = PLAYER_PISTOL_WALK_ANIMATION_FRAMES;
		}
		else if (myPlayer.myWeapon[myPlayer.currentWeapon].getName() == "assault rifle")
		{
			myPlayer.currentTotalFrame = PLAYER_RIFLE_WALK_ANIMATION_FRAMES;
		}
		break;
	case playerState::FIRE:
		if (myPlayer.myWeapon[myPlayer.currentWeapon].getName() == "pistol")
		{
			myPlayer.currentTotalFrame = PLAYER_PISTOL_FIRE_ANIMATION_FRAMES;
		}
		else if (myPlayer.myWeapon[myPlayer.currentWeapon].getName() == "assault rifle")
		{
			myPlayer.currentTotalFrame = PLAYER_RIFLE_FIRE_ANIMATION_FRAMES;
		}
		break;
	case playerState::RELOAD:
		if (myPlayer.myWeapon[myPlayer.currentWeapon].getName() == "pistol")
		{
			myPlayer.currentTotalFrame = PLAYER_PISTOL_RELOAD_ANIMATION_FRAMES;
		}
		else if (myPlayer.myWeapon[myPlayer.currentWeapon].getName() == "assault rifle")
		{
			myPlayer.currentTotalFrame = PLAYER_RIFLE_RELOAD_ANIMATION_FRAMES;
		}
		break;
	case playerState::DEAD:

		break;
	}
	if (myPlayer.myWeapon[myPlayer.currentWeapon].getName() == "pistol")
	{
		myPlayer.setAnimation(gPlayerPistolTexture[myPlayer.currentState],
			gPlayerPistolClips[myPlayer.currentState][myPlayer.currentFrame]);
	}
	else if (myPlayer.myWeapon[myPlayer.currentWeapon].getName() == "assault rifle")
	{
		myPlayer.setAnimation(gPlayerRifleTexture[myPlayer.currentState],
			gPlayerRifleClips[myPlayer.currentState][myPlayer.currentFrame]);
	}
}

void setDifficulty()
{
	DIFFICULTY = 1 + totalZombieKilled / DIFFICULTY_REQUIREMENT;
	MAX_ZOMBIE_NUM = DIFFICULTY * ZOMBIE_NUMBER_STEP;
}

void updatePlayer()
{
	//set player's animation based on current state
	setPlayerAnimation();

	//calulate player rotation
	myPlayer.calRotation(camera, mouseX, mouseY);

	myPlayer.speed = PLAYER_SPEED;
	//slow down player if player walk into trees
	for (int i = 0; i < trees.size(); i++)
	{
		if (myPlayer.checkCollision(trees[i]))
		{
			myPlayer.speed *= 0.75;
			myPlayer.speed *= 0.75;
			break;
		}
	}

	//calulate player position
	float dirX = myPlayer.vx * myPlayer.speed * deltaTimer.getDeltaTime();
	float dirY = myPlayer.vy * myPlayer.speed * deltaTimer.getDeltaTime();
	myPlayer.px += dirX;
	myPlayer.py += dirY;

	//check collision with game objects
	//zombies
	for (int i = 0; i < zombies.size(); i++)
	{
		if (zombies[i].currentState != zombieState::DEAD && myPlayer.checkCollision(zombies[i], zombies[i].attackRange))
		{
			myPlayer.px -= dirX;
			myPlayer.py -= dirY;
			break;
		}
	}
	//health pickups
	for (int i = 0; i < healthPickUps.size(); i++)
	{
		if (myPlayer.checkCollision(healthPickUps[i], HEALTH_PICKUP_SIZE / 2))
		{
			if (myPlayer.health < 100)
			{
				if (myPlayer.health + HEALTH_PICKUP_HEAL > 100)
				{
					myPlayer.health = 100;
				}
				else
				{
					myPlayer.health += HEALTH_PICKUP_HEAL;
				}
				healthPickUps.erase(healthPickUps.begin() + i);
				myAudio.playCollectObject();
			}
			break;
		}
	}

	//set player's render position
	myPlayer.updateRenderPosition();

	//check if player is in harm zone
	for (int i = 0; i < harmZones.size(); i++)
	{
		float distance = myPlayer.calDistance(harmZones[i]);
		if (distance < harmZones[i].size / COLLIDER_TOLERANCE)
		{
			myPlayer.health -= ((MAX_HARM_ZONE_DAMAGE * DIFFICULTY) - map(distance, 0, harmZones[i].size / COLLIDER_TOLERANCE, 0, (MAX_HARM_ZONE_DAMAGE * DIFFICULTY))) * deltaTimer.getDeltaTime();
			myAudio.playPlayerHurt(myPlayer);
			break;
		}
	}

	//render player
	myPlayer.render(camera);
}

void updateBullet()
{
	int i = 0;
	gBulletTexture.setColor(255, 200, 0);
	while (i < bullets.size())
	{
		bullets[i].px += bullets[i].vx;
		bullets[i].py += bullets[i].vy;

		//delete bullet if its out of view
		if (bullets[i].calDistance(myPlayer) > SCREEN_WIDTH)
		{
			bullets.erase(bullets.begin() + i);
		}
		else
		{
			bool collised = false;
			//check bullet collision
			//tree
			for (int j = 0; j < trees.size(); j++)
			{
				if (bullets[i].checkCollision(trees[j]))
				{
					collised = true;
					myAudio.playHitTree(bullets[i]);
					break;
				}
			}
			//zombie
			for (int j = 0; j < zombies.size(); j++)
			{
				if (bullets[i].checkCollision(zombies[j]))
				{
					collised = true;
					zombies[j].hurt();
					myAudio.playHitZombie(bullets[i]);

					//remove zombie if it's health is below 0
					if (zombies[j].health < 0)
					{
						zombies[j].currentState = zombieState::DEAD;
						bloodpools.push_back(zombies[j]);
						zombies.erase(zombies.begin() + j);
						allowSpawnZombie = true;
						totalZombieKilled++;

						//for objective 4
						checkObjective3();
					}
					break;
				}
			}
			//delete current bullet if it's collided with something
			if (collised)
			{
				bullets.erase(bullets.begin() + i);
			}
			else
			{
				//update bullet position
				bullets[i].updateRenderPosition();
				//render bullet
				gBulletTexture.render(camera, bullets[i].rx, bullets[i].ry, bullets[i].size, bullets[i].size, NULL, bullets[i].rotation);
				i++;
			}
		}
	}
}

void renderLighting()
{
	//bullet lightings
	for (int i = 0; i < bullets.size(); i++)
	{
		int lightSize1 = 750;
		int lightSize2 = 300;
		//render bullet lighting
		gLightTexture.setColor(200, 150, 0, 100);
		gLightTexture.render(camera, bullets[i].rx - lightSize1 / 2, bullets[i].ry - lightSize1 / 2, lightSize1, lightSize1);
		gLightTexture.setColor(250, 175, 0, 125);
		gLightTexture.render(camera, bullets[i].rx - lightSize2 / 2, bullets[i].ry - lightSize2 / 2, lightSize2, lightSize2);
	}
}

void spawnZombie()
{
	if (allowSpawnZombie)
	{
		while (zombies.size() < MAX_ZOMBIE_NUM)
		{
			myZombie.init();
			zombies.push_back(myZombie);
		}
		printf("--- spawned zombies. total zombie = %i ---\n", zombies.size());
		allowSpawnZombie = false;
	}
}

void setZombieAnimation(zombie& source)
{
	if (source.currentTotalFrame == ZOMBIE_ATTACK_ANIMATION_FRAMES && source.currentFrame < ZOMBIE_ATTACK_ANIMATION_FRAMES)
	{
		source.currentState = zombieState::ATTACK;
	}

	switch (source.currentState)
	{
	case zombieState::WALK:
		if (source.currentFrame > ZOMBIE_WALK_ANIMATION_FRAMES - 1)
		{
			source.currentFrame = 0;
		}
		source.currentTotalFrame = ZOMBIE_WALK_ANIMATION_FRAMES;
		break;
	case zombieState::ATTACK:
		if (source.currentFrame > ZOMBIE_ATTACK_ANIMATION_FRAMES - 1)
		{
			source.currentFrame = 0;
		}
		source.currentTotalFrame = ZOMBIE_ATTACK_ANIMATION_FRAMES;
		break;
	}

	source.setAnimation(gZombieTexture[source.currentState], gZombieClips[source.currentState][source.currentFrame]);
}

void updateZombie()
{
	spawnZombie();
	int i = 0;

	while (i < zombies.size())
	{
		//attack player if close
		if (zombies[i].calDistance(myPlayer) < zombies[i].attackRange + myPlayer.size)
		{
			if (zombies[i].attack(myPlayer))
			{
				myAudio.playZombieAttack(zombies[i]);
				myAudio.playPlayerHurt(myPlayer);
			}
		}
		//only move if its not attacking
		else
		{
			zombies[i].move(myPlayer);
		}
		//render zombie
		setZombieAnimation(zombies[i]);
		zombies[i].render(camera);
		i++;
	}//
}

void renderBloodPool()
{
	int i = 0;

	//delete old blood pool
	if (bloodpools.size() > MAX_BLOOD_POOL_NUM)
	{
		bloodpools.erase(bloodpools.begin());
	}

	//render blood pool
	while (i < bloodpools.size())
	{
		gBloodPoolTexture.render(camera, bloodpools[i].px, bloodpools[i].py, bloodpools[i].size, bloodpools[i].size, NULL, bloodpools[i].rotation);
		i++;
	}
}

void renderCrosshair()
{
	if (myPlayer.currentState == playerState::RELOAD || !myPlayer.myWeapon[myPlayer.currentWeapon].checkAmmo())
	{
		gCrosshairTexture.setColor(150, 150, 150);
	}
	else
	{
		gCrosshairTexture.setColor(255, 255, 255);
	}
	gCrosshairTexture.render(mouseX + CROSSHAIR_SIZE / 2, mouseY + CROSSHAIR_SIZE / 2, CROSSHAIR_SIZE, CROSSHAIR_SIZE);
}

void updateAnimation()
{
	animationTimeCounter += deltaTimer.getDeltaTime();

	if (animationTimeCounter > ANIMATION_INTERVAL)
	{
		animationTimeCounter = 0;
		myPlayer.currentFrame++;

		for (int i = 0; i < zombies.size(); i++)
		{
			zombies[i].currentFrame++;
		}
	}

	//Cycle player animation
	if (myPlayer.currentFrame > myPlayer.currentTotalFrame - 1)
	{
		myPlayer.currentFrame = 0;
	}

}

void setCamera(SDL_Rect& camera, gameObject target) {
	//Center the camera over the target
	camera.x = int(target.px - SCREEN_WIDTH / 2);
	camera.y = int(target.py - SCREEN_HEIGHT / 2);

	//Keep the camera in bounds.
	if (camera.x < 0)
	{
		camera.x = 0;
	}
	if (camera.y < 0)
	{
		camera.y = 0;
	}
	if (camera.x > LEVEL_WIDTH - camera.w)
	{
		camera.x = LEVEL_WIDTH - camera.w;
	}
	if (camera.y > LEVEL_HEIGHT - camera.h)
	{
		camera.y = LEVEL_HEIGHT - camera.h;
	}
}

void handleGameEvent()
{
	Uint32 windowID = SDL_GetWindowID(gWindow);

	//Poll events
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT: //User hit the X
			showConfirmScreen(confirmState::QUIT);
			break;
		case SDL_WINDOWEVENT:
			//if window is resized
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				//resize window
				SDL_SetWindowSize(gWindow, event.window.data1, event.window.data2);
				//SCREEN_WIDTH = event.window.data1;
				//SCREEN_HEIGHT = event.window.data2;
			}
			//if window have lost focus
			if (event.window.windowID == windowID)
			{
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_FOCUS_LOST:

					paused = true;
					break;
				}
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (myPlayer.myWeapon[myPlayer.currentWeapon].checkAmmo() &&
				!myPlayer.myWeapon[myPlayer.currentWeapon].checkReload()) //shoot
			{
				myPlayer.currentState = playerState::FIRE;
				myPlayer.currentFrame = 0;
				myPlayer.myWeapon[myPlayer.currentWeapon].resetTimer();
			}
			else if (!myPlayer.myWeapon[myPlayer.currentWeapon].checkAmmo())
			{
				myAudio.playGunEmpty();
			}
			break;
		case SDL_MOUSEWHEEL:
			myPlayer.cycleWeapon();
			myAudio.playSwapWeapon();
			myAudio.stopReload();
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_q) //cycle weapon
			{
				myPlayer.cycleWeapon();
				myAudio.playSwapWeapon();
				myAudio.stopReload();
			}
			if (event.key.keysym.sym == SDLK_r) //reload weapon
			{
				if (myPlayer.myWeapon[myPlayer.currentWeapon].ammo < myPlayer.myWeapon[myPlayer.currentWeapon].getClipSize() &&
					!myPlayer.myWeapon[myPlayer.currentWeapon].getReloadFlag())
				{
					myPlayer.myWeapon[myPlayer.currentWeapon].reload();
					myPlayer.currentState = playerState::RELOAD;
					myPlayer.currentFrame = 0;
					myAudio.playReload();

					//for objective 3
					checkObjective2();
				}
			}
			if (event.key.keysym.sym == SDLK_1) //weapon 1
			{
				if (myPlayer.currentWeapon != 0)
				{
					myPlayer.currentWeapon = 0;
					myAudio.playSwapWeapon();
					myAudio.stopReload();
				}
			}
			if (event.key.keysym.sym == SDLK_2) //weapon 2
			{
				if (myPlayer.currentWeapon != 1)
				{
					myPlayer.currentWeapon = 1;
					myAudio.playSwapWeapon();
					myAudio.stopReload();
				}
			}
			if (event.key.keysym.sym == SDLK_ESCAPE) //esc
			{
				paused = true;
			}

			if (event.key.keysym.sym == SDLK_BACKQUOTE) //cheat toggle
			{
				cheat = !cheat;
				printf("---cheat mode: %i---\n", cheat);
			}

			if (event.key.keysym.sym == SDLK_SPACE) //cheat: restore health
			{
				if (cheat)
				{
					myPlayer.health = 100;
					printf("Cheat: player health restored\n");
				}
			}

			if (event.key.keysym.sym == SDLK_HOME) //cheat: toggle god mode
			{
				if (cheat)
				{
					static bool godMode = false;
					godMode = !godMode;
					if (godMode) { myPlayer.health = 99999999; }
					if (!godMode) { myPlayer.health = 100; }
					printf("Cheat: god mode = %i\n", godMode);
				}
			}

			if (event.key.keysym.sym == SDLK_END) //cheat: instant kill
			{
				if (cheat)
				{
					myPlayer.health = -100;
					printf("Cheat: instant kill\n");
				}
			}

			if (event.key.keysym.sym == SDLK_DELETE) //cheat: kill all zombies
			{
				if (cheat)
				{
					zombies.clear();
					printf("Cheat: killed all zombies\n");
				}
			}

			if (event.key.keysym.sym == SDLK_INSERT) //cheat: spawn zombies
			{
				if (cheat)
				{
					allowSpawnZombie = !allowSpawnZombie;
					printf("Cheat: force spawn zombies\n");
				}
			}

			if (event.key.keysym.sym == SDLK_UP) //cheat: increase total zombie numbers
			{
				if (cheat)
				{
					totalZombieKilled += DIFFICULTY_REQUIREMENT;
					printf("Cheat: maximum zombies: %i\n", MAX_ZOMBIE_NUM);
				}
			}

			if (event.key.keysym.sym == SDLK_DOWN) //cheat: decrease total zombie numbers
			{
				if (cheat)
				{
					totalZombieKilled -= DIFFICULTY_REQUIREMENT;
					printf("Cheat: maximum zombies: %i\n", MAX_ZOMBIE_NUM);
				}
			}
			break;
		}
	}
}

void handleGameInput()
{
	keys = SDL_GetKeyboardState(NULL);
	mouses = SDL_GetMouseState(&mouseX, &mouseY);

	//check keyboard input
	if (keys[SDL_SCANCODE_W])
	{
		if (myPlayer.py - myPlayer.size < 0)
		{
			myPlayer.vy = 0;
		}
		else
		{
			myPlayer.vy = -1;
			myPlayer.currentState = playerState::WALK;
		}

		//for objective 1
		checkObjective0(0);
	}
	if (keys[SDL_SCANCODE_S])
	{
		if (myPlayer.py + myPlayer.size > LEVEL_HEIGHT)
		{
			myPlayer.vy = 0;
		}
		else
		{
			myPlayer.vy = 1;
			myPlayer.currentState = playerState::WALK;
		}

		//for objective 1		
		checkObjective0(1);
	}
	if (keys[SDL_SCANCODE_A])
	{
		if (myPlayer.px - myPlayer.size < 0)
		{
			myPlayer.vx = 0;
		}
		else
		{
			myPlayer.vx = -1;
			myPlayer.currentState = playerState::WALK;
		}

		//for objective 1
		checkObjective0(2);
	}
	if (keys[SDL_SCANCODE_D])
	{
		if (myPlayer.px + myPlayer.size > LEVEL_WIDTH)
		{
			myPlayer.vx = 0;
		}
		else
		{
			myPlayer.vx = 1;
			myPlayer.currentState = playerState::WALK;
		}

		//for objective 1
		checkObjective0(3);
	}

	if (keys[SDL_SCANCODE_LSHIFT]) //cheat: double speed
	{
		if (cheat)
		{
			myPlayer.vx *= 2;
			myPlayer.vy *= 2;
		}
	}

	//check mouse input
	if (mouses & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		if (myPlayer.myWeapon[myPlayer.currentWeapon].checkRateOfFire() &&
			myPlayer.myWeapon[myPlayer.currentWeapon].checkAmmo() &&
			!myPlayer.myWeapon[myPlayer.currentWeapon].checkReload())
		{
			myPlayer.currentState = playerState::FIRE;
			myPlayer.currentFrame = 0;
			myPlayer.myWeapon[myPlayer.currentWeapon].ammo--;
			bullet myBullet(camera, myPlayer, mouseX, mouseY);
			bullets.push_back(myBullet);
			myAudio.playGunshot(myPlayer);

			//for objective 2
			checkObjective1();
		}
	}
}

void Game()
{
	clearScreen();

	if (!initedLevel)
	{
		//init level
		initLevel();

		//play background music
		if (setting_Music)
		{
			myAudio.playMainMusic();
		}
		myAudio.playBackgroundLoop();

		initedLevel = true;
	}

	//While application is running
	while (initedLevel && !quit && !paused && !confirmScreen && !endGameScreen)
	{
		deltaTimer.tick();
		SDL_GetMouseState(&mouseX, &mouseY);
		SDL_ShowCursor(SDL_DISABLE);
		myPlayer.previousState = myPlayer.currentState;
		setCamera(camera, myPlayer);

		//Set the player back to idle mode
		myPlayer.currentState = playerState::IDLE;
		myPlayer.vx = 0;
		myPlayer.vy = 0;

		setDifficulty();

		handleGameEvent();
		handleGameInput();

		//Clear screen
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
		SDL_RenderClear(gRenderer);

		//Render ground
		for (int y = 0; y < LEVEL_SIZE; y++)
		{
			for (int x = 0; x < LEVEL_SIZE; x++)
			{
				gGroundTexture.render(camera, x * GROUND_TILE_SIZE, y * GROUND_TILE_SIZE, GROUND_TILE_SIZE, GROUND_TILE_SIZE);
			}
		}

		//Render blood pool
		renderBloodPool();

		//Update and render bullets
		updateBullet();

		//render health pickups
		renderGameObject(camera, gHealthPickUpTexture, healthPickUps);

		//Update and render player
		updatePlayer();

		//Update and render zombie
		updateZombie();

		//Render lightings
		renderLighting();

		//Render other game objects
		//render trees
		renderGameObject(camera, gTreeTexture, trees, gTreeClips);
		//render harm zone
		gLightTexture.setColor(255, 0, 0, 200);
		renderGameObject(camera, gLightTexture, harmZones);
		//check objective 4 and render objective zones
		checkObjective4();

		//Render screen effect
		//color filter
		gWhiteTexture.setColor(75, 100, 200, 50);
		gWhiteTexture.render(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		//vignette effect
		int healthColor = int((255 - map(myPlayer.health, 0, 100, 0, 255)) * 0.2);
		gVignetteTexture.setColor(healthColor, 0, 0);
		gVignetteTexture.render(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		//lens dirt effect
		gLensDirtTexture.setColor(255, unsigned char(255 - healthColor), unsigned char(255 - healthColor), 25);
		gLensDirtTexture.render(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		//blood overlay effect
		gBloodOverlayTexture.setColor(255, 255, 255, unsigned char(healthColor));
		gBloodOverlayTexture.render(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		//Render UI
		drawUI();

		//Render crosshair
		renderCrosshair();

		//Update animation timer
		updateAnimation();

		//cap frame rate
		frameCap();

		//check if end game condition is met
		checkEndGame();

		float offsetX = 0;
		float offsetY = 20;
		float distance = 20.0f;
		double theta = myPlayer.rotation * (M_PI / 180);
		float posX = myPlayer.px + (offsetX * cos(theta) - offsetY * sin(theta));
		float posY = myPlayer.py + (offsetX * sin(theta) + offsetY * cos(theta));

		//Update screen
		SDL_RenderPresent(gRenderer);
	}

	//get backdrop
	SDL_Surface* screencap = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(gRenderer, NULL, SDL_PIXELFORMAT_ARGB8888, screencap->pixels, screencap->pitch);
	backdrop = SDL_CreateTextureFromSurface(gRenderer, screencap);
	SDL_FreeSurface(screencap);

	StateStruct temp;
	//handle menus
	if (paused)
	{
		temp.StatePointer = Pause;
		g_StateStack.push(temp);
	}
	if (confirmScreen)
	{
		temp.StatePointer = Confirm;
		g_StateStack.push(temp);
	}
	if (endGameScreen)
	{
		temp.StatePointer = EndGame;
		g_StateStack.push(temp);
	}
	if (quit)
	{
		while (!g_StateStack.empty())
		{
			g_StateStack.pop();
		}
	}
}
#pragma endregion

#pragma region Pause_Screen
void handlePauseEvent()
{
	//Poll events
	while (SDL_PollEvent(&event))
	{
		//check events
		switch (event.type)
		{
		case SDL_QUIT: //User hit the X	
			showConfirmScreen(confirmState::QUIT);
			break;
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				//resize window
				SDL_SetWindowSize(gWindow, event.window.data1, event.window.data2);
				//SCREEN_WIDTH = event.window.data1;
				//SCREEN_HEIGHT = event.window.data2;
			}
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE) //esc
			{
				paused = false;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			//resume button
			if (buttons[0].checkInside(mouseX, mouseY))
			{
				paused = false;
			}
			//retry button
			if (buttons[1].checkInside(mouseX, mouseY))
			{
				showConfirmScreen(confirmState::RETRY);
			}
			//quit to menu button
			if (buttons[2].checkInside(mouseX, mouseY))
			{
				showConfirmScreen(confirmState::QUIT_TO_MENU);
			}
			break;
		}
	}
}

void Pause()
{
	//pause all playing audios
	Mix_PauseMusic();
	Mix_Pause(-1);

	//show back the cursor
	SDL_ShowCursor(SDL_ENABLE);
	//SDL_WarpMouseInWindow(gWindow, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2); //set the cursor to center of the window

	//set text positions
	int pausedOffset = SCREEN_HEIGHT / 3.5;
	int pausedX = SCREEN_WIDTH / 2;
	int pausedY = SCREEN_HEIGHT / 2 - pausedOffset;
	int tipsX = SCREEN_WIDTH / 2;
	int tipsY = pausedY + SCREEN_HEIGHT / 5;

	//add buttons
	//resume button
	int buttonpy = tipsY + 75;
	myButton.init(SCREEN_WIDTH / 2, buttonpy, 50, "Resume", regularFont);
	buttons.push_back(myButton);
	//retry button
	buttonpy += 75;
	myButton.init(SCREEN_WIDTH / 2, buttonpy, 50, "Retry", regularFont);
	buttons.push_back(myButton);
	//quit to menu button
	buttonpy += 75;
	myButton.init(SCREEN_WIDTH / 2, buttonpy, 50, "Quit to menu", regularFont);
	buttons.push_back(myButton);

	//set tips
	std::string fullTips;
	dialogueTips.currentLine = GetRandomInt(0, tipsLine.size() - 2, 1);
	fullTips = "Tips: " + tipsLine[dialogueTips.currentLine];

	while (paused && !confirmScreen)
	{
		deltaTimer.tick();
		mouses = SDL_GetMouseState(&mouseX, &mouseY);
		handlePauseEvent();

		//Clear screen
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
		SDL_RenderClear(gRenderer);

		//Render backdrop
		SDL_RenderCopy(gRenderer, backdrop, NULL, NULL);

		//Render black overlay 
		gWhiteTexture.setColor(0, 0, 0, 175);
		gWhiteTexture.render(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		//Render paused text
		drawText(pausedX, pausedY, boldFontTitle, UIColor, "Paused", 1);

		//Render tips
		drawText(tipsX, tipsY, regularFontSmall, UIColor, fullTips, 1);

		//Render buttons
		for (int i = 0; i < buttons.size(); i++)
		{
			buttons[i].checkButton(mouses, mouseX, mouseY);
			buttons[i].render(gRenderer);
		}

		//Update screen
		SDL_RenderPresent(gRenderer);

		frameCap();
	}
	//resume all paused audios
	Mix_ResumeMusic();
	Mix_Resume(-1);

	//remove all buttons
	buttons.clear();

	clearScreen();

	StateStruct temp;
	if (confirmScreen)
	{
		temp.StatePointer = Confirm;
		g_StateStack.push(temp);
	}
	else
	{
		g_StateStack.pop();
	}
}
#pragma endregion

#pragma region Confirm_Screen
void showConfirmScreen(confirmState m)
{
	confirmScreen = true;
	confirmMode = m;
}

void hideConfirmScreen()
{
	confirmScreen = false;
	confirmMode = confirmState::FALSE;
	g_StateStack.pop();
}

void handleConfirmEvent(int& choice)
{
	//Poll events
	while (SDL_PollEvent(&event))
	{
		//check events
		switch (event.type)
		{
		case SDL_QUIT: //User hit the X
			choice = 2;
			//confirmMode = confirmState::QUIT;
			break;
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				//resize window
				SDL_SetWindowSize(gWindow, event.window.data1, event.window.data2);
				//SCREEN_WIDTH = event.window.data1;
				//SCREEN_HEIGHT = event.window.data2;
			}
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE) //esc
			{
				choice = 1;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			//yes button
			if (buttons[0].checkInside(mouseX, mouseY))
			{
				choice = 0;
			}
			//no button
			if (buttons[1].checkInside(mouseX, mouseY))
			{
				choice = 1;
			}
			break;
		}
	}
}

void Confirm()
{
	//pause all playing audios
	Mix_PauseMusic();
	Mix_Pause(-1);

	//show back the cursor
	SDL_ShowCursor(SDL_ENABLE);
	//SDL_WarpMouseInWindow(gWindow, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2); //set the cursor to center of the window

	//set text positions
	int textOffset = SCREEN_HEIGHT / 10;
	int textX = SCREEN_WIDTH / 2;
	int textY = SCREEN_HEIGHT / 2 - textOffset;

	//add buttons
	//yes button
	int buttonpy = textY + 75;
	myButton.init(SCREEN_WIDTH / 2, buttonpy, 50, "Yes", regularFont);
	buttons.push_back(myButton);
	//no button
	buttonpy += 75;
	myButton.init(SCREEN_WIDTH / 2, buttonpy, 50, "No", regularFont);
	buttons.push_back(myButton);

	int choice = -1; //0 for yes, 1 for no

	while (choice == -1)
	{
		deltaTimer.tick();
		mouses = SDL_GetMouseState(&mouseX, &mouseY);
		handleConfirmEvent(choice);

		//Clear screen
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
		SDL_RenderClear(gRenderer);

		//Render backdrop
		SDL_RenderCopy(gRenderer, backdrop, NULL, NULL);

		//Render black overlay 
		gWhiteTexture.setColor(0, 0, 0, 175);
		gWhiteTexture.render(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		std::string confirmText = "";
		//Render text
		switch (confirmMode)
		{
		case confirmState::RETRY:
			confirmText = "Are you sure you want to retry the level?";
			break;
		case confirmState::QUIT:
			confirmText = "Are you sure you want to quit?";
			break;
		case confirmState::QUIT_TO_MENU:
			confirmText = "Are you sure you want to quit to menu?";
			break;
		}
		drawText(textX, textY, boldFontLarge, UIColor, confirmText, 1);

		//Render buttons
		for (int i = 0; i < buttons.size(); i++)
		{
			buttons[i].checkButton(mouses, mouseX, mouseY);
			buttons[i].render(gRenderer);
		}

		//Update screen
		SDL_RenderPresent(gRenderer);

		frameCap();
	}

	//resume all paused audios
	Mix_ResumeMusic();
	Mix_Resume(-1);

	//remove all buttons
	buttons.clear();

	clearScreen();

	StateStruct temp;
	switch (choice)
	{
	case 0:
		if (confirmMode == confirmState::RETRY) //retry
		{
			hideConfirmScreen();
			initedLevel = false;
			paused = false;
			g_StateStack.pop();
		}
		if (confirmMode == confirmState::QUIT) //quit
		{
			temp.StatePointer = Exit;
			g_StateStack.push(temp);
		}
		if (confirmMode == confirmState::QUIT_TO_MENU) //quit to menu
		{
			g_StateStack.swap(emptyStack);
			temp.StatePointer = Menu;
			g_StateStack.push(temp);
		}
		break;
	case 1:
		hideConfirmScreen();
		break;
	case 2:
		hideConfirmScreen();
		showConfirmScreen(confirmState::QUIT);
		temp.StatePointer = Confirm;
		g_StateStack.push(temp);
	}
}
#pragma endregion

#pragma region End_Game_Screen
void showEndGamecreen(endState m)
{
	endGameScreen = true;
	endGameMode = m;
}

void hideEndGameScreen()
{
	endGameScreen = false;
	endGameMode = endState::FALSE;
	paused = false;
	confirmScreen = false;
	g_StateStack.pop();
}

void handleEndGameEvent(int& choice)
{
	//Poll events
	while (SDL_PollEvent(&event))
	{
		//check events
		switch (event.type)
		{
		case SDL_QUIT: //User hit the X
			choice = 2;
			//confirmMode = confirmState::QUIT;
			break;
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				//resize window
				SDL_SetWindowSize(gWindow, event.window.data1, event.window.data2);
				//SCREEN_WIDTH = event.window.data1;
				//SCREEN_HEIGHT = event.window.data2;
			}
			break;
		case SDL_KEYDOWN:
			break;
		case SDL_MOUSEBUTTONUP:
			//retry button
			if (buttons[0].checkInside(mouseX, mouseY))
			{
				choice = 0;
			}
			//quit to menu button
			if (buttons[1].checkInside(mouseX, mouseY))
			{
				choice = 1;
			}
			break;
		}
	}
}

void EndGame()
{
	//pause all playing audios
	Mix_PauseMusic();
	Mix_Pause(-1);
	switch (endGameMode)
	{
	case endState::WIN:
		myAudio.playGameWin();
		break;
	case endState::LOSE:
		myAudio.playGameLose();
		break;
	case endState::TIME_OVER:
		myAudio.playGameLose();
		break;
	}

	//show back the cursor
	SDL_ShowCursor(SDL_ENABLE);
	//SDL_WarpMouseInWindow(gWindow, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2); //set the cursor to center of the window

	//set text positions
	int textOffset = SCREEN_HEIGHT / 5;
	int textX = SCREEN_WIDTH / 2;
	int textY = SCREEN_HEIGHT / 2 - textOffset;
	int tipsX = SCREEN_WIDTH / 2;
	int tipsY = textY + SCREEN_HEIGHT / 5;

	//add buttons
	//yes button
	int buttonpy = tipsY + 75;
	myButton.init(SCREEN_WIDTH / 2, buttonpy, 50, "Retry", regularFont);
	buttons.push_back(myButton);
	//no button
	buttonpy += 75;
	myButton.init(SCREEN_WIDTH / 2, buttonpy, 50, "Quit to menu", regularFont);
	buttons.push_back(myButton);

	int choice = -1; //0 for retry, 1 for quit to menu

	//set tips
	std::string fullTips;
	dialogueTips.currentLine = GetRandomInt(0, tipsLine.size() - 2, 1);
	fullTips = "Tips: " + tipsLine[dialogueTips.currentLine];

	while (choice == -1)
	{
		deltaTimer.tick();
		mouses = SDL_GetMouseState(&mouseX, &mouseY);
		handleConfirmEvent(choice);

		//Clear screen
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
		SDL_RenderClear(gRenderer);

		//Render backdrop
		SDL_RenderCopy(gRenderer, backdrop, NULL, NULL);

		std::string endGameText = "";
		SDL_Color endGameColor = { 255, 255, 255 };
		//Render text
		switch (endGameMode)
		{
		case endState::WIN:
			gWhiteTexture.setColor(0, 100, 0, 175);
			endGameText = "You win!";
			endGameColor = { 0, 200, 0 };
			break;
		case endState::LOSE:
			gWhiteTexture.setColor(100, 0, 0, 175);
			endGameText = "You died!";
			endGameColor = { 255, 0, 0 };
			break;
		case endState::TIME_OVER:
			gWhiteTexture.setColor(100, 0, 0, 175);
			endGameText = "Time over!";
			endGameColor = { 255, 0, 0 };
			break;
		}
		//Render overlay 
		gWhiteTexture.render(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		//Render text
		drawText(textX, textY, boldFontTitle, endGameColor, endGameText, 1);

		//Render tips
		drawText(tipsX, tipsY, regularFontSmall, UIColor, fullTips, 1);

		//Render buttons
		for (int i = 0; i < buttons.size(); i++)
		{
			buttons[i].checkButton(mouses, mouseX, mouseY);
			buttons[i].render(gRenderer);
		}

		//Update screen
		SDL_RenderPresent(gRenderer);

		frameCap();
	}

	//resume all paused audios
	Mix_ResumeMusic();
	Mix_Resume(-1);

	//remove all buttons
	buttons.clear();

	clearScreen();

	StateStruct temp;
	switch (choice)
	{
	case 0: //retry		
		hideEndGameScreen();
		initedLevel = false;
		break;
	case 1: //quit to menu
		hideEndGameScreen();
		showConfirmScreen(confirmState::QUIT_TO_MENU);
		temp.StatePointer = Confirm;
		g_StateStack.push(temp);
		break;
	case 2: //pressed exit
		hideEndGameScreen();
		showConfirmScreen(confirmState::QUIT);
		temp.StatePointer = Confirm;
		g_StateStack.push(temp);
	}
}
#pragma endregion

#pragma region Exit
void close()
{
	//Free static textures
	gGroundTexture.free();
	gTreeTexture.free();
	gBulletTexture.free();
	gWhiteTexture.free();
	gBloodPoolTexture.free();
	gHealthPickUpTexture.free();
	gMenuTexture.free();

	//Free loaded screen effect textures
	gVignetteTexture.free();
	gLightTexture.free();
	gLensDirtTexture.free();
	gBloodOverlayTexture.free();

	//Free loaded UI textures
	gHealthIconTexture.free();
	gPistolIconTexture.free();
	gRifleIconTexture.free();
	gCrosshairTexture.free();

	//free backdrop
	SDL_DestroyTexture(backdrop);

	//clear vectors
	trees.clear();
	harmZones.clear();
	zombies.clear();
	bloodpools.clear();
	bullets.clear();
	signalZones.clear();
	healthPickUps.clear();

	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Free fonts
	TTF_CloseFont(boldFont);
	boldFont = NULL;
	TTF_CloseFont(boldFontSmall);
	boldFontSmall = NULL;
	TTF_CloseFont(boldFontLarge);
	boldFontLarge = NULL;
	TTF_CloseFont(boldFontTitle);
	boldFontTitle = NULL;
	TTF_CloseFont(regularFont);
	regularFont = NULL;
	TTF_CloseFont(regularFontSmall);
	regularFontSmall = NULL;

	//Quit SDL subsystems
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();

	g_StateStack.swap(emptyStack);
}

void Exit()
{
	close();
}
#pragma endregion

#pragma region Main
int main(int argc, char* argv[])
{
	srand((unsigned)time(0)); //random seed

	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//start the timers
			systemTimer.tick();

			// Our game loop is just a while loop that breaks when our state stack is empty. //
			while (!g_StateStack.empty())
			{
				g_StateStack.top().StatePointer();
			}
		}

		return 0;
	}
}
#pragma endregion