#include "RemoteClient.h"
#include "Server.h"

void RemoteClient::sendUpdate() {
	DWORD nBytes = 0;

	WriteFile(hPipeGameData, Server::sharedMemory.getGameData(), sizeof(GameData), NULL, NULL);
}