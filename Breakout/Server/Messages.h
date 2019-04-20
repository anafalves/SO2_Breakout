#pragma once
#include <tchar.h>
#include "GameData.h"
#include "UnicodeConfigs.h"

enum ServerMessages {
	ACCEPT = 0,
	DENY_USERNAME,
	DENY_SERVER_FULL,
	CLOSE,
	//GAMEDATA,
};

enum ClientMessages {
	LOGIN = 0,
	TOP10,
	LEAVE,
	MOVE,
	//READY
};

enum CLientMoves {
	LEFT = 0,
	RIGHT
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

typedef union {
	TCHAR name[MAX_NAME_LENGHT];
	CLientMoves move;
}BufferContent;

typedef struct{
	ClientMessages msg_type;
	BufferContent content;
}ClientBufferCell;

typedef struct {
	ClientBufferCell cli_buffer[MAX_MESSAGE_BUFFER_SIZE];
	int read_pos, write_pos;
}ClientMsgBuffer;

typedef struct {
	ServerMessages serv_buffer[MAX_MESSAGE_BUFFER_SIZE];
	int read_pos, write_pos;
}ServerMsgBuffer;