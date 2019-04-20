#include "SharedMemoryManager.h"

SharedMemoryManager::SharedMemoryManager()
{
}

SharedMemoryManager::~SharedMemoryManager()
{
	CloseHandle(hServerSemEmpty);
	CloseHandle(hServerSemFilled);
	CloseHandle(hClientSemEmpty);
	CloseHandle(hClientSemFilled);
	CloseHandle(hUpdateEvent);

	UnmapViewOfFile(viewClientBuffer);
	UnmapViewOfFile(viewServerBuffer);
	UnmapViewOfFile(viewGameData);

	CloseHandle(hClientBuffer);
	CloseHandle(hServerBuffer);
	CloseHandle(hGameData);
}

int SharedMemoryManager::initSharedMemory() {
	LARGE_INTEGER size;
	size.QuadPart = sizeof(ServerMsgBuffer);

	//Server Message Buffer
	hServerBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
					size.HighPart, size.LowPart,SharedMemoryConstants::SHA_MEM_SERVER_BUFFER.c_str());

	if (hServerBuffer == NULL)
	{
		this->~SharedMemoryManager();
		return -1;
	}

	viewServerBuffer = (ServerMsgBuffer *)MapViewOfFile(hServerBuffer, FILE_MAP_READ | FILE_MAP_WRITE,
						0, 0, (SIZE_T)size.QuadPart);
	if (viewServerBuffer == NULL)
	{
		this->~SharedMemoryManager();
		return -1;
	}

	viewServerBuffer->read_pos = viewServerBuffer->write_pos = 0;


	//Local Client Message Buffer
	hClientBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
		size.HighPart, size.LowPart, SharedMemoryConstants::SHA_MEM_CLIENT_BUFFER.c_str());

	if (hClientBuffer == NULL)
	{
		this->~SharedMemoryManager();
		return -1;
	}

	viewClientBuffer = (ClientMsgBuffer *)MapViewOfFile(hClientBuffer,FILE_MAP_WRITE,
						0, 0, (SIZE_T)size.QuadPart);
	if (viewClientBuffer == NULL)
	{
		this->~SharedMemoryManager();
		return -1;
	}

	viewClientBuffer->read_pos = viewClientBuffer->write_pos = 0;

	//GameData Shared memory
	size.QuadPart = sizeof(GameData);
	hGameData = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
		size.HighPart, size.LowPart, SharedMemoryConstants::SHA_MEM_GAMEDATA.c_str());

	if (hGameData == NULL)
	{
		this->~SharedMemoryManager();
		return -1;
	}

	viewGameData = (GameData *)MapViewOfFile(hGameData, FILE_MAP_WRITE | FILE_MAP_READ,
					0, 0, (SIZE_T)size.QuadPart);
	if (viewGameData == NULL)
	{
		this->~SharedMemoryManager();
		return -1;
	}
	
	//Set all newly allocated shared memory to 0 ( clear memory )
	memset(viewGameData, 0, sizeof(GameData));
	memset(viewClientBuffer, 0, sizeof(ClientMsgBuffer));
	memset(viewServerBuffer, 0, sizeof(ServerMsgBuffer));

	if (initSemaphores() < 0) {
		return -2;
	}

	return 0;
}

int SharedMemoryManager::initSemaphores() {
	hServerSemEmpty = CreateSemaphore(NULL, MAX_MESSAGE_BUFFER_SIZE, MAX_MESSAGE_BUFFER_SIZE,
						SharedMemoryConstants::SEM_SERVER_EMPTY.c_str());
	if (hServerSemEmpty == NULL)
	{
		this->~SharedMemoryManager();
		return -1;
	}

	hServerSemFilled = CreateSemaphore(NULL, 0, MAX_MESSAGE_BUFFER_SIZE,
						SharedMemoryConstants::SEM_SERVER_FILLED.c_str());
	if (hServerSemFilled == NULL)
	{
		this->~SharedMemoryManager();
		return -1;
	}

	hClientSemEmpty = CreateSemaphore(NULL, MAX_MESSAGE_BUFFER_SIZE, MAX_MESSAGE_BUFFER_SIZE,
						SharedMemoryConstants::SEM_CLIENT_EMPTY.c_str());
	if (hClientSemEmpty == NULL)
	{
		this->~SharedMemoryManager();
		return -1;
	}

	hClientSemFilled = CreateSemaphore(NULL, 0, MAX_MESSAGE_BUFFER_SIZE,
						SharedMemoryConstants::SEM_CLIENT_FILLED.c_str());
	if (hClientSemFilled == NULL)
	{
		this->~SharedMemoryManager();
		return -1;
	}

	hUpdateEvent = CreateEvent(NULL,TRUE, FALSE, SharedMemoryConstants::EVENT_GAMEDATA_UPDATE.c_str());
	if (hUpdateEvent == NULL)
	{
		this->~SharedMemoryManager();
		return -1;
	}

	return 0;
}