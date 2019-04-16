#pragma once
#include <Windows.h>
#include "UnicodeConfigs.h"
#include "Server.h"

class ThreadManager {
private:
	HANDLE hBallThread;
	bool ballThreadRunning;

	//TODO: add other handlers here!


public:
	ThreadManager() {
		hBallThread = nullptr;
		ballThreadRunning = false;
	};

	bool isBallThreadRunning() const {
		return ballThreadRunning;
	}

	bool startBallThread();

	void endBallThread() {//TODO: might need to do something else to end the thread ( close handles and what not )
		ballThreadRunning = false;
		CloseHandle(hBallThread);
	}

};