#pragma once
#include "Client.h"
class RemoteClient : public Client
{
private:
	HANDLE hPipe;
	HANDLE hPipeGameData;

public:
	RemoteClient(tstring username, Player * p, HANDLE pipe, HANDLE hGamedata)
		:Client(username, p), hPipe(pipe), hPipeGameData(hGamedata)
	{
	}

	~RemoteClient() {
		FlushFileBuffers(hPipe);
		FlushFileBuffers(hPipeGameData);
		DisconnectNamedPipe(hPipe);
		DisconnectNamedPipe(hPipeGameData);
		CloseHandle(hPipe);
		CloseHandle(hPipeGameData);
	}
};

