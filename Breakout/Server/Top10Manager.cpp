#include "Top10Manager.h"
#include <iostream>

Top10Manager::Top10Manager()
{
	HKEY key;
	DWORD result;
	DWORD size;
	
	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\SO2_Breakout"), 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &result) != ERROR_SUCCESS)
	{
		throw TEXT("Error while trying to create/open key (%d)\n"), GetLastError();
	}

	if (result == REG_CREATED_NEW_KEY) 
	{
		for (auto &x : top10.position) {
			x.points = 0;
			_tcscpy_s(x.username, TEXT(""));
		}

		RegSetValueEx(key, TEXT("Top10"), 0, REG_BINARY,(LPBYTE)&top10,sizeof(Top10));
	}
	else if (result == REG_OPENED_EXISTING_KEY) {
		RegQueryValueEx(key, TEXT("Top10"), NULL, NULL, (LPBYTE)&top10, &size);
	}

	RegCloseKey(key);
}

Top10Manager::~Top10Manager()
{
	HKEY key;
	DWORD result;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\SO2_Breakout"), 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &result) != ERROR_SUCCESS)
	{
		return;
	}

	RegSetValueEx(key, TEXT("Top10"), 0, REG_BINARY, (LPBYTE)&top10, sizeof(Top10));
	RegCloseKey(key);
}

void Top10Manager::addPlayer(Player & player) {
	Place tempA = { 0 };
	Place tempB = { 0 };
	int i;

	for (i = 9; i >= 0; i--) {
		if (player.points < top10.position[i].points) {
			//if the player's points is not higher than the last place in the list then it does nothing
			if (i == 9)
				return;
		}
	}

	tempA = top10.position[i];
	top10.position[i].points = player.points;
	_tcscpy_s(top10.position[i].username, player.name);

	//starts to iterate directly after the index of the new place
	for (i = i + 1; i < 10; i++) {
		tempB = top10.position[i];
		top10.position[i] = tempA;
		tempA = tempB;
	}
}

tstring Top10Manager::getAsString() {
	tstringstream tss;

	for (int i = 0; i < 10; i++) {
		tss << i+1 << " - Player: " << top10.position[i].username;
		tss << " - Pts: " << top10.position[i].points << endl;
	}

	return tss.str();
}
