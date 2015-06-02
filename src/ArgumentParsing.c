#include "ArgumentParsing.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int ParseArguments(int argc, char * argv[], PPROGRAM_CONTEXT ProgramContext)
{
	char Argument;
	int Index;

	opterr = 0;

	while ((Argument = getopt (argc, argv, "f:n:t:")) != -1)
	{
		switch(Argument)
		{
			case 'f':
				ProgramContext->Filename = optarg;
				break;
			case 'n':
				ProgramContext->NumberOfFiles = atoi(optarg);
				break;
			case 't':
				ProgramContext->TweetsPerFile = atoi(optarg);
				break;
			case '?':
				if(IsMasterNode(&(ProgramContext->NodeContext)))
				{
					printf ("Option -%c requires an argument.\n", optopt);
				}
          		break;
			default:
				abort();
		}
	}

	if(optind < argc)
	{
		ProgramContext->SearchTerm = argv[optind];
		optind++;
	}

	if(ProgramContext->SearchTerm == NULL)
	{
		if(IsMasterNode(&(ProgramContext->NodeContext)))
		{
			printf("Error no Search Term provided!\n");	
		}
		return 1;
	}

	if(ProgramContext->Filename == NULL)
	{
		if(IsMasterNode(&(ProgramContext->NodeContext)))
		{
			printf("Error no Filename provided!\n");	
		}
		return 1;
	}
	
	if(ProgramContext->NumberOfFiles == 0)
	{
		if(IsMasterNode(&(ProgramContext->NodeContext)))
		{
			printf("Error, please provide the Number of Files!\n");	
		}
		return 1;
	}
	
	if(ProgramContext->TweetsPerFile == 0)
	{
		if(IsMasterNode(&(ProgramContext->NodeContext)))
		{
			printf("Error, please provide the Number of Tweets per File!\n");	
		}
		return 1;
	}

	return 0;
}
