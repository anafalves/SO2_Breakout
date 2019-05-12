#include "ThreadManager.h"
#include "Server.h"

enum TimeConversions {
	MILLISECOND = 1000 * 10,
};

DWORD WINAPI BallManager(LPVOID args) {
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;
	bool * CONTINUE = (bool *)args;
	int time;

	liDueTime.QuadPart = 100LL;
	*CONTINUE = true;
	time = Server::config.getMovementSpeed() * SPEEDFACTOR;

	// Create an unnamed waitable timer.
	hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	if (NULL == hTimer)
	{
		tcout << "CreateWaitableTimer failed: " << GetLastError() << endl;
		return -1;
	}

	Server::gameData.setupGameStart();
	//TODO: sperar pelo sem�foro / mutex de inicio do jogo para poder avan�ar

	if (!SetWaitableTimer(hTimer, &liDueTime, time, NULL, NULL, 0)) {
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

void handleLocalMessage(ClientMsg & message) {
	ServerMsg reply;

	switch (message.type) {
	case MOVE:
		break;

	case PRECISE_MOVE:
		break;

	case TOP10:
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

void handleRemoteMessage(ClientMsg & message) {
	ServerMsg reply;

	switch (message.type) {
		case MOVE:
			break;

		case PRECISE_MOVE:
			break;

		case TOP10:
			break;

		case LOGIN:
			reply.type = Server::clients.AddClient(message.message.name, reply.id);
			if (reply.type == ACCEPT) {
				//Reply with accept
				//Create GameDataPipe
				//Wait for client to connect, if client is not connected within 10 seconds, they get disconnected.
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
		handleLocalMessage(message);
	}

	tcout << "Client Handler thread ended" << endl;

	return 0;
}

DWORD WINAPI RemoteClientHandler(LPVOID args) {
	// 1 - Receives named pipe Handle
	// 2 - Starts handling messages from named pipe
	// 3 - When received login and login successful, creates a named pipe called GameDataPipe and open it for client connection.
	// 4 - After connectiing the thread creates and adds the user to the active users pool.
	// 5 - Once the user leaves, crashes or pipe closes/error, the thread removes the user from the pool.
	// 6 - The thread breaks the cycle and stop.
	return 0;
}

DWORD WINAPI RemoteConnectionHandler(LPVOID args) {
	bool * CONTINUE = (bool *) args;
	bool clientConnected = false;
	vector<HANDLE> clientThreads;
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	TCHAR pipeName[] = TEXT("\\\\.\\pipe\\messages");

	*CONTINUE = true;

	while (*CONTINUE) {
		hPipe = CreateNamedPipe(
			pipeName,
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE |
			PIPE_READMODE_MESSAGE |
			PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			BUFFERSIZE,
			BUFFERSIZE,
			NMPWAIT_USE_DEFAULT_WAIT,
			NULL
		);

		if (hPipe == INVALID_HANDLE_VALUE) {
			tcout << "couldn't create pipe instance!\n" << endl;
			return -1;
		}

		clientConnected = ConnectNamedPipe(hPipe, NULL);
		if (!clientConnected && (GetLastError() == ERROR_PIPE_CONNECTED))
			clientConnected = true;

		if (clientConnected) {
			//TODO: call method from SERVER::threadManager.createNewRemoteClientHandler(HANDLE hPipe);
			//If this returns false, then the server closes the connection before exiting.
		}
		else {
			CloseHandle(hPipe);
		}
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