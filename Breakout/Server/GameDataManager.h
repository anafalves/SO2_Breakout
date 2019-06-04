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
	GameData * gameData;

public:
	GameDataManager(GameData * gdata);

	Player * getAvailablePlayer();
	GameData * getGameData();

	void lockAccessGameData();
	void releaseAccessGameData();

	//Sets initial values and positions of things, such as ball, players, etc.
	void setupGameStart();

	void generateLevel(int difficulty);
	

	void setupBall();
	void setupPlayers();
	void setupTiles();
	//TODO: make random stuff spawn and add handle to tell u when to start

	void movePlayer(Player * player, int direction);
	void movePlayerPrecise(Player * player, int x);
};
