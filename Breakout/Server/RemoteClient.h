#pragma once
#include "Client.h"
class RemoteClient : public Client
{
private:
	HANDLE hPipe;
	HANDLE hThread;

public:
	RemoteClient(std::tstring username, Player * p, HANDLE pipe, HANDLE thread)
		:Client(username, p), hPipe(pipe), hThread(thread)
	{};

	//TODO: move this to cpp
	void sendMessage() {
		//TODO: Send Message
	}

	~RemoteClient() {
		//TODO: Disconnect pipe and close stuff as such.
	}
};

