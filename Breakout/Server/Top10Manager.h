#pragma once
#include <Windows.h>
#include <tchar.h>
#include <string>
#include <sstream>

#include "GameData.h"
#include "UnicodeConfigs.h"

using namespace std;

typedef struct {
	TCHAR username[50];
	int points;
}Place;

typedef struct {
	Place position[10];
}Top10;


class Top10Manager
{
private:
	Top10 top10;

public:
	int loadTop10();
	int saveTop10();
	void addPlayer(Player & player);

	Top10 getTop10() const {
		return top10;
	}

	tstring getAsString();
};

