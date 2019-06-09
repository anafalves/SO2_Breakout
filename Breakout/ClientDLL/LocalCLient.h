#pragma once
#include "Client.h"
#include "SharedMemoryManager.h"
#include "../Server/Messages.h"

class CLIENT_API LocalCLient : public Client
{
private:
	SharedMemoryManager * sharedMemmoryContent;

	ServerMsg receiveMessageLogin(tstring name);
public:
	LocalCLient();
	~LocalCLient();
	int login(TCHAR * name);
	GameData receiveBroadcast();
	bool sendMessage(ClientMsg msg);
	ServerMsg receiveMessage();

	bool isReady() const;
};