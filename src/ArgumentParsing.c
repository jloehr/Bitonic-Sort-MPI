#include "ArgumentParsing.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "ErrorCodes.h"

#include "Program.h"

int ParseArguments(int argc, char * argv[], PPROGRAM_CONTEXT ProgramContext)
{
	char Argument;
	opterr = 0;

	while ((Argument = getopt (argc, argv, "f:n:o:t:")) != -1)
	{
		switch(Argument)
		{
			case 'f':
				ProgramContext->Filename = optarg;
				break;
			case 'n':
				ProgramContext->NumberOfFiles = atoi(optarg);
				break;
			case 'o':
				ProgramContext->OutputDir = optarg;
				break;
			case 't':
				ProgramContext->TweetsPerFile = atoi(optarg);
				break;
			case '?':
				if(IsMasterNode(&(ProgramContext->NodeContext)))
				{
					wprintf (L"Option -%c requires an argument.\n", optopt);
				}
          		break;
			default:
				abort();
		}
	}

	if(optind < argc)
	{
		//get the memory for the wstring
		const char * SearchTerm = argv[optind];
		size_t WCharLength = mbsrtowcs(NULL, &SearchTerm, 0, NULL);
		
		//allocate memory
		ProgramContext->SearchTerm = calloc(WCharLength + 1,sizeof(wchar_t));
		
		//convert argument to wchar *
		mbsrtowcs((wchar_t *)ProgramContext->SearchTerm, &SearchTerm, WCharLength, NULL);
		optind++;
		
	}

	if(ProgramContext->SearchTerm == NULL)
	{
		if(IsMasterNode(&(ProgramContext->NodeContext)))
		{
			wprintf(L"Error no Search Term provided!\n");	
		}
		return ERROR_NON_VALID_ARGUMENTS;
	}

	if(ProgramContext->Filename == NULL)
	{
		if(IsMasterNode(&(ProgramContext->NodeContext)))
		{
			wprintf(L"Error no Filename provided!\n");	
		}
		return ERROR_NON_VALID_ARGUMENTS;
	}
	
	if(ProgramContext->OutputDir == NULL)
	{
		if(IsMasterNode(&(ProgramContext->NodeContext)))
		{
			wprintf(L"Error no Output dir provided!\n");	
		}
		
		return ERROR_NON_VALID_ARGUMENTS;
	}
	
	if(ProgramContext->NumberOfFiles == 0)
	{
		if(IsMasterNode(&(ProgramContext->NodeContext)))
		{
			wprintf(L"Error, please provide the Number of Files!\n");	
		}
		return ERROR_NON_VALID_ARGUMENTS;
	}
	
	if(ProgramContext->TweetsPerFile == 0)
	{
		if(IsMasterNode(&(ProgramContext->NodeContext)))
		{
			wprintf(L"Error, please provide the Number of Tweets per File!\n");	
		}
		return ERROR_NON_VALID_ARGUMENTS;
	}

	return NO_ERROR;
}
