#include "ThreadManager.h"
#include "Server.h"

enum TimeConversions {
	MILLISECOND = 1000 * 10,
};

void APIENTRY MoveBall() {

}

DWORD WINAPI BallManager(LPVOID args) {
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;
	bool * CONTINUE = (bool *)args;

	liDueTime.QuadPart = 100LL;//(long long) -100 * (MILLISECOND);
	*CONTINUE = true;

	// Create an unnamed waitable timer.
	hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	if (NULL == hTimer)
	{
		tcout << "CreateWaitableTimer failed: " << GetLastError() << endl;
		return -1;
	}

	Server::gameData.setupGameStart();
	//TODO: sperar pelo sem�foro / mutex de inicio do jogo para poder avan�ar

	if (!SetWaitableTimer(hTimer, &liDueTime, 100, NULL, NULL, 0)) {
		tcout << "SetWaitableTimer failed: " << GetLastError() << endl;
		return -1;
	}

	while (*CONTINUE)
	{
		WaitForSingleObject(hTimer, INFINITE);

		Server::gameData.moveActiveBalls();
		Server::sharedMemory.setUpdate();
	}
	tcout << "Ball Thread Ended" << endl;
	CloseHandle(hTimer);

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
		if (reply.type == ACCEPT) {
			Server::threadManager.startBallThread(); //TODO: Remove this later
		}
		_tcscpy_s(reply.message.receiver, message.message.name);

		Server::sharedMemory.writeMessage(reply);
		//TODO: do something about this later!
		tcout << "Client: " << message.message.name << " -> " << reply.type << endl;
		break;

	case LEAVE:
		Server::clients.removeClient(message.id);
		break;
	}
}

DWORD WINAPI SharedMemClientHandler(LPVOID args) {
	bool * CONTINUE = (bool *)args;
	ClientMsg message;

	*CONTINUE = true;

	while (*CONTINUE) {
		message = Server::sharedMemory.readMessage();
		handleMessage(message);
	}

	tcout << "Client Handler thread ended" << endl;

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

	tcout << "Game Broadcast Thread Ended" << endl;

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

bool ThreadManager::isBroadcastRunning() const {
	return broadcastRunning;
}

bool ThreadManager::isBallThreadRunning() const {
	return ballThreadRunning;
}

bool ThreadManager::isLocalClientHandlerRunning() const {
	return localClientHandlerRunning;
}

void ThreadManager::endBallThread() {
	ballThreadRunning = false;
}

void ThreadManager::endLocalClientHandler() {
	localClientHandlerRunning = false;
}

void ThreadManager::endGameDataBroadcasterThread(){
	 broadcastRunning = false;
}

void ThreadManager::waitForGameDataBroadcaster() {
	WaitForSingleObject(hBroadcastThread, INFINITE);
	CloseHandle(hBroadcastThread);
}

void ThreadManager::waitForLocalClientThread() {
	WaitForSingleObject(hLocalClientHandler, INFINITE);
	CloseHandle(hLocalClientHandler);
}

void ThreadManager::waitForBallThread() {
	WaitForSingleObject(hBallThread, INFINITE);
	CloseHandle(hBallThread);
}