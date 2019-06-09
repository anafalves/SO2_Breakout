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
	Client(std::tstring pname, Player * p);

	virtual HANDLE & getPrimaryHandle() = 0;
	int getId() const;
	tstring getName() const;
	bool isInGame() const;
	void setInGame(bool status);
	Player * getPlayer();

	virtual void sendUpdate() = 0;
	virtual tstring getAsString();

	virtual ~Client();
};