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

	HANDLE hUpdateEvent;
	HANDLE hServerSemEmpty;
	HANDLE hServerSemFilled;

	HANDLE hClientSemEmpty;
	HANDLE hClientSemFilled;

	GameData * viewGameData;
	ServerMsgBuffer * viewServerBuffer;
	ClientMsgBuffer * viewClientBuffer;

	int initSemaphores();

public:

	GameData * getGameData() const {
		return viewGameData;
	};

	int initSharedMemory();

	SharedMemoryManager();
	~SharedMemoryManager();
};

