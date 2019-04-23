#pragma once
#include "Client.h"
class RemoteClient : public Client
{
private:
	HANDLE hPipe;
	HANDLE hThread;

public:
	RemoteClient(std::tstring username, HANDLE pipe, HANDLE thread)
		:Client(username), hPipe(pipe), hThread(thread)
	{};

	~RemoteClient() {
		//TODO: Disconnect pipe and close stuff as such.
	}
};

