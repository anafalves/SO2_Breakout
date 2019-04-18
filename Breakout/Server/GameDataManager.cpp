#include "GameDataManager.h"

enum ScreenDimensions {
	HEIGHT = 1080,
	WIDTH = 1920
};

void GameDataManager::setupGameStart()
{
	//Enable and set ball [0] in the center of the screen
	gameData->balls->active = true;
	gameData->balls->posX = WIDTH / 2;
	gameData->balls->posY = HEIGHT / 2;


	////Ball Init
	//for (auto &x : viewGameData->balls) {
	//	x.active = false;
	//	x.posX = 0;
	//	x.posY = 0;
	//	x.width = 0;
	//	x.height = 0;
	//}

	////Tile Init
	//for (auto &x : viewGameData->tiles) {
	//	x.active = false;
	//	x.bonus = false;
	//	x.resistance = 0;
	//	x.posX = 0;
	//	x.posY = 0;
	//	x.width = 0;
	//	x.height = 0;
	//}

	////Bonuses init
	//for (auto &x : viewGameData->bonuses) {
	//	x.active = false;
	//	x.type = 0;
	//	x.posX = 0;
	//	x.posY = 0;
	//	x.width = 0;
	//	x.height = 0;
	//}

	////Player init
	//for (auto &x : viewGameData->players) {
	//	x.active = false;
	//	x.lives = 0;
	//	x.points = 0;
	//	x.posX = 0;
	//	x.posY = 0;
	//	x.width = 0;
	//	x.height = 0;
	//}
}