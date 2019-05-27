#pragma once
#include <Windows.h>
class Spectator
{
private:
	const int spectatorId;
	HANDLE hUpdateFlag;

public:
	Spectator(int id,  HANDLE updateFlag);
	~Spectator();

	int getId() const;
	HANDLE getFlag() const;
};

