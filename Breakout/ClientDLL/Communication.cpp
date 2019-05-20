#include "Communication.h"

LocalCLient * getLocalClientInstance() {
		return new LocalCLient();
}

RemoteClient * getRemoteClientInstance() {
	return new RemoteClient();
}