#include "Program.h"

#include "ArgumentParsing.h"

int InitProgramContext(PPROGRAM_CONTEXT ProgramContext, int argc, char * argv[])
{
	int Result = 0;
	
	InitNodeContext(&(ProgramContext->NodeContext));
	
	Result = ParseArguments(argc, argv, ProgramContext);
	
	return Result;
}