#include "ThreadManager.h"
#include "Server.h"

DWORD WINAPI BallManager(LPVOID args) {

	bool CONTINUE = (bool)args;
	CONTINUE = true;
	int time = Server::config.getMovementSpeed() * 10;

	Server::gameData.setupGameStart();
	//TODO: sperar pelo semáforo / mutex de inicio do jogo para poder avançar

	while (CONTINUE)
	{
		Sleep(time);
		Server::gameData.moveActiveBalls();

	}

	return 0;
}

bool ThreadManager::startBallThread() {
	if (ballThreadRunning) {
		return false;
	}

	hBallThread = CreateThread(nullptr, 0, BallManager, (LPVOID) &ballThreadRunning, 0, nullptr);
	if (hBallThread == nullptr) {
		return false;
	}

	return true;
}