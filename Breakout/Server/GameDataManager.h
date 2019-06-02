#pragma once
#include <Windows.h>
#include "Messages.h"
#include "GameData.h"

enum ScreenDimensions {
	MIN_HEIGHT = 0,
	MIN_WIDTH = 0,
	MAX_HEIGHT = 400,
	MAX_WIDTH = 800
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

	void setupBall();
	void setupPlayers();
	void setupTiles();
	//TODO: make random stuff spawn and add handle to tell u when to start

	void movePlayer(Player * player, int direction);
	void movePlayerPrecise(Player * player, int x);
};
