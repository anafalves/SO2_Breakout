#include "Client.h"

int Client::idCounter = 1;

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