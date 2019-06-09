#pragma once
#include "Client.h"
#include "Messages.h"

class LocalClient : public Client
{
private:
	HANDLE notifyResourceFreed; //Notifies server that client is no longer using resource
	HANDLE notifyUpdate;  //Notifies client of update of data

public:
	LocalClient(std::tstring username, HANDLE resourceFreed, HANDLE updateReady, Player * p)
		:Client(username, p), notifyResourceFreed(resourceFreed), notifyUpdate(updateReady)
	{
	}

	HANDLE & getPrimaryHandle();
	void sendUpdate();

	const HANDLE getResourceFreedNotifier() const;
	const HANDLE getUpdateReadyNotifier() const;

	virtual ~LocalClient();
};

