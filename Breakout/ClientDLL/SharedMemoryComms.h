#pragma once
#include "SharedMemoryManager.h"

class SharedMemoryComms {
private:
	
public:
	void Login(string username);
	void ReceiveBroadcast();
	void SendMessage(string msg);
	void ReceiveMessage();


	SharedMemoryComms();
	~SharedMemoryComms();

};