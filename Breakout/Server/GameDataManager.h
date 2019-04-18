#pragma once
#include <Windows.h>
#include "GameData.h"

//No destructor needed since gameData is not owned by it
class GameDataManager
{
	GameData * gameData;
	

public:
	GameDataManager(GameData * gdata) {

		gameData = gdata;
		//TODO: initialize values such as default sizes for field, e.g. width and height, etc
	}

	//Sets initial values and positions of things, such as ball, players, etc.
	void setupGameStart();
};
