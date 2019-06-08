#include "ThreadManager.h"
#include "Server.h"

enum GamePoints {
	POINTS_FOR_TILE_HIT = 10,
};

DWORD WINAPI BonusHandler(LPVOID args) {

	GameData * gameData = Server::gameData->getGameData();
	Bonus * bonus = nullptr;
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;
	Tile * tile = (Tile*)args;

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

	Server::gameData->lockAccessGameData();
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
	Server::gameData->releaseAccessGameData();

	if (bonus == nullptr)
		return 1;

	while (bonus->active) {
		WaitForSingleObject(hTimer, INFINITE);

		Server::gameData->lockAccessGameData();
		Server::sharedMemory.waitForUpdateFlags();

		bonus->posY += Server::config.getMovementSpeed();

		for (auto & player : gameData->players) {
			if (!player.active)
				continue;

			//If there is a collision with a player, applies effect
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
				Server::gameData->releaseAccessGameData();
				Server::sharedMemory.setUpdate();
				return 0;
			}
		}
		Server::sharedMemory.setUpdate();
		Server::gameData->releaseAccessGameData();
	}

	return 0;
}

DWORD WINAPI BallManager(LPVOID args) {
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;
	GameData * gameData;

	bool hit;
	bool ballAvailable;
	bool playersAlive;
	bool tilesAvailable;

	int ballLeft, ballRight, ballTop, ballBottom;
	int tileLeft, tileRight, tileTop, tileBottom;
	int playerLeft, playerRight, playerTop, playerBottom;

	bool * CONTINUE = (bool *)args;

	*CONTINUE = true;
	gameData = Server::gameData->getGameData();
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

	while (*CONTINUE)
	{
		WaitForSingleObject(hTimer, INFINITE);
		Server::gameData->lockAccessGameData();
		Server::sharedMemory.waitForUpdateFlags();

		ballAvailable = false;
		playersAlive = false;
		tilesAvailable = false;
		hit = false;

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

				if (tile.resistance != UNBREAKABLE)
					tilesAvailable = true;

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
					hit = true;

					if (tile.resistance == UNBREAKABLE)
						break;

					if (--tile.resistance == 0)
						tile.active = false;

					gameData->players[ball.playerId].points += POINTS_FOR_TILE_HIT;

					if (tile.bonus)
					{
						Server::threadManager.startBonusThread(&tile);
					}
				}

				//if ball hits top or bottom of a tile
				if ((ballTop < tileTop && tileTop < ballBottom) ||
					(ballTop < tileBottom && tileBottom < ballBottom) &&
					(tileLeft <= ballLeft || tileRight >= ballRight))
				{
					ball.up = !ball.up;
					hit = true;

					if (tile.resistance == UNBREAKABLE)
						break;

					if (--tile.resistance == 0)
						tile.active = false;

					gameData->players[ball.playerId].points += POINTS_FOR_TILE_HIT;

					if (tile.bonus)
					{
						Server::threadManager.startBonusThread(&tile);
					}
				}
			}

			if (!tilesAvailable) {
				gameData->gameState = NEXT_LEVEL;
				Server::gameData->setGameEvent();
				break;
			}

			if (hit)
				continue;

			for (auto &player : gameData->players) {
				if (!player.active || player.lives == 0) {
					continue;
				}

				playersAlive = true;

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

			if (!playersAlive) {
				gameData->gameState = GAME_OVER;
				Server::gameData->setGameEvent();
				break;
			}

			//Verify if ball is in one of the of the limits, so it can change position
			if (ball.posX == MAX_GAME_WIDTH || ball.posX == MIN_GAME_WIDTH) {
				ball.right = !ball.right;
			}

			if (ball.posY == MAX_GAME_HEIGHT) {
				ball.up = !ball.up;
			}

			if (ball.posY == MIN_GAME_HEIGHT) {
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
		//If theres no balls available, check for palyers with lives so another ball can spawn
		if (!ballAvailable) {
			for (auto & player : gameData->players) {
				if (player.active) {
					if (player.lives > 0) {
						Server::gameData->setupBall();
						ballAvailable = true;
						break;
					}
				}
			}
		}

		Server::sharedMemory.setUpdate();
		Server::gameData->releaseAccessGameData();

		if (!ballAvailable) {
			gameData->gameState = GAME_OVER;
		}

		if (gameData->gameState != RUNNING)
			break;
	}

	tcout << "Ball Thread Ended" << endl;
	CloseHandle(hTimer);

	return 0;
}

DWORD WINAPI GameThread(LPVOID args) {
	int difficulty = 0;
	GameData * gameData = Server::gameData->getGameData();

	while (true) {
		// 1 - Position users
		//If there are no clients, stop the thread.
		if (Server::clients.getClientArray().size() == 0)
			return -1;

		Server::gameData->setupPlayers(); //TODO: this does nothing atm, needs to be coded.
		// 2 - Generate map
		Server::gameData->generateLevel(difficulty); //TODO: needs to be coded, this is doing nothing atm.
		// 3 - Start ball thread
		Server::threadManager.startBallThread();
		// 4 - wait for gameEvent
		Server::gameData->waitForGameEvent();
		if (gameData->gameState == NEXT_LEVEL && difficulty < 10) {//TODO: maybe change this number, we can always pass a argument to compare here
			Server::threadManager.waitForBallThread();
			continue; //Create a new level and all that great stuff
		}
		else { //if it's anyting else, then just quits
			return 0; //take a look at this
		}
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
				player = Server::clients.getClientPlayer(request.id);
				if (player == nullptr)
					break;

				if(player->lives > 0)
					Server::gameData->movePlayer(player, request.message.basicMove);
				break;

			case PRECISE_MOVE:
				player = Server::clients.getClientPlayer(request.id);
				if (player == nullptr)
					break;

				if (player->lives > 0)
					Server::gameData->movePlayerPrecise(player, request.message.preciseMove);
				break;

			case TOP10:
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
					player = Server::gameData->getAvailablePlayer();
					if (player == nullptr) {
						tcout << "Something went wrong while trying to get a pointer to an available player while there's still room." << endl;
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
				break;

			case LEAVE:
				Server::clients.removeClient(request.id);
				break;
		}//End of switch
	}//End of while

	tcout << "Local client Handler thread ended" << endl;

	return 0;
}

bool sendMessage(const HANDLE hPipe, const ServerMsg &reply, HANDLE writeReady) {
	
	DWORD nBytes = 0;
	HANDLE write[2];
	OVERLAPPED flag = {0};

	write[0] = writeReady;
	write[1] = Server::sharedMemory.hExitEvent;

	flag.hEvent = writeReady;
	ResetEvent(writeReady);

	WriteFile(hPipe, &reply, sizeof(ServerMsg), &nBytes, &flag);
	WaitForMultipleObjects(2, write, FALSE, INFINITE);

	GetOverlappedResult(hPipe, &flag, &nBytes, FALSE);
	if (nBytes != sizeof(ServerMsg)) {
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

	bool * CONTINUE = info->CONTINUE;
	bool ACTIVE = true;

	free(info);

	Player * player = nullptr;
	ClientMsg request;
	ServerMsg reply;
	DWORD nBytes = 0;

	OVERLAPPED flag;
	HANDLE read[2];
	HANDLE readReady;
	HANDLE writeReady;
	
	reply.id = -1;

	//Overlapped IO for stopping app in case of exit
	readReady = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (readReady == INVALID_HANDLE_VALUE) {
		return -1;
	}

	writeReady = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (writeReady == INVALID_HANDLE_VALUE) {
		return -1;
	}

	read[0] = readReady;
	read[1] = Server::sharedMemory.hExitEvent;

	while (*CONTINUE && ACTIVE) 
	{
		SecureZeroMemory(&flag, sizeof(OVERLAPPED));
		ResetEvent(readReady);
		flag.hEvent = readReady;

		// 2 - Starts handling messages from named pipe
		ReadFile(hPipe,
			&request,
			sizeof(ClientMsg),
			&nBytes,
			&flag);

		WaitForMultipleObjects(2, read, FALSE, INFINITE);

		GetOverlappedResult(hPipe, &flag, &nBytes, FALSE);
		if (nBytes != sizeof(ClientMsg) || GetLastError() == ERROR_BROKEN_PIPE) {
			break;
		}

		switch (request.type) {
			case MOVE:
				if (player == nullptr)
					break;

				if (player->lives > 0)
					Server::gameData->movePlayer(player, request.message.basicMove);
				break;

			case PRECISE_MOVE:
				if (player == nullptr)
					break;

				if (player->lives > 0)
					Server::gameData->movePlayerPrecise(player, request.message.preciseMove);
				break;

			case TOP10:
				reply.type = TOP10;
				reply.message.top10 = Server::topPlayers.getTop10();
				if (!sendMessage(hPipe, reply, writeReady) && GetLastError() == ERROR_BROKEN_PIPE)
					ACTIVE = false;

				break;

			case LOGIN:
			{
				HANDLE hGameDataPipe;
				tstring pipeName = TEXT("\\\\.\\");

				if (!Server::clients.hasRoom()) {
					reply.type = DENY_SERVER_FULL;
					if (!sendMessage(hPipe, reply, writeReady) && GetLastError() == ERROR_BROKEN_PIPE) {
						ACTIVE = false;
						break;
					}
				}
				else if (!Server::clients.isNameAvailable(request.message.name)) {
					reply.type = DENY_USERNAME;
					if (!sendMessage(hPipe, reply, writeReady) && GetLastError() == ERROR_BROKEN_PIPE) {
						ACTIVE = false;
						break;
					}
					break;
				}
				else 
				{
					player = Server::gameData->getAvailablePlayer();
					if (player == nullptr) {
						tcout << "Something went wront while trying to get a pointer to an available player while there's still room." << endl;
						ACTIVE = false;
						break;
					}

					// 3 - When received login and login successful, creates a named pipe called GameDataPipe and open it for client connection.
					//Creates GameData namedpipe
					pipeName += PipeConstants::GAMEDATA_PIPE_NAME.c_str();
					pipeName += request.message.name;

					hGameDataPipe = CreateNamedPipe(
						pipeName.c_str(),
						PIPE_ACCESS_DUPLEX |
						FILE_FLAG_OVERLAPPED,
						PIPE_TYPE_MESSAGE |
						PIPE_READMODE_MESSAGE |
						PIPE_WAIT,
						1,
						sizeof(GameData),
						sizeof(GameData),
						NMPWAIT_USE_DEFAULT_WAIT,
						NULL
					);

					if (hGameDataPipe == INVALID_HANDLE_VALUE) {
						tcout << "couldn't create GameData pipe!\n" << endl;

						ACTIVE = false;
						break;
					}

					//Send message to client, so they can connect to the GameData namedpipe
					reply.type = ACCEPT;
					if (!sendMessage(hPipe, reply, writeReady) && GetLastError() == ERROR_BROKEN_PIPE){
						tcout << "couldn't send message to client!" << endl;
						ACTIVE = false;
						break;
					}
					tcout << endl << "Client: " << request.message.name << " -> " << reply.type << endl;

					Server::clients.AddClient(request.message.name, player, hPipe, hGameDataPipe, reply.id);
					player = Server::clients.getClientPlayer(reply.id);
					break;
				}
				break;
			} //End of Login case

			case LEAVE:
				// 6 The thread breaks the cycle and stop.
				ACTIVE = false;
				break;
		}
	}
	// 5 - Once the user leaves, crashes or pipe closes/error, the thread removes the user from the pool.
	Server::clients.removeClient(reply.id);
	CloseHandle(readReady);
	CloseHandle(writeReady);

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
			PIPE_ACCESS_DUPLEX |
			FILE_FLAG_OVERLAPPED,
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
			CloseHandle(thread);
		}
		else {
			CloseHandle(clientInfo->pipe);
		}
	}

	*CONTINUE = false;
	WaitForMultipleObjects(clientThreads.size(), clientThreads.data(), TRUE, INFINITE);

	tcout << "Remote client handler Thread ended" << endl;

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

	Server::gameData->setGameDataState(SHUTDOWN);
	Server::sharedMemory.setUpdate();
	Server::clients.broadcastGameData();

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

bool ThreadManager::startBonusThread(Tile * tile) {
	HANDLE hThread;

	hThread = CreateThread(nullptr, 0, BonusHandler, (LPVOID) tile, 0, nullptr);
	if (hThread == nullptr) {
		return false;
	}

	hBonuses.push_back(hThread);

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
	tstring pipeName(TEXT("\\\\.\\") + PipeConstants::MESSAGE_PIPE_NAME);
	HANDLE hPipeMessage = CreateFile(
		pipeName.c_str(),
		GENERIC_READ |
		GENERIC_WRITE,
		0 | FILE_SHARE_READ |
		FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	CloseHandle(hPipeMessage);
	WaitForSingleObject(hRemoteConnectionHandler, INFINITE);
	CloseHandle(hRemoteConnectionHandler);
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

void ThreadManager::waitForBonusesThreads() {
	if (hBonuses.size() > 0) {
		WaitForMultipleObjects(hBonuses.size(), hBonuses.data(), TRUE, INFINITE);
	}

	for (auto & thread : hBonuses) {
		CloseHandle(thread);
	}
};