#pragma once
#include "Client.h"
#include "SharedMemoryManager.h"
#include "../Server/Messages.h"

class LocalCLient :
	public Client
{
private:
	SharedMemoryManager sharedMemmoryContent;

public:
	LocalCLient()
		:Client()
	{};

	bool login(TCHAR * name);
	GameData receiveBroadcast();
	void sendMessage(ClientMsg msg);
	ServerMsg receiveMessage();
	ServerMsg receiveMessageWithTimeout();
};