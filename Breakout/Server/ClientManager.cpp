#include "ClientManager.h"
#include "Server.h"

ClientManager::ClientManager() {
	hClientMutex = CreateMutex(NULL, FALSE, NULL);
	if (hClientMutex == NULL) {
		throw EXCEPTION_ACCESS_VIOLATION;
	}
}

const vector<Client *> ClientManager::getClientArray() const {
	return clients;
}

Player * ClientManager::getClientPlayer(int id) {
	for (auto & client : clients) {
		if (id == client->getId())
			return client->getPlayer();
	}

	return nullptr;
}

void ClientManager::AddLocalClient(std::tstring name, Player * p, HANDLE & resourceFreed, HANDLE & updateReady, int &myId)
{
	HANDLE modify[2];

	modify[0] = hClientMutex;
	modify[1] = Server::sharedMemory.hExitEvent;

	WaitForMultipleObjects(2, modify, FALSE, INFINITE);

	LocalClient * temp = new LocalClient(name, resourceFreed, updateReady, p);
	myId = temp->getId();
	clients.push_back(temp);

	ReleaseMutex(hClientMutex);
}

void ClientManager::AddRemoteClient(std::tstring name, Player * p, 
							HANDLE hPipe, HANDLE hGameDataPipe, int &myId)
{
	HANDLE modify[2];

	modify[0] = hClientMutex;
	modify[1] = Server::sharedMemory.hExitEvent;

	WaitForMultipleObjects(2, modify, FALSE, INFINITE);
	
	RemoteClient * client = new RemoteClient(name, p, hPipe, hGameDataPipe);
	myId = client->getId();
	clients.push_back(client);

	ReleaseMutex(hClientMutex);
}

bool ClientManager::removeClient(std::tstring name) {
	HANDLE modify[2];

	modify[0] = hClientMutex;
	modify[1] = Server::sharedMemory.hExitEvent;

	WaitForMultipleObjects(2, modify, FALSE, INFINITE);

	for (unsigned i = 0; i < clients.size(); i++) {
		if (clients[i]->getName() == name) {
			clients.erase(clients.begin() + i);
			ReleaseMutex(hClientMutex);
			return true;
		}
	}

	ReleaseMutex(hClientMutex);

	return false;
}

bool ClientManager::removeClient(int id) {
	HANDLE modify[2];

	modify[0] = hClientMutex;
	modify[1] = Server::sharedMemory.hExitEvent;

	WaitForMultipleObjects(2, modify, FALSE, INFINITE);

	for (unsigned i = 0; i < clients.size(); i++) {
		if (clients[i]->getId() == id) {
			delete clients[i];
			clients.erase(clients.begin() + i);
			ReleaseMutex(hClientMutex);
			return true;
		}
	}

	ReleaseMutex(hClientMutex);

	return false;
}

bool ClientManager::removeClient(const HANDLE & primaryHandle)
{
	HANDLE modify[2];

	modify[0] = hClientMutex;
	modify[1] = Server::sharedMemory.hExitEvent;

	WaitForMultipleObjects(2, modify, FALSE, INFINITE);

	for (unsigned i = 0; i < clients.size(); i++) {
		if (clients[i]->getPrimaryHandle() == primaryHandle) {
			delete clients[i];
			clients.erase(clients.begin() + i);
			ReleaseMutex(hClientMutex);
			return true;
		}
	}

	ReleaseMutex(hClientMutex);

	return false;
}

void ClientManager::broadcastGameData() {
	HANDLE access[2];

	access[0] = hClientMutex;
	access[1] = Server::sharedMemory.hExitEvent;

	WaitForMultipleObjects(2, access, FALSE, INFINITE);

	for (auto const &client : clients) {
		client->sendUpdate();
	}

	ReleaseMutex(hClientMutex);
}

bool ClientManager::isNameAvailable(std::tstring name) const {
	HANDLE access[2];

	access[0] = hClientMutex;
	access[1] = Server::sharedMemory.hExitEvent;

	WaitForMultipleObjects(2, access, FALSE, INFINITE);

	for (const auto &client : clients) {
		if (client->getName() == name) {
			ReleaseMutex(hClientMutex);
			return false;
		}
	}

	ReleaseMutex(hClientMutex);

	return true;
}

bool ClientManager::hasRoom() const {
	bool result;
	HANDLE access[2];

	access[0] = hClientMutex;
	access[1] = Server::sharedMemory.hExitEvent;

	WaitForMultipleObjects(2, access, FALSE, INFINITE);

	result = clients.size() < (unsigned int)Server::config.getMaxPlayerCount();

	ReleaseMutex(hClientMutex);

	return result;
}

tstring ClientManager::getClientsAsString() const{
	tstringstream tss;

	tss << TEXT(" - Listing [ ") << clients.size() << TEXT(" ] client(s): ") << endl;
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