#pragma once
#include "Client.h"
#include "Messages.h"

class LocalClient : public Client
{
private:

public:
	LocalClient(std::tstring username, Player * p)
		:Client(username, p)
	{
	}

	~LocalClient();
};

