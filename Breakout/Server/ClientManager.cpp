#include "ClientManager.h"
#include "Server.h"

void ClientManager::AddClient(std::tstring name, Player * p, int &myId) {
	LocalClient * temp = new LocalClient(name, p);
	myId = temp->getId();

	clients.push_back(temp);
}

void ClientManager::AddClient(std::tstring name, Player * p, HANDLE hPipe, HANDLE hGameDataPipe, int &myId){
	RemoteClient * client = new RemoteClient(name, p, hPipe, hGameDataPipe);
	myId = client->getId();

	clients.push_back(client);
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

void ClientManager::broadcastGameData() {
	for (auto const &client : clients) {
		client->sendUpdate();
	}
}

bool ClientManager::isNameAvailable(std::tstring name) const {
	for (const auto &client : clients) {
		if (client->getName() == name) {
			return false;
		}
	}

	return true;
}

bool ClientManager::hasRoom() const {
	return clients.size() < (unsigned int)Server::config.getMaxPlayerCount();
}

tstring ClientManager::getClientsAsString() const{
	tstringstream tss;

	tss << TEXT("Listing [ ") << clients.size() << TEXT(" ] client(s): ") << endl;
	tss << TEXT("id :: name :: status") << endl << endl;
	for (const auto &client : clients) {
		tss << TEXT(" - ") << client->getAsString();
	}

	return tss.str();
}

ClientManager::~ClientManager() {
	for (auto &client : clients) {
		delete client;
	}

	clients.clear();
}

tostream& operator <<(tostream & tos, const ClientManager& cli) {
	tstringstream tss;

	tos << cli.getClientsAsString();
	
	return tos;
}