#pragma once
#include <Windows.h>
#include "Messages.h"
#include "GameData.h"

enum ScreenDimensions {
	MIN_GAME_HEIGHT = 0,
	MIN_GAME_WIDTH = 0,
	MAX_GAME_HEIGHT = 800,
	MAX_GAME_WIDTH = 1200
};

enum BitmapDimensions {
	BALL_DEFAULT_WIDTH = 20,
	BALL_DEFAULT_HEIGHT = 20,

	PLAYER_DEFAULT_WIDTH = 100,
	PLAYER_DEFAULT_HEIGHT = 50,

	BONUS_DEFAULT_WIDTH = 50,
	BONUS_DEFAULT_HEIGHT = 50,

	TILE_DEFAULT_WIDTH = 80,
	TILE_DEFAULT_HEIGHT = 50
};

//No destructor needed since gameData is not owned by it
class GameDataManager
{
private:
	HANDLE hAccessMutex;
	HANDLE hGameEvent;
	GameData * gameData;

public:
	GameDataManager(GameData * gdata);
	~GameDataManager();

	Player * getAvailablePlayer();
	GameData * getGameData();

	void lockAccessGameData();
	void releaseAccessGameData();
	void setGameEvent();
	void waitForGameEvent();

	//Sets initial values and positions of things, such as ball, players, etc.
	void setupGameStart();

	void generateLevel(int difficulty);//TODO: create this function

	void setupBall();
	void setupTiles();
	void setupPlayers(int difficulty); //TODO: needs to be coded
	//TODO: make random stuff spawn and add handle to tell u when to start

	void setupTiles(int difficulty);

	void setupPlayers();

	void movePlayer(Player * player, int direction);
	void movePlayerPrecise(Player * player, int x);
};
