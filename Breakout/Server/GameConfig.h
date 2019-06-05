#pragma once
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <ostream>
#include <sstream>
#include "UnicodeConfigs.h"

using namespace std;

enum GameConfigDefaults {
	DEFAULT_PLAYER_COUNT = 10,
	DEFAULT_LEVEL_COUNT = 10,
	DEFAULT_SPEEDUP_COUNT = 15,
	DEFAULT_SLOWDOWN_COUNT = 15,
	DEFAULT_LIVES_COUNT = 3,
	DEFAULT_TILE_COUNT = 80,
	DEFAULT_MOVEMENT_SPEED = 10,
	DEFAULT_BONUS_RATE = 25,
	DEFAULT_TRIPLE_COUNT = 5,
};

class GameConfig {
private:
	int initialMovementSpeed;
	int maxPlayerCount;
	int levelCount;
	int speedUpCount;
	int slowDownCount;
	int initialLives;
	int initialTileCount;
	int movementSpeed;
	int ballTripleCount;
	double bonusDropRate;

public:
	GameConfig();

	int getMaxPlayerCount() const;
	int getLevelCount() const;
	int getSpeedUpCount() const;
	int getSlowDownCount() const;
	int getInitialLives() const;
	int getInitialTileCount() const;
	int getMovementSpeed() const;
	int getBallTripleCount() const;
	double getBonusDropRate() const;

	bool setInitialLives(int lives);
	bool setInitialTileCount(int tileCount);
	bool setLevelCount(int numberOfLevels);
	bool setMaxPlayerCount(int numberOfPlayers);
	bool setMovementSpeed(int speed);
	bool setSlowDownCount(int count);
	bool setSpeedUpCount(int count);
	bool setBallTripleCount(int count);
	bool setBonusDropRate(double rate);
};

tostream& operator<< (tostream& tos, const GameConfig& cfg);