#pragma once
#include <tchar.h>
#include <Windows.h>
#include "../Server/UnicodeConfigs.h"
#include "../Server/GameData.h"
#include "../Server/Messages.h"
#include "../Server/GeneralConstants.h"

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
	virtual GameData receiveBroadcast() = 0;
	virtual bool sendMessage(ClientMsg message) = 0;
	virtual ServerMsg receiveMessage() = 0;
	virtual ServerMsg receiveMessageWithTimeout() = 0;
};
