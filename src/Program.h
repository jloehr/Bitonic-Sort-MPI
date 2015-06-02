#pragma once

#include <stdint.h>

#include "Node.h"

typedef struct _PROGRAM_CONTEXT 
{
	const char * Filename;
	uint32_t NumberOfFiles;
	uint32_t TweetsPerFile;
	const char * SearchTerm;
	
	NODE_CONTEXT NodeContext;

} PROGRAM_CONTEXT, * PPROGRAM_CONTEXT;

int InitProgramContext(PPROGRAM_CONTEXT ProgramContext, int argc, char * argv[]);
void FinalizeProgramContext(PPROGRAM_CONTEXT ProgramContext);