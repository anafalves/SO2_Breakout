#pragma once
#include <iostream>
#include <io.h>
#include <tchar.h>
#include "GameConfig.h"

using namespace std;

class ConfigHandler
{
private:
	TCHAR fileName;
	GameConfig config;

public:
	ConfigHandler(TCHAR filepath)
		:fileName(filepath)
	{}
	
	bool ImportConfigs();
};