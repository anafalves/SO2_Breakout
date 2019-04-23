#pragma once
#include "Client.h"
class LocalClient : public Client
{
private:

public:
	LocalClient(std::tstring username, Player * p)
		:Client(username, p)
	{
	}

	void sendMessage() {
		//TODO: Send message
	}

	~LocalClient();
};

