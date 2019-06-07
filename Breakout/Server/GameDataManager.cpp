#include "GameDataManager.h"
#include "Server.h"

GameDataManager::GameDataManager(GameData * gdata) {
	gameData = gdata;
	hAccessMutex = CreateMutex(NULL, FALSE, NULL);
	if (hAccessMutex == NULL)
	{
		throw 1;
	}

	hGameEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hGameEvent == NULL) {
		throw 2;
	}
}

GameDataManager::~GameDataManager() {
	CloseHandle(hAccessMutex);
	CloseHandle(hGameEvent);
}

void GameDataManager::lockAccessGameData() {
	WaitForSingleObject(hAccessMutex, INFINITE);
}

void GameDataManager::releaseAccessGameData() {
	ReleaseMutex(hAccessMutex);
}

void GameDataManager::setGameEvent() {
	SetEvent(hGameEvent);
}

void GameDataManager::waitForGameEvent() {
	HANDLE game[2];

	game[0] = hGameEvent;
	game[1] = Server::sharedMemory.hExitEvent;

	WaitForMultipleObjects(2, game, FALSE, INFINITE);
}

GameData * GameDataManager::getGameData() {
	return gameData;
}

void GameDataManager::setupGameStart()
{
	SecureZeroMemory(gameData, sizeof(GameData));

	setupBall();
	setupPlayers();

}

void GameDataManager::generateLevel(int difficulty) {
	//TODO: Code here
}

void GameDataManager::setupBall() {
	lockAccessGameData();

	for (auto &ball : gameData->balls) {
		ball.active = false;
		ball.width = BALL_DEFAULT_WIDTH;
		ball.height = BALL_DEFAULT_HEIGHT;
	}

	//Enable and set ball [0] in the center of the screen
	gameData->balls->active = true;
	gameData->balls->posX = MAX_GAME_WIDTH / 2;
	gameData->balls->posY = MAX_GAME_HEIGHT / 2;
	gameData->balls->up = false;
	gameData->balls->right = true;

	releaseAccessGameData();
}

void GameDataManager::setupPlayers() { //TODO: add ACTIVE players in the correct positions
	Server::gameData->lockAccessGameData();

	for (int i = 0; i < MAX_PLAYERS; i++) {
		gameData->players[i].id = i;
		gameData->players[i].height = PLAYER_DEFAULT_HEIGHT;
		gameData->players[i].width = PLAYER_DEFAULT_WIDTH;
		gameData->players[i].lives = Server::config.getInitialLives();
		gameData->players[i].points = 0;
	}

	for (auto & clients : Server::clients.getClientArray()) {
		clients->getPlayer()->active = true;
	}

	//TODO: put players in the right position regarding the size of the playing field

	Server::gameData->releaseAccessGameData();
}

void GameDataManager::movePlayer(Player * selectedPlayer, int direction) {
	int selectedPlayerLeft, selectedPlayerRight;
	int speed;
	bool collision = false;

	lockAccessGameData();

	speed = Server::config.getMovementSpeed();
	selectedPlayerLeft = selectedPlayer->posX;
	selectedPlayerRight = selectedPlayer->posX + selectedPlayer->width;

	//if it's on the edge of the world leaves without change
	if (selectedPlayerLeft == MIN_GAME_WIDTH || selectedPlayerRight == MAX_GAME_WIDTH) {
		releaseAccessGameData();
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

	releaseAccessGameData();
}

void GameDataManager::movePlayerPrecise(Player * selectedPlayer, int x) {
	int selectedPlayerLeft, selectedPlayerRight;
	bool collision = false;

	lockAccessGameData();
	
	selectedPlayerRight = selectedPlayer->posX + selectedPlayer->width;
	selectedPlayerLeft = selectedPlayer->posX;

	x -= selectedPlayer->width / 2;
	
	//Verify if the positions are within bounds and fix them if they are not
	if (x < MIN_GAME_WIDTH) {
		x = MIN_GAME_WIDTH;
	}
	else if (x + selectedPlayer->width > MAX_GAME_WIDTH) {
		x -= (x + selectedPlayer->width) - MAX_GAME_WIDTH;
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

	releaseAccessGameData();
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