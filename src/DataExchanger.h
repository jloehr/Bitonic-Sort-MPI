#pragma once

#include <stdbool.h>

#include "ForwardDeclarations.h"

int ExchangeTweetData(PPROGRAM_CONTEXT ProgramContext, int PartnerNodeID, bool LeftNode);
int ExchangeTweetDataBack(PPROGRAM_CONTEXT ProgramContext, int PartnerNodeID, bool LeftNode);