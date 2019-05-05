#pragma once
#include <Windows.h>
#include "UnicodeConfigs.h"

class Server;

class ThreadManager {
private:
	HANDLE hBallThread;
	bool ballThreadRunning;

	HANDLE hLocalClientHandler;
	bool localClientHandlerRunning;

	HANDLE hBroadcastThread;
	bool broadcastRunning;

	//TODO: add other handlers here!
	HANDLE hRemoteMsgHandler;
	bool remoteMsgHandlerRunning;

public:
	ThreadManager() {
		hBallThread = nullptr;
		ballThreadRunning = false;

		hLocalClientHandler = nullptr;
		localClientHandlerRunning = false;

		hBroadcastThread = nullptr;
		broadcastRunning = false;

		hRemoteMsgHandler = nullptr;
		remoteMsgHandlerRunning = false;
	};

	bool isBallThreadRunning() const;
	bool isBroadcastRunning() const;
	bool isLocalClientHandlerRunning() const;

	bool startLocalClientHandler();
	bool startBallThread();

	void endBallThread();
	void endGameDataBroadcasterThread();
	void endLocalClientHandler();

	void waitForGameDataBroadcaster();
	void waitForLocalClientThread();
	void waitForBallThread();
};