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
	void initGameData();

public:
	GameData * viewGameData;
	MessageBuffer * viewServerBuffer;
	MessageBuffer * viewClientBuffer;

	HANDLE hServerSemEmpty;
	HANDLE hServerSemFilled;

	HANDLE hClientSemEmpty;
	HANDLE hClientSemFilled;

	SharedMemoryManager();
	~SharedMemoryManager();
};

