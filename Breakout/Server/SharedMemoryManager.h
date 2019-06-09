#pragma once
#include <Windows.h>
#include <vector>
#include "Messages.h"
#include "GeneralConstants.h"
#include "GameData.h"

class SharedMemoryManager
{
private:
	vector<HANDLE> updateNotifications;
	vector<HANDLE> clientReadyNotifications;

	HANDLE hClientReadyMutex;
	HANDLE hUpdateMutex;

	HANDLE hGameData;
	HANDLE hServerBuffer;
	HANDLE hClientBuffer;

	HANDLE hServerSemEmpty;
	HANDLE hServerSemFilled;

	HANDLE hClientSemEmpty;
	HANDLE hClientSemFilled;

	GameData * viewGameData;
	ServerMsgBuffer * viewServerBuffer;
	ClientMsgBuffer * viewClientBuffer;

	int initSemaphores();

public:
	HANDLE hExitEvent;

	int initSharedMemory();
	ClientMsg readMessage();
	void writeMessage(ServerMsg message);
	void setUpdate(HANDLE & handle) const;
	GameData * getGameData() const;

	bool addNotifiers(HANDLE & clientReady, HANDLE & updateReady, tstring name);
	void removeNotifiers(HANDLE & updateReady, HANDLE & clientReady);
	void waitForAllClientsReady();

	SharedMemoryManager();
	~SharedMemoryManager();
};

