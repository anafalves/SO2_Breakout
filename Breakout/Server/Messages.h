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
	TOP10 = 100,
	//GAMEDATA,
};

enum ClientMessages {
	LOGIN = 0,
	LEAVE,
	MOVE,
	PRECISE_MOVE,
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

//Pipe Messages Server -> Client
typedef union {
	TCHAR name[MAX_NAME_LENGHT];
	Top10 top10;
	GameData gameData;
}Message;

typedef struct {
	int type;
	int id;
	Message message;
}Carrier;
//End

//Pipe and Local Client -> Server messages
typedef struct {
	int x;
	int y;
}PreciseMove;

typedef union {
	TCHAR name[MAX_NAME_LENGHT];
	PreciseMove preciseMove;
	int basicMove;
}ClientRequest;

typedef struct{
	int type;
	int id;
	ClientRequest message;
}ClientMsg;
//end

//Server -> local client
typedef union{
	TCHAR receiver[MAX_NAME_LENGHT]; // for the login instance
	Top10 top10;
}ServerResponse;

typedef struct {
	int type;
	int id;
	ServerResponse message;
}ServerMsg;
//end

typedef struct {
	ClientMsg buffer[MAX_MESSAGE_BUFFER_SIZE];
	int read_pos, write_pos;
}ClientMsgBuffer;

typedef struct {
	ServerMsg buffer[MAX_MESSAGE_BUFFER_SIZE];
	int read_pos, write_pos;
}ServerMsgBuffer;