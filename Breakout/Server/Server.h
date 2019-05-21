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
	REMOTE_CONNECTION_HANDLER_ERROR,

};

using namespace std;

class Server {
private:
	
	static bool isServerRunning();
public:

	static GameConfig config;
	static Top10Manager topPlayers;
	static ClientManager clients;
	static SharedMemoryManager sharedMemory;
	static ThreadManager threadManager;
	static GameDataManager gameData;

	static int startServer(tstring fileName);
	static void exitServer();
};