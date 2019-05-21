#include "Server.h"

GameConfig Server::config;
Top10Manager Server::topPlayers;
SharedMemoryManager Server::sharedMemory;
GameDataManager Server::gameData(nullptr);
ThreadManager Server::threadManager;
ClientManager Server::clients;


bool Server::isServerRunning() {
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, TRUE, SharedMemoryConstants::EVENT_GAMEDATA_UPDATE.c_str());
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

	gameData = GameDataManager(sharedMemory.getGameData());

	if (!threadManager.startLocalClientHandler()) {
		return LOCAL_CLIENT_HANDLER_ERROR;
	}

	if (!threadManager.startRemoteConnectionHandler()) {
		return REMOTE_CONNECTION_HANDLER_ERROR;
	}
	//TODO:Launch server threads, comm threads and others.

	//TODO: Create a game thread, that will handle the game and start the ball as the players loose it and what not
	//TODO: Call Connection Handler to startup connection threads and services

	return SERVER_STARTED;
}

void Server::exitServer() {

	SetEvent(sharedMemory.hExitEvent);
	threadManager.endBallThread();
	threadManager.endGameDataBroadcasterThread();
	threadManager.endLocalClientHandler();

	threadManager.waitForBallThread();
	threadManager.waitForGameDataBroadcaster();
	threadManager.waitForLocalClientThread();
}