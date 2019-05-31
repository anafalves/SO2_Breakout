#pragma once
#include "Client.h"
#include "..\Server\Messages.h"

class CLIENT_API RemoteClient : public Client
{
private:
	HANDLE hWriteMutex;
	HANDLE hExitEvent;

	tstring ipAddress;
	HANDLE hPipeMessage;
	HANDLE hPipeGameData;

	bool connectToGameDataPipe(TCHAR * name);

public:
	RemoteClient();
	bool isConnected();
	bool connect(TCHAR * ipAddr);
	bool login(TCHAR * name);
	GameData receiveBroadcast();
	bool sendMessage(ClientMsg message);
	ServerMsg receiveMessage();
	~RemoteClient();
};