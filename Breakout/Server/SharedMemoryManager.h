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

	void initSharedMemory();
	void initSemaphores();

public:
	GameData * viewGameData;
	MessageBuffer * viewServerBuffer;
	MessageBuffer * viewClientBuffer;

	//TODO: Add trigger HANDLE that will be used as an EVENT for gameDataUpdates!
	//HANDLE hUpdateEvent;
	HANDLE hServerSemEmpty;
	HANDLE hServerSemFilled;

	HANDLE hClientSemEmpty;
	HANDLE hClientSemFilled;

	SharedMemoryManager();
	~SharedMemoryManager();
};

