#include "RemoteClient.h"
#include "Communication.h"

RemoteClient::RemoteClient()
	:Client()
{
	hPipeGameData = INVALID_HANDLE_VALUE;
	hPipeMessage = INVALID_HANDLE_VALUE;

	ipAddress = new tstring();

	hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hExitEvent == NULL) {
		throw EXCEPTION_ACCESS_VIOLATION;
	}

	hWriteReady = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hWriteReady == INVALID_HANDLE_VALUE) {
		throw EXCEPTION_ACCESS_VIOLATION;
	}
	
	hReadReady = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hReadReady == INVALID_HANDLE_VALUE) {
		throw EXCEPTION_ACCESS_VIOLATION;
	}
	
	hReadGameDataReady = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hReadGameDataReady == INVALID_HANDLE_VALUE) {
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
	HANDLE writeReady[2] = { hWriteReady, hExitEvent };
	OVERLAPPED flag = { 0 };
	DWORD nBytes = 0;

	ResetEvent(hWriteReady);
	flag.hEvent = hWriteReady;

	WriteFile(hPipeMessage, &message, sizeof(ClientMsg), &nBytes, &flag);
	WaitForMultipleObjects(2, writeReady, FALSE, INFINITE);

	GetOverlappedResult(hPipeMessage, &flag, &nBytes, FALSE);
	if (nBytes != sizeof(ClientMsg) || GetLastError() == ERROR_BROKEN_PIPE) {
		return false;
	}

	return true;
}

ServerMsg RemoteClient::receiveMessage() {
	HANDLE readReady[2] = { hReadReady, hExitEvent };
	OVERLAPPED flag = { 0 };
	DWORD nBytes = 0;
	ServerMsg response;

	ResetEvent(hReadReady);
	flag.hEvent = hReadReady;

	ReadFile(hPipeMessage, &response, sizeof(ServerMsg), &nBytes, &flag);
	WaitForMultipleObjects(2, readReady, FALSE, INFINITE);

	GetOverlappedResult(hPipeMessage, &flag, &nBytes, FALSE);
	if (nBytes != sizeof(ServerMsg) || GetLastError() == ERROR_BROKEN_PIPE) {
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
		return CONNECTION_ERROR;
	}

	response = receiveMessage();
	if (response.type == DENY_SERVER_FULL) {
		return SERVER_FULL;
	}
	else if (response.type == DENY_USERNAME) {
		return INVALID_USERNAME;
	}

	if (!connectToGameDataPipe(name)) {
		FlushFileBuffers(hPipeMessage);
		DisconnectNamedPipe(hPipeMessage);
		CloseHandle(hPipeMessage);
		return CONNECTION_TIMED_OUT;
	}

	return CONNECTED;
}

GameData RemoteClient::receiveBroadcast() {
	HANDLE readReady[2] = { hReadGameDataReady, hExitEvent };
	OVERLAPPED flag = { 0 };
	DWORD nBytes = 0;
	GameData data;

	ResetEvent(hReadGameDataReady);
	flag.hEvent = hReadGameDataReady;

	ReadFile(hPipeGameData, &data, sizeof(GameData), &nBytes, &flag);
	WaitForMultipleObjects(2, readReady, FALSE, INFINITE);

	GetOverlappedResult(hPipeGameData, &flag, &nBytes, FALSE);
	if (nBytes != sizeof(GameData) || GetLastError() == ERROR_BROKEN_PIPE) {
		data.gameState = -1;
	}

	return data;
}

RemoteClient::~RemoteClient() {
	//TODO: send exit message if pipe is open

	delete ipAddress;
	SetEvent(hExitEvent);
	FlushFileBuffers(hPipeMessage);
	FlushFileBuffers(hPipeGameData);
	DisconnectNamedPipe(hPipeMessage);
	DisconnectNamedPipe(hPipeGameData);
	CloseHandle(hPipeMessage);
	CloseHandle(hPipeGameData);
	CloseHandle(hWriteReady);
	CloseHandle(hReadReady);
	CloseHandle(hReadGameDataReady);
}