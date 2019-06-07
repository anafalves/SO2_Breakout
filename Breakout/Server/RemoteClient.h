#pragma once
#include "Client.h"
class RemoteClient : public Client
{
private:
	HANDLE hPipe;
	HANDLE hPipeGameData;
	HANDLE hWriteReady;

public:
	RemoteClient(tstring username, Player * p, HANDLE pipe, HANDLE hGamedata);

	void sendUpdate();

	~RemoteClient() {
		FlushFileBuffers(hPipe);
		FlushFileBuffers(hPipeGameData);
		DisconnectNamedPipe(hPipe);
		DisconnectNamedPipe(hPipeGameData);
		CloseHandle(hPipe);
		CloseHandle(hPipeGameData);
	}
};

