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
	HANDLE hBallThread;
	bool ballThreadRunning;

	HANDLE hLocalClientHandler;
	bool localClientHandlerRunning;

	HANDLE hBroadcastThread;
	bool broadcastRunning;

	HANDLE hRemoteConnectionHandler;
	bool remoteConnectionHandlerRunning;

	vector<HANDLE> hBonuses;

public:
	ThreadManager() {
		hBallThread = nullptr;
		ballThreadRunning = false;

		hLocalClientHandler = nullptr;
		localClientHandlerRunning = false;

		hBroadcastThread = nullptr;
		broadcastRunning = false;

		hRemoteConnectionHandler = nullptr;
		remoteConnectionHandlerRunning = false;
	};

	bool isBallThreadRunning() const;
	bool isBroadcastRunning() const;
	bool isLocalClientHandlerRunning() const;
	bool isRemoteConnectionHandlerRunning() const;

	bool startGameDataBroadcaster();
	bool startLocalClientHandler();
	bool startRemoteConnectionHandler();
	bool startBallThread();
	bool startBonusThread(Tile * tile);

	void endBallThread();
	void endGameDataBroadcasterThread();
	void endLocalClientHandler();
	void endRemoteConnectionHandler();

	void waitForGameDataBroadcaster();
	void waitForLocalClientThread();
	void waitForRemoteConnectionThread();
	void waitForBallThread();
	void waitForBonusesThreads();
};