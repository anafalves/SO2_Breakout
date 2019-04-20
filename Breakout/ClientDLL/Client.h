#pragma once
#include<string>
#include "../Server/UnicodeConfigs.h"

class Client
{
public:
	Client();
	virtual ~Client() = 0;

	virtual bool login(std::tstring name) = 0;
	//virtual receiveBroadcast() = 0;
	//virtual sendMessage() = 0;
	//virtual receiveMessage() = 0;
};


