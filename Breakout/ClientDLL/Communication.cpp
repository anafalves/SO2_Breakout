#include "Communication.h"

Client * getClientInstance() {
	HANDLE server = INVALID_HANDLE_VALUE;
	server = OpenEvent(EVENT_ALL_ACCESS, FALSE, SharedMemoryConstants::EVENT_GAMEDATA_UPDATE.c_str());
	if (server == NULL) {
		//Is remote
		tcout << "Is remote, but doesn't work!" << endl;
		//TODO: return new RemoteClient();
		return NULL;
	}
	else {
		//Is local
		CloseHandle(server);
		return new LocalCLient();
	}
}