#include "ConfigHandler.h"
#include "Server.h"

int _tmain(int argc, TCHAR **argv) {

	#ifdef UNICODE
		_setmode(_fileno(stdin), _O_WTEXT);
		_setmode(_fileno(stdout), _O_WTEXT);
	#endif

	if (argc != 2) {
		_tprintf(TEXT("Argument count invalid!\n"));
		_tprintf(TEXT("Usage: Server My/File/Path/filename.txt\n"));
		return -1;
	}

	GameConfig cfg;

	return 0;
}