#pragma once
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <string>

#include "UnicodeConfigs.h"
#include "GameData.h"

class Client
{
private:
	static int idCounter;
	const int id;
	Player * const player;

	std::tstring name;
	bool inGame;

public:
	Client(std::tstring pname, Player * p)
		:name(pname), player(p), id(idCounter++)
	{};

	std::tstring getName() const {
		return name;
	}

	bool isInGame() const {
		return inGame;
	}

	void setInGame(bool status) {
		inGame = status;
	}

	~Client() {
		player->active = false;
		player->id = -1;
	};

	virtual void sendMessage() = 0;
	//virtual ClientMsg receiveMessage() = 0;
};

