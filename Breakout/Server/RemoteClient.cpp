#include "RemoteClient.h"
#include "Server.h"

RemoteClient::RemoteClient(tstring username, Player * p, HANDLE pipe, HANDLE hGamedata)
	:Client(username, p), hPipe(pipe), hPipeGameData(hGamedata)
{
	hWriteReady = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hWriteReady == INVALID_HANDLE_VALUE)
		throw EXCEPTION_ACCESS_VIOLATION;
}

HANDLE & RemoteClient::getPrimaryHandle() {
	return hPipe;
}

void RemoteClient::sendUpdate() {

	DWORD nBytes = 0;
	HANDLE write[2];
	OVERLAPPED flag = { 0 };

	ResetEvent(hWriteReady);
	write[0] = hWriteReady;
	write[1] = Server::sharedMemory.hExitEvent;

	flag.hEvent = hWriteReady;

	WriteFile(hPipeGameData, Server::sharedMemory.getGameData(), sizeof(GameData), &nBytes, &flag);

	WaitForMultipleObjects(2, write, FALSE, INFINITE);
	GetOverlappedResult(hPipeGameData, &flag, &nBytes, FALSE);
	if (nBytes != sizeof(ServerMsg) || GetLastError() == ERROR_BROKEN_PIPE) {
		return;
	}
}