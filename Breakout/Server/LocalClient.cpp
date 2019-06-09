#include "LocalClient.h"
#include "Server.h"

const HANDLE LocalClient::getResourceFreedNotifier() const
{
	return notifyResourceFreed;
}

void LocalClient::sendUpdate() {
	SetEvent(notifyUpdate);
}

const HANDLE LocalClient::getUpdateReadyNotifier() const
{
	return notifyUpdate;
}

HANDLE & LocalClient::getPrimaryHandle() {
	return notifyResourceFreed;
}

LocalClient::~LocalClient()
{
	Server::sharedMemory.removeNotifiers(notifyUpdate, notifyResourceFreed);
	/*CloseHandle(notifyResourceFreed);
	CloseHandle(notifyUpdate);*/
}