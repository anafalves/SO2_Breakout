#pragma once
enum Constants {
	MAX_PLAYERS = 20,
	MAX_BONUSES = 50,
	MAX_BALLS = 3,
	MAX_TILES = 300
};

enum GameState {
	RUNNING,
	GAME_OVER,
	GAME_WON,
	NEXT_LEVEL,
	LOBBY,
	SHUTDOWN
};

enum TilesValue {
	UNBREAKABLE = -1,
};

enum BonusType {
	NORMAL = 0,
	SPEED_UP,
	SLOW_DOWN,
	LIFE,
	TRIPLE
};

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
	int bonus;
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

typedef struct _gameData {
	int gameState;
	Ball balls[MAX_BALLS];
	Tile tiles[MAX_TILES];
	Bonus bonuses[MAX_BONUSES];
	Player players[MAX_PLAYERS];
}GameData;