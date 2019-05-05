#pragma once
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <string>
#include "UnicodeConfigs.h"

#include "ThreadManager.h"
#include "ConfigHandler.h"
#include "Top10Manager.h"
#include "SharedMemoryManager.h"
#include "GameConfig.h"
#include "GameDataManager.h"
#include "ClientManager.h"

enum ErrorCodes{
	SERVER_STARTED = 0,
	CONFIG_LOADING_ERROR,
	SERVER_ALREADY_RUNNING,
	TOP10_LOADING_ERROR,
	SHARED_MEMORY_ERROR,
	LOCAL_CLIENT_HANDLER_ERROR,

};

using namespace std;

class Server {
private:
	
	static bool isServerRunning() {
		HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, TRUE, SharedMemoryConstants::EVENT_GAMEDATA_UPDATE.c_str());
		if (!hEvent) {
			return false;
		}

		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			return true;
		}

		return false;
	}
public:

	static GameConfig config;
	static Top10Manager topPlayers;
	static ClientManager clients;
	static SharedMemoryManager sharedMemory;
	static ThreadManager threadManager;
	static GameDataManager gameData;

	static int startServer(tstring fileName) {
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

		//TODO:Launch server threads, comm threads and others.
		
		//TODO: Create a game thread, that will handle the game and start the ball as the players loose it and what not
		//TODO: Call Connection Handler to startup connection threads and services

		return SERVER_STARTED;
	}

	static void exitServer() {

		SetEvent(sharedMemory.hExitEvent);
		threadManager.endBallThread();
		threadManager.endGameDataBroadcasterThread();
		threadManager.endLocalClientHandler();

		threadManager.waitForBallThread();
		threadManager.waitForGameDataBroadcaster();
		threadManager.waitForLocalClientThread();
	}
};