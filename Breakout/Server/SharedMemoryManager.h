#pragma once
#include <Windows.h>
#include "Messages.h"
#include "SharedMemoryConstants.h"
#include "GameData.h"

class SharedMemoryManager
{
private:
	GameData * viewGameData;
	MessageBuffer * viewServerBuffer;
	MessageBuffer * viewClientBuffer;
	
	HANDLE hGameData;

	HANDLE hServerBuffer;
	HANDLE hServerSemEmpty;
	HANDLE hServerSemFilled;

	HANDLE hClientBuffer;
	HANDLE hClientSemEmpty;
	HANDLE hClientSemFilled;

	void initSharedMemory();
	void initSemaphores();
	void initGameData();
public:
	SharedMemoryManager();
	~SharedMemoryManager();
};

