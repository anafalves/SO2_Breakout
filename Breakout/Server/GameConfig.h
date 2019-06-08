#pragma once
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <ostream>
#include <sstream>
#include "UnicodeConfigs.h"

using namespace std;

enum GameConfigMaxs {
	MAX_PLAYER_COUNT = 10,
	MAX_LEVEL_COUNT = 20,
	MAX_SPEEDUP_COUNT = 42,
	MAX_SLOWDOWN_COUNT = 42,
	MAX_LIVES_COUNT = 30,
	MAX_TILE_COUNT = 288, //MAX line_count * max col_count
	MAX_BONUS_RATE = 50,
	MAX_TRIPLE_COUNT = 30,
	MAX_BONUS_TIME = 3
};

enum GameConfigMins {
	MIN_PLAYER_COUNT = 1,
	MIN_LEVEL_COUNT = 1,
	MIN_SPEEDUP_COUNT = 1,
	MIN_SLOWDOWN_COUNT = 1,
	MIN_LIVES_COUNT = 1,
	MIN_TILE_COUNT = 24, //1 tile line
	MIN_BONUS_RATE = 10,
	MIN_TRIPLE_COUNT = 1,
	MIN_BONUS_TIME = 1
};

enum GameConfigDefaults {
	DEFAULT_PLAYER_COUNT = 5,
	DEFAULT_LEVEL_COUNT = 3,
	DEFAULT_SPEEDUP_COUNT = 15,
	DEFAULT_SLOWDOWN_COUNT = 15,
	DEFAULT_LIVES_COUNT = 3,
	DEFAULT_TILE_COUNT = 200,
	DEFAULT_MOVEMENT_SPEED = 10,
	DEFAULT_BONUS_RATE = 25,
	DEFAULT_TRIPLE_COUNT = 5,
	DEFAULT_BONUS_TIME = 1
};

class GameConfig {
private:
	int initialMovementSpeed;
	int maxPlayerCount;
	int levelCount;
	int speedUpCount; 
	int slowDownCount; 
	int ballTripleCount; 
	int extraLifeCount; 
	int initialLives;
	int initialTileCount;
	int movementSpeed;
	double bonusDropRate;
	int bonusTime;

public:
	GameConfig();

	int getMaxPlayerCount() const;
	int getLevelCount() const;
	int getSpeedUpCount() const;
	int getSlowDownCount() const;
	int getBallTripleCount() const;
	int getExtraLifeCount() const;
	int getInitialLives() const;
	int getInitialTileCount() const;
	int getMovementSpeed() const;
	double getBonusDropRate() const;
	int getBonusTime() const; //TODO: to do, can only be ints or change xD

	bool setInitialLives(int lives);
	void setInitialTileCount(int tileCount);
	void setLevelCount(int numberOfLevels);
	void setMaxPlayerCount(int numberOfPlayers);
	void setMovementSpeed(int speed);
	void setSlowDownCount(int count);
	void setSpeedUpCount(int count);
	void setBallTripleCount(int count);
	void setExtraLifeCount(int count);
	void setBonusDropRate(double rate);
	void setBonusTime(int time);
};

tostream& operator<< (tostream& tos, const GameConfig& cfg);