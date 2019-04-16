#pragma once
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <string>
#include "UnicodeConfigs.h"

#include "ConfigHandler.h"
#include "GameConfig.h"

using namespace std;

class Server {
private:
	

public:

	static GameConfig config;

	Server(tstring fileName) {
		//TODO: Verify if file exists, if not server closes indication error.
		//TODO: Make a start function instead ???
		ConfigHandler a(config, fileName);
		a.importConfigs();

		//tcout << config;

		//TODO: Call Connection Handler to startup connection threads and services
	};

	void showConfigs() {
		tcout << config;
	}
};