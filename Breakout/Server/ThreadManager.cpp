#include "ThreadManager.h"

void setupBall() { //TODO: isto deve ser feito pelo Game Manager que vai estar publico
	//TODO: inicializa a bola entre os tiles e os jogadores
	//TODO: faz random de que posi��o come�a se � para esquerda ou direita
}

DWORD WINAPI BallManager(LPVOID args) {
	setupBall();

	//TODO: sperar pelo sem�foro / mutex de inicio do jogo para poder avan�ar
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