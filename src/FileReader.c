#include "FileReader.h"

#include "Int.h"
#include <stdlib.h>
#include <string.h>
#include <math.h> 

#include "ErrorCodes.h"

#include "Benchmark.h"
#include "Tweet.h"
#include "Node.h"
#include "Program.h"

static int InitFileReaderContext(PPROGRAM_CONTEXT ProgramContext, PFILE_READER_CONTEXT FileReaderContext);
static int AllocateMemory(PPROGRAM_CONTEXT ProgramContext, PFILE_READER_CONTEXT FileReaderContext);
static int AllocateStringMemory(PPROGRAM_CONTEXT ProgramContext, PFILE_READER_CONTEXT FileReaderContext);
static int ReadInFiles(PPROGRAM_CONTEXT ProgramContext, PFILE_READER_CONTEXT FileReaderContext);
static int ReadInFile(PPROGRAM_CONTEXT ProgramContext, PFILE_READER_CONTEXT FileReaderContext, uint64_t FileID);
static int ReadInLine(PPROGRAM_CONTEXT ProgramContext, PFILE_READER_CONTEXT FileReaderContext);
static int WriteCharacterToString(PPROGRAM_CONTEXT ProgramContext, PFILE_READER_CONTEXT FileReaderContext, wchar_t Charachter);

int ReadInTweets(PPROGRAM_CONTEXT ProgramContext)
{		
	int Result = NO_ERROR;
	FILE_READER_CONTEXT FileReaderContext;
	
	Result = InitFileReaderContext(ProgramContext, &FileReaderContext);
	if(Result != NO_ERROR)
	{
		return Result;
	}
	
	Result = AllocateMemory(ProgramContext, &FileReaderContext);
	if(Result != NO_ERROR)
	{
		return Result;
	}
	
	Result = ReadInFiles(ProgramContext, &FileReaderContext);
	if(Result != NO_ERROR)
	{
		return Result;
	}
	
	return Result;
}

int InitFileReaderContext(PPROGRAM_CONTEXT ProgramContext, PFILE_READER_CONTEXT FileReaderContext)
{
	FileReaderContext->EndOfBlock = 0;
	FileReaderContext->WritePointer = NULL;
	
	FileReaderContext->CurrentTweetNumber = 0;
	FileReaderContext->NodeTweetNumberStart = ProgramContext->NodeContext.NodeID * ProgramContext->NodeContext.ElementsPerNode;
	FileReaderContext->NodeTweetNumberEnd = FileReaderContext->NodeTweetNumberStart + ProgramContext->NodeContext.ElementsPerNode;
	FileReaderContext->CurrentTweet = NULL;
	
	return NO_ERROR;
}

int AllocateMemory(PPROGRAM_CONTEXT ProgramContext, PFILE_READER_CONTEXT FileReaderContext)
{	
	int Result = NO_ERROR;
	
	//Alloctate Size for the Tweet Data
	uint64_t TweetDataSize = ProgramContext->NodeContext.ElementsPerNode * sizeof(TWEET);
	
	//Allocate Space
	ProgramContext->NodeContext.Data = malloc(TweetDataSize);
	ProgramContext->NodeContext.BenchmarkContext.TweetDataMemory = TweetDataSize;
	
	if(ProgramContext->NodeContext.Data == NULL)
	{
		wprintf(L"Node %i: failed to allocate Data Memory\n", ProgramContext->NodeContext.NodeID);
		return ERROR_NOT_ENOUGH_MEMORY;
	}
	
	//Allocate Starting Space for the Strings
	Result = AllocateStringMemory(ProgramContext, FileReaderContext);
	if(Result != NO_ERROR)
	{
		return Result;
	}
	
	return Result;
}

int AllocateStringMemory(PPROGRAM_CONTEXT ProgramContext, PFILE_READER_CONTEXT FileReaderContext)
{
	uint64_t CurrentWritePosition = FileReaderContext->WritePointer - ProgramContext->TweetStrings;
	ProgramContext->TweetStringsSize += ProgramContext->PageSize;
	void * Result = realloc(ProgramContext->TweetStrings, ProgramContext->TweetStringsSize);
	
	if(Result == NULL)
	{
		wprintf(L"Node %i: failed to allocate Tweet Array Memory\n", ProgramContext->NodeContext.NodeID);
		return ERROR_NOT_ENOUGH_MEMORY;
	}
	
	ProgramContext->TweetStrings = Result;
	FileReaderContext->WritePointer = ProgramContext->TweetStrings + CurrentWritePosition;
	FileReaderContext->EndOfBlock = ((void *)ProgramContext->TweetStrings) + ProgramContext->TweetStringsSize;
	
	return NO_ERROR;
}

int ReadInFiles(PPROGRAM_CONTEXT ProgramContext, PFILE_READER_CONTEXT FileReaderContext)
{
	int Result = NO_ERROR;
	FileReaderContext->CurrentTweet = ProgramContext->NodeContext.Data;
	
	for(uint64_t FileID = 0; FileID < ProgramContext->NumberOfFiles; FileID++)
	{
		wprintf(L"Node %i: Reading In File %"PRIu64"\n", ProgramContext->NodeContext.NodeID, FileID);
		
		Result = ReadInFile(ProgramContext, FileReaderContext, FileID);
		if(Result != NO_ERROR)
		{
			return Result;
		}
		
		wprintf(L"Node %i: Done Reading In File %"PRIu64"\n", ProgramContext->NodeContext.NodeID, FileID);
	}
	
	return Result;
}

static int ReadInFile(PPROGRAM_CONTEXT ProgramContext, PFILE_READER_CONTEXT FileReaderContext, uint64_t FileID)
{
	int Result = NO_ERROR;
	
	//Get Memory for Filename + the point + the FileID
	uint32_t NumberOfDigits = log10(FileID) + 1;
	uint32_t FilenameLength = strlen(ProgramContext->Filename);
	char * FilenameBuffer = malloc(FilenameLength + NumberOfDigits + 1);
	
	sprintf(FilenameBuffer, "%s.%"PRIu64"", ProgramContext->Filename, FileID);
			
	//Open File
	FileReaderContext->File = fopen(FilenameBuffer, "r");
	if(FileReaderContext->File == NULL)
	{
		wprintf(L"Error on Node %i: Unable to open File %s\n", ProgramContext->NodeContext.NodeID, FilenameBuffer);
		free(FilenameBuffer);
		return ERROR_UNABLE_TO_OPEN_FILE;
	}
	
	free(FilenameBuffer);
	
	//Parse Lines
	for(; FileReaderContext->CurrentTweetNumber < ((FileID + 1) * ProgramContext->TweetsPerFile); FileReaderContext->CurrentTweetNumber++)
	{
		Result = ReadInLine(ProgramContext, FileReaderContext);
		if(Result != NO_ERROR)
		{
			return Result;
		}
	}
	
	//Close File
	fclose(FileReaderContext->File);
	
	return Result;
}

int ReadInLine(PPROGRAM_CONTEXT ProgramContext, PFILE_READER_CONTEXT FileReaderContext)
{
	int Result = NO_ERROR;
	bool NodeTweet = ((FileReaderContext->CurrentTweetNumber >= FileReaderContext->NodeTweetNumberStart) && (FileReaderContext->CurrentTweetNumber < FileReaderContext->NodeTweetNumberEnd));
	uint16_t TweetSize = 0;
	wint_t ReadCharacter;
	
	//Save Starting Offset
	if(NodeTweet)
	{
		FileReaderContext->CurrentTweet->TweetStringOffset = FileReaderContext->WritePointer - ProgramContext->TweetStrings;
	}
	
	//Read Tweet
	while((ReadCharacter = fgetwc(FileReaderContext->File)) != WEOF)
	{
		TweetSize++;
		if(ReadCharacter == '\n')
		{
			Result = WriteCharacterToString(ProgramContext, FileReaderContext, '\0');
			if(Result != NO_ERROR)
			{
				return Result;
			}
			break;
		}
		else
		{
			Result = WriteCharacterToString(ProgramContext, FileReaderContext, ReadCharacter);
			if(Result != NO_ERROR)
			{
				return Result;
			}
		}
	}
	
	if(ReadCharacter == WEOF)
	{
		TweetSize++;
		Result = WriteCharacterToString(ProgramContext, FileReaderContext, '\0');
		if(Result != NO_ERROR)
		{
			return Result;
		}
	}
	
	if(NodeTweet)
	{
		FileReaderContext->CurrentTweet->Size = TweetSize;
		FileReaderContext->CurrentTweet++;
	}
	
	return NO_ERROR;
}
int WriteCharacterToString(PPROGRAM_CONTEXT ProgramContext, PFILE_READER_CONTEXT FileReaderContext, wchar_t Charachter)
{
	if((void *)FileReaderContext->WritePointer > (FileReaderContext->EndOfBlock - sizeof(wchar_t)))
	{
		int Result = AllocateStringMemory(ProgramContext, FileReaderContext);
		if(Result != NO_ERROR)
		{
			return Result;
		}
	}
	
	(*FileReaderContext->WritePointer++) = Charachter;
	
	return NO_ERROR;
}
