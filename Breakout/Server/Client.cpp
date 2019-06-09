#include "Client.h"

int Client::idCounter = 1;

Client::Client(tstring pname, Player * p)
	:name(pname), player(p), id(idCounter++)
{
	p->active = true;
	p->id = id;
	_tcscpy_s(p->name, pname.c_str());
};

int Client::getId() const {
	return id;
}

tstring Client::getName() const {
	return name;
}

bool Client::isInGame() const {
	return inGame;
}

void Client::setInGame(bool status) {
	inGame = status;
}

Player * Client::getPlayer() {
	return player;
}

tstring Client::getAsString() {
	tstringstream tss;

	tss << getId() << TEXT(" :: ") << getName() << TEXT(" :: ");
	
	if (isInGame()) {
		tss << TEXT("In-Game") << endl;
	}
	else {
		tss << TEXT("In-Lobby") << endl;
	}

	return tss.str();
}

Client::~Client() {
	player->active = false;
}