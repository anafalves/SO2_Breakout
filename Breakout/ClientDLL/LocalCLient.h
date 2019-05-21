#pragma once
#include "Client.h"
#include "SharedMemoryManager.h"
#include "../Server/Messages.h"

class CLIENT_API LocalCLient : public Client
{
private:
	SharedMemoryManager sharedMemmoryContent;

public:
	LocalCLient()
	:Client()
	{};

	bool login(TCHAR * name);
	GameData receiveBroadcast();
	bool sendMessage(ClientMsg msg);
	ServerMsg receiveMessage();
	ServerMsg receiveMessageWithTimeout();
};