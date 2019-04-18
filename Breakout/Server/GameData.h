#pragma once
enum Constants {
	MAX_PLAYERS = 20,
	MAX_BONUSES = 50,
	MAX_BALLS = 3,
	MAX_TILES = 1000
};

enum BonusType {
	SPEED_UP = 0,
	SLOW_DOWN,
	LIFE,
	TRIPLE
};

//TODO: Add enum with default sizes for objects
//TODO: Verify if these structures need to be changed

typedef struct ball {
	int posX;
	int posY;
	int width;
	int height;
	int playerId;
	bool up;
	bool right;
	bool active;
}Ball;

typedef struct player {
	int posX;
	int posY;
	int width;
	int height;
	int points;
	int lives;
	int id;
	bool active;
	TCHAR name[50];
}Player;

typedef struct tile {
	int posX;
	int posY;
	int width;
	int height;
	int resistance;
	bool bonus;
	bool active;
}Tile;

typedef struct bonus {
	int posX;
	int posY;
	int width;
	int height;
	int type;
	bool active;
}Bonus;

typedef struct gameData {
	Ball balls[MAX_BALLS];
	Tile tiles[MAX_TILES];
	Bonus bonuses[MAX_BONUSES];
	Player players[MAX_PLAYERS];
}GameData;