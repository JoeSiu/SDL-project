#pragma once
#include "Global.h"
#include "GameObject.h"

extern SDL_Rect camera;

class audioManager {
public:
	audioManager();

	void loadAudio();
	bool loadMultiFile(std::vector<Mix_Chunk*>& target, std::string name, std::string format, int total);
	void freeSound(Mix_Chunk* target);
	void freeMultiSounds(std::vector<Mix_Chunk*>& target);
	void playBackgroundMusic();
	void setSoundEffect(int channel, Mix_Chunk* soundEffect);
	void setSoundEffect(int channel, Mix_Chunk* soundEffect, gameObject source);
	void playRadio();
	void playPlayerHurt(gameObject source);
	void resumeWalk(gameObject source);
	void playHitTree(gameObject source);
	void pauseWalk();
	void playReload();
	void stopReload();
	void playZombieAttack(gameObject source);
	void playHitZombie(gameObject source);
	void playGunshot(gameObject source);
	void playSwapWeapon();
	void playGunEmpty();
	void freeAudio();

private:
	//Fixed channel for some sound effect
	const int AMBIENT_CHANNEL = 0;
	const int WALK_CHANNEL = 1;
	const int RELOAD_CHANNEL = 2;
	const int SWAP_WEAPON_CHANNEL = 3;
	const int PLAYER_HURT_CHANNEL = 4;

	//number of variations for some sound effect
	const int PLAYER_HURT_VARIATION = 6;
	const int GUNSHOT_VARIATION = 3;
	const int ZOMBIE_ATTACK_VARIATION = 7;
	const int HIT_ZOMBIE_VARIATION = 4;

	//The music that will be played
	Mix_Music* music;

	//The sound effects that will be used
	Mix_Chunk* ambient;
	Mix_Chunk* radio;
	Mix_Chunk* walk;
	Mix_Chunk* reload;
	Mix_Chunk* swapWeapon;
	Mix_Chunk* gunEmpty;
	Mix_Chunk* hitTree;
	std::vector<Mix_Chunk*> playerHurt;
	std::vector<Mix_Chunk*> gunshot;
	std::vector<Mix_Chunk*> zombieAttack;
	std::vector<Mix_Chunk*> hitZombie;
};