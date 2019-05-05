#include "ServerInterface.h"

void displayHelp() {
	tcout << "Command list: " << endl;
	tcout << "\t \'help\' -> Displays this help message." << endl;
	tcout << "\t \'exit\' -> Closes server." << endl;
	tcout << "\t \'show top10\' -> Displays a list with the top 10 players." << endl;
	tcout << "\t \'show configs\' -> Displays a list with the server's game configs." << endl;
	tcout << "\t \'show clients\' -> Displays a list of clients signed-in the server." << endl;
	tcout << "\t \'addtotop10 <player name> <points>\' - adds an user to the top 10 player's list if their points are high enough." << endl;
}

void ClearScreen()
{
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = { 0, 0 };

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	/* Get the number of cells in the current buffer */
	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
	cellCount = csbi.dwSize.X *csbi.dwSize.Y;

	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(
		hStdOut,
		(TCHAR) ' ',
		cellCount,
		homeCoords,
		&count
	)) return;

	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(
		hStdOut,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count
	)) return;

	/* Move the cursor home */
	SetConsoleCursorPosition(hStdOut, homeCoords);
}

void ServerInterface::startCommandLine() {
	tstringstream tss;
	vector<tstring> argv;
	tstring line, command, argument;

	displayHelp();

	do
	{
		tcout << "> ";
		getline(tcin, line);
		
		tss = tstringstream(line);
		getline(tss, command, TEXT(' '));

		while(getline(tss, argument, TEXT(' '))) {
			argv.push_back(argument);
		}

		handleCommand(command, argv);
		argv.clear();

	} while (command.compare(TEXT("exit")));
}

void ServerInterface::handleCommand(tstring command, vector<tstring> argv) {
	tstringstream tss;

	for (auto & x : command) { //convert command from text file to lower-case to make it case insensitive
		x = _totlower(x);
	}

	if (command == TEXT("show"))
	{
		if (argv.size() != (unsigned)1) {
			tcout << "invalid arguments, show usage: show top10 / show configs / show clients" << endl;
			return;
		}

		if (argv[0] == TEXT("top10")) {
			tcout << Server::topPlayers.getAsString();
		}
		else if (argv[0] == TEXT("configs")) {
			tcout << Server::config;
		}
		else if (argv[0] == TEXT("clients")) {
			tcout << Server::clients;
		}
		else {
			tcout << command << " " << argv[0] << " does not exists!" << endl;
		}
	}
	else if (command == TEXT("addtotop10"))
	{
		if (argv.size() != (unsigned)2) {
			tcout << "invalid arguments! usage: addtop10 <username> <points>" << endl;
			return;
		}

		tss = tstringstream(argv[1]);
		Player p;

		_tcscpy_s(p.name, argv[0].c_str());
		tss >> p.points;

		if (tss.fail()) {
			tcout << "Error: invalid points value!" << endl;
			return;
		}
		Server::topPlayers.addPlayer(p);
		Server::topPlayers.saveTop10();
	}
	else if (command == TEXT("help")) 
	{
		displayHelp();
	}
	else if (command == TEXT("exit")) 
	{
		//TODO: Create function on server to end with server
	}
	else if (command == TEXT("clear")) 
	{
		ClearScreen();
	}
	else
	{
		tcout << "\'" << command << "\' is not a valid command" << endl;
	}
}

void ServerInterface::startServer(tstring filename) {
	int result = Server::startServer(filename);

	switch (result) {
		case SERVER_STARTED:
			tcout << "Server has started!" << endl;
			break;

		case CONFIG_LOADING_ERROR:
			tcout << "Error loading game configs, server is shutting down" << endl;
			return;

		case SERVER_ALREADY_RUNNING:
			tcout << "An instance of the serve is already running in this machine!" << endl;
			return;

		case TOP10_LOADING_ERROR:
			tcout << "Error while trying to read top10 from the registry!" << endl;
			return;

		case SHARED_MEMORY_ERROR:
			tcout << "Error while initializing the shared memory!" << endl;
			return;

		case LOCAL_CLIENT_HANDLER_ERROR:
			tcout << "Local client handler THREAD could not be started!" << endl;
			return;

		default:
			tcout << "uh oh, something happened and I have no idea of what!" << endl;
	}

	startCommandLine();

	Server::exitServer();
}
