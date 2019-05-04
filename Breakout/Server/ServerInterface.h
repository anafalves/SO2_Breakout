#pragma once
#include "UnicodeConfigs.h"
#include "Server.h"

class ServerInterface
{
private:

	void handleCommand(tstring command, vector<tstring> argv);
	void startCommandLine();

public:
	void startServer(tstring filename);

};

