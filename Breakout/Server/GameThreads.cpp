#include <Windows.h>
#include "UnicodeConfigs.h"

void setupBall() {
	//TODO: inicializa a bola entre os tiles e os jogadores
	//TODO: faz random de que posi��o come�a se � para esquerda ou direita
}

void BallManager(LPVOID * args) {
	setupBall();

	//TODO: sperar pelo sem�foro / mutex de inicio do jogo para poder avan�ar

//	while (/* TODO:  Criar variavel CONTINUE NO .h para poder parar a thread no fim do jogo */)
	//{

//	}
}

class ThreadManager {
private:
	HANDLE hBallThread;
	bool isBallThreadRunning;


public:
	ThreadManager() {
		hBallThread = nullptr;
		isBallThreadRunning = false;
	};

	bool startBallThread() {
		if (isBallThreadRunning) {
			return false;
		}

		//TODO: create ball thread.
	}

	void endBallThread() {

	}

};