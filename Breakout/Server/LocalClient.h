#pragma once
#include "Client.h"
class LocalClient : public Client
{
private:


public:
	LocalClient(std::tstring username)
		:Client(username)
	{

	}
	~LocalClient();
};

