#pragma once
#include <Windows.h>
#include <vector>
#include "Messages.h"
#include "GeneralConstants.h"
#include "GameData.h"
#include "Spectator.h"

class SharedMemoryManager
{
private:
	int updateCounter;
	vector<Spectator *> spectators;
	vector<HANDLE> updateFlags;

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
	void setUpdate();
	GameData * getGameData() const;

	int addClientUpdateFlag();
	void removeClientUpdateFlag(int id);
	void waitForUpdateFlags() const;

	SharedMemoryManager();
	~SharedMemoryManager();
};

