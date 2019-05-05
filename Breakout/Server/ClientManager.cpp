#include "ClientManager.h"
#include "Server.h"

int ClientManager::AddClient(std::tstring name, int &myId) {
	if (clients.size() >= (unsigned int)Server::config.getMaxPlayerCount()) {
		return DENY_SERVER_FULL;
	}

	for (auto &client : clients) {
		if (client->getName() == name) {
			return DENY_USERNAME;
		}
	}

	Player * p = Server::gameData.getAvailablePlayer();
	if (p == nullptr) {
		return -1;
	}

	LocalClient * temp = new LocalClient(name, p);
	myId = temp->getId();

	clients.push_back(temp);

	return ACCEPT;
}

int ClientManager::AddClient(std::tstring name, HANDLE pipe, HANDLE thread){
	if (clients.size() >= (unsigned int) Server::config.getMaxPlayerCount()) {
		return DENY_SERVER_FULL;
	}

	for (auto &client : clients) {
		if (client->getName() == name) {
			return DENY_USERNAME;
		}
	}

	Player * p = Server::gameData.getAvailablePlayer();
	if (p == nullptr) {
		return -1;
	}

	clients.push_back(new RemoteClient(name,p,pipe,thread));

	return ACCEPT;
}

bool ClientManager::removeClient(std::tstring name) {
	for (unsigned i = 0; i < clients.size(); i++) {
		if (clients[i]->getName() == name) {
			clients.erase(clients.begin() + i);
			return true;
		}
	}

	return false;
}

bool ClientManager::removeClient(int id) {
	for (unsigned i = 0; i < clients.size(); i++) {
		if (clients[i]->getId() == id) {
			clients.erase(clients.begin() + i);
			return true;
		}
	}

	return false;
}

ClientManager::~ClientManager() {
	for (auto &client : clients) {
		delete client;
	}

	clients.clear();
}

tstring ClientManager::getClientsAsString() const{
	tstringstream tss;

	for (const auto &client : clients) {
		tss << client;
	}

	return tss.str();
}

tostream& operator <<(tostream & tos, const ClientManager& cli) {
	tstringstream tss;
	
	tss << cli.getClientsAsString();
	tos << tss.str();
	return tos;
}