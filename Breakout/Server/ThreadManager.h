#pragma once
#include <Windows.h>
#include <tchar.h>
#include "UnicodeConfigs.h"

enum ThreadConstants {
	SPEEDFACTOR = 10,
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

	bool startLocalClientHandler();
	bool startRemoteConnectionHandler();
	bool startBallThread();

	void endBallThread();
	void endGameDataBroadcasterThread();
	void endLocalClientHandler();
	void endRemoteConnectionHandler();

	void waitForGameDataBroadcaster();
	void waitForLocalClientThread();
	void waitForRemoteConnectionThread();
	void waitForBallThread();
};