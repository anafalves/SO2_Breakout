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
	bool isBroadcastRunning;

	//TODO: add other handlers here!
	HANDLE hRemoteMsgHandler;
	bool remoteMsgHandlerRunning;

public:
	ThreadManager() {
		hBallThread = nullptr;
		ballThreadRunning = false;

		hLocalClientHandler = nullptr;
		localClientHandlerRunning = false;

		hRemoteMsgHandler = nullptr;
		remoteMsgHandlerRunning = false;
	};

	bool isBallThreadRunning() const {
		return ballThreadRunning;
	}

	bool startLocalClientHandler();
	bool startBallThread();

	void endBallThread() {//TODO: might need to do something else to end the thread ( close handles and what not )
		ballThreadRunning = false;
		CloseHandle(hBallThread);
	}

	void waitForLocalClientThread() {
		WaitForSingleObject(hLocalClientHandler, INFINITE);
	}

	void waitForBallThread() {
		WaitForSingleObject(hBallThread, INFINITE);
	}
};