#include "RemoteClient.h"

RemoteClient::RemoteClient()
	:Client()
{
	hPipeGameData = INVALID_HANDLE_VALUE;
	hPipeMessage = INVALID_HANDLE_VALUE;

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
	ipAddress = ipAddr;
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
		0,
		NULL,
		OPEN_EXISTING,
		0,
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
	pipeName += ipAddress;
	pipeName += TEXT("\\");
	pipeName += PipeConstants::GAMEDATA_PIPE_NAME.c_str();
	pipeName += name;

	tcout << "opening pipe:" << pipeName << endl;

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
		0,
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

		tcout << "true!" << endl;
		return true;
	}

	tcout << "failed to create pipe" << endl;
	return false;
}

bool RemoteClient::login(TCHAR * name) {
	ServerMsg response;
	ClientMsg request;

	request.type = LOGIN;
	_tcscpy_s(request.message.name, name);

	if (!sendMessage(request)) {
		return false;
	}

	response = receiveMessage();
	if (response.type != ACCEPT) {
		return false;
	}

	if (!connectToGameDataPipe(name)) {
		return false;
	}

	return true;
}

GameData RemoteClient::receiveBroadcast() {
	DWORD nBytes = 0;
	GameData data;
	bool success = false;

	success = ReadFile(hPipeGameData, &data, sizeof(GameData), &nBytes, NULL);
	if (!success || nBytes != sizeof(GameData) || GetLastError() == ERROR_BROKEN_PIPE) {
		data.balls[0].playerId = -1;
	}

	return data;
}

RemoteClient::~RemoteClient() {
	FlushFileBuffers(hPipeMessage);
	FlushFileBuffers(hPipeGameData);
	DisconnectNamedPipe(hPipeMessage);
	DisconnectNamedPipe(hPipeGameData);
	CloseHandle(hPipeMessage);
	CloseHandle(hPipeGameData);
}