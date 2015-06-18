#include "FileWriter.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <stdio.h>
#include <math.h> 

#include "ErrorCodes.h"

#include "Tweet.h"

int OpenOutputFile(PPROGRAM_CONTEXT ProgramContext, PFILE_WRITER_CONTEXT FileWriterContext);
void WriteTweetsToFile(PNODE_CONTEXT NodeContext, PFILE_WRITER_CONTEXT FileWriterContext);
int MMapInputFiles(PPROGRAM_CONTEXT ProgramContext, PFILE_WRITER_CONTEXT FileWriterContext);
int MUnMapInputFiles(PPROGRAM_CONTEXT ProgramContext, PFILE_WRITER_CONTEXT FileWriterContext);

int WriteOutResults(PPROGRAM_CONTEXT ProgramContext)
{
	FILE_WRITER_CONTEXT FileWriterContext;
	int Result = NO_ERROR;
	
	//Open File
	Result = OpenOutputFile(ProgramContext, &FileWriterContext);
	if(Result != NO_ERROR)
	{
		return Result;
	}	
	
	//Write all Elements
	WriteTweetsToFile(&ProgramContext->NodeContext, &FileWriterContext);
	
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


void WriteTweetsToFile(PNODE_CONTEXT NodeContext, PFILE_WRITER_CONTEXT FileWriterContext)
{
	for(PTWEET Tweet = NodeContext->Data; Tweet != &(NodeContext->Data[NodeContext->ElementsPerNode]); Tweet++)
	{		
#ifdef WRITE_DEBUG_INFO_TO_FILE
		PrintTweetDebugInfoToStream(FileWriterContext->OutputFile, Tweet);
#endif	
		fputws(Tweet->Tweet, FileWriterContext->OutputFile);
		fputwc(L'\n', FileWriterContext->OutputFile);
	}	
}