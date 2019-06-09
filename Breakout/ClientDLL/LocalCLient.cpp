#include "LocalCLient.h"
#include "Communication.h"

int LocalCLient::login(TCHAR * name)
{
	ClientMsg msg;
	ServerMsg answer;

	msg.type = LOGIN;
	_tcscpy_s(msg.message.name, name);

	sendMessage(msg);
	answer = receiveMessageLogin(name);

	if (answer.type == ACCEPT) {
		setClientID(answer.id);

		if (sharedMemmoryContent->getResourceReadyNotifier(name) == false) {
			msg.id = getClientID();
			msg.type = LEAVE;
			sendMessage(msg);
			return CONNECTION_ERROR;
		}

		return CONNECTED;
	}
	else if (answer.type == DENY_SERVER_FULL) {
		return SERVER_FULL;
	}
	else if (answer.type == DENY_USERNAME) {
		return INVALID_USERNAME;
	}
	else 
		return CONNECTION_TIMED_OUT;
}

GameData LocalCLient::receiveBroadcast() {
	HANDLE update[2];

	update[0] = sharedMemmoryContent->hUpdateEvent;
	update[1] = sharedMemmoryContent->hExitEvent;

	SetEvent(sharedMemmoryContent->hReadyForUpdate);

	WaitForMultipleObjects(2, update, FALSE, INFINITE);

	return (*sharedMemmoryContent->viewGameData);
	
}

bool LocalCLient::sendMessage(ClientMsg msg)
{
	HANDLE write[2];
	HANDLE empty[2];

	write[0] = sharedMemmoryContent->hClientWriteMutex;
	write[1] = sharedMemmoryContent->hExitEvent;

	empty[0] = sharedMemmoryContent->hClientSemEmpty;
	empty[1] = sharedMemmoryContent->hExitEvent;

	WaitForMultipleObjects(2, empty, FALSE, INFINITE);
	WaitForMultipleObjects(2, write, FALSE, INFINITE);

	sharedMemmoryContent->viewClientBuffer->buffer[sharedMemmoryContent->viewClientBuffer->write_pos] = msg;
	sharedMemmoryContent->viewClientBuffer->write_pos = sharedMemmoryContent->viewClientBuffer->write_pos + 1;
	sharedMemmoryContent->viewClientBuffer->write_pos = sharedMemmoryContent->viewClientBuffer->write_pos % MAX_MESSAGE_BUFFER_SIZE;

	ReleaseSemaphore(sharedMemmoryContent->hClientSemFilled, 1, NULL);
	ReleaseMutex(sharedMemmoryContent->hClientWriteMutex);

	return true;
}

ServerMsg LocalCLient::receiveMessage() {
	ServerMsg msg;

	HANDLE full[2];
	HANDLE read[2];

	full[0] = sharedMemmoryContent->hServerSemFilled;
	full[1] = sharedMemmoryContent->hExitEvent;
	read[0] = sharedMemmoryContent->hClientReadMutex;
	read[1] = sharedMemmoryContent->hExitEvent;

	while (true) {
		WaitForMultipleObjects(2, full, FALSE, INFINITE);
		WaitForMultipleObjects(2, read, FALSE, INFINITE);

		msg = sharedMemmoryContent->viewServerBuffer->buffer[sharedMemmoryContent->viewServerBuffer->read_pos];
		if (msg.id == getClientID()) {
			sharedMemmoryContent->viewServerBuffer->read_pos++;
			sharedMemmoryContent->viewServerBuffer->read_pos = sharedMemmoryContent->viewServerBuffer->read_pos % MAX_MESSAGE_BUFFER_SIZE;

			ReleaseMutex(sharedMemmoryContent->hClientReadMutex);
			ReleaseSemaphore(sharedMemmoryContent->hServerSemEmpty, 1, NULL);
			break;
		}
		else {
			ReleaseMutex(sharedMemmoryContent->hClientReadMutex);
			ReleaseSemaphore(sharedMemmoryContent->hClientSemFilled, 1, NULL);
		}
	}

	return msg;
}

ServerMsg LocalCLient::receiveMessageLogin(tstring name)
{
	ServerMsg msg;

	int timeOut = 3000;
	DWORD dWaitResult;

	HANDLE full[2];
	HANDLE read[2];

	full[0] = sharedMemmoryContent->hServerSemFilled;
	full[1] = sharedMemmoryContent->hExitEvent;

	read[0] = sharedMemmoryContent->hClientReadMutex;
	read[1] = sharedMemmoryContent->hExitEvent;

	while (true) {
		dWaitResult = WaitForMultipleObjects(2, full, FALSE, timeOut);
		if (dWaitResult == WAIT_TIMEOUT) {
			msg.type = TIMEDOUT;
			return msg;
		}

		WaitForMultipleObjects(2, read, FALSE, INFINITE);

		msg = sharedMemmoryContent->viewServerBuffer->buffer[sharedMemmoryContent->viewServerBuffer->read_pos];

		if (name == msg.message.receiver) {
			sharedMemmoryContent->viewServerBuffer->read_pos++;
			sharedMemmoryContent->viewServerBuffer->read_pos = sharedMemmoryContent->viewServerBuffer->read_pos % MAX_MESSAGE_BUFFER_SIZE;

			ReleaseMutex(sharedMemmoryContent->hClientReadMutex);
			ReleaseSemaphore(sharedMemmoryContent->hServerSemEmpty, 1, NULL);
			break;
		}
		else {
			ReleaseMutex(sharedMemmoryContent->hClientReadMutex);
			ReleaseSemaphore(sharedMemmoryContent->hClientSemFilled, 1, NULL);
		}
	}

	return msg;
}

LocalCLient::LocalCLient()
	:Client()
{
	sharedMemmoryContent = new SharedMemoryManager();
}

LocalCLient::~LocalCLient()
{
	delete sharedMemmoryContent;
}

bool LocalCLient::isReady() const
{
	return sharedMemmoryContent->isReady();
}
