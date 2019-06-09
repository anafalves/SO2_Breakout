#include "GameConfig.h"

GameConfig::GameConfig() 
	:initialMovementSpeed(DEFAULT_MOVEMENT_SPEED), maxPlayerCount(DEFAULT_PLAYER_COUNT), 
	levelCount(DEFAULT_LEVEL_COUNT), speedUpCount(DEFAULT_SPEEDUP_COUNT), 
	slowDownCount(DEFAULT_SLOWDOWN_COUNT), initialLives(DEFAULT_LIVES_COUNT), 
	initialTileCount(DEFAULT_TILE_COUNT), movementSpeed(DEFAULT_MOVEMENT_SPEED), 
	bonusDropRate(DEFAULT_BONUS_RATE), ballTripleCount(DEFAULT_TRIPLE_COUNT)
{}

//Getters
int GameConfig::getInitialLives() const {
	return initialLives;
}

int GameConfig::getBallTripleCount() const {
	return ballTripleCount;
}

int GameConfig::getExtraLifeCount() const{
	return extraLifeCount;
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
	return bonusDropRate / 100.00;
}

//TDOD: new func, it's not being called from anywhere
int GameConfig::getBonusTime() const {
	return bonusTime;
}


//Setters
bool GameConfig::setInitialLives(int lives) {
	if (lives < 0)
		return false;
	
	initialLives = lives;
	return true;
}

void GameConfig::setBallTripleCount(int count) {
	if (count < MIN_TRIPLE_COUNT)
		count = MIN_TRIPLE_COUNT;
	else if (count > MAX_TRIPLE_COUNT)
		count = MAX_TRIPLE_COUNT;

	ballTripleCount = count;
}

void GameConfig::setExtraLifeCount(int count)
{
	if(count < MIN_LIVES_COUNT)
		count = MIN_LIVES_COUNT;
	else if (count > MAX_LIVES_COUNT)
		count = MAX_LIVES_COUNT;
		
	initialLives = count;
}

void GameConfig::setInitialTileCount(int tileCount) {
	if (tileCount < MIN_TILE_COUNT)
		tileCount = MIN_TILE_COUNT;
	else if (tileCount > MAX_TILE_COUNT)
		tileCount = MAX_TILE_COUNT;

	initialTileCount = tileCount;
}

void GameConfig::setLevelCount(int numberOfLevels) {
	if (numberOfLevels < MIN_LEVEL_COUNT)
		numberOfLevels = MIN_LEVEL_COUNT;
	else if (numberOfLevels > MAX_LEVEL_COUNT)
		numberOfLevels = MAX_LEVEL_COUNT;

	levelCount = numberOfLevels;
}

void GameConfig::setMaxPlayerCount(int numberOfPlayers) {
	if (numberOfPlayers < MIN_PLAYER_COUNT)
		numberOfPlayers = MIN_PLAYER_COUNT;
	if (numberOfPlayers > MAX_PLAYER_COUNT)
		numberOfPlayers = MAX_PLAYER_COUNT;

	maxPlayerCount = numberOfPlayers;
}

void GameConfig::setMovementSpeed(int speed) {
	if (speed < initialMovementSpeed * 0.6)
		speed = (int) (initialMovementSpeed * 0.6);
	else if (speed > initialMovementSpeed * 1.5)
		speed = (int) (initialMovementSpeed * 1.5);

	movementSpeed = speed;
}

void GameConfig::setSlowDownCount(int count) {
	if (count < MIN_SLOWDOWN_COUNT)
		count = MIN_SLOWDOWN_COUNT;
	if (count > MAX_SLOWDOWN_COUNT)
		count = MAX_SLOWDOWN_COUNT;

	slowDownCount = count;
}

void GameConfig::setSpeedUpCount(int count) {
	if (count < MIN_SPEEDUP_COUNT)
		count = MIN_SPEEDUP_COUNT;
	else if (count > MAX_SPEEDUP_COUNT)
		count = MAX_SPEEDUP_COUNT;

	speedUpCount = count;
}

void GameConfig::setBonusDropRate(int rate) {
	if (rate < MIN_BONUS_RATE)
		rate = MIN_BONUS_RATE;
	else if(rate > MAX_BONUS_RATE)
		rate = MAX_BONUS_RATE;

	bonusDropRate = rate;
}

void GameConfig::setBonusTime(int time) {
	if (time < MIN_BONUS_TIME)
		time = MIN_BONUS_TIME;
	else if (time > MAX_BONUS_TIME)
		time = MAX_BONUS_TIME;

	bonusTime = time;
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
	//tss << TEXT("Bonus duraction") << cfg.getBonusTime() << endl;	//TODO: to implement in file

	tos << tss.str();
	return tos;
}