//All music and sound effect is from https://freesound.org/
#include "Include/Audio.h"

audioManager::audioManager()
{
	//The music that will be played
	music = NULL;

	//The sound effects that will be used
	radio = NULL;
	walk = NULL;
	reload = NULL;
	swapWeapon = NULL;
	gunEmpty = NULL;
};

bool audioManager::loadMultiFile(std::vector<Mix_Chunk*>& target, std::string name, std::string format, int total)
{
	for (int i = 0; i < total; i++)
	{
		std::string path = "Assets/Audio/" + name + " (" + std::to_string(i + 1) + ")." + format;
		target.push_back(Mix_LoadWAV(path.c_str()));
		if (target[i] == NULL)
		{
			printf("Failed to load file %i!\n", i + 1);
			return false;
		}
	}
	return true;
}

void audioManager::freeSound(Mix_Chunk* target)
{
	Mix_FreeChunk(target);
	target = NULL;
}

void audioManager::freeMultiSounds(std::vector<Mix_Chunk*>& target)
{
	for (int i = 0; i < target.size(); i++)
	{
		Mix_FreeChunk(target[i]);
		target[i] = NULL;
	}
}

void audioManager::loadAudio()
{
	//load music
	music = Mix_LoadMUS("Assets/Audio/background music.mp3");
	if (music == NULL)
	{
		printf("Failed to load background music! SDL_mixer Error: %s\n", Mix_GetError());
		exit(1);
	}

	//load sound effects
	ambient = Mix_LoadWAV("Assets/Audio/background ambient.mp3");
	if (ambient == NULL)
	{
		printf("Failed to load background ambient sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		exit(1);
	}

	radio = Mix_LoadWAV("Assets/Audio/radio beep.mp3");
	if (radio == NULL)
	{
		printf("Failed to load radio sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		exit(1);
	}

	gunEmpty = Mix_LoadWAV("Assets/Audio/gun empty.wav");
	if (gunEmpty == NULL)
	{
		printf("Failed to load gun empty sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		exit(1);
	}

	reload = Mix_LoadWAV("Assets/Audio/reload.wav");
	if (reload == NULL)
	{
		printf("Failed to load reload sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		exit(1);
	}

	swapWeapon = Mix_LoadWAV("Assets/Audio/swap weapon.mp3");
	if (swapWeapon == NULL)
	{
		printf("Failed to load swap weapon sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		exit(1);
	}

	walk = Mix_LoadWAV("Assets/Audio/walk.mp3");
	if (walk == NULL)
	{
		printf("Failed to load walk effect! SDL_mixer Error: %s\n", Mix_GetError());
		exit(1);
	}

	hitTree = Mix_LoadWAV("Assets/Audio/hit tree.mp3");
	if (walk == NULL)
	{
		printf("Failed to load hit tree effect! SDL_mixer Error: %s\n", Mix_GetError());
		exit(1);
	}

	collectObject = Mix_LoadWAV("Assets/Audio/collect object.mp3");
	if (walk == NULL)
	{
		printf("Failed to load collect object effect! SDL_mixer Error: %s\n", Mix_GetError());
		exit(1);
	}

	gameLose = Mix_LoadWAV("Assets/Audio/game lose.wav");
	if (walk == NULL)
	{
		printf("Failed to load game lose effect! SDL_mixer Error: %s\n", Mix_GetError());
		exit(1);
	}

	gameWin = Mix_LoadWAV("Assets/Audio/game win.wav");
	if (walk == NULL)
	{
		printf("Failed to load game win effect! SDL_mixer Error: %s\n", Mix_GetError());
		exit(1);
	}

	if (!loadMultiFile(playerHurt, "player hurt", "mp3", PLAYER_HURT_VARIATION))
	{
		printf("Failed to load player hurt effect! SDL_mixer Error: %s\n", Mix_GetError());
		exit(1);
	}

	if (!loadMultiFile(gunshot, "gunshot", "mp3", GUNSHOT_VARIATION))
	{
		printf("Failed to load walk effect! SDL_mixer Error: %s\n", Mix_GetError());
		exit(1);
	}

	if (!loadMultiFile(zombieAttack, "zombie attack", "mp3", ZOMBIE_ATTACK_VARIATION))
	{
		printf("Failed to load zombie attack effect! SDL_mixer Error: %s\n", Mix_GetError());
		exit(1);
	}

	if (!loadMultiFile(hitZombie, "hit zombie", "wav", HIT_ZOMBIE_VARIATION))
	{
		printf("Failed to load hit zombie effect! SDL_mixer Error: %s\n", Mix_GetError());
		exit(1);
	}

	//set music volume
	Mix_VolumeMusic(48);

	//set sound effect volume
	Mix_Volume(-1, 64);
}

void audioManager::playMainMusic()
{
	//Play the music
	Mix_PlayMusic(music, -1);
}


void audioManager::playBackgroundLoop()
{
	//Play ambient sound and walk loop
	Mix_PlayChannel(AMBIENT_CHANNEL, ambient, -1); //start the infinite ambient sound effect
	Mix_PlayChannel(WALK_CHANNEL, walk, -1); //start the infinite walk sound effect
}

void audioManager::stopBackgroundLoop()
{
	Mix_HaltChannel(AMBIENT_CHANNEL);
	Mix_HaltChannel(WALK_CHANNEL);
}


void audioManager::setSoundEffect(int channel, Mix_Chunk* soundEffect)
{
	//Mix_HaltChannel(channel);  //to stop the last sound effect from playing
	Mix_SetPanning(channel, 255, 255); //reset the directional sound effect
	Mix_PlayChannel(channel, soundEffect, 0);
}

void audioManager::setSoundEffect(int channel, Mix_Chunk* soundEffect, gameObject source)
{
	int right = calOnScreenXPosition(camera, source.px);
	right = map(right, 0, SCREEN_WIDTH, 0, 255);
	//Mix_HaltChannel(channel); //to stop the last sound effect from playing
	Mix_SetPanning(channel, 255 - right, right); //for the directional sound effect
	Mix_PlayChannel(channel, soundEffect, 0);
}

void audioManager::playRadio()
{
	setSoundEffect(-1, radio);
}


void audioManager::pauseWalk()
{
	Mix_Pause(WALK_CHANNEL);
}

void audioManager::resumeWalk(gameObject source)
{
	int right = calOnScreenXPosition(camera, source.px);
	right = map(right, 0, SCREEN_WIDTH, 0, 255);
	Mix_SetPanning(WALK_CHANNEL, 255 - right, right); //for the directional sound effect
	Mix_Resume(WALK_CHANNEL);
}

void audioManager::playPlayerHurt(gameObject source)
{
	if (Mix_Playing(PLAYER_HURT_CHANNEL) == 0)
	{
		int i = GetRandomInt(0, PLAYER_HURT_VARIATION - 2, 1);
		setSoundEffect(PLAYER_HURT_CHANNEL, playerHurt[i], source);
	}
}

void audioManager::playHitTree(gameObject source)
{
	setSoundEffect(-1, hitTree, source);
}

void audioManager::playReload()
{
	setSoundEffect(RELOAD_CHANNEL, reload);
}

void audioManager::stopReload()
{
	Mix_HaltChannel(RELOAD_CHANNEL); //to stop the last sound effect from playing
}

void audioManager::playSwapWeapon() {
	setSoundEffect(SWAP_WEAPON_CHANNEL, swapWeapon);
}

void audioManager::playGunshot(gameObject source) {
	int i = GetRandomInt(0, GUNSHOT_VARIATION - 2, 1);
	setSoundEffect(-1, gunshot[i], source);
}

void audioManager::playZombieAttack(gameObject source) {
	int i = GetRandomInt(0, ZOMBIE_ATTACK_VARIATION - 2, 1);
	setSoundEffect(-1, zombieAttack[i], source);
}

void audioManager::playHitZombie(gameObject source) {
	int i = GetRandomInt(0, HIT_ZOMBIE_VARIATION - 2, 1);
	setSoundEffect(-1, hitZombie[i], source);
}

void audioManager::playGunEmpty()
{
	setSoundEffect(4, gunEmpty);
}

void audioManager::playCollectObject()
{
	setSoundEffect(-1, collectObject);
}

void audioManager::playGameLose()
{
	setSoundEffect(-1, gameLose);
}

void audioManager::playGameWin()
{
	setSoundEffect(-1, gameWin);
}

void audioManager::freeAudio()
{
	freeSound(ambient);
	freeSound(radio);
	freeSound(walk);
	freeSound(reload);
	freeSound(swapWeapon);
	freeSound(gunEmpty);
	freeSound(hitTree);
	freeSound(collectObject);
	freeSound(gameLose);
	freeSound(gameWin);

	freeMultiSounds(playerHurt);
	freeMultiSounds(gunshot);
	freeMultiSounds(zombieAttack);
	freeMultiSounds(hitZombie);

	//Free the music
	Mix_FreeMusic(music);
	music = NULL;
}