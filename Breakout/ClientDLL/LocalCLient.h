#pragma once
#include "Client.h"
#include "SharedMemoryManager.h"

class LocalCLient :
	public Client
{
private:
	SharedMemoryManager sharedMemmoryContent;

public:
	LocalCLient();
	~LocalCLient()
	{
		sharedMemmoryContent.~SharedMemoryManager();
	};

	bool login(std::tstring name);
	//receiveBroadcast() = 0;
	//sendMessage() = 0;
	//receiveMessage() = 0;

}