#include "Server.h"

GameConfig Server::config;
Top10Manager Server::topPlayers;
SharedMemoryManager Server::sharedMemory;
GameDataManager Server::gameData(nullptr);
ThreadManager Server::threadManager;
