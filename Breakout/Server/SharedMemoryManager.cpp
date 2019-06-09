#include "SharedMemoryManager.h"
#include "Server.h"

SharedMemoryManager::SharedMemoryManager()
{
	hClientReadyMutex = CreateMutex(NULL, FALSE, NULL);
	if (hClientReadyMutex == NULL) {
		throw EXCEPTION_ACCESS_VIOLATION;
	}
	
	hUpdateMutex = CreateMutex(NULL, FALSE, NULL);
	if (hUpdateMutex == NULL) {
		throw EXCEPTION_ACCESS_VIOLATION;
	}
}

SharedMemoryManager::~SharedMemoryManager()
{
	CloseHandle(hServerSemEmpty);
	CloseHandle(hServerSemFilled);
	CloseHandle(hClientSemEmpty);
	CloseHandle(hClientSemFilled);
	CloseHandle(hExitEvent);

	UnmapViewOfFile(viewClientBuffer);
	UnmapViewOfFile(viewServerBuffer);
	UnmapViewOfFile(viewGameData);

	CloseHandle(hClientBuffer);
	CloseHandle(hServerBuffer);
	CloseHandle(hGameData);

	CloseHandle(hClientReadyMutex);

	for (auto &handle : updateNotifications) {
		CloseHandle(handle);
	}

	for (auto &handle : clientReadyNotifications) {
		CloseHandle(handle);
	}
}

void SharedMemoryManager::setUpdate(HANDLE & handle) const {
	if (handle == NULL || handle == INVALID_HANDLE_VALUE)
		return;
	
	WaitForSingleObject(hUpdateMutex, INFINITE);
	
	SetEvent(handle);

	ReleaseMutex(hUpdateMutex);

}

GameData * SharedMemoryManager::getGameData() const {
	return viewGameData;
}

bool SharedMemoryManager::addNotifiers(HANDLE & clientReady, HANDLE & updateReady, tstring name)
{
	tstring update(SharedMemoryConstants::EVENT_UPDATE + name);
	tstring clientNotif(SharedMemoryConstants::EVENT_CLIENT_NOTIFICATION + name);

	updateReady = CreateEvent(NULL, FALSE, FALSE, update.c_str());
	if (updateReady == INVALID_HANDLE_VALUE) {
		return false;
	}

	clientReady = CreateEvent(NULL, FALSE, FALSE, clientNotif.c_str());
	if (clientReady == INVALID_HANDLE_VALUE) {
		CloseHandle(updateReady);
		return false;
	}

	WaitForSingleObject(hClientReadyMutex, INFINITE);
	clientReadyNotifications.push_back(clientReady);
	ReleaseMutex(hClientReadyMutex);


	WaitForSingleObject(hUpdateMutex, INFINITE);
	updateNotifications.push_back(updateReady);
	ReleaseMutex(hUpdateMutex);

	return true;
}

void SharedMemoryManager::removeNotifiers(HANDLE & updateReady, HANDLE & clientReady)
{
	WaitForSingleObject(hUpdateMutex, INFINITE);

	for (size_t i = 0; i < updateNotifications.size(); i++) {
		if (updateNotifications[i] == updateReady) {
			CloseHandle(updateNotifications[i]);
			updateNotifications.erase(updateNotifications.begin() + i);
			break;
		}
	}

	ReleaseMutex(hUpdateMutex);

	WaitForSingleObject(hClientReadyMutex, INFINITE);

	for (size_t i = 0; i < clientReadyNotifications.size(); i++) {
		if (clientReadyNotifications[i] == clientReady) {
			CloseHandle(clientReadyNotifications[i]);
			clientReadyNotifications.erase(clientReadyNotifications.begin() + i);
			break;
		}
	}

	ReleaseMutex(hClientReadyMutex);
}

void SharedMemoryManager::waitForAllClientsReady()
{
	HANDLE notification[2];
	notification[1] = hExitEvent;

	WaitForSingleObject(hClientReadyMutex, INFINITE);
	for (auto & handle : clientReadyNotifications) {
		notification[0] = handle;

		if (WaitForMultipleObjects(2, notification, FALSE, 3000) == WAIT_TIMEOUT) {
			ReleaseMutex(hClientReadyMutex);
			Server::clients.removeClient(handle);
			return;
		}
	}
	ReleaseMutex(hClientReadyMutex);
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

	viewClientBuffer = (ClientMsgBuffer *)MapViewOfFile(hClientBuffer, FILE_MAP_READ | FILE_MAP_WRITE,
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
	SecureZeroMemory(viewGameData, sizeof(GameData));
	SecureZeroMemory(viewClientBuffer, sizeof(ClientMsgBuffer));
	SecureZeroMemory(viewServerBuffer, sizeof(ServerMsgBuffer));

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

	hExitEvent = CreateEvent(NULL, TRUE, FALSE, SharedMemoryConstants::EXIT_EVENT.c_str());
	if (hExitEvent == NULL)
	{
		this->~SharedMemoryManager();
		return -1;
	}

	return 0;
}

void SharedMemoryManager::writeMessage(ServerMsg message) {
	HANDLE writeMessage[2];

	writeMessage[0] = hServerSemEmpty;
	writeMessage[1] = hExitEvent;

	//Wait for empty slot to place message
	WaitForMultipleObjects(2, writeMessage, FALSE, INFINITE);

	//Write message and update write counter
	viewServerBuffer->buffer[viewServerBuffer->write_pos] = message;
	viewServerBuffer->write_pos = viewServerBuffer->write_pos + 1;
	viewServerBuffer->write_pos = viewServerBuffer->write_pos % MAX_MESSAGE_BUFFER_SIZE;

	//Release 1 filled position for the local clients to read
	ReleaseSemaphore(hServerSemFilled, 1, NULL);
}

ClientMsg SharedMemoryManager::readMessage() {
	ClientMsg message;
	HANDLE clientMessages[2];

	clientMessages[0] = hClientSemFilled;
	clientMessages[1] = hExitEvent;

	//Wait for a message to be available
	WaitForMultipleObjects(2, clientMessages, FALSE, INFINITE);

	//Read buffer and update read_pos, which is our index
	message = viewClientBuffer->buffer[viewClientBuffer->read_pos];
	viewClientBuffer->read_pos = viewClientBuffer->read_pos + 1;
	viewClientBuffer->read_pos = viewClientBuffer->read_pos % MAX_MESSAGE_BUFFER_SIZE;
	
	//Releases 1 empty position to write on the client buffer
	ReleaseSemaphore(hClientSemEmpty, 1, NULL);

	return message;
}