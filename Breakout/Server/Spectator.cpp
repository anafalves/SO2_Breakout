#include "Spectator.h"

Spectator::Spectator(int id, HANDLE myHandle)
	:spectatorId(id)
{
	hUpdateFlag = myHandle;
}

Spectator::~Spectator()
{
	CloseHandle(hUpdateFlag);
}

int Spectator::getId() const {
	return spectatorId;
}

HANDLE Spectator::getFlag() const {
	return hUpdateFlag;
}
