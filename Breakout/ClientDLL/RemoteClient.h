#pragma once
#include "Client.h"
#include "..\Server\Messages.h"

class CLIENT_API RemoteClient : public Client
{
private:
	HANDLE hExitEvent;

	tstring * ipAddress;
	HANDLE hPipeMessage;
	HANDLE hPipeGameData;

	HANDLE hReadReady;
	HANDLE hWriteReady;
	HANDLE hReadGameDataReady;

	bool connectToGameDataPipe(TCHAR * name);

public:
	RemoteClient();
	bool isConnected();
	bool connect(TCHAR * ipAddr);
	int login(TCHAR * name);
	GameData receiveBroadcast();
	bool sendMessage(ClientMsg message);
	ServerMsg receiveMessage();
	~RemoteClient();
};