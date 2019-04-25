#pragma once
#include "Client.h"
#include "SharedMemoryManager.h"
#include "../Server/Messages.h"

#ifdef CLIENTDLL_EXPORTS
#define CLIENT_API __declspec(dllexport)
#else
#define CLIENT_API __declspec(dllimport)
#endif

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
	LocalCLient()
		:Client()
	{};

	CLIENT_API bool login(TCHAR * name);
	CLIENT_API GameData receiveBroadcast();
	void sendMessage(ClientMsg msg);
	ServerMsg receiveMessage();
	ServerResult receiveLoginAnswer(tstring name);

};

CLIENT_API LocalCLient * getClientInstance();