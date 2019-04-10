#include "Server.h"
#include "ConfigHandler.h"

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

	return 0;
}