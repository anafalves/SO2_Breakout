#include "LocalClient.h"
#include "Server.h"

LocalClient::~LocalClient()
{
	Server::sharedMemory.removeClientUpdateFlag(updateFlag);
	CloseHandle(updateFlag);
}

const HANDLE LocalClient::getFlag() const {
	return updateFlag;
}