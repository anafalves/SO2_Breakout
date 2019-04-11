#pragma once
#include <tchar.h>
#include "GameData.h"
#include "UnicodeConfigs.h"

enum ServerMessages {
	ACCEPT = 0,
	DENY_USERNAME,
	DENY_SERVER_FULL,
	GAMEDATA,
};

enum ClientMessages {
	LOGIN = 0,
	TOP10,
	LEAVE,
	READY
};

enum MessageConstants {
	MAX_NAME_LENGHT = 50,
	MAX_MESSAGE_BUFFER_SIZE = 20
};

typedef union {
	TCHAR name[MAX_NAME_LENGHT];
	GameData gameData;
}Message;

typedef struct {
	int type;
	Message message;
}Carrier;

typedef struct {
	Carrier buffer[MAX_MESSAGE_BUFFER_SIZE];
	int in, out;
}MessageBuffer;