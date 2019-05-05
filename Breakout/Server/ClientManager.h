#pragma once
#include <vector>
#include <string>
#include "UnicodeConfigs.h"
#include "Messages.h"
#include "Client.h"
#include "LocalClient.h"
#include "RemoteClient.h"

class ClientManager {
private:
	std::vector<Client *> clients;

public:
	int AddClient(std::tstring name, int & myId);
	int AddClient(std::tstring name, HANDLE pipe, HANDLE thread);
	bool removeClient(std::tstring name);
	bool removeClient(int id);
	tstring getClientsAsString() const;

	//TODO: add broadcast method here

	~ClientManager();
};

tostream& operator <<(tostream & os, const ClientManager & clients);