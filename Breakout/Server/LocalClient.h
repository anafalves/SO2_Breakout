#pragma once
#include "Client.h"
#include "Messages.h"

class LocalClient : public Client
{
private:
	HANDLE updateFlag;

public:
	LocalClient(std::tstring username, HANDLE flag, Player * p)
		:Client(username, p), updateFlag(flag)
	{
	}

	const HANDLE getFlag() const;

	~LocalClient();
};

