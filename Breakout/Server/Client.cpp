#include "Client.h"

int Client::idCounter = 1;

tostream& operator <<(tostream & tos, const Client & cli) {
	tstringstream tss;

	tss << "Client: " << cli.getId() << " :: " << cli.getName() << " :: " << cli.isInGame() << endl;

	tos << tss.str();
	return tos;
}