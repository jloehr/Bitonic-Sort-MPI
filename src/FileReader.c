#include "FileReader.h"

#include "Int.h"
#include <stdlib.h>
#include <string.h>
#include <math.h> 

#include "ErrorCodes.h"

#include "Benchmark.h"

static int InitFileReaderContext(PPROGRAM_CONTEXT ProgramContext, PFILE_READER_CONTEXT FileReaderContext);
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
	
	Result = AllocateStringMemory(ProgramContext, &FileReaderContext);
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
	
	return NO_ERROR;
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
	for(uint64_t LineNumber = 0; LineNumber < ProgramContext->TweetsPerFile; LineNumber++)
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
	wint_t ReadCharacter;
	uint16_t TweetSize = 0;
	
	//Read Tweet
	while((ReadCharacter = fgetwc(FileReaderContext->File)) != WEOF)
	{
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
			
			TweetSize++;
		}
	}
	
	if(ReadCharacter == WEOF)
	{
		Result = WriteCharacterToString(ProgramContext, FileReaderContext, '\0');
		if(Result != NO_ERROR)
		{
			return Result;
		}
	}
	
	if(ProgramContext->MaxTweetSize < TweetSize)
	{
		ProgramContext->MaxTweetSize = TweetSize;	
	}
	
	return NO_ERROR;
}
int WriteCharacterToString(PPROGRAM_CONTEXT ProgramContext, PFILE_READER_CONTEXT FileReaderContext, wchar_t Charachter)
{
	while((void *)FileReaderContext->WritePointer > (FileReaderContext->EndOfBlock - sizeof(wchar_t)))
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
