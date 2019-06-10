#include "ThreadManager.h"
#include "Server.h"

enum GamePoints {
	POINTS_FOR_TILE_HIT = 10,
};

DWORD WINAPI MovingBlocks(LPVOID args) {
	bool *CONTINUE = (bool *)args;
	*CONTINUE = true;

	GameData * gameData = Server::gameData->getGameData();

	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;
	liDueTime.QuadPart = 100LL;

	int pos = 0;
	bool down = true;

	// Create an unnamed waitable timer.
	hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	if (NULL == hTimer)
	{
		tcout << "CreateWaitableTimer failed: " << GetLastError() << endl;
		return -1;
	}

	if (!SetWaitableTimer(hTimer, &liDueTime, 200, NULL, NULL, 0)) {
		tcout << "SetWaitableTimer failed: " << GetLastError() << endl;
		return -1;
	}

	while (*CONTINUE) {
		WaitForSingleObject(hTimer, INFINITE);
		Server::sharedMemory.waitForAllClientsReady();
		Server::gameData->lockAccessGameData();
		
		if (gameData->gameState == GAME_OVER)
		{
			Server::gameData->releaseAccessGameData();
			Server::clients.broadcastGameData();
			break;
		}

		for (auto & tile : gameData->tiles) {
			if(down)
				tile.posY += Server::config.getMovementSpeed() * 2;
			else
				tile.posY -= Server::config.getMovementSpeed() * 2;
		}

		Server::gameData->releaseAccessGameData();
		Server::clients.broadcastGameData();

		if (pos >= 15) {
			pos = 0;
			down = !down;
		}

		pos++;
	}
	CloseHandle(hTimer);

	return 0;
}

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

	Server::sharedMemory.waitForAllClientsReady();
	Server::gameData->lockAccessGameData();

	for (auto & b : gameData->bonuses) {
		if (!b.active) {
			b.active = true;
			bonus->type = tile->bonus;
			bonus = &b;
			break;
		}
	}

	Server::gameData->releaseAccessGameData();
	Server::clients.broadcastGameData();

	if (bonus == nullptr)
		return 1;

	while (bonus->active) {
		WaitForSingleObject(hTimer, INFINITE);
		Server::sharedMemory.waitForAllClientsReady();
		Server::gameData->lockAccessGameData();

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
				Server::clients.broadcastGameData();
				return 0;
			}
		}

		Server::gameData->releaseAccessGameData();
		Server::clients.broadcastGameData();
	}

	return 0;
}

DWORD WINAPI BallManager(LPVOID args) {
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;
	GameData * gameData;
	HANDLE hMovingBlocks;

	bool ballAvailable;
	bool playersAlive;
	bool tilesAvailable;

	int ballLeft, ballRight, ballTop, ballBottom;
	int tileLeft, tileRight, tileTop, tileBottom;

	int difLeft = 0, difRight = 0, difUp = 0, difDown = 0;
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

	if (!SetWaitableTimer(hTimer, &liDueTime, 40, NULL, NULL, 0)) {
		tcout << "SetWaitableTimer failed: " << GetLastError() << endl;
		return -1;
	}

	hMovingBlocks = CreateThread(NULL, 0, MovingBlocks, CONTINUE, 0, NULL);
	if (hMovingBlocks == NULL) {
		//RIP
		CloseHandle(hTimer);
		return -1;
	}
	CloseHandle(hMovingBlocks);
	
	while (*CONTINUE)
	{
		WaitForSingleObject(hTimer, INFINITE);
		Server::sharedMemory.waitForAllClientsReady();
		Server::gameData->lockAccessGameData();

		ballAvailable = false;
		playersAlive = false;
		tilesAvailable = false;

		for (auto &ball : gameData->balls) {
			if (!ball.active) {
				continue;
			}

			if (ball.up) {
				ball.posY += Server::config.getMovementSpeed();
			}
			else {
				ball.posY -= Server::config.getMovementSpeed();
			}

			if (ball.right) {
				ball.posX += Server::config.getMovementSpeed();
			}
			else {
				ball.posX -= Server::config.getMovementSpeed();
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

				tilesAvailable = true;

				if (tile.resistance != UNBREAKABLE)
					tilesAvailable = true;

				tileLeft = tile.posX;
				tileRight = tile.posX + tile.width;
				tileTop = tile.posY;
				tileBottom = tile.posY + tile.height;

				//if ther's a collision:
				if (!((ballRight < tileLeft || ballLeft > tileLeft) ||
					(ballBottom < tileTop || ballTop > tileBottom))) {

					difLeft = ballRight - tileLeft;
					difRight = tileRight - ballLeft;
					difUp = ballBottom - tileTop;
					difDown = tileBottom - ballTop;


					if ((difUp > difLeft && difUp > difRight) || (difDown > difLeft && difDown > difRight))
						ball.up = !ball.up;
					else if ((difUp < difLeft && difDown < difLeft) || (difUp < difRight && difDown < difRight))
						ball.right = !ball.right;
					else {
						ball.up = !ball.up;
						ball.right = !ball.right;
					}

					if (tile.resistance == UNBREAKABLE)
						continue;

					if (--tile.resistance == 0)
						tile.active = false;

					gameData->players[ball.playerId].points += POINTS_FOR_TILE_HIT;
				}


				//	if (tile.bonus)
				//	{
				//		//Server::threadManager.startBonusThread(&tile);
				//	}
				//}

			}

			if (!tilesAvailable) {
				gameData->gameState = NEXT_LEVEL;
				Server::gameData->setGameEvent();

				Server::gameData->releaseAccessGameData();
				Server::clients.broadcastGameData();
				CloseHandle(hTimer);
				return 0;
			}

			for (auto &player : gameData->players) {
				if (!player.active || player.lives == 0) {
					continue;
				}

				playersAlive = true;


				if (!(ballRight < player.posX || ballLeft > (player.posX + player.width) ||
					ballBottom < player.posY || (ballTop > player.posY + player.height))) {
					
					ball.playerId = player.id;
					ball.up = !ball.up;
				}
			}

			if (!playersAlive) {
				gameData->gameState = GAME_OVER;
				Server::gameData->setGameEvent();

				Server::gameData->releaseAccessGameData();
				Server::clients.broadcastGameData();
				CloseHandle(hTimer);
				return 0;
			}

			//Verify if ball is in one of the of the limits, so it can change position
			if ((ball.posX + ball.width) >= MAX_GAME_WIDTH || ball.posX <= MIN_GAME_WIDTH) {
				ball.right = !ball.right;
			}

			if ((ball.posY + ball.height) >= MAX_GAME_HEIGHT) {
				//ball.up = !ball.up;
				ball.active = false;

				if (ball.playerId >= 0)
					gameData->players[ball.playerId].lives--;
			}

			if (ball.posY <= MIN_GAME_HEIGHT) {
				ball.up = !ball.up;
			}

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

		Server::gameData->releaseAccessGameData();
		Server::clients.broadcastGameData();

		if (!ballAvailable) {
			gameData->gameState = GAME_OVER;
			Server::gameData->setGameEvent();
			break;
		}
	}

	tcout << "Ball Thread Ended" << endl;
	CloseHandle(hTimer);

	for (auto & client : Server::clients.getClientArray())
		Server::topPlayers.addPlayer(*client->getPlayer());

	Server::topPlayers.saveTop10();
	
	return 0;
}

DWORD WINAPI GameThread(LPVOID args) {
	bool * CONTINUE = (bool *)args;
	*CONTINUE = true;

	int difficulty = 0;
	GameData * gameData = Server::gameData->getGameData();

	//while (*CONTINUE) {
		Server::gameData->setGameDataState(RUNNING);
		Server::gameData->setupBall();

		// 1 - Position users
		//If there are no clients, stop the thread.
		if (Server::clients.getClientArray().size() == 0)
			return -1;

		Server::gameData->setupPlayers();
		
		// 2 - Generate map
		Server::gameData->generateLevel(difficulty);
		
		// 3 - Start ball thread
		Server::threadManager.startBallThread();
		
		// 4 - wait for gameEvent
		//Server::gameData->waitForGameEvent();
		//if (difficulty++ < 5)
			//continue;
		//if (gameData->gameState == NEXT_LEVEL && difficulty < 10) {//TODO: maybe change this number, we can always pass a argument to compare here
		//	Server::threadManager.waitForBallThread();
		//	continue; //Create a new level and all that great stuff
		//}
		//else { //if it's anyting else, then just quits
		//	return 0; //TODO: take a look at this
		//}
	//}

	return 0;
}

DWORD WINAPI SharedMemClientHandler(LPVOID args) {
	bool * CONTINUE = (bool *)args;
	bool ACTIVE = true;
	Player * player = nullptr;
	ClientMsg request;
	ServerMsg reply;
	HANDLE update, ready;

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

					if(Server::sharedMemory.addNotifiers(ready, update, request.message.name) == false){
						reply.type = DENY_SERVER_FULL;
						Server::sharedMemory.writeMessage(reply);
						break;
					}
					Server::clients.AddLocalClient(request.message.name, player, ready, update, reply.id);
					
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

					Server::clients.AddRemoteClient(request.message.name, player, hPipe, hGameDataPipe, reply.id);
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
	WaitForMultipleObjects((DWORD) clientThreads.size(), clientThreads.data(), TRUE, INFINITE);

	tcout << "Remote client handler Thread ended" << endl;

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

bool ThreadManager::startGameThread() {

	hGameThread = CreateThread(nullptr, 0, GameThread, (LPVOID)&gameThreadRunning, 0, nullptr);
	if (hGameThread == nullptr)
		return false;

	return true;
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

bool ThreadManager::isGameRunning() const{
	return gameThreadRunning;
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

void ThreadManager::endGame()
{
	gameThreadRunning = false;
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
		WaitForMultipleObjects((DWORD)hBonuses.size(), hBonuses.data(), TRUE, INFINITE);
	}

	for (auto & thread : hBonuses) {
		CloseHandle(thread);
	}
}

void ThreadManager::waitForGameThread()
{
	WaitForSingleObject(hGameThread,INFINITE);
	CloseHandle(hGameThread);
}