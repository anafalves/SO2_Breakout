#pragma once
#include "Client.h"
#include "LocalCLient.h"
#include "RemoteClient.h"

#ifdef CLIENTDLL_EXPORTS
#define CLIENT_API __declspec(dllexport)
#else
#define CLIENT_API __declspec(dllimport)
#endif

extern "C" CLIENT_API LocalCLient * getLocalClientInstance();
extern "C" CLIENT_API RemoteClient * getRemoteClientInstance();