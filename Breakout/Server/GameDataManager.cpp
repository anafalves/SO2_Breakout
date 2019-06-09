#include "GameDataManager.h"
#include "Server.h"
#include "time.h"

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

void GameDataManager::setGameDataState(int state) {
	lockAccessGameData();
	gameData->gameState = state;
	releaseAccessGameData();
}

void GameDataManager::lockAccessGameData() {
	HANDLE access[2];

	access[0] = hAccessMutex;
	access[1] = Server::sharedMemory.hExitEvent;

	WaitForMultipleObjects(2, access, FALSE, INFINITE);
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
	lockAccessGameData();
	SecureZeroMemory(gameData, sizeof(GameData));
	releaseAccessGameData();

	setupBall();
	setupPlayers();
}

void GameDataManager::generateLevel(int difficulty) {
	//TODO: Code here
	setupTiles(difficulty);
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

void GameDataManager::setupTiles(int difficulty) {

	//for each lvl, the number of tiles increase, until they reach the maximum amount in the last available lvl
	int tile_dif = TILES_MAX_COL_COUNT * TILES_MAX_LINE_COUNT - Server::config.getInitialTileCount();
	int lvl_ratio = difficulty / Server::config.getLevelCount();
	int tiles_number = tile_dif * (lvl_ratio +1); 

	//number of completed rows of tiles
	int tiles_line_count = tiles_number / TILES_MAX_COL_COUNT; 
	//number of tiles in the lasst line (only line uncompleted)
	int tiles_left = tiles_number - tiles_line_count * TILES_MAX_COL_COUNT; 
	int ocuppancyArray[TILES_MAX_COL_COUNT][TILES_MAX_LINE_COUNT] = { {0} };
	
	int n_undestructables = tiles_number / 4;

	int bonus_sum = Server::config.getBallTripleCount() + Server::config.getSlowDownCount() +
					Server::config.getSpeedUpCount() + Server::config.getExtraLifeCount();
	int n_bonus_byRatio = (int) (tiles_number* Server::config.getBonusDropRate());
	int n_normals = tiles_number - n_bonus_byRatio - n_undestructables;

	int  n_bonus_byType[N_BONUS_TYPES] = {
		Server::config.getSpeedUpCount() * n_bonus_byRatio / bonus_sum,
		Server::config.getSlowDownCount() * n_bonus_byRatio / bonus_sum,
		Server::config.getExtraLifeCount() * n_bonus_byRatio / bonus_sum,
		Server::config.getBallTripleCount() * n_bonus_byRatio / bonus_sum
	};

	int index = 0;
	int x;
	int y;

	srand((unsigned) time(NULL));

	lockAccessGameData();

	for (int i = 0; i < n_undestructables; i++) {
		
		do {
			x = rand() % TILES_MAX_COL_COUNT;
			y = rand() % tiles_line_count;
		} while ((ocuppancyArray[x][y] != 0) && (x%2 != 0));
		ocuppancyArray[x][y] = 1;

		gameData->tiles[index].width = TILE_DEFAULT_WIDTH;
		gameData->tiles[index].height = TILE_DEFAULT_HEIGHT;
		gameData->tiles[index].active = true;
		gameData->tiles[index].bonus = NORMAL;
		gameData->tiles[index].resistance = UNBREAKABLE;
		gameData->tiles[index].posX = x* TILE_DEFAULT_WIDTH;
		gameData->tiles[index].posY = y * TILE_DEFAULT_HEIGHT + TILE_DEFAULT_SEPARATOR;

		index++;
	}
	
	BonusType type = SPEED_UP;
	for (int k = 0; k < N_BONUS_TYPES; k++) {
		for (int i = 0; i < n_bonus_byType[k]; i++) {
			do {
				x = rand() % TILES_MAX_COL_COUNT;
				y = rand() % tiles_line_count;
			} while (ocuppancyArray[x][y] != 0);
			ocuppancyArray[x][y] = 1;

			gameData->tiles[index].width = TILE_DEFAULT_WIDTH;
			gameData->tiles[index].height = TILE_DEFAULT_HEIGHT;
			gameData->tiles[index].active = true;
			gameData->tiles[index].bonus = type + k;
			gameData->tiles[index].resistance = 1; //TODO: confirm
			gameData->tiles[index].posX = x * TILE_DEFAULT_WIDTH;
			gameData->tiles[index].posY = y * TILE_DEFAULT_HEIGHT + TILE_DEFAULT_SEPARATOR;

			index++;
		}
	}
	
	for (int i = 0; i < TILES_MAX_COL_COUNT; i++) {
		for (int j = 0; j < tiles_line_count ; j++) {
			if (ocuppancyArray[i][j] == 1)
				continue;

			gameData->tiles[index].width = TILE_DEFAULT_WIDTH;
			gameData->tiles[index].height = TILE_DEFAULT_HEIGHT;
			gameData->tiles[index].active = true;

			gameData->tiles[index].bonus = NORMAL;
			gameData->tiles[index].resistance = 4; //TODO: confirm
			gameData->tiles[index].posX = i * TILE_DEFAULT_WIDTH;
			gameData->tiles[index].posY = j * TILE_DEFAULT_HEIGHT + TILE_DEFAULT_SEPARATOR;
			
			index++;
		}
	}

	for (int i = 0; i < tiles_left; i++) {

		gameData->tiles[index].width = TILE_DEFAULT_WIDTH;
		gameData->tiles[index].height = TILE_DEFAULT_HEIGHT;
		gameData->tiles[index].active = true;

		gameData->tiles[index].bonus = NORMAL;
		gameData->tiles[index].resistance = 4; //TODO: confirm
		gameData->tiles[index].posX = i * TILE_DEFAULT_WIDTH;
		gameData->tiles[index].posY = tiles_line_count * TILE_DEFAULT_HEIGHT + TILE_DEFAULT_SEPARATOR;

		index++;
	}

	releaseAccessGameData();
}

void GameDataManager::setupPlayers() {
	lockAccessGameData();

	int screenSize = MAX_GAME_WIDTH;
	int separator = MAX_GAME_WIDTH /  (Server::clients.getClientArray().size() + 1) ;

	for (int i = 0; i < MAX_PLAYERS; i++) {
		gameData->players[i].id = i;
		gameData->players[i].height = PLAYER_DEFAULT_HEIGHT;
		gameData->players[i].width = PLAYER_DEFAULT_WIDTH;
		gameData->players[i].lives = Server::config.getInitialLives();
		gameData->players[i].points = 0;
		gameData->players[i].posX = i + separator - PLAYER_DEFAULT_WIDTH/2;
		gameData->players[i].posY = MAX_GAME_HEIGHT - 60;
	}

	for (auto & clients : Server::clients.getClientArray()) {
		clients->getPlayer()->active = true;
		_tcscpy_s(clients->getPlayer()->name, clients->getName().c_str());
	}

	releaseAccessGameData();
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

	lockAccessGameData();

	for (auto &player : gameData->players) {
		if (!player.active) {
			releaseAccessGameData();
			return &player;
		}
	}

	releaseAccessGameData();

	return nullptr;
}