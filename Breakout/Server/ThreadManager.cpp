#include "ThreadManager.h"
#include "Server.h"

DWORD WINAPI BallManager(LPVOID args) {

	bool *CONTINUE = (bool *) args;
	(*CONTINUE)= true;
	int time = Server::config.getMovementSpeed() * 10;

	Server::gameData.setupGameStart();
	//TODO: sperar pelo semáforo / mutex de inicio do jogo para poder avançar

	while (*CONTINUE)
	{
		Sleep(time);
		Server::gameData.moveActiveBalls();
		Server::sharedMemory.setUpdate();
	}

	return 0;
}

void handleMessage(ClientMsg & message) {
	ServerMsg reply;

	switch (message.type) {
	case MOVE:
		break;

	case PRECISE_MOVE:
		break;

	case LOGIN:
		reply.type = Server::clients.AddClient(message.message.name, reply.id);
		_tcscpy_s(reply.message.receiver, message.message.name);
		Server::sharedMemory.writeMessage(reply);
		Server::threadManager.startBallThread(); //TODO: Remove this later

	case LEAVE:
		Server::clients.removeClient(message.id);
	}
}

DWORD WINAPI SharedMemClientHandler(LPVOID args) {
	bool * CONTINUE = (bool *)args;
	ClientMsg message;

	(*CONTINUE) = true;

	while (*CONTINUE) {
		message = Server::sharedMemory.readMessage();
		handleMessage(message);
	}

	return 0;
}

//This thread handles the updates for the clients
DWORD WINAPI GameDataBroadcast(LPVOID args) {
	bool * CONTINUE = (bool *)args;
	HANDLE update[2];

	(*CONTINUE) = true;
	update[0] = Server::sharedMemory.hUpdateEvent;
	update[1] = Server::sharedMemory.hExitEvent;

	while (*CONTINUE) {
		WaitForMultipleObjects(2,update,FALSE,INFINITE);
		//TODO: Broadcast gameData to all PIPE clients

	}

	return 0;
}

bool ThreadManager::startLocalClientHandler(){
	if (localClientHandlerRunning) {
		return false;
	}

	hLocalClientHandler = CreateThread(nullptr, 0, SharedMemClientHandler,
			(LPVOID)&localClientHandlerRunning, 0, nullptr);

	if (hLocalClientHandler == nullptr) {
		return false;
	}

	return true;
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