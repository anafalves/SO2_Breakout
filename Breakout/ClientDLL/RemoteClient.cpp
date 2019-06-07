#include "RemoteClient.h"
#include "Communication.h"

RemoteClient::RemoteClient()
	:Client()
{
	hPipeGameData = INVALID_HANDLE_VALUE;
	hPipeMessage = INVALID_HANDLE_VALUE;

	ipAddress = new tstring();

	hExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hExitEvent == NULL) {
		throw EXCEPTION_ACCESS_VIOLATION;
	}

	hWriteMutex = CreateMutex(NULL, FALSE, NULL);
	if (hWriteMutex == NULL) {
		throw EXCEPTION_ACCESS_VIOLATION;
	}
}

bool RemoteClient::isConnected() {
	return hPipeMessage != INVALID_HANDLE_VALUE;
}

bool RemoteClient::connect(TCHAR * ipAddr) {
	*ipAddress = ipAddr;
	bool success = false;
	DWORD dwState = 0;
	tstring pipeName = TEXT("\\\\");
	pipeName += ipAddr;
	pipeName += TEXT("\\");
	pipeName += PipeConstants::MESSAGE_PIPE_NAME.c_str();

	hPipeMessage = CreateFile(
		pipeName.c_str(),
		GENERIC_READ |
		GENERIC_WRITE,
		0 | FILE_SHARE_READ |
		FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0 | FILE_FLAG_OVERLAPPED,
		NULL
	);

	if (hPipeMessage != INVALID_HANDLE_VALUE) {
		dwState = PIPE_READMODE_MESSAGE;

		success = SetNamedPipeHandleState(hPipeMessage, &dwState, NULL, NULL);
		if (!success) {
			DisconnectNamedPipe(hPipeMessage);
			CloseHandle(hPipeMessage);
			return false;
		}
		return true;
	}

	return false;
}

bool RemoteClient::sendMessage(ClientMsg message) {
	HANDLE write[2] = { hWriteMutex, hExitEvent };
	DWORD nBytes = 0;
	bool success = false;
	//TODO: add overlapped IO operations here
	WaitForMultipleObjects(2, write, FALSE, INFINITE);

	success = WriteFile(hPipeMessage, &message, sizeof(ClientMsg), &nBytes, NULL);
	if (!success || nBytes != sizeof(ClientMsg)) {
		return false;
	}

	ReleaseMutex(hWriteMutex);

	return true;
}

ServerMsg RemoteClient::receiveMessage() {
	DWORD nBytes = 0;
	ServerMsg response;
	bool success = false;

	//TODO: add overlapped IO operations here
	success = ReadFile(hPipeMessage, &response, sizeof(ServerMsg), &nBytes, NULL);
	if (!success || nBytes != sizeof(ServerMsg) || GetLastError() == ERROR_BROKEN_PIPE) {
		response.type = -1;
	}

	return response;
}

bool RemoteClient::connectToGameDataPipe(TCHAR * name) {
	bool success = false;
	DWORD dwState = 0;
	tstring pipeName = TEXT("\\\\");
	pipeName += *ipAddress;
	pipeName += TEXT("\\");
	pipeName += PipeConstants::GAMEDATA_PIPE_NAME.c_str();
	pipeName += name;

	if (!WaitNamedPipe(pipeName.c_str(), 5000)) {
		return false;
	}

	hPipeGameData = CreateFile(
		pipeName.c_str(),
		GENERIC_READ |
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0 | FILE_FLAG_OVERLAPPED,
		NULL
	);

	if (hPipeGameData != INVALID_HANDLE_VALUE) {
		dwState = PIPE_READMODE_MESSAGE;

		success = SetNamedPipeHandleState(hPipeGameData, &dwState, NULL, NULL);
		if (!success) {
			DisconnectNamedPipe(hPipeGameData);
			CloseHandle(hPipeGameData);
			return false;
		}

		return true;
	}

	return false;
}

int RemoteClient::login(TCHAR * name) {
	ServerMsg response;
	ClientMsg request;

	request.type = LOGIN;
	_tcscpy_s(request.message.name, name);

	if (!sendMessage(request)) {
		return false;
	}

	response = receiveMessage();
	if (response.type == DENY_SERVER_FULL) {
		return SERVER_FULL;
	}
	else if (response.type == DENY_USERNAME) {
		return INVALID_USERNAME;
	}
	else
		return CONNECTION_TIMED_OUT;

	if (!connectToGameDataPipe(name)) {
		FlushFileBuffers(hPipeMessage);
		DisconnectNamedPipe(hPipeMessage);
		CloseHandle(hPipeMessage);
		return CONNECTION_TIMED_OUT;
	}

	return CONNECTED;
}

GameData RemoteClient::receiveBroadcast() {
	DWORD nBytes = 0;
	GameData data;
	bool success = false;

	//TODO: make this return false or something in case it fails due to pipe.
	success = ReadFile(hPipeGameData, &data, sizeof(GameData), &nBytes, NULL);
	if (!success || nBytes != sizeof(GameData) || GetLastError() == ERROR_BROKEN_PIPE) {
		data.balls[0].playerId = -1; //TODO: set game state here
	}

	return data;
}

RemoteClient::~RemoteClient() {
	delete ipAddress;
	FlushFileBuffers(hPipeMessage);
	FlushFileBuffers(hPipeGameData);
	DisconnectNamedPipe(hPipeMessage);
	DisconnectNamedPipe(hPipeGameData);
	CloseHandle(hPipeMessage);
	CloseHandle(hPipeGameData);
}