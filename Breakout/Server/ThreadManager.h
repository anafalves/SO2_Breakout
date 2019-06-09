#pragma once
#include <Windows.h>
#include <tchar.h>
#include <vector>
#include "GameDataManager.h"
#include "UnicodeConfigs.h"

enum ThreadConstants {
	BUFFERSIZE = 4096,
};

typedef struct {
	HANDLE pipe;
	bool * CONTINUE;
}PipeInfo;

class Server;

class ThreadManager {
private:
	HANDLE hGameThread;
	bool gameThreadRunning;

	HANDLE hBallThread;
	bool ballThreadRunning;

	HANDLE hLocalClientHandler;
	bool localClientHandlerRunning;

	HANDLE hRemoteConnectionHandler;
	bool remoteConnectionHandlerRunning;

	vector<HANDLE> hBonuses;

public:
	ThreadManager() {
		hBallThread = nullptr;
		ballThreadRunning = false;

		hLocalClientHandler = nullptr;
		localClientHandlerRunning = false;

		hRemoteConnectionHandler = nullptr;
		remoteConnectionHandlerRunning = false;
	};

	bool isBallThreadRunning() const;
	bool isLocalClientHandlerRunning() const;
	bool isRemoteConnectionHandlerRunning() const;
	bool isGameRunning() const;

	bool startGameThread();
	bool startLocalClientHandler();
	bool startRemoteConnectionHandler();
	bool startBallThread();
	bool startBonusThread(Tile * tile);

	void endBallThread();
	void endLocalClientHandler();
	void endRemoteConnectionHandler();
	void endGame();

	void waitForLocalClientThread();
	void waitForRemoteConnectionThread();
	void waitForBallThread();
	void waitForBonusesThreads();
	void waitForGameThread();
};