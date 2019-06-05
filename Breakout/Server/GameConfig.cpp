#include "GameConfig.h"

GameConfig::GameConfig() 
	:initialMovementSpeed(-1), maxPlayerCount(DEFAULT_PLAYER_COUNT), 
	levelCount(DEFAULT_LEVEL_COUNT), speedUpCount(DEFAULT_SPEEDUP_COUNT), 
	slowDownCount(DEFAULT_SLOWDOWN_COUNT), initialLives(DEFAULT_LIVES_COUNT), 
	initialTileCount(DEFAULT_TILE_COUNT), movementSpeed(DEFAULT_MOVEMENT_SPEED), 
	bonusDropRate(DEFAULT_BONUS_RATE / 100.0), ballTripleCount(DEFAULT_TRIPLE_COUNT)
{}

//Getters
int GameConfig::getInitialLives() const {
	return initialLives;
}

int GameConfig::getBallTripleCount() const {
	return ballTripleCount;
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

double GameConfig::getBonusDropRate() const {
	return bonusDropRate;
}


//Setters
bool GameConfig::setInitialLives(int lives) {
	if (lives < 0)
		return false;

	initialLives = lives;
	return true;
}

bool GameConfig::setBallTripleCount(int count) {
	if (count < 0)
		return false;

	ballTripleCount = count;
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

	if (initialMovementSpeed < 0) {
		initialMovementSpeed = speed;
	}

	if(initialMovementSpeed / speed > initialMovementSpeed * 0.1)
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

bool GameConfig::setBonusDropRate(double rate) {
	if (rate < 0 || rate > 1)
		return false;

	bonusDropRate = rate;
	return true;
}

tostream& operator << (tostream& tos, const GameConfig& cfg) {
	tstringstream tss;

	tss << TEXT("Number of slow-downs: ") << cfg.getSlowDownCount() << endl;
	tss << TEXT("Number of speed-ups: ") << cfg.getSpeedUpCount() << endl;
	tss << TEXT("Number of Levels: ") << cfg.getLevelCount() << endl;
	tss << TEXT("Number of Tiles: ") << cfg.getInitialTileCount() << endl;
	tss << TEXT("Number of Lives: ") << cfg.getInitialLives() << endl;
	tss << TEXT("Bonus Drop Rate: ") << cfg.getBonusDropRate() << endl;
	tss << TEXT("Movement Speed: ") << cfg.getMovementSpeed() << endl;
	tss << TEXT("Max players: ") << cfg.getMaxPlayerCount() << endl;

	tos << tss.str();
	return tos;
}