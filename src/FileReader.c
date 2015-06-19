#include "FileReader.h"

#include "Int.h"
#include <stdlib.h>
#include <string.h>
#include <math.h> 

#include "ErrorCodes.h"

#include "Benchmark.h"
#include "Node.h"
#include "Program.h"

static int AllocateMemory(PPROGRAM_CONTEXT ProgramContext);
static int ReadInFiles(PPROGRAM_CONTEXT ProgramContext);
static int ReadInFile(PPROGRAM_CONTEXT ProgramContext, PWSTRING * TweetPointer, uint64_t FileID);
static int ReadInLine(PFILE_READER_CONTEXT FileReaderContext, PWSTRING TweetPointer, PNODE_CONTEXT NodeContext);

int ReadInTweets(PPROGRAM_CONTEXT ProgramContext)
{	
	int Result = NO_ERROR;
	
	Result = AllocateMemory(ProgramContext);	
	if(Result != NO_ERROR)
	{
		return Result;
	}
		
	Result = ReadInFiles(ProgramContext);
	if(Result != NO_ERROR)
	{
		return Result;
	}
	
	return Result;
}

int AllocateMemory(PPROGRAM_CONTEXT ProgramContext)
{
	uint64_t TweetArraySize = ProgramContext->TotalAmountOfTweets * sizeof(WSTRING);
	
	ProgramContext->Tweets = malloc(TweetArraySize);
	ProgramContext->NodeContext.BenchmarkContext.TweetArrayMemory = TweetArraySize;
	
	if(ProgramContext->Tweets == NULL)
	{
		wprintf(L"Node %i: failed to allocate Tweet Array Memory\n", ProgramContext->NodeContext.NodeID);
		return ERROR_NOT_ENOUGH_MEMORY;
	}
	
	return NO_ERROR;
}

int ReadInFiles(PPROGRAM_CONTEXT ProgramContext)
{
	int Result = NO_ERROR;
	PWSTRING TweetPointer = ProgramContext->Tweets;
	
	for(uint64_t FileID = 0; FileID < ProgramContext->NumberOfFiles; FileID++)
	{
		wprintf(L"Node %i: Reading In File %"PRIu64"\n", ProgramContext->NodeContext.NodeID, FileID);
		
		Result = ReadInFile(ProgramContext, &TweetPointer, FileID);
		if(Result != NO_ERROR)
		{
			return Result;
		}
		
		wprintf(L"Node %i: Done Reading In File %"PRIu64"\n", ProgramContext->NodeContext.NodeID, FileID);
	}
	
	return Result;
}

int ReadInFile(PPROGRAM_CONTEXT ProgramContext, PWSTRING * TweetPointer, uint64_t FileID)
{
	int Result = NO_ERROR;
	FILE_READER_CONTEXT FileReaderContext;
	
	//Get Memory for Filename + the point + the FileID
	uint32_t NumberOfDigits = log10(FileID) + 1;
	uint32_t FilenameLength = strlen(ProgramContext->Filename);
	char * FilenameBuffer = malloc(FilenameLength + NumberOfDigits + 1);
	
	sprintf(FilenameBuffer, "%s.%"PRIu64"", ProgramContext->Filename, FileID);
			
	//Open File
	FileReaderContext.File = fopen(FilenameBuffer, "r");
	if(FileReaderContext.File == NULL)
	{
		wprintf(L"Error on Node %i: Unable to open File %s\n", ProgramContext->NodeContext.NodeID, FilenameBuffer);
		free(FilenameBuffer);
		return ERROR_UNABLE_TO_OPEN_FILE;
	}
	
	free(FilenameBuffer);
	
	//Parse Lines
	while((*TweetPointer) != ProgramContext->Tweets + ((FileID + 1) * ProgramContext->TweetsPerFile))
	{
		ReadInLine(&FileReaderContext, (*TweetPointer), &ProgramContext->NodeContext);
		((*TweetPointer)++);
	}
	
	//Close File
	fclose(FileReaderContext.File);
	
	return Result;
}

int ReadInLine(PFILE_READER_CONTEXT FileReaderContext, PWSTRING TweetPointer, PNODE_CONTEXT NodeContext)
{
	uint16_t TweetSize = 0;
	wint_t ReadCharacter;
	wchar_t * WritePointer = FileReaderContext->TweetBuffer;
	
	//Read Tweet into Buffer
	while((ReadCharacter = fgetwc(FileReaderContext->File)) != WEOF)
	{
		TweetSize++;
		if(ReadCharacter == '\n')
		{
			(*WritePointer) = '\0';
			break;
		}
		else
		{
			(*WritePointer++) = ReadCharacter;
		}
	}
	
	if(ReadCharacter == WEOF)
	{
		TweetSize++;
		(*WritePointer) = '\0';
	}
	
	// Copy Tweet from Buffer
	uint64_t NumberOfTweetBytes = TweetSize * sizeof(wchar_t);
	void * TweetString = malloc(NumberOfTweetBytes);
	if(TweetString == NULL)
	{
		wprintf(L"Error on Node %i: Out of Memory\n", NodeContext->NodeID);
		return ERROR_OUT_OF_MEMORY;
	}
	
	memcpy(TweetString, FileReaderContext->TweetBuffer, NumberOfTweetBytes);
	(*TweetPointer) = TweetString;
	NodeContext->BenchmarkContext.TweetStringsMemory += NumberOfTweetBytes;
	
	return NO_ERROR;
}
