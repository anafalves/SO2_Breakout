#include "Server.h"
#include "ConfigHandler.h"

//TODO: this needs to become a class and a MAIN FILE MUST BE CREATED to start the server
int _tmain(int argc, TCHAR **argv) {

	#ifdef UNICODE
		_setmode(_fileno(stdin), _O_WTEXT);
		_setmode(_fileno(stdout), _O_WTEXT);
	#endif

	if (argc != 2) {
		tcout << TEXT("Argument count invalid!") << endl;
		tcout << TEXT("Usage: Server My/File/Path/filename.txt") << endl;
		return -1;
	}
	GameConfig config;
	ConfigHandler a(config, argv[1]);
	a.importConfigs();

	tcout << config;

	return 0;
}