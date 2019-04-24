#pragma once
#include <tchar.h>
#include "GameData.h"
#include "UnicodeConfigs.h"
#include "Top10Manager.h"

enum ServerMessages {
	INVALID = 0, //the msg is for other person
	ACCEPT,
	DENY_USERNAME,
	DENY_SERVER_FULL,
	CLOSE,
	TOP10,
	//GAMEDATA,
};

enum ClientMessages {
	LOGIN = 0,
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
	int type;
	int id_receiver;
	BufferContent content;
}ClientMsg;

typedef struct {
	ClientMsg buffer[MAX_MESSAGE_BUFFER_SIZE];
	int read_pos, write_pos;
}ClientMsgBuffer;

typedef union{
	TCHAR receiver[MAX_NAME_LENGHT]; // for the login instance
	Top10 top10;
}PonctualMsg;

typedef struct {
	int type;
	int id_receiver;
	PonctualMsg ponctualMsg;
}ServerMsg;

typedef struct {
	ServerMsg buffer[MAX_MESSAGE_BUFFER_SIZE];
	int read_pos, write_pos;
}ServerMsgBuffer;