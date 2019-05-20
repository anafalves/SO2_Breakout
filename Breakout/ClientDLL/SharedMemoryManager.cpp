#include "SharedMemoryManager.h"

SharedMemoryManager::SharedMemoryManager()
{
	initSharedMemory();
	initSyncVariables();
}


SharedMemoryManager::~SharedMemoryManager()
{
	CloseHandle(hServerSemEmpty);
	CloseHandle(hServerSemFilled);
	CloseHandle(hClientSemEmpty);
	CloseHandle(hClientSemFilled);

	UnmapViewOfFile(viewClientBuffer);
	UnmapViewOfFile(viewServerBuffer);
	UnmapViewOfFile(viewGameData);

	CloseHandle(hClientWriteMutex);
	CloseHandle(hClientBuffer);
	CloseHandle(hServerBuffer);
	CloseHandle(hGameData);
}

void SharedMemoryManager::initSharedMemory() {
	LARGE_INTEGER size;
	size.QuadPart = sizeof(ServerMsgBuffer);

	//Server Message Buffer
	hServerBuffer = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, NULL,
		SharedMemoryConstants::SHA_MEM_SERVER_BUFFER.c_str());

	if (hServerBuffer == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("couldn't create file mapping for server messageBuffer!");
	}

	viewServerBuffer = (ServerMsgBuffer *)MapViewOfFile(hServerBuffer, FILE_MAP_READ | FILE_MAP_WRITE,
		0, 0, (SIZE_T)size.QuadPart);
	if (viewServerBuffer == NULL)
	{
		tcout << "couldn't create map view of file for server messageBuffer!";
		this->~SharedMemoryManager();
		throw 10;//TEXT("couldn't create map view of file for server messageBuffer!");
	}

	//Local Client Message Buffer
	size.QuadPart = sizeof(ClientMsgBuffer);

	hClientBuffer = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,NULL,
		SharedMemoryConstants::SHA_MEM_CLIENT_BUFFER.c_str());

	if (hClientBuffer == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("couldn't create file mapping for client messageBuffer!");
	}

	viewClientBuffer = (ClientMsgBuffer *)MapViewOfFile(hClientBuffer, FILE_MAP_READ | FILE_MAP_WRITE,
		0, 0, (SIZE_T)size.QuadPart);
	if (viewClientBuffer == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("couldn't create map view of file for client messageBuffer!");
	}


	//GameData Shared memory
	size.QuadPart = sizeof(GameData);
	hGameData = OpenFileMapping(FILE_MAP_READ, NULL,
		SharedMemoryConstants::SHA_MEM_GAMEDATA.c_str());

	if (hGameData == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("couldn't create file mapping GameData!");
	}

	viewGameData = (GameData *)MapViewOfFile(hGameData, FILE_MAP_READ,
		0, 0, (SIZE_T)size.QuadPart);
	if (viewGameData == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("couldn't create map view of file for GameData!");
	}
}

void SharedMemoryManager::initSyncVariables() {
	hServerSemEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false,
		SharedMemoryConstants::SEM_SERVER_EMPTY.c_str());
	if (hServerSemEmpty == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("Error while trying to create ServerSemaphore empty");
	}

	hServerSemFilled = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false,
		SharedMemoryConstants::SEM_SERVER_FILLED.c_str());
	if (hServerSemFilled == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("Error while trying to create ServerSemaphore filled");
	}

	hClientSemEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false,
		SharedMemoryConstants::SEM_CLIENT_EMPTY.c_str());
	if (hClientSemEmpty == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("Error while trying to create ClientSemaphore empty");
	}

	hClientSemFilled = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false,
		SharedMemoryConstants::SEM_CLIENT_FILLED.c_str());
	if (hClientSemFilled == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("Error while trying to create ClientSemaphore filled");
	}
	
	hUpdateEvent = OpenEvent(EVENT_ALL_ACCESS, false, SharedMemoryConstants::EVENT_GAMEDATA_UPDATE.c_str());
	if (hUpdateEvent == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("Error while trying to open gamedata update event");
	}

	hExitEvent = CreateEvent(NULL, false, false, NULL);
	if (hExitEvent == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("Error while trying to create gamedata update event");
	}

	hClientWriteMutex = CreateMutex(NULL, FALSE, SharedMemoryConstants::MUT_CLI_WRITE.c_str());
	if (hClientWriteMutex == NULL) {
		this->~SharedMemoryManager();
		throw TEXT("Error while trying to create ClientWriteMutex");
	}
}