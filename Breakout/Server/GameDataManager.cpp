#include "GameDataManager.h"
#include "Server.h"

GameDataManager::GameDataManager(GameData * gdata) {
	gameData = gdata;
	hAccessMutex = CreateMutex(NULL, FALSE, NULL);
	if (hAccessMutex == NULL)
	{
		throw 1;
	}
	//TODO: initialize values such as default sizes for field, e.g. width and height, etc
}

void GameDataManager::lockAccessGameData() {
	WaitForSingleObject(hAccessMutex, INFINITE);
}

void GameDataManager::releaseAccessGameData() {
	ReleaseMutex(hAccessMutex);
}

GameData * GameDataManager::getGameData() {
	return gameData;
}

void GameDataManager::setupGameStart()
{
	setupBall();
	setupTiles();


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
	WaitForSingleObject(hAccessMutex, INFINITE);
	//Enable and set ball [0] in the center of the screen
	gameData->balls->active = true;
	gameData->balls->posX = MAX_WIDTH / 2;
	gameData->balls->posY = MAX_HEIGHT / 2;
	gameData->balls->height = 20;
	gameData->balls->width = 20;
	gameData->balls->up = false;
	gameData->balls->right = true;

	ReleaseMutex(hAccessMutex);
}

void GameDataManager::setupPlayers() { //TODO: add ACTIVE players in the correct positions

}

void GameDataManager::setupTiles() {
	for (int i = 0; i < 10; i++) {
		gameData->tiles[i].active = true;
		gameData->tiles[i].resistance = 1;
		gameData->tiles[i].bonus = NORMAL;
		gameData->tiles[i].width = 40;
		gameData->tiles[i].height = 20;
		gameData->tiles[i].posX = i * 45;
		gameData->tiles[i].posY = 300;
	}

	//TODO: generate random tiles? 
}

void GameDataManager::movePlayer(Player * selectedPlayer, int direction) {
	int selectedPlayerLeft, selectedPlayerRight;
	int speed;
	bool collision = false;

	WaitForSingleObject(hAccessMutex, INFINITE);

	speed = Server::config.getMovementSpeed();
	selectedPlayerLeft = selectedPlayer->posX;
	selectedPlayerRight = selectedPlayer->posX + selectedPlayer->width;

	//if it's on the edge of the world leaves without change
	if (selectedPlayerLeft == MIN_WIDTH || selectedPlayerRight == MAX_WIDTH) {
		ReleaseMutex(hAccessMutex);
		return;
	}

	if (direction == LEFT)
	{
		for (const auto & player : gameData->players) {
			if (!player.active)
				continue;

			//if there is a collision ativates a flag
			if ((selectedPlayerLeft - speed) < player.posX + player.width) {
				collision = true;
				break;
			}
		}

		if (!collision)
			selectedPlayer->posX -= speed;
	}
	else
	{
		for (const auto & player : gameData->players) {
			if (!player.active)
				continue;

			//if there is a collision ativates a flag
			if ((selectedPlayerRight + speed) < player.posX) {
				collision = true;
				break;
			}
		}

		if (!collision)
			selectedPlayer->posX += speed;
	}

	ReleaseMutex(hAccessMutex);
}

void GameDataManager::movePlayerPrecise(Player * selectedPlayer, int x) {
	int selectedPlayerLeft, selectedPlayerRight;
	bool collision = false;

	WaitForSingleObject(hAccessMutex, INFINITE);
	
	selectedPlayerRight = selectedPlayer->posX + selectedPlayer->width;
	selectedPlayerLeft = selectedPlayer->posX;

	x -= selectedPlayer->width / 2;
	
	//Verify if the positions are within bounds and fix them if they are not
	if (x < MIN_WIDTH) {
		x = MIN_WIDTH;
	}
	else if (x + selectedPlayer->width > MAX_WIDTH) {
		x -= (x + selectedPlayer->width) - MAX_WIDTH;
	}

	for (const auto & player : gameData->players) {
		if (!player.active)
			continue;

		//if there is a collision ativates a flag
		if (x <= player.posX + player.width || x + selectedPlayer->width >= player.posX) {
			collision = true;
			break;
		}
	}

	if (!collision)
		selectedPlayer->posX = x;

	ReleaseMutex(hAccessMutex);
}

Player * GameDataManager::getAvailablePlayer() {
	WaitForSingleObject(hAccessMutex, INFINITE);

	for (auto &player : gameData->players) {
		if (!player.active) {
			ReleaseMutex(hAccessMutex);
			return &player;
		}
	}

	ReleaseMutex(hAccessMutex);

	return nullptr;
}