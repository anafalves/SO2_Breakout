#pragma once
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <string>

#include "UnicodeConfigs.h"
#include "Messages.h"
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
	{
		p->active = true;
		p->id = id;
		_tcscpy_s(p->name, pname.c_str());
	};

	int getId() const {
		return id;
	}

	std::tstring getName() const {
		return name;
	}

	bool isInGame() const {
		return inGame;
	}

	void setInGame(bool status) {
		inGame = status;
	}

	Player * getPlayer() {
		return player;
	}

	virtual void sendUpdate() {};

	virtual tstring getAsString();

	~Client() {
		player->active = false;
	};
};