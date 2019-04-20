#pragma once
#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include "UnicodeConfigs.h"

using namespace std;
struct SharedMemoryConstants
{
	static const tstring SHA_MEM_GAMEDATA;
	static const tstring SHA_MEM_CLIENT_BUFFER;
	static const tstring SEM_CLIENT_EMPTY;
	static const tstring SEM_CLIENT_FILLED;

	static const tstring SHA_MEM_SERVER_BUFFER;
	static const tstring SEM_SERVER_EMPTY;
	static const tstring SEM_SERVER_FILLED;
	static const tstring MUT_GAMEDATA_UPDATE;

	static const tstring MUT_CLI_READ;
	static const tstring MUT_CLI_WRITE;
};

