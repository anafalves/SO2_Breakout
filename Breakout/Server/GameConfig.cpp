#include "GameConfig.h"

GameConfig::GameConfig() 
	:maxPlayerCount(DEFAULT_PLAYER_COUNT), levelCount(DEFAULT_LEVEL_COUNT),
	speedUpCount(DEFAULT_SPEEDUP_COUNT), slowDownCount(DEFAULT_SLOWDOWN_COUNT),
	initialLives(DEFAULT_LIVES_COUNT), initialTileCount(DEFAULT_TILE_COUNT),
	movementSpeed(DEFAULT_MOVEMENT_SPEED), bonusDropRate(DEFAULT_BONUS_RATE)
{}

//Getters
int GameConfig::getInitialLives() const {
	return initialLives;
}

int GameConfig::getInitialTileCount() const {
	return initialTileCount;
}

int GameConfig::getLevelCount() const {
	return levelCount;
}

int GameConfig::getMaxPlayerCount() const {
	return maxPlayerCount;
}

int GameConfig::getMovementSpeed() const {
	return movementSpeed;
}

int GameConfig::getSlowDownCount() const {
	return slowDownCount;
}

int GameConfig::getSpeedUpCount() const {
	return speedUpCount;
}

int GameConfig::getBonusDropRate() const {
	return bonusDropRate;
}


//Setters
bool GameConfig::setInitialLives(int lives) {
	if (lives < 0)
		return false;

	initialLives = lives;
	return true;
}

bool GameConfig::setInitialTileCount(int tileCount) {
	if (tileCount < 0)
		return false;

	initialTileCount = tileCount;
	return true;
}

bool GameConfig::setLevelCount(int numberOfLevels) {
	if (numberOfLevels < 0)
		return false;

	levelCount = numberOfLevels;
	return true;
}

bool GameConfig::setMaxPlayerCount(int numberOfPlayers) {
	if (maxPlayerCount <= 0)
		return false;

	maxPlayerCount = numberOfPlayers;
	return true;
}

bool GameConfig::setMovementSpeed(int speed) {
	if (speed <= 0)
		return false;

	movementSpeed = speed;
	return true;
}

bool GameConfig::setSlowDownCount(int count) {
	if (count < 0)
		return false;

	slowDownCount = count;
	return true;
}

bool GameConfig::setSpeedUpCount(int count) {
	if(count < 0)
		return false;

	speedUpCount = count;
	return true;
}

bool GameConfig::setBonusDropRate(int rate) {
	if (rate < 0 || rate > 100)
		return false;

	bonusDropRate = rate;
	return true;
}