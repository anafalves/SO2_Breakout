#pragma once
#include "Client.h"
#include "SharedMemoryManager.h"
#include "../Server/Messages.h"

class CLIENT_API LocalCLient : public Client
{
private:
	SharedMemoryManager sharedMemmoryContent;
	int update_id;

public:
	LocalCLient()
	:Client()
	{
		update_id = 0;
	};

	bool login(TCHAR * name);
	GameData receiveBroadcast();
	bool sendMessage(ClientMsg msg);
	ServerMsg receiveMessage();
	ServerMsg receiveMessageWithTimeout();

	void setUpdateId(int id);
	int getUpdateId() const;
};