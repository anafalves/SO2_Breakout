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

	GameData * viewGameData;
	ServerMsgBuffer * viewServerBuffer;
	ClientMsgBuffer * viewClientBuffer;

	int initSemaphores();

public:

	HANDLE hUpdateEvent;
	HANDLE hServerSemEmpty;
	HANDLE hServerSemFilled;
	HANDLE hExitEvent;
	HANDLE hClientSemEmpty;
	HANDLE hClientSemFilled;

	GameData * getGameData() const {
		return viewGameData;
	};

	int initSharedMemory();

	//TODO: Add read functions: ReadMsg, WriteMsg, setUpdate.
	void setUpdate() {
		SetEvent(hUpdateEvent);
		ResetEvent(hUpdateEvent);
	}

	SharedMemoryManager();
	~SharedMemoryManager();
};

