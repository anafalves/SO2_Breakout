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


DWORD WINAPI SharedMemClientHandler(LPVOID args) {
	bool * CONTINUE = (bool *)args;
	bool ACTIVE = true;
	Player * player = nullptr;
	ClientMsg request;
	ServerMsg reply;

	*CONTINUE = true;
	

	while (*CONTINUE && ACTIVE) {
		request = Server::sharedMemory.readMessage();

		switch (request.type) {
			case MOVE:
				//TODO: handle movement
				break;

			case PRECISE_MOVE:
				//TODO: handle precise movement
				break;

			case TOP10:
				//Send top 10 back
				reply.type = TOP10;
				reply.message.top10 = Server::topPlayers.getTop10();
				Server::sharedMemory.writeMessage(reply);
				break;

			case LOGIN:
				if (!Server::clients.hasRoom()) {
					reply.type = DENY_SERVER_FULL;
				}
				else if(!Server::clients.isNameAvailable(request.message.name)) {
					reply.type = DENY_USERNAME;
				}
				else {
					player = Server::gameData.getAvailablePlayer();
					if (player == nullptr) {
						tcout << "Something went wront while trying to get a pointer to an available player while there's still room." << endl;
						ACTIVE = false;
						reply.type = DENY_SERVER_FULL; //todo: CHANGE THIS
						Server::sharedMemory.writeMessage(reply);
						break;
					}
					Server::clients.AddClient(request.message.name, player, reply.id);
					
					Server::threadManager.startBallThread(); //TODO: Remove this later
					reply.type = ACCEPT;

					//TODO: do something about this later!
					tcout << endl << "Client: " << request.message.name << " -> " << reply.type << endl;
				}

				Server::sharedMemory.writeMessage(reply);
				break;

			case LEAVE:
				ACTIVE = false;
				break;
		}//End of switch
	}//End of while

	Server::clients.removeClient(request.id);
	tcout << "Local client Handler thread ended" << endl;

	return 0;
}

bool sendMessage(const HANDLE hPipe, const ServerMsg &reply) {
	bool success = false;
	DWORD nBytes = 0;

	success = WriteFile(hPipe, &reply, sizeof(ServerMsg), &nBytes, NULL); //TODO: add exit event
	if (!success || nBytes != sizeof(ServerMsg)) {
		return false;
	}
	
	return true;
}

void closePipeConnection(HANDLE pipe) {
	FlushFileBuffers(pipe);
	DisconnectNamedPipe(pipe);
	CloseHandle(pipe);
}

DWORD WINAPI RemoteClientHandler(LPVOID args) {
	// 1 - Receives named pipe Handle
	PipeInfo * info = (PipeInfo *) args;
	HANDLE hPipe = info->pipe;

	bool success = false;
	bool * CONTINUE = info->CONTINUE;
	bool ACTIVE = true;

	Player * player = nullptr;
	ClientMsg request;
	ServerMsg reply;
	DWORD nBytes = 0;

	free(info);

	while (*CONTINUE && ACTIVE) 
	{
		// 2 - Starts handling messages from named pipe
		success = ReadFile(hPipe,
			&request,
			sizeof(ClientMsg),
			&nBytes,
			NULL);

		if (!success || nBytes == 0 || GetLastError() == ERROR_BROKEN_PIPE)
			break; //Pipe is no longer active

		switch (request.type) {
			case MOVE:
				break;

			case PRECISE_MOVE:
				break;

			case TOP10:
				reply.type = TOP10;
				reply.message.top10 = Server::topPlayers.getTop10();
				if (!sendMessage(hPipe, reply) && GetLastError() == ERROR_BROKEN_PIPE)
					ACTIVE = false;

				break;

			case LOGIN:
			{
				HANDLE hGameDataPipe;
				bool clientConnected = false;
				tstring pipeName = TEXT("\\\\.\\");

				if (!Server::clients.hasRoom()) {
					reply.type = DENY_SERVER_FULL;
					if (!sendMessage(hPipe, reply) && GetLastError() == ERROR_BROKEN_PIPE) {
						ACTIVE = false;
						break;
					}
				}
				else if (!Server::clients.isNameAvailable(request.message.name)) {
					reply.type = DENY_USERNAME;
					if (!sendMessage(hPipe, reply) && GetLastError() == ERROR_BROKEN_PIPE) {
						ACTIVE = false;
						break;
					}
				}
				else {
					player = Server::gameData.getAvailablePlayer();
					if (player == nullptr) {
						tcout << "Something went wront while trying to get a pointer to an available player while there's still room." << endl;
						ACTIVE = false;
						break;
					}

					// 3 - When received login and login successful, creates a named pipe called GameDataPipe and open it for client connection.
					//Creates GameData namedpipe
					pipeName += PipeConstants::GAMEDATA_PIPE_NAME.c_str();
					pipeName += request.message.name;

					tcout << "opening gamedata pipe to client :" << pipeName << endl;

					hGameDataPipe = CreateNamedPipe(
						pipeName.c_str(),
						PIPE_ACCESS_DUPLEX,
						PIPE_TYPE_MESSAGE |
						PIPE_READMODE_MESSAGE |
						PIPE_WAIT,
						1,
						sizeof(GameData),
						sizeof(GameData),
						NMPWAIT_USE_DEFAULT_WAIT, //TODO: change this wait
						NULL
					);

					if (hGameDataPipe == INVALID_HANDLE_VALUE) {
						tcout << "couldn't create GameData pipe!\n" << endl;

						ACTIVE = false;
						break;
					}

					//Send message to client, so they can connect to the GameData namedpipe
					reply.type = ACCEPT;
					if (!sendMessage(hPipe, reply) && GetLastError() == ERROR_BROKEN_PIPE){
						tcout << "couldn't send message to client!" << endl;
						ACTIVE = false;
						break;
					}

					clientConnected = ConnectNamedPipe(hGameDataPipe, NULL);
					if (clientConnected || (GetLastError() == ERROR_PIPE_CONNECTED))
						clientConnected = true;

					if (clientConnected) {
						// 4 - After connectiing the thread creates and adds the user to the active users pool.
						Server::clients.AddClient(request.message.name, player, hPipe, hGameDataPipe, reply.id);
					}
					else {
						tcout << "client couldn't connect to GameDataPipe!" << endl;
						closePipeConnection(hGameDataPipe);
						break;
					}
					tcout << "user ready!" << endl;
					Server::threadManager.startBallThread();

					break;
				}
			} //End of Login case

			case LEAVE:
				// 6 The thread breaks the cycle and stop.
				ACTIVE = false;
				break;
		}
	}
	// 5 - Once the user leaves, crashes or pipe closes/error, the thread removes the user from the pool.
	Server::clients.removeClient(request.id);

	return 0;
}

DWORD WINAPI RemoteConnectionHandler(LPVOID args) {
	bool * CONTINUE = (bool *) args;
	bool clientConnected = false;
	vector<HANDLE> clientThreads;
	HANDLE thread = NULL;
	PipeInfo * clientInfo;

	tstring pipeName(TEXT("\\\\.\\") + PipeConstants::MESSAGE_PIPE_NAME);

	*CONTINUE = true;

	while (*CONTINUE) {
		clientInfo = (PipeInfo *)malloc(sizeof(PipeInfo));
		if (clientInfo == NULL)
		{
			tcout << "Memory couldn't be allocated" << endl;
			break;
		}

		clientInfo->CONTINUE = CONTINUE;

		clientInfo->pipe = CreateNamedPipe(
			pipeName.c_str(),
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

		if (clientInfo->pipe == INVALID_HANDLE_VALUE) {
			tcout << "couldn't create pipe instance!\n" << endl;
			return -1;
		}

		clientConnected = ConnectNamedPipe(clientInfo->pipe, NULL);
		if (clientConnected || (GetLastError() == ERROR_PIPE_CONNECTED))
			clientConnected = true;

		if (clientConnected) {
			thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RemoteClientHandler, (LPVOID)clientInfo, 0, NULL);
			if (thread == NULL) {
				tcout << "couldn't create thread to handle remote client!" << endl;
				CloseHandle(clientInfo->pipe);
			}
			clientThreads.push_back(thread);
		}
		else {
			CloseHandle(clientInfo->pipe);
		}
	}

	CONTINUE = false;
	WaitForMultipleObjects((DWORD) clientThreads.size(), clientThreads.data(), TRUE, INFINITE);

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
		Server::clients.broadcastGameData();
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

bool ThreadManager::startRemoteConnectionHandler() {
	if (remoteConnectionHandlerRunning) {
		return false;
	}

	hRemoteConnectionHandler = CreateThread(nullptr, 0, RemoteConnectionHandler,
		(LPVOID)&remoteConnectionHandlerRunning, 0, nullptr);

	if (hRemoteConnectionHandler == nullptr) {
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

bool ThreadManager::startGameDataBroadcaster() {
	if (broadcastRunning) {
		return false;
	}

	hBroadcastThread = CreateThread(nullptr, 0, GameDataBroadcast, (LPVOID)&broadcastRunning, 0, nullptr);
	if (hBroadcastThread == nullptr) {
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

bool ThreadManager::isRemoteConnectionHandlerRunning() const {
	return remoteConnectionHandlerRunning;
}


void ThreadManager::endBallThread() {
	ballThreadRunning = false;
}

void ThreadManager::endLocalClientHandler() {
	localClientHandlerRunning = false;
}

void ThreadManager::endRemoteConnectionHandler() {
	remoteConnectionHandlerRunning = false;
}

void ThreadManager::endGameDataBroadcasterThread(){
	 broadcastRunning = false;
}

void ThreadManager::waitForRemoteConnectionThread() {
	WaitForSingleObject(hRemoteConnectionHandler, INFINITE);
	CloseHandle(hBroadcastThread);
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