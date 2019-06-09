#include "SharedMemoryManager.h"

SharedMemoryManager::SharedMemoryManager()
{
	ready = true;
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
	CloseHandle(hClientReadMutex);
	CloseHandle(hClientBuffer);
	CloseHandle(hServerBuffer);
	CloseHandle(hGameData);
	CloseHandle(hExitEvent);
	CloseHandle(hReadyForUpdate);

	CloseHandle(hUpdateEvent);
	CloseHandle(hReadyForUpdate);
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
		ready = false;
	}

	viewServerBuffer = (ServerMsgBuffer *)MapViewOfFile(hServerBuffer, FILE_MAP_READ | FILE_MAP_WRITE,
		0, 0, (SIZE_T)size.QuadPart);
	if (viewServerBuffer == NULL)
	{
		tcout << "couldn't create map view of file for server messageBuffer!";
		this->~SharedMemoryManager();
		ready = false;
	}

	//Local Client Message Buffer
	size.QuadPart = sizeof(ClientMsgBuffer);

	hClientBuffer = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,NULL,
		SharedMemoryConstants::SHA_MEM_CLIENT_BUFFER.c_str());

	if (hClientBuffer == NULL)
	{
		this->~SharedMemoryManager();
		ready = false;
	}

	viewClientBuffer = (ClientMsgBuffer *)MapViewOfFile(hClientBuffer, FILE_MAP_READ | FILE_MAP_WRITE,
		0, 0, (SIZE_T)size.QuadPart);
	if (viewClientBuffer == NULL)
	{
		this->~SharedMemoryManager();
		ready = false;
	}


	//GameData Shared memory
	size.QuadPart = sizeof(GameData);
	hGameData = OpenFileMapping(FILE_MAP_READ, NULL,
		SharedMemoryConstants::SHA_MEM_GAMEDATA.c_str());

	if (hGameData == NULL)
	{
		this->~SharedMemoryManager();
		ready = false;
	}

	viewGameData = (GameData *)MapViewOfFile(hGameData, FILE_MAP_READ,
		0, 0, (SIZE_T)size.QuadPart);
	if (viewGameData == NULL)
	{
		this->~SharedMemoryManager();
		ready = false;
	}
}

void SharedMemoryManager::initSyncVariables() {
	hServerSemEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false,
		SharedMemoryConstants::SEM_SERVER_EMPTY.c_str());
	if (hServerSemEmpty == NULL)
	{
		this->~SharedMemoryManager();
		ready = false;
	}

	hServerSemFilled = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false,
		SharedMemoryConstants::SEM_SERVER_FILLED.c_str());
	if (hServerSemFilled == NULL)
	{
		this->~SharedMemoryManager();
		ready = false;
	}

	hClientSemEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false,
		SharedMemoryConstants::SEM_CLIENT_EMPTY.c_str());
	if (hClientSemEmpty == NULL)
	{
		this->~SharedMemoryManager();
		ready = false;
	}

	hClientSemFilled = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false,
		SharedMemoryConstants::SEM_CLIENT_FILLED.c_str());
	if (hClientSemFilled == NULL)
	{
		this->~SharedMemoryManager();
		ready = false;
	}

	hExitEvent = CreateEvent(NULL, false, false, NULL);
	if (hExitEvent == NULL)
	{
		this->~SharedMemoryManager();
		ready = false;
	}

	hClientWriteMutex = CreateMutex(NULL, FALSE, SharedMemoryConstants::MUT_CLI_WRITE.c_str());
	if (hClientWriteMutex == NULL) {
		this->~SharedMemoryManager();
		ready = false;
	}

	hClientReadMutex = CreateMutex(NULL, FALSE, SharedMemoryConstants::MUT_CLI_READ.c_str());
	if (hClientReadMutex == NULL) {
		this->~SharedMemoryManager();
		ready = false;
	}
}


bool SharedMemoryManager::getResourceReadyNotifier(tstring name)
{
	tstring clientReady(SharedMemoryConstants::EVENT_CLIENT_NOTIFICATION + name);
	tstring updateReady(SharedMemoryConstants::EVENT_UPDATE + name);

	tcout << "update: " << updateReady << endl;
	tcout << "cli: " << clientReady << endl;

	hReadyForUpdate = OpenEvent(EVENT_ALL_ACCESS, NULL, clientReady.c_str());
	if (hReadyForUpdate == NULL) {
		ready = false;
		return false;
	}
	
	hUpdateEvent = OpenEvent(EVENT_ALL_ACCESS, NULL, updateReady.c_str());
	if (hUpdateEvent == NULL) {
		CloseHandle(hReadyForUpdate);
		ready = false;
		return false;
	}

	return true;
}

bool SharedMemoryManager::isReady() const
{
	return ready;
}