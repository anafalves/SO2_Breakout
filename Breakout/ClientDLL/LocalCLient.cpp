#include "LocalCLient.h"

bool LocalCLient::login(TCHAR name[MAX_NAME_LENGHT])
{
	ClientMsg msg = { LOGIN,*name };
	ServerMsg answer;

	sendMessage(msg);

	answer = receiveLoginAnswer(name);
	if (answer.type == ACCEPT) {
		setClientID(answer.id_receiver);
		return true;
	}//TODO: return aproprietly the server answer
	return false;
}


void LocalCLient::sendMessage(ClientMsg msg)
{
	WaitForSingleObject(sharedMemmoryContent.hServerSemEmpty, INFINITE);
	WaitForSingleObject(sharedMemmoryContent.hClientWriteMutex, INFINITE);

	sharedMemmoryContent.viewClientBuffer->buffer[sharedMemmoryContent.viewClientBuffer->write_pos] = msg;
	sharedMemmoryContent.viewClientBuffer->write_pos++;
	sharedMemmoryContent.viewClientBuffer->write_pos = sharedMemmoryContent.viewClientBuffer->write_pos % MAX_MESSAGE_BUFFER_SIZE;

	ReleaseSemaphore(sharedMemmoryContent.hServerSemFilled, 1, NULL);
	ReleaseMutex(sharedMemmoryContent.hClientWriteMutex);
}

ServerMsg LocalCLient::receiveMessage() {
	ServerMsg msg;

	WaitForSingleObject(sharedMemmoryContent.hServerSemFilled, INFINITE);
	WaitForSingleObject(sharedMemmoryContent.hClientReadMutex,INFINITE);

	if (sharedMemmoryContent.viewServerBuffer->buffer[sharedMemmoryContent.viewServerBuffer->read_pos].id_receiver == getClientID())
	{
		msg = sharedMemmoryContent.viewServerBuffer->buffer[sharedMemmoryContent.viewServerBuffer->read_pos];
		sharedMemmoryContent.viewServerBuffer->read_pos++;
		sharedMemmoryContent.viewServerBuffer->read_pos = sharedMemmoryContent.viewServerBuffer->read_pos % MAX_MESSAGE_BUFFER_SIZE;

		ReleaseSemaphore(sharedMemmoryContent.hServerSemEmpty, 1, NULL);
	}
	else {
		ReleaseSemaphore(sharedMemmoryContent.hClientSemFilled, 1, NULL);
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
		if (sharedMemmoryContent.viewServerBuffer->buffer[sharedMemmoryContent.viewServerBuffer->read_pos].ponctualMsg.receiver == name)
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




