#include <Windows.h>
#include "UnicodeConfigs.h"

void setupBall() {
	//TODO: inicializa a bola entre os tiles e os jogadores
	//TODO: faz random de que posição começa se é para esquerda ou direita
}

void BallManager(LPVOID * args) {
	setupBall();

	//TODO: sperar pelo semáforo / mutex de inicio do jogo para poder avançar

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