#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "UnicodeConfigs.h"
#include "CommandConstants.h"
#include "GameConfig.h"

using namespace std;
class ConfigHandler
{
private:
	tstring filename;
	GameConfig& config;

	void handleCommand(tstring command, tstring value);
public:
	ConfigHandler(GameConfig& cfg, tstring filename);
	int importConfigs();
};