#include "ArgumentParsing.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int ParseArguments(int argc, char * argv[], PPROGRAM_CONTEXT ProgramContext)
{
	char Argument;
	int Index;

	ProgramContext->Filename = NULL;
	ProgramContext->TweetCount = 0;

	opterr = 0;

	while ((Argument = getopt (argc, argv, "t:")) != -1)
	{
		switch(Argument)
		{
			case 't':
				ProgramContext->TweetCount = atoi(optarg);
				break;
		}
	}


	if(optind < argc)
	{
		ProgramContext->Filename = argv[optind];
	}

	if(ProgramContext->Filename == NULL)
	{
		if(IsMasterNode(&(ProgramContext->NodeContext)))
		printf("Error no File provided\n");
		return 1;
	}

	return 0;
}
