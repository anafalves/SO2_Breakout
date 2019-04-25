#include "LocalCLient.h"

bool LocalCLient::login(TCHAR * name)
{
	ClientMsg msg;
	ServerResult answer;

	msg.type = LOGIN;
	_tcscpy_s(msg.message.name, name);

	sendMessage(msg);

	answer = receiveLoginAnswer(name);
	if (answer.msg.type == ACCEPT) {
		setClientID(answer.msg.id);
		return true;
	}//TODO: return the server answer aproprietly, no server, server full, change name
	return false;
}

GameData LocalCLient::receiveBroadcast() {
	HANDLE update[2];
	update[0] = sharedMemmoryContent.hUpdateEvent;
	update[1] = sharedMemmoryContent.hExitEvent;

	WaitForMultipleObjects(2, update, false, INFINITE);
	return (*sharedMemmoryContent.viewGameData);
}

void LocalCLient::sendMessage(ClientMsg msg)
{
	long n = 0;
	WaitForSingleObject(sharedMemmoryContent.hClientSemEmpty, INFINITE);
	WaitForSingleObject(sharedMemmoryContent.hClientWriteMutex, INFINITE);

	sharedMemmoryContent.viewClientBuffer->buffer[sharedMemmoryContent.viewClientBuffer->write_pos] = msg;
	sharedMemmoryContent.viewClientBuffer->write_pos++;
	sharedMemmoryContent.viewClientBuffer->write_pos = sharedMemmoryContent.viewClientBuffer->write_pos % MAX_MESSAGE_BUFFER_SIZE;

	ReleaseSemaphore(sharedMemmoryContent.hClientSemFilled, 1, &n);
	ReleaseMutex(sharedMemmoryContent.hClientWriteMutex);
	tcout << "N: " << n; //TODO: Remove
}

ServerMsg LocalCLient::receiveMessage() {
	ServerMsg msg;

	WaitForSingleObject(sharedMemmoryContent.hServerSemFilled, INFINITE);
	WaitForSingleObject(sharedMemmoryContent.hClientReadMutex,INFINITE);

	if (sharedMemmoryContent.viewServerBuffer->buffer[sharedMemmoryContent.viewServerBuffer->read_pos].id == getClientID())
	{
		msg = sharedMemmoryContent.viewServerBuffer->buffer[sharedMemmoryContent.viewServerBuffer->read_pos];
		sharedMemmoryContent.viewServerBuffer->read_pos++;
		sharedMemmoryContent.viewServerBuffer->read_pos = sharedMemmoryContent.viewServerBuffer->read_pos % MAX_MESSAGE_BUFFER_SIZE;

		ReleaseSemaphore(sharedMemmoryContent.hServerSemEmpty, 1, NULL);
	}
	else {
		ReleaseSemaphore(sharedMemmoryContent.hServerSemFilled, 1, NULL);
		msg.type = INVALID;
	}
	ReleaseMutex(sharedMemmoryContent.hClientReadMutex);
	return msg;
}

ServerResult resultFeedBack(ServerResult result, tstring txt) {
	result.errorMsg = txt;
	result.msg.type = INVALID;
	return result;
}

ServerResult LocalCLient::receiveLoginAnswer(tstring name)
{
	ServerResult result;
	int timeOut = 3000;
	DWORD dWaitResult;

	while (CONTINUE) {
		dWaitResult = WaitForSingleObject(sharedMemmoryContent.hServerSemFilled, timeOut);
		if (dWaitResult == WAIT_TIMEOUT) {
			return resultFeedBack(result, TEXT("Filled semaphore wait timed out"));
		}
		dWaitResult = WaitForSingleObject(sharedMemmoryContent.hClientReadMutex, timeOut);
		if (dWaitResult == WAIT_TIMEOUT) {
			return resultFeedBack(result, TEXT("Reading mutex wait timed out"));
		}

		if (sharedMemmoryContent.viewServerBuffer->buffer[sharedMemmoryContent.viewServerBuffer->read_pos].message.receiver == name)
		{
			result.msg = sharedMemmoryContent.viewServerBuffer->buffer[sharedMemmoryContent.viewServerBuffer->read_pos];
			sharedMemmoryContent.viewServerBuffer->read_pos++;
			sharedMemmoryContent.viewServerBuffer->read_pos = sharedMemmoryContent.viewServerBuffer->read_pos % MAX_MESSAGE_BUFFER_SIZE;

			if (!ReleaseSemaphore(sharedMemmoryContent.hServerSemEmpty, 1, NULL)) {
				result.errorMsg = TEXT("Release Empty Server Semaphore error.");//TODO: check if its more appropriate change msg flag to invalid
				return result;
			}

			if (!ReleaseMutex(sharedMemmoryContent.hClientReadMutex)) {
				result.errorMsg = TEXT("Release Reading Mutex error."); //TODO: check if its more appropriate change msg flag to invalid
				return result;
			}

			if (!ReleaseSemaphore(sharedMemmoryContent.hClientSemFilled, 1, NULL))
			{
				return resultFeedBack(result, TEXT("Release Filled Server Semaphore error."));
			}

			if (!ReleaseMutex(sharedMemmoryContent.hClientReadMutex)) {
				return resultFeedBack(result, TEXT("Release Reading Mutex error."));
			}
		}
	}
	return result;
}

LocalCLient * getClientInstance() {
	return new LocalCLient();
}


