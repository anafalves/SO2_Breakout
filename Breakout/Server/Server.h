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
	CONFIG_LOADING_ERROR = 1,
	SERVER_ALREADY_RUNNING,
	TOP10_LOADING_ERROR,
	SHARED_MEMORY_ERROR,


	
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

		//TODO:Launch server threads, comm threads and others.
		
		//TODO: Create a game thread, that will handle the game and start the ball as the players loose it and what not
		//TODO: Call Connection Handler to startup connection threads and services

		//waitForThreads();
		return 0;
	}

	static void waitForThreads() {
		threadManager.waitForBallThread();
	}

	static void showConfigs() {
		tcout << config;
	}
};