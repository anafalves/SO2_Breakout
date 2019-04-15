#pragma once
#include <iostream>
#include <vector>
#include "SharedMemoryManager.h"
#include "Client.h"

class ServerManager
{
private:
	SharedMemoryManager sharedMem;
	vector<Client *> clients; //TODO: management funcs

public:
	ServerManager();
	~ServerManager();

	int getClientCount() {
		return clients.size();
	};

	tstring getConnectedClients();
	void broadcastGameData();
};

