#include "FileWriter.h"

#include <unistd.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <stdio.h>
#include <math.h> 

#include "ErrorCodes.h"

#include <sys/mman.h>

#include "Tweet.h"
#include "Node.h"
#include "Program.h"

static int OpenOutputFile(PPROGRAM_CONTEXT ProgramContext, PFILE_WRITER_CONTEXT FileWriterContext);
static void WriteTweetsToFile(PPROGRAM_CONTEXT ProgramContext, PFILE_WRITER_CONTEXT FileWriterContext);

int WriteOutResults(PPROGRAM_CONTEXT ProgramContext)
{
	FILE_WRITER_CONTEXT FileWriterContext;
	int Result = NO_ERROR;
	
#ifndef WRITE_DEBUG_INFO_TO_FILE
	//Free the UnicodeAppearances
	if(ProgramContext->UnicodeAppearances != NULL)
	{
		free(ProgramContext->UnicodeAppearances);
		ProgramContext->UnicodeAppearancesSize = 0;
		ProgramContext->UnicodeAppearances = NULL;
	}
#endif
	
	//Open File
	Result = OpenOutputFile(ProgramContext, &FileWriterContext);
	if(Result != NO_ERROR)
	{
		return Result;
	}	
	
	mlock(ProgramContext->TweetStrings, ProgramContext->TweetStringsSize);
	
	//Write all Elements
	WriteTweetsToFile(ProgramContext, &FileWriterContext);
	
	munlock(ProgramContext->TweetStrings, ProgramContext->TweetStringsSize);
	
	//close file
	fclose(FileWriterContext.OutputFile);

	return Result;
}

int OpenOutputFile(PPROGRAM_CONTEXT ProgramContext, PFILE_WRITER_CONTEXT FileWriterContext)
{	
	//Get Memory for Filename + ".out." + NodeID
	uint32_t NumberOfDigits = log10(ProgramContext->NodeContext.NodeID) + 1;
	uint32_t FilenameLength = strlen(ProgramContext->Filename);
	char * FilenameBuffer = malloc(FilenameLength + NumberOfDigits + 5);
	
	sprintf(FilenameBuffer, "%s.out.%d", ProgramContext->Filename, ProgramContext->NodeContext.NodeID);
	
	//Open File
	FileWriterContext->OutputFile = fopen(FilenameBuffer, "w");
	if(FileWriterContext->OutputFile == NULL)
	{
		wprintf(L"Error on Node %i: Unable to open File %s\n", ProgramContext->NodeContext.NodeID, FilenameBuffer);
		free(FilenameBuffer);
		return ERROR_UNABLE_TO_OPEN_FILE;
	}
	
	free(FilenameBuffer);
	
	return NO_ERROR;
}


void WriteTweetsToFile(PPROGRAM_CONTEXT ProgramContext, PFILE_WRITER_CONTEXT FileWriterContext)
{
	for(PTWEET Tweet = ProgramContext->NodeContext.Data; Tweet != ProgramContext->NodeContext.Data + ProgramContext->NodeContext.ElementsPerNode; Tweet++)
	{		
#ifdef WRITE_DEBUG_INFO_TO_FILE
		PrintTweetDebugInfoToStream(FileWriterContext->OutputFile, ProgramContext, Tweet);
#endif	
		fputws(ProgramContext->TweetStrings + Tweet->TweetStringOffset, FileWriterContext->OutputFile);
		fputwc(L'\n', FileWriterContext->OutputFile);
	}	
}