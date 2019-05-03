#pragma once
#include <tchar.h>
#include "../Server/UnicodeConfigs.h"

class Client
{
private:
	int client_id;//variable to control the reading cycle of the memmory buffer
public:
	bool CONTINUE;

	Client() {
		CONTINUE = true;
	}
	//TODO: add wait functions to end with threads or stuff that may be running using this ( CONTINUE = FALSE)
	int getClientID() { return client_id; }
	void setClientID(int id) { client_id = id; }

	virtual bool login(TCHAR * name) = 0;
	//virtual receiveBroadcast() = 0;
	//virtual sendMessage() = 0;
	//virtual receiveMessage() = 0;
};


