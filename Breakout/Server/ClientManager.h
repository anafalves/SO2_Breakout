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
	HANDLE hClientMutex;

public:
	ClientManager();
	void AddLocalClient(std::tstring name, Player * p, HANDLE & resourceFreed, HANDLE & updateReady, int & myId);
	void AddRemoteClient(std::tstring name, Player * p, HANDLE hPipe, HANDLE hGameDataPipe, int &myId);
	bool removeClient(std::tstring name);
	bool removeClient(int id);
	bool removeClient(const HANDLE & handle);
	Player * getClientPlayer(int id);

	tstring getClientsAsString() const;

	bool hasRoom() const;
	bool isNameAvailable(std::tstring name) const;

	void broadcastGameData();
	const vector<Client *> getClientArray() const;

	virtual ~ClientManager();
};

tostream& operator <<(tostream & os, const ClientManager & clients);