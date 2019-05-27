#include "LocalCLient.h"

bool LocalCLient::login(TCHAR * name)
{
	ClientMsg msg;
	ServerMsg answer;

	msg.type = LOGIN;
	_tcscpy_s(msg.message.name, name);

	sendMessage(msg);

	answer = receiveMessageWithTimeout();
	if (answer.type == ACCEPT) {
		setClientID(answer.id);
		setUpdateId(answer.message.update_id);

		if (sharedMemmoryContent.getUpdateFlag(update_id) == false) {
			msg.type = LEAVE;
			sendMessage(msg);
			return false;
		}

		return true;
	}//TODO: return the server answer aproprietly, no server, server full, change name
	return false;
}

GameData LocalCLient::receiveBroadcast() {
	HANDLE update[2];
	GameData data;

	update[0] = sharedMemmoryContent.hUpdateEvent;
	update[1] = sharedMemmoryContent.hExitEvent;

	WaitForMultipleObjects(2, update, FALSE, INFINITE);
	data = (*sharedMemmoryContent.viewGameData);
	SetEvent(sharedMemmoryContent.hReadyForUpdate);
	
	return data;
}

bool LocalCLient::sendMessage(ClientMsg msg)
{
	HANDLE write[2];
	HANDLE empty[2];

	write[0] = sharedMemmoryContent.hClientSemEmpty;
	write[1] = sharedMemmoryContent.hExitEvent;
	empty[0] = sharedMemmoryContent.hClientWriteMutex;
	empty[1] = sharedMemmoryContent.hExitEvent;

	WaitForMultipleObjects(2, empty, FALSE, INFINITE);
	WaitForMultipleObjects(2, write, FALSE, INFINITE);

	sharedMemmoryContent.viewClientBuffer->buffer[sharedMemmoryContent.viewClientBuffer->write_pos] = msg;
	sharedMemmoryContent.viewClientBuffer->write_pos = sharedMemmoryContent.viewClientBuffer->write_pos + 1;
	sharedMemmoryContent.viewClientBuffer->write_pos = sharedMemmoryContent.viewClientBuffer->write_pos % MAX_MESSAGE_BUFFER_SIZE;

	ReleaseSemaphore(sharedMemmoryContent.hClientSemFilled, 1, NULL);
	ReleaseMutex(sharedMemmoryContent.hClientWriteMutex);

	return true;
}

ServerMsg LocalCLient::receiveMessage() {
	ServerMsg msg;

	HANDLE full[2];
	HANDLE read[2];

	full[0] = sharedMemmoryContent.hServerSemFilled;
	full[1] = sharedMemmoryContent.hExitEvent;
	read[0] = sharedMemmoryContent.hClientReadMutex;
	read[1] = sharedMemmoryContent.hExitEvent;

	WaitForMultipleObjects(2, full, FALSE, INFINITE);
	WaitForMultipleObjects(2, read, FALSE, INFINITE);

	msg = sharedMemmoryContent.viewServerBuffer->buffer[sharedMemmoryContent.viewServerBuffer->read_pos];
	if (msg.id == getClientID()) {
		sharedMemmoryContent.viewServerBuffer->read_pos++;
		sharedMemmoryContent.viewServerBuffer->read_pos = sharedMemmoryContent.viewServerBuffer->read_pos % MAX_MESSAGE_BUFFER_SIZE;
		
		ReleaseMutex(sharedMemmoryContent.hClientReadMutex);
		ReleaseSemaphore(sharedMemmoryContent.hServerSemEmpty, 1, NULL);
	}
	else {
		ReleaseMutex(sharedMemmoryContent.hClientReadMutex);
		ReleaseSemaphore(sharedMemmoryContent.hClientSemFilled, 1, NULL);
	}

	return msg;
}

ServerMsg LocalCLient::receiveMessageWithTimeout()
{
	ServerMsg msg;

	int timeOut = 3000;
	DWORD dWaitResult;

	HANDLE full[2];
	HANDLE read[2];

	full[0] = sharedMemmoryContent.hServerSemFilled;
	full[1] = sharedMemmoryContent.hExitEvent;

	read[0] = sharedMemmoryContent.hClientReadMutex;
	read[1] = sharedMemmoryContent.hExitEvent;

	dWaitResult = WaitForMultipleObjects(2, full, FALSE, timeOut);
	if (dWaitResult == WAIT_TIMEOUT) {
		msg.type = TIMEDOUT;
		return msg;
	}

	WaitForMultipleObjects(2, read, FALSE, INFINITE);

	msg = sharedMemmoryContent.viewServerBuffer->buffer[sharedMemmoryContent.viewServerBuffer->read_pos];
	if (msg.id == getClientID()) {
		sharedMemmoryContent.viewServerBuffer->read_pos++;
		sharedMemmoryContent.viewServerBuffer->read_pos = sharedMemmoryContent.viewServerBuffer->read_pos % MAX_MESSAGE_BUFFER_SIZE;

		ReleaseMutex(sharedMemmoryContent.hClientReadMutex);
		ReleaseSemaphore(sharedMemmoryContent.hServerSemEmpty, 1, NULL);
	}
	else {
		ReleaseMutex(sharedMemmoryContent.hClientReadMutex);
		ReleaseSemaphore(sharedMemmoryContent.hClientSemFilled, 1, NULL);
	}

	return msg;
}

void LocalCLient::setUpdateId(int id) {
	update_id = id;
}

int LocalCLient::getUpdateId() const {
	return update_id;
}