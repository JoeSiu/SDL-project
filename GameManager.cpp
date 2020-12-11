#include "Global.h"
#include "Graphics.h"
#include "Player.h"
#include "Zombie.h"
#include "Weapon.h"
#include "Timer.h"
#include "Audio.h"
#include "PopUpText.h"
#include "Button.h"

//render window
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

//for menus
std::stack<StateStruct> g_StateStack;
void Menu();
void Game();
void Pause();
void Exit();

//confirm screen
void Confirm();
std::string confirmText = "";
enum class confirmState { RETRY, QUIT, QUIT_TO_MENU };
confirmState confirmMode;
void showConfirmScreen(std::string t, confirmState m);
void hideConfirmScreen();

//void handleMenuEvent();
//void handleMenuInput();
void handleGameEvent();
void handleGameInput();
void handlePauseEvent();
//void handlePauseInput();
//void handleExitEvent();
//void handleExitInput();
void handleConfirmEvent(int& choice);
void clearScreen();

//Create game objects
player myPlayer;
gameObject myTree;
gameObject myHarmZone;
zombie myZombie;

//textures
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
//backdrop texture used for pause screen
SDL_Texture* backdrop;

//colors
SDL_Color whiteColor = { 255, 255, 255 };
SDL_Color blackColor = { 0, 0, 0 };
SDL_Color UIColor = whiteColor;

//animation related
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

//tree clips
std::vector <SDL_Rect> gTreeClips;

//audio
audioManager myAudio;

//fonts
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

//objectives related
int totalZombieKilled = 0;
bool tutorial = true;
int currentObjective = -1;
bool objective[TOTAL_OBJECTIVE];
std::string objectiveText;

//Dialogue related
std::vector <std::string> dialogueLine; //dialogue texts
std::vector <std::string> tipsLine; //tips texts
popUpText dialogue(3.0f, 1.0f);
popUpText dialogueTips(3.0f, 2.0f);

//framerate related
LTimer systemTimer; //The frames per second timer
LTimer deltaTimer; //The frames per second cap timer
int countedFrames = 0; //total frames

//input
int mouseX;
int mouseY;
const Uint8* keys;
Uint32 mouses;

//flags
bool allowSpawnZombie = false; //flag for zombie spawning
bool initedLevel = false;
bool quit = false;
bool paused = false;
bool confirmScreen = false;
bool win = false;
bool lose = false; //or use int or enums

//Event handler
SDL_Event event;

//Game camera
SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

//store the game objects in vector
std::vector<gameObject> trees;
std::vector<gameObject> harmZones;
std::vector<zombie> zombies;
std::vector<gameObject> bloodpools;
std::vector<bullet> bullets;

//buttons
button myButton;
std::vector<button> buttons;

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

	//add exit
	StateStruct state;
	state.StatePointer = Exit;
	g_StateStack.push(state);
	//add menu //temp: game
	state.StatePointer = Game;
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

void close()
{
	//Free static textures
	gGroundTexture.free();
	gTreeTexture.free();
	gBulletTexture.free();
	gWhiteTexture.free();
	gBloodPoolTexture.free();

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
	TTF_CloseFont(regularFont);
	regularFont = NULL;
	TTF_CloseFont(regularFontSmall);
	regularFontSmall = NULL;

	//Quit SDL subsystems
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
	std::stack<StateStruct> emptyStack;
	g_StateStack.swap(emptyStack);
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

void initLevel()
{
	//init player
	myPlayer.initPlayer();

	//create weapon
	myPlayer.myWeapon[0].init(PISTOL_NAME, PISTOL_DAMAGE, PISTOL_RATE_OF_FIRE, PISTOL_CLIP_SIZE, PISTOL_CLIP_SIZE, PISTOL_RELOAD_TIME);
	myPlayer.myWeapon[1].init(RIFLE_NAME, RIFLE_DAMAGE, RIFLE_RATE_OF_FIRE, RIFLE_CLIP_SIZE, RIFLE_CLIP_SIZE, RIFLE_RELOAD_TIME);

	//create trees
	createGameObjectRandom(myTree, trees, MAX_TREE_NUM, MIN_TREE_SIZE, MAX_TREE_SIZE, -1, TREES_CLIP);

	//create harmzone
	createGameObjectRandom(myHarmZone, harmZones, MAX_HARM_ZONE_NUM * DIFFICULTY, MIN_HARM_ZONE_SIZE, MAX_HARM_ZONE_SIZE);

	//init objective
	for (int i = 0; i < TOTAL_OBJECTIVE; i++)
	{
		objective[i] = false;
	}
}

void checkObjective0()
{
	if (dialogue.currentPart == 1)
	{
		currentObjective++;
	}
}

void checkObjective1(int key)
{
	if (currentObjective == 0)
	{
		static bool keyPressed[4] = { false, false, false, false }; //whether w, a, s, d have been pressed
		keyPressed[key] = true;
		if (keyPressed[0] && keyPressed[1] && keyPressed[2] && keyPressed[3])
		{
			objective[0] = true;
		}
	}
}

void checkObjective2()
{
	if (currentObjective == 1)
	{
		objective[1] = true;
	}
}

void checkObjective3()
{
	if (currentObjective == 2)
	{
		objective[2] = true;
		tutorial = false;
	}
}

void checkObjective4()
{
	if (currentObjective == 3)
	{
		static int zombieKilled = 0;
		zombieKilled++;

		if (zombieKilled >= 5) //killed 5 zombies
		{
			objective[3] = true;
		}
	}
}

void updateObjective()
{
	static int spawned = false;

	switch (currentObjective)
	{
	case -1: //waiting for first dialogue to finish
		objectiveText = "waiting for objective";
		checkObjective0();
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
	case 3: //objective 4: kill 5 zombies (tutorial ended, zombie start spawning now)
		objectiveText = "kill 5 zombies";
		//spawn zombie
		if (!spawned)
		{
			allowSpawnZombie = true;
			spawned = true;
		}
		break;
	case 4: //objective 5: reach to the 4 corners
		objectiveText = "get to the 4 corners of this area";
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
	static int timeLeft = TIME_LIMIT;
	static float timeCounter = 0;
	//calulate countdown time
	if (!systemTimer.isPaused() && !tutorial && timeLeft > 0)
	{
		timeCounter += systemTimer.getDeltaTime();
	}
	else if (timeLeft <= 0)
	{
		//TODO: pop game and go to death screen (or push death screem) if objective not complete
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
			showConfirmScreen("Are you sure you want to quit?", confirmState::QUIT);
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
					checkObjective3();
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
			if (event.key.keysym.sym == SDLK_LCTRL) //temp
			{
				trees.clear();
				harmZones.clear();
				zombies.clear();
				allowSpawnZombie = true;
				initLevel();
			}
			if (event.key.keysym.sym == SDLK_UP) //temp
			{
				MAX_ZOMBIE_NUM++;
				printf("maximum zombie number = %i\n", MAX_ZOMBIE_NUM);
			}
			if (event.key.keysym.sym == SDLK_DOWN) //temp
			{
				MAX_ZOMBIE_NUM--;
				printf("maximum zombie number = %i\n", MAX_ZOMBIE_NUM);
			}
			if (event.key.keysym.sym == SDLK_SPACE) //temp
			{
				myPlayer.health = 100;
				printf("player health restored\n");
			}
			if (event.key.keysym.sym == SDLK_KP_ENTER) //temp
			{
				objective[4] = true; //temp
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
		checkObjective1(0);
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
		checkObjective1(1);
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
		checkObjective1(2);
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
		checkObjective1(3);
	}

	if (keys[SDL_SCANCODE_LSHIFT]) //temp
	{
		myPlayer.vx *= 2;
		myPlayer.vy *= 2;
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
			checkObjective2();
		}
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

	//calulate player position
	float dirX = myPlayer.vx * myPlayer.speed * deltaTimer.getDeltaTime();
	float dirY = myPlayer.vy * myPlayer.speed * deltaTimer.getDeltaTime();
	myPlayer.px += dirX;
	myPlayer.py += dirY;

	//check collision with game objects
		//trees
	/*for (int i = 0; i < trees.size(); i++)
	{
		if (myPlayer.checkCollision(trees[i]))
		{
			myPlayer.px -= dirX;
			myPlayer.py -= dirY;
			break;
		}
	}*/
	for (int i = 0; i < zombies.size(); i++)
	{
		if (zombies[i].currentState != zombieState::DEAD && myPlayer.checkCollision(zombies[i], zombies[i].attackRange))
		{
			myPlayer.px -= dirX;
			myPlayer.py -= dirY;
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

void renderGameObject(LTexture& sourceTexture, std::vector<gameObject>& vectorList)
{
	if (vectorList.size() > 0)
	{
		for (int i = 0; i < vectorList.size(); i++)
		{
			sourceTexture.render(camera, vectorList[i].rx, vectorList[i].ry, vectorList[i].size, vectorList[i].size, NULL, vectorList[i].rotation);
		}
	}
}

void renderGameObject(LTexture& sourceTexture, std::vector<gameObject>& vectorList, std::vector<SDL_Rect> clips)
{
	if (vectorList.size() > 0 && clips.size() > 0)
	{
		for (int i = 0; i < vectorList.size(); i++)
		{
			sourceTexture.render(camera, vectorList[i].rx, vectorList[i].ry, vectorList[i].size, vectorList[i].size, &clips[vectorList[i].type], vectorList[i].rotation);
		}
	}
}

void updateBullet()
{
	int i = 0;
	gBulletTexture.setColor(255, 200, 0);
	while (i < bullets.size())
	{
		//float offsetX = cos(myPlayer.rotation * M_PI / 180.0) * PLAYER_SIZE / 2;
		//float offsetY = sin(myPlayer.rotation * M_PI / 180.0) * PLAYER_SIZE / 2;
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
						checkObjective4();
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
			printf("spawned a zombie, type = %i\n", zombies.back().type); //temp
		}
		printf("--- total zombie = %i ---\n", zombies.size()); //temp
		allowSpawnZombie = false;
	}
}

void setZombieAnimation(zombie& source)
{
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

void Menu()
{

}

void Game()
{
	clearScreen();

	if (!initedLevel)
	{
		//init level
		initLevel();

		//play background music
		myAudio.playBackgroundMusic();

		//start the timers
		systemTimer.tick();
		systemTimer.tick();

		initedLevel = true;
	}

	//While application is running
	while (!quit && !paused &&!confirmScreen)
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

		//Update and render player
		updatePlayer();

		//Update and render zombie
		updateZombie();

		//Render lightings
		renderLighting();

		//Render game objects
		//render trees
		renderGameObject(gTreeTexture, trees, gTreeClips);
		//render harm zone
		gLightTexture.setColor(255, 0, 0, 200);
		renderGameObject(gLightTexture, harmZones);

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

		//Render text
		drawUI();

		//Render crosshair
		renderCrosshair();

		//Update screen
		SDL_RenderPresent(gRenderer);

		//Update animation timer
		updateAnimation();

		//cap frame rate
		frameCap();
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
	if (quit)
	{
		while (!g_StateStack.empty())
		{
			g_StateStack.pop();
		}
	}
}

void handlePauseEvent()
{
	//Poll events
	while (SDL_PollEvent(&event))
	{
		//check events
		switch (event.type)
		{
		case SDL_QUIT: //User hit the X		
			paused = false;
			showConfirmScreen("Are you sure you want to quit?", confirmState::QUIT);
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
				paused = false;
				showConfirmScreen("Are you sure you want to retry the level?", confirmState::RETRY);
			}
			//quit to menu button
			if (buttons[2].checkInside(mouseX, mouseY))
			{
				paused = false;
				showConfirmScreen("Are you sure you want to quit to menu?", confirmState::QUIT_TO_MENU);
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
	int pausedOffset = SCREEN_HEIGHT / 4;
	int pausedX = SCREEN_WIDTH / 2;
	int pausedY = SCREEN_HEIGHT / 2 - pausedOffset;
	int tipsX = SCREEN_WIDTH / 2;
	int tipsY = pausedY + SCREEN_HEIGHT / 5;

	//add buttons
	//resume button
	int button1py = tipsY + 75;
	myButton.init(SCREEN_WIDTH / 2, button1py, 50, "Resume", regularFont);
	buttons.push_back(myButton);
	//retry button
	int button2py = button1py + 75;
	myButton.init(SCREEN_WIDTH / 2, button2py, 50, "Retry", regularFont);
	buttons.push_back(myButton);
	//quit to menu button
	int button3py = button2py + 75;
	myButton.init(SCREEN_WIDTH / 2, button3py, 50, "Quit to menu", regularFont);
	buttons.push_back(myButton);

	//set tips
	std::string fullTips;
	dialogueTips.currentLine = GetRandomInt(0, tipsLine.size() - 2, 1);
	fullTips = "Tips: " + tipsLine[dialogueTips.currentLine];

	while (paused)
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

	if (confirmScreen)
	{
		StateStruct temp;
		temp.StatePointer = Confirm;
		g_StateStack.push(temp);
	}
	else
	{
		g_StateStack.pop();
	}
}

void Exit()
{
	close();
}

void showConfirmScreen(std::string t, confirmState m)
{
	confirmScreen = true;
	confirmText = t;
	confirmMode = m;
}

void hideConfirmScreen()
{
	confirmScreen = false;
	confirmText = "";
	paused = true;
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
			choice = 0;
			confirmMode = confirmState::QUIT;
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
	int button1py = textY + 75;
	myButton.init(SCREEN_WIDTH / 2, button1py, 50, "Yes", regularFont);
	buttons.push_back(myButton);
	//no button
	int button2py = button1py + 75;
	myButton.init(SCREEN_WIDTH / 2, button2py, 50, "No", regularFont);
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

		//Render text
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

	switch (choice)
	{
	case 0:
		if (confirmMode == confirmState::RETRY) //retry
		{
			hideConfirmScreen();

			//TODO: add init level and reset objective, dialogue
			g_StateStack.pop();
		}
		if (confirmMode == confirmState::QUIT) //quit
		{
			StateStruct temp;
			temp.StatePointer = Exit;
			g_StateStack.push(temp);
		}
		if (confirmMode == confirmState::QUIT_TO_MENU) //quit to menu
		{

		}		
		break;
	case 1:
		hideConfirmScreen();
		g_StateStack.pop();
		break;
	}
}

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
			// Our game loop is just a while loop that breaks when our state stack is empty. //
			while (!g_StateStack.empty())
			{
				g_StateStack.top().StatePointer();
			}
		}

		return 0;
	}
}
