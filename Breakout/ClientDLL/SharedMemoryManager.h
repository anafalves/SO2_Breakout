#pragma once
#include <Windows.h>
#include "../Server/Messages.h"
#include "../Server/SharedMemoryConstants.h"
#include "../Server/GameData.h"

class SharedMemoryManager
{
private:
	HANDLE hGameData;
	HANDLE hServerBuffer;
	HANDLE hClientBuffer;

	void initSharedMemory();
	void initSyncVariables();

public:
	GameData * viewGameData;
	MessageBuffer * viewServerBuffer;
	MessageBuffer * viewClientBuffer;

	HANDLE hUpdateEvent;
	HANDLE hServerSemEmpty;
	HANDLE hServerSemFilled;

	HANDLE hClientSemEmpty;
	HANDLE hClientSemFilled;

	SharedMemoryManager();
	~SharedMemoryManager();
};

