#include "ThreadManager.h"

void setupBall() { //TODO: isto deve ser feito pelo Game Manager que vai estar publico
	//TODO: inicializa a bola entre os tiles e os jogadores
	//TODO: faz random de que posição começa se é para esquerda ou direita
}

DWORD WINAPI BallManager(LPVOID args) {
	setupBall();

	//TODO: sperar pelo semáforo / mutex de inicio do jogo para poder avançar
	/*
	while (TODO:  Criar variavel CONTINUE NO .h para poder parar a thread no fim do jogo )
	{

	}*/
	return 0;
}

bool ThreadManager::startBallThread() {
	if (ballThreadRunning) {
		return false;
	}

	hBallThread = CreateThread(nullptr, 0, BallManager, nullptr, 0, nullptr);
	if (hBallThread == nullptr) {
		throw TEXT("Ball control thread couldn't be started!");
	}

	ballThreadRunning = true;
	return true;
}