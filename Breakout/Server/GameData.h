#pragma once
enum Constants {
	MAX_PLAYERS = 20,
	MAX_BONUSES = 50,
	MAX_BALLS = 100,
	MAX_TILES = 1000
};

enum BonusType {
	SPEED_UP = 0,
	SLOW_DOWN,
	LIFE,
	TRIPLE
};

typedef struct ball {
	int posX;
	int posY;
	int width;
	int height;
	int heading;
	bool active;
}Ball;

typedef struct player {
	int posX;
	int posY;
	int width;
	int height;
	int points;
	int lives;
	bool active;
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