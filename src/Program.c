#include "Program.h"

#include <stdlib.h>

#include "ArgumentParsing.h"

int InitProgramContext(PPROGRAM_CONTEXT ProgramContext, int argc, char * argv[])
{
	int Result = 0;
	
	InitNodeContext(&(ProgramContext->NodeContext));
	
	ProgramContext->Filename = NULL;
	ProgramContext->NumberOfFiles = 0;
	ProgramContext->TweetsPerFile = 0;
	ProgramContext->SearchTerm = NULL;
	
	Result = ParseArguments(argc, argv, ProgramContext);
	
	return Result;
}