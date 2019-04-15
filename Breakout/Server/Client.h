#pragma once
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <string>
#include "UnicodeConfigs.h"

using namespace std;

enum ClientStatus {
	LOBBY = 0,
	IN_GAME
};

class Client
{
private:
	HANDLE hPipe;
	HANDLE hThread;
	//TODO: add pointer to their object in shared memory
	int STATUS;

public:
	Client();
	~Client();
	bool sendMessage();
	//myMessagexpto receiveMessage();

};

