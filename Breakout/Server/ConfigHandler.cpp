#include "ConfigHandler.h"

ConfigHandler::ConfigHandler(GameConfig& cfg, tstring fname)
	:config(cfg), filename(fname)
{}

void ConfigHandler::importConfigs() {
	tifstream file(filename);
	tstringstream tss;
	tstring line, command, value;

	if (!file.is_open())
		throw TEXT("Could not open file Exception...");

	while (getline(file, line))
	{
		tss = tstringstream(line);
		getline(tss, command,TEXT(' '));
		getline(tss, value, TEXT(' '));

		handleCommand(command, value);
	}
	file.close();
}

void ConfigHandler::handleCommand(tstring command, tstring value) {
	int convertedValue;
	tstringstream tss(value);

	for (auto & x : command) { //convert command from text file to lower-case to make it case insensitive
		x = _totlower(x);
	}

	if (command == CommandConstants::bonusDropRate)
	{
		double convertedDouble;

		tss >> convertedDouble;
		if (tss.good()) {
			config.setBonusDropRate(convertedDouble);
		}
	}
	else if (command == CommandConstants::levelCount)
	{
		tss >> convertedValue;
		if (tss.good()) {
			config.setLevelCount(convertedValue);
		}
	}
	else if (command == CommandConstants::maxLives)
	{
		tss >> convertedValue;
		if (tss.good()) {
			config.setInitialLives(convertedValue);
		}
	}
	else if (command == CommandConstants::movementSpeed)
	{
		tss >> convertedValue;
		if (tss.good()) {
			config.setMovementSpeed(convertedValue);
		}
	}
	else if (command == CommandConstants::playerCount)
	{
		tss >> convertedValue;
		if (tss.good()) {
			config.setMaxPlayerCount(convertedValue);
		}
	}
	else if (command == CommandConstants::slowDown)
	{
		tss >> convertedValue;
		if (tss.good()) {
			config.setSlowDownCount(convertedValue);
		}
	}
	else if (command == CommandConstants::speedUp)
	{
		tss >> convertedValue;
		if (tss.good()) {
			config.setSpeedUpCount(convertedValue);
		}
	}
	else if (command == CommandConstants::tileCount)
	{
		tss >> convertedValue;
		if (tss.good()) {
			config.setInitialTileCount(convertedValue);
		}
	}
}