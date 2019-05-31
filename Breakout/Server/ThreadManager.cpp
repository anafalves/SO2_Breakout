#include "ThreadManager.h"
#include "Server.h"
#include "GameDataManager.h"

DWORD WINAPI BallManager(LPVOID args) {
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;
	GameData * gameData;

	int ballLeft, ballRight, ballTop, ballBottom;
	int tileLeft, tileRight, tileTop, tileBottom;
	int playerLeft, playerRight, playerTop, playerBottom;

	bool ballAvailable;
	bool * CONTINUE = (bool *)args;

	*CONTINUE = true;
	gameData = Server::gameData.getGameData();
	liDueTime.QuadPart = 100LL;

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
		Server::gameData.lockAccessGameData();
		Server::sharedMemory.waitForUpdateFlags();

		for (auto &ball : gameData->balls) {
			if (!ball.active) {
				continue;
			}

			if (ball.up) {
				ball.posY++;
			}
			else {
				ball.posY--;
			}

			if (ball.right) {
				ball.posX++;
			}
			else {
				ball.posX--;
			}

			//TODO: verify if the Y it's in the tile zone before doing this
			ballLeft = ball.posX;
			ballRight = ball.posX + ball.width;
			ballTop = ball.posY;
			ballBottom = ball.posY + ball.height;

			for (auto &tile : gameData->tiles) {
				if (!tile.active) {
					continue;
				}

				tileLeft = tile.posX;
				tileRight = tile.posX + tile.width;
				tileTop = tile.posY;
				tileBottom = tile.posY + tile.height;

				//if ball hits right or left of a tile
				if ((ballLeft < tileRight && tileRight < ballRight) ||
					(ballLeft < tileLeft && tileLeft < ballRight) &&
					(tileTop <= ballTop || tileBottom >= ballBottom))
				{
					ball.right = !ball.right;

					if (--tile.resistance == 0)
						tile.active = false;

					//TODO: placeholder
					gameData->players[ball.playerId].points += 10;

					if (tile.bonus)
					{
						//TODO: create thread to generate and handle that thing
					}
				}

				//if ball hits top or bottom of a tile
				if ((ballTop < tileTop && tileTop < ballBottom) ||
					(ballTop < tileBottom && tileBottom < ballBottom) &&
					(tileLeft <= ballLeft || tileRight >= ballRight))
				{
					ball.up = !ball.up;

					if (--tile.resistance == 0)
						tile.active = false;

					//TODO: placeholder
					gameData->players[ball.playerId].points += 10;

					if (tile.bonus)
					{
						//TODO: create thread to generate and handle that thing
					}
				}
			}

			for (auto &player : gameData->players) {
				if (!player.active) {
					continue;
				}

				playerLeft = player.posX;
				playerRight = player.posX + player.width;
				playerTop = player.posY;
				playerBottom = player.posY + player.height;

				//ball hit player on top
				if ((ballTop < playerTop && playerTop < ballBottom) &&
					(playerLeft <= ballLeft || playerLeft <= ballRight))
				{
					ball.up = !ball.up;
					ball.playerId = player.id;
				}

				if ((ballLeft < playerRight && playerRight < ballRight) ||
					(ballLeft < playerLeft && playerLeft < ballRight) &&
					(playerTop <= ballTop || playerBottom >= ballBottom))
				{
					ball.up = !ball.up;
					ball.right = !ball.right;
					ball.playerId = player.id;
				}
			}

			//Verify if ball is in one of the of the limits, so it can change position
			if (ball.posX == MAX_WIDTH || ball.posX == MIN_WIDTH) {
				ball.right = !ball.right;
			}

			if (ball.posY == MAX_HEIGHT) {
				ball.up = !ball.up;
			}

			if (ball.posY == MIN_HEIGHT) {
				ball.active = false;
				gameData->players[ball.playerId].lives--;
			}
		}

		//Verify if there are still balls in the game
		ballAvailable = false;

		for (auto & ball : gameData->balls) {
			if (ball.active)
				ballAvailable = true;
		}

		if (!ballAvailable) {
			for (auto & player : gameData->players) {
				if (player.active) {
					if (player.lives > 0) {
						Server::gameData.setupBall();
						ballAvailable = true;
						break;
					}
				}
			}
		}

		Server::sharedMemory.setUpdate();
		Server::gameData.releaseAccessGameData();

		if (!ballAvailable) {
			gameData->gameState = GAME_OVER;
			break;
		}
	}

	tcout << "Ball Thread Ended" << endl;
	CloseHandle(hTimer);

	return 0;
}


DWORD WINAPI BonusHandler(LPVOID args) {
	GameData * gameData = Server::gameData.getGameData();
	Bonus * bonus = nullptr;
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;
	Tile * tile = (Tile*) args;
	int nextPos;

	liDueTime.QuadPart = 100LL;

	// Create an unnamed waitable timer.
	hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	if (NULL == hTimer)
	{
		tcout << "CreateWaitableTimer failed: " << GetLastError() << endl;
		return -1;
	}

	if (!SetWaitableTimer(hTimer, &liDueTime, 100, NULL, NULL, 0)) {
		tcout << "SetWaitableTimer failed: " << GetLastError() << endl;
		return -1;
	}

	Server::gameData.lockAccessGameData();
	Server::sharedMemory.waitForUpdateFlags();

	for (auto & b : gameData->bonuses) {
		if (!b.active) {
			b.active = true;
			bonus->type = tile->bonus;
			bonus = &b;
			break;
		}
	}
	Server::sharedMemory.setUpdate();
	Server::gameData.releaseAccessGameData();

	if (bonus == nullptr)
		return 1;

	while (bonus->active) {
		WaitForSingleObject(hTimer, INFINITE);
		nextPos = bonus->posY + Server::config.getMovementSpeed();

		Server::gameData.lockAccessGameData();
		Server::sharedMemory.waitForUpdateFlags();

		for (auto & player : gameData->players) {
			if (!player.active)
				continue;

			if (bonus->posY + bonus->height >= player.posY &&
				bonus->posY <= player.posY + player.height &&
				bonus->posX >= player.posX + player.width  &&
				bonus->posX + bonus->width <= player.posX)
			{
				switch (bonus->type) {
					case LIFE:
						player.lives++;
						break;

					case TRIPLE:
						gameData->balls[1] = gameData->balls[2] = gameData->balls[0];
						gameData->balls[1].right = !gameData->balls[1].right;
						gameData->balls[2].up = !gameData->balls[2].up;
						break;

					case SPEED_UP:
						Server::config.setMovementSpeed(Server::config.getMovementSpeed() + 5);
						break;

					case SLOW_DOWN:
						Server::config.setMovementSpeed(Server::config.getMovementSpeed() - 2);
						break;
				}

				bonus->active = false;
				Server::gameData.releaseAccessGameData();
				Server::sharedMemory.setUpdate();
				return 0;
			}
		}
		Server::sharedMemory.setUpdate();
		Server::gameData.releaseAccessGameData();
	}

	return 0;
}

DWORD WINAPI SharedMemClientHandler(LPVOID args) {
	bool * CONTINUE = (bool *)args;
	bool ACTIVE = true;
	Player * player = nullptr;
	ClientMsg request;
	ServerMsg reply;
	HANDLE flag;

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
						reply.type = DENY_SERVER_FULL;
						Server::sharedMemory.writeMessage(reply);
						break;
					}

					reply.update_id = Server::sharedMemory.addClientUpdateFlag(flag); //Creats a handle to sync server write and local clirne read operations
					if (reply.update_id < 0) {
						reply.type = DENY_SERVER_FULL;
						Server::sharedMemory.writeMessage(reply);
						break;
					}

					Server::clients.AddClient(request.message.name, player, flag, reply.id);
					
					reply.type = ACCEPT;
					_tcscpy_s(reply.message.receiver, request.message.name);

					tcout << endl << "Client: " << request.message.name << " -> " << reply.type << endl;
				}

				Server::sharedMemory.writeMessage(reply);
				Server::threadManager.startBallThread();
				break;

			case LEAVE:
				Server::sharedMemory.writeMessage(reply);
				Server::clients.removeClient(request.id);
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

					Server::clients.AddClient(request.message.name, player, hPipe, hGameDataPipe, reply.id);
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
	Server::clients.removeClient(reply.id);

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