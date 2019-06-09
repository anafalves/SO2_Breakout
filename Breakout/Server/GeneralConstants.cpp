#include "GeneralConstants.h"

const tstring SharedMemoryConstants::SHA_MEM_GAMEDATA = TEXT("SharedMemoryGameData");

const tstring SharedMemoryConstants::SHA_MEM_CLIENT_BUFFER = TEXT("SharedMemoryClientBuffer");
const tstring SharedMemoryConstants::SEM_CLIENT_EMPTY = TEXT("SemClientEmpty");
const tstring SharedMemoryConstants::SEM_CLIENT_FILLED = TEXT("SemClientFilled");

const tstring SharedMemoryConstants::SHA_MEM_SERVER_BUFFER = TEXT("SharedMemoryServerBuffer");
const tstring SharedMemoryConstants::SEM_SERVER_EMPTY = TEXT("SemServerEmpty");
const tstring SharedMemoryConstants::SEM_SERVER_FILLED = TEXT("SemServerFilled");
const tstring SharedMemoryConstants::EVENT_GAMEDATA_UPDATE = TEXT("EventGamedataUpdate");
const tstring SharedMemoryConstants::EXIT_EVENT = TEXT("ExitEventClient");

const tstring SharedMemoryConstants::MUT_CLI_WRITE = TEXT("MutClientWrite");
const tstring SharedMemoryConstants::MUT_CLI_READ = TEXT("MutClientRead");

const tstring SharedMemoryConstants::EVENT_UPDATE(TEXT("UpdateFlag_"));
const tstring SharedMemoryConstants::EVENT_CLIENT_NOTIFICATION(TEXT("ClientReadyFlag_"));

//Named pipe constants
const tstring PipeConstants::MESSAGE_PIPE_NAME = TEXT("pipe\\messages");
const tstring PipeConstants::GAMEDATA_PIPE_NAME = TEXT("pipe\\gamedata_");