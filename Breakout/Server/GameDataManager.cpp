#include "GameDataManager.h"

enum ScreenDimensions {
	MIN_HEIGHT = 0,
	MIN_WIDTH = 0,
	MAX_HEIGHT = 400,
	MAX_WIDTH = 800
};

void GameDataManager::setupGameStart()
{
	setupBall();


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

void GameDataManager::setupBall() {
	//Enable and set ball [0] in the center of the screen
	gameData->balls->active = true;
	gameData->balls->posX = MAX_WIDTH / 2;
	gameData->balls->posY = MAX_HEIGHT / 2;
	gameData->balls->up = true;
	gameData->balls->right = true;
}

void GameDataManager::moveActiveBalls() {
	//TODO: Mutex here 
	for (auto &ball : gameData->balls) {

		if (!ball.active) {
			continue;
		}

		if (ball.up) {
			ball.posY++;
		}
		else {
			ball.posY--;
		}

		if (ball.right) {
			ball.posX++;
		}
		else {
			ball.posX--;
		}


		//Verify if ball is in one of the of the limits, so it can change position
		if (ball.posX == MAX_WIDTH || ball.posX == MIN_WIDTH) {
			ball.right = !ball.right;
		}

		if (ball.posY == MAX_HEIGHT || ball.posY == MIN_HEIGHT) {
			ball.up = !ball.up;
		}

		//TODO: if ball hits lower limit, ball is lost, and players lose one life.
	}
}

Player * GameDataManager::getAvailablePlayer() {
	for (auto &player : gameData->players) {
		if (!player.active) {
			return &player;
		}
	}

	return nullptr;
}