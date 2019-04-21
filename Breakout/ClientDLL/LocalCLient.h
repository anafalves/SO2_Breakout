#pragma once
#include "Client.h"
#include "SharedMemoryManager.h"
#include "../Server/Messages.h"

typedef struct {
	ServerMsg msg;
	tstring errorMsg;
}ServerResult;

class LocalCLient :
	public Client
{
private:
	SharedMemoryManager sharedMemmoryContent;

public:
	LocalCLient() {};

	bool login(TCHAR name[MAX_NAME_LENGHT]);
	//receiveBroadcast() = 0;
	void sendMessage(ClientMsg msg);
	ServerMsg receiveMessage();
	ServerResult receiveLoginAnswer(tstring name);

};