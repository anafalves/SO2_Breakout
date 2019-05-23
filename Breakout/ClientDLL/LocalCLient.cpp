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
		return true;
	}//TODO: return the server answer aproprietly, no server, server full, change name
	return false;
}

GameData LocalCLient::receiveBroadcast() {
	//HANDLE update[2];
	//update[0] = sharedMemmoryContent.hUpdateEvent;
	//update[1] = sharedMemmoryContent.hExitEvent; //TODO: make this verification outside of here, so that the client can quit. or something else

	//WaitForMultipleObjects(2, update, false, INFINITE);
	WaitForSingleObject(sharedMemmoryContent.hUpdateEvent, INFINITE);
	return (*sharedMemmoryContent.viewGameData);
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

	HANDLE read[2];
	HANDLE full[2];

	write[0] = sharedMemmoryContent.hClientSemFilled;
	write[1] = sharedMemmoryContent.hExitEvent;
	empty[0] = sharedMemmoryContent.hClientReadMutex;
	empty[1] = sharedMemmoryContent.hExitEvent;

	WaitForSingleObject(sharedMemmoryContent.hServerSemFilled, INFINITE);

	msg = sharedMemmoryContent.viewServerBuffer->buffer[sharedMemmoryContent.viewServerBuffer->read_pos];
	sharedMemmoryContent.viewServerBuffer->read_pos++;
	sharedMemmoryContent.viewServerBuffer->read_pos = sharedMemmoryContent.viewServerBuffer->read_pos % MAX_MESSAGE_BUFFER_SIZE;

	ReleaseSemaphore(sharedMemmoryContent.hServerSemEmpty, 1, NULL);

	return msg;
}

ServerMsg LocalCLient::receiveMessageWithTimeout()
{
	ServerMsg result;
	int timeOut = 3000;
	DWORD dWaitResult;

	dWaitResult = WaitForSingleObject(sharedMemmoryContent.hServerSemFilled, timeOut);
	if (dWaitResult == WAIT_TIMEOUT) {
		result.type = TIMEDOUT;
		return result;
	}

	result = sharedMemmoryContent.viewServerBuffer->buffer[sharedMemmoryContent.viewServerBuffer->read_pos];
	sharedMemmoryContent.viewServerBuffer->read_pos++;
	sharedMemmoryContent.viewServerBuffer->read_pos = sharedMemmoryContent.viewServerBuffer->read_pos % MAX_MESSAGE_BUFFER_SIZE;

	ReleaseSemaphore(sharedMemmoryContent.hServerSemEmpty, 1, NULL);

	return result;
}


