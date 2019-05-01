#include <Windows.h>
#include <tchar.h>
#include "..\Server\UnicodeConfigs.h"
#include "..\ClientDLL\LocalCLient.h"
#include "..\Server\Messages.h"

LocalCLient * client;
HANDLE thread = INVALID_HANDLE_VALUE;
bool CONTINUE;

DWORD WINAPI receiveBallUpdates() {

	for (;CONTINUE;) {
		GameData data = client->receiveBroadcast();

		tcout << "X: " << data.balls[0].posX;
		tcout << "Y: " << data.balls[0].posY << endl << endl;
	}
	return 0;
}

int _tmain(int argc, TCHAR ** argv) {
	

	if (argc != 2) {
		return -1;
	}

	CONTINUE = true;
	client = getClientInstance();

	if (client->login(argv[1])) {
		tcout << "login accepted" << endl;
		
		thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) receiveBallUpdates, NULL, 0, NULL);
	}

	_gettchar();
	CONTINUE = false;

	WaitForSingleObject(thread, INFINITE);

	return 0;
}