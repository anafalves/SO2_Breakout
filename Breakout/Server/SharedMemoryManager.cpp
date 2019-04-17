#include "SharedMemoryManager.h"

SharedMemoryManager::SharedMemoryManager()
{
	initSharedMemory();
	initGameData();
	initSemaphores();
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

	CloseHandle(hClientBuffer);
	CloseHandle(hServerBuffer);
	CloseHandle(hGameData);
}

void SharedMemoryManager::initSharedMemory() {
	LARGE_INTEGER size;
	size.QuadPart = sizeof(MessageBuffer);

	//Server Message Buffer
	hServerBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
					size.HighPart, size.LowPart,SharedMemoryConstants::SHA_MEM_SERVER_BUFFER.c_str());

	if (hServerBuffer == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("couldn't create file mapping for server messageBuffer!");
	}

	viewServerBuffer = (MessageBuffer *)MapViewOfFile(hServerBuffer, FILE_MAP_READ,
						0, 0, (SIZE_T)size.QuadPart);
	if (viewServerBuffer == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("couldn't create map view of file for server messageBuffer!");
	}

	viewServerBuffer->in = viewServerBuffer->out = 0;


	//Local Client Message Buffer
	hClientBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
		size.HighPart, size.LowPart, SharedMemoryConstants::SHA_MEM_CLIENT_BUFFER.c_str());

	if (hClientBuffer == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("couldn't create file mapping for client messageBuffer!");
	}

	viewClientBuffer = (MessageBuffer *)MapViewOfFile(hClientBuffer,FILE_MAP_WRITE,
						0, 0, (SIZE_T)size.QuadPart);
	if (viewClientBuffer == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("couldn't create map view of file for client messageBuffer!");
	}

	viewClientBuffer->in = viewClientBuffer->out = 0;


	//GameData Shared memory
	size.QuadPart = sizeof(GameData);
	hGameData = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
		size.HighPart, size.LowPart, SharedMemoryConstants::SHA_MEM_GAMEDATA.c_str());

	if (hGameData == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("couldn't create file mapping GameData!");
	}

	viewGameData = (GameData *)MapViewOfFile(hGameData, FILE_MAP_WRITE | FILE_MAP_READ,
					0, 0, (SIZE_T)size.QuadPart);
	if (viewGameData == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("couldn't create map view of file for GameData!");
	}
}

void SharedMemoryManager::initSemaphores() {
	hServerSemEmpty = CreateSemaphore(NULL, MAX_MESSAGE_BUFFER_SIZE, MAX_MESSAGE_BUFFER_SIZE,
						SharedMemoryConstants::SEM_SERVER_EMPTY.c_str());
	if (hServerSemEmpty == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("Error while trying to create ServerSemaphore empty");
	}

	hServerSemFilled = CreateSemaphore(NULL, 0, MAX_MESSAGE_BUFFER_SIZE,
						SharedMemoryConstants::SEM_SERVER_FILLED.c_str());
	if (hServerSemFilled == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("Error while trying to create ServerSemaphore filled");
	}

	hClientSemEmpty = CreateSemaphore(NULL, MAX_MESSAGE_BUFFER_SIZE, MAX_MESSAGE_BUFFER_SIZE,
						SharedMemoryConstants::SEM_CLIENT_EMPTY.c_str());
	if (hClientSemEmpty == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("Error while trying to create ClientSemaphore empty");
	}


	hClientSemFilled = CreateSemaphore(NULL, 0, MAX_MESSAGE_BUFFER_SIZE,
						SharedMemoryConstants::SEM_CLIENT_FILLED.c_str());
	if (hClientSemFilled == NULL)
	{
		this->~SharedMemoryManager();
		throw TEXT("Error while trying to create ClientSemaphore filled");
	}
}

//TODO: add constants to size of objects and check this out
//TODO: remove these for autos and replace them by MEMSET
void SharedMemoryManager::initGameData() {
	//Ball Init
	for (auto &x : viewGameData->balls) {
		x.active = false;
		x.posX = 0;
		x.posY = 0;
		x.width = 0;
		x.height = 0;
	}

	//Tile Init
	for (auto &x : viewGameData->tiles) {
		x.active = false;
		x.bonus = false;
		x.resistance = 0;
		x.posX = 0;
		x.posY = 0;
		x.width = 0;
		x.height = 0;
	}

	//Bonuses init
	for (auto &x : viewGameData->bonuses) {
		x.active = false;
		x.type = 0;
		x.posX = 0;
		x.posY = 0;
		x.width = 0;
		x.height = 0;
	}

	//Player init
	for (auto &x : viewGameData->players) {
		x.active = false;
		x.lives = 0;
		x.points = 0;
		x.posX = 0;
		x.posY = 0;
		x.width = 0;
		x.height = 0;
	}
}