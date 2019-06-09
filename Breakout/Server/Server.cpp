#include "Server.h"

GameConfig Server::config;
Top10Manager Server::topPlayers;
SharedMemoryManager Server::sharedMemory;
GameDataManager * Server::gameData = NULL;
ThreadManager Server::threadManager;
ClientManager Server::clients;


bool Server::isServerRunning() {
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("ServerUp"));
	if (!hEvent) {
		return false;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		return true;
	}

	return false;
}

int Server::startServer(tstring fileName) {
	if (isServerRunning()) {
		return SERVER_ALREADY_RUNNING;
	}

	if (topPlayers.loadTop10() < 0) {
		return TOP10_LOADING_ERROR;
	}

	ConfigHandler a(config, fileName);
	if (a.importConfigs()) {
		return CONFIG_LOADING_ERROR;
	}

	if (sharedMemory.initSharedMemory() < 0) {
		return SHARED_MEMORY_ERROR;
	}

	gameData = new GameDataManager(sharedMemory.getGameData());

	if (!threadManager.startLocalClientHandler()) {
		return LOCAL_CLIENT_HANDLER_ERROR;
	}

	if (!threadManager.startRemoteConnectionHandler()) {
		return REMOTE_CONNECTION_HANDLER_ERROR;
	}

	//TODO: Create a game thread, that will handle the game and start the ball as the players loose it and what not

	return SERVER_STARTED;
}

void Server::exitServer() {
	SetEvent(sharedMemory.hExitEvent);

	threadManager.endLocalClientHandler();
	threadManager.endRemoteConnectionHandler();
	threadManager.endBallThread();

	threadManager.waitForRemoteConnectionThread();
	threadManager.waitForLocalClientThread();
	threadManager.waitForBallThread();

	delete gameData;
}