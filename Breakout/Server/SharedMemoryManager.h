#pragma once
#include <Windows.h>
#include "Messages.h"
#include "SharedMemoryConstants.h"
#include "GameData.h"

class SharedMemoryManager
{
private:
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
	HANDLE hUpdateEvent;
	HANDLE hExitEvent;

	int initSharedMemory();
	ClientMsg readMessage();
	void writeMessage(ServerMsg message);

	void setUpdate() {
		SetEvent(hUpdateEvent);
		ResetEvent(hUpdateEvent);
	}

	GameData * getGameData() const {
		return viewGameData;
	};

	SharedMemoryManager();
	~SharedMemoryManager();
};

