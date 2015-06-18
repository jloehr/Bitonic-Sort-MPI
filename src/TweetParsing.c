#include "TweetParsing.h"

#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <stdio.h>
#include <math.h> 

#include "ErrorCodes.h"

#include "Tweet.h"

uint64_t UnicodeAppearanceMemory = 0;
uint64_t UnicodeAppearanceFields = 0;
uint64_t TweetStringMemory = 0;
uint64_t TweetMemory = 0;

int AllocateMemory(PPROGRAM_CONTEXT ProgramContext);
int ParseFiles(PPROGRAM_CONTEXT ProgramContext);
int ParseFile(PPROGRAM_CONTEXT ProgramContext, PTWEET * DataPointer, uint64_t FileID, uint64_t StartingLine, uint64_t LastLine);
int ParseTweetLine(PTWEET_PARSING_CONTEXT TweetParsingContext, const wchar_t * SearchTerm, PTWEET Tweet, PNODE_CONTEXT NodeContext);
void AddCharacterToUnicodeAppearance(wint_t ReadChar, PTWEET_PARSING_CONTEXT Tweet);


int ParseTweets(PPROGRAM_CONTEXT ProgramContext)
{	
	int Result = NO_ERROR;
	
	Result = AllocateMemory(ProgramContext);	
	if(Result != NO_ERROR)
	{
		return Result;
	}
		
	Result = ParseFiles(ProgramContext);
	if(Result != NO_ERROR)
	{
		return Result;
	}
	
	wprintf(L"UnicodeAppearanceMemory: %"PRIu64"\n",UnicodeAppearanceMemory);
	wprintf(L"UnicodeAppearanceFields: %"PRIu64"\n",UnicodeAppearanceFields);
	wprintf(L"Tweet String Memory: %"PRIu64"\n",TweetStringMemory);
	wprintf(L"TweetMemory: %"PRIu64"\n",TweetMemory);
	wprintf(L"----\n");
	wprintf(L"Total: %"PRIu64"\n",UnicodeAppearanceMemory + TweetStringMemory + TweetMemory);
	
	
	return Result;
}

int AllocateMemory(PPROGRAM_CONTEXT ProgramContext)
{
	//Tweets Per Node
	uint64_t TotalAmountOfTweets = ProgramContext->TweetsPerFile * ProgramContext->NumberOfFiles;
	
	ProgramContext->NodeContext.ElementsPerNode = TotalAmountOfTweets/ProgramContext->NodeContext.NumberOfNodes;
	
	uint64_t DataSize = ProgramContext->NodeContext.ElementsPerNode * sizeof(TWEET);
	
	//Allocate Space
	ProgramContext->NodeContext.Data = malloc(DataSize);
	TweetMemory += DataSize;
	
	if(IsMasterNode(&(ProgramContext->NodeContext)))
	{
        wprintf(L"Total Amount of Tweets: %"PRIu64"\n", TotalAmountOfTweets);
        wprintf(L"Tweets Per Node: %"PRIu64"\n", ProgramContext->NodeContext.ElementsPerNode);
        wprintf(L"Size per Tweet: %llu\n", sizeof(TWEET));
		wprintf(L"Allocated Space: %"PRIu64"\n", DataSize);
	}
	
	if(ProgramContext->NodeContext.Data == NULL)
	{
		wprintf(L"Node %i: failed to allocate Data Memory\n", ProgramContext->NodeContext.NodeID);
		return ERROR_NOT_ENOUGH_MEMORY;
	}
	
	return NO_ERROR;
}

int ParseFiles(PPROGRAM_CONTEXT ProgramContext)
{
	int Result = NO_ERROR;
	PTWEET DataPointer = ProgramContext->NodeContext.Data;
	
	uint64_t StartingTweetID = ProgramContext->NodeContext.NodeID * ProgramContext->NodeContext.ElementsPerNode;
	uint64_t EndTweetID = StartingTweetID + ProgramContext->NodeContext.ElementsPerNode - 1;
	
	uint64_t FirstFileID = StartingTweetID / ProgramContext->TweetsPerFile;
	uint64_t LastFileID = EndTweetID / ProgramContext->TweetsPerFile;
	
	for(uint64_t FileID = FirstFileID; FileID <= LastFileID; FileID++)
	{
		uint64_t StartingLine = (FileID == FirstFileID) ? (StartingTweetID - (FirstFileID * ProgramContext->TweetsPerFile)) : 0 ;
		uint64_t LastLine = (FileID == LastFileID) ? (EndTweetID - (LastFileID * ProgramContext->TweetsPerFile)) : ProgramContext->TweetsPerFile - 1;
		
		wprintf(L"Node %i: Parsing File %"PRIu64" from %"PRIu64" to %"PRIu64"\n", ProgramContext->NodeContext.NodeID, FileID, StartingLine, LastLine);
		
		Result = ParseFile(ProgramContext, &DataPointer, FileID, StartingLine, LastLine);
		if(Result != NO_ERROR)
		{
			return Result;
		}
		
		wprintf(L"Node %i: Done Parsing File %"PRIu64"\n", ProgramContext->NodeContext.NodeID, FileID);
		
	}
	
	return Result;
}

int ParseFile(PPROGRAM_CONTEXT ProgramContext, PTWEET * DataPointer, uint64_t FileID, uint64_t StartingLine, uint64_t LastLine)
{
	TWEET_PARSING_CONTEXT TweetParsingContext;
	TweetParsingContext.FileID = FileID;
	int Result = NO_ERROR;
	uint64_t LineNumber = 0;
	
	//Get Memory for Filename + the point + the FileID
	uint32_t NumberOfDigits = log10(FileID) + 1;
	uint32_t FilenameLength = strlen(ProgramContext->Filename);
	char * FilenameBuffer = malloc(FilenameLength + NumberOfDigits + 1);
	
	sprintf(FilenameBuffer, "%s.%"PRIu64"", ProgramContext->Filename, FileID);
	
	//Open File
	TweetParsingContext.File = fopen(FilenameBuffer, "r");
	if(TweetParsingContext.File == NULL)
	{
		wprintf(L"Error on Node %i: Unable to open File %s\n", ProgramContext->NodeContext.NodeID, FilenameBuffer);
		free(FilenameBuffer);
		return ERROR_UNABLE_TO_OPEN_FILE;
	}
	
	free(FilenameBuffer);
	
	//Go to Start Line
	while(LineNumber < StartingLine)
	{
		while(fgetc(TweetParsingContext.File) != '\n');
		LineNumber++;
	}
	
	//Parse Lines
	for(; LineNumber <= LastLine; LineNumber++)
	{
		ParseTweetLine(&TweetParsingContext, ProgramContext->SearchTerm, (*DataPointer), &ProgramContext->NodeContext);
		(*DataPointer)++;
	}
	
	//Close File
	fclose(TweetParsingContext.File);
	
	return Result;
}


int ParseTweetLine(PTWEET_PARSING_CONTEXT TweetParsingContext, const wchar_t * SearchTerm, PTWEET Tweet, PNODE_CONTEXT NodeContext)
{
	Tweet->Size = 0;
	Tweet->SearchTermValue = 0;
	
	TweetParsingContext->NumberOfDifferentUnicodes = 0;
	
	wint_t ReadChar;
	const wchar_t * SearchTermPointer = SearchTerm;
	wchar_t * WritePointer = TweetParsingContext->TweetBuffer; 
	
	while((ReadChar = fgetwc(TweetParsingContext->File)) != WEOF)
	{
		if(ReadChar == '\n')
		{
			(*WritePointer) = '\0'; 
			break;
		}
		
		(*WritePointer++) = ReadChar; 
		
		//Unicode Appearance
		AddCharacterToUnicodeAppearance(ReadChar, TweetParsingContext);
		
		//SearchTerm Substring
		//If unequal out pointer to first position so it is checked against first search term character
		if((*SearchTermPointer) != ReadChar)
		{
			SearchTermPointer = SearchTerm;
		}
		
		if((*SearchTermPointer) == ReadChar)
		{
			SearchTermPointer++;
			if((*SearchTermPointer) == '\0')
			{
				SearchTermPointer = SearchTerm;
				Tweet->SearchTermValue++;
			}
		}
	
		
		Tweet->Size++;
	}
	
	//Copy Unicode Apperance Array
	Tweet->NumberOfDifferentUnicodes = TweetParsingContext->NumberOfDifferentUnicodes;
	uint64_t NumberOfBytes = TweetParsingContext->NumberOfDifferentUnicodes * sizeof(UNICODE_APPEARANCE);
	Tweet->UnicodeAppearance = malloc(NumberOfBytes);
	if(Tweet->UnicodeAppearance == NULL)
	{
		wprintf(L"Error on Node %i: Out of Memory\n", NodeContext->NodeID);
		return ERROR_OUT_OF_MEMORY;
	}
	memcpy(Tweet->UnicodeAppearance, TweetParsingContext->UnicodeAppearance, NumberOfBytes);

	UnicodeAppearanceMemory += NumberOfBytes;
	UnicodeAppearanceFields += TweetParsingContext->NumberOfDifferentUnicodes;

	// Copy Tweet from Buffer
	// Tweet Size + 1 for the null terminator
	uint64_t NumberOfTweetBytes = (Tweet->Size + 1) * sizeof(wchar_t);
	Tweet->Tweet = malloc(NumberOfTweetBytes);
	if(Tweet->Tweet == NULL)
	{
		wprintf(L"Error on Node %i: Out of Memory\n", NodeContext->NodeID);
		return ERROR_OUT_OF_MEMORY;
	}
	
	memcpy(Tweet->Tweet, TweetParsingContext->TweetBuffer, NumberOfTweetBytes);
	
	TweetStringMemory += NumberOfTweetBytes;

	return NO_ERROR;
}

void AddCharacterToUnicodeAppearance(wint_t ReadChar, PTWEET_PARSING_CONTEXT Tweet)
{
	for(uint32_t i = 0; i < Tweet->NumberOfDifferentUnicodes; i++)
	{
		PUNICODE_APPEARANCE UnicodeAppearance = &(Tweet->UnicodeAppearance[i]);
		
		//If the exact Unicode is already in the List, increase its Appearance Number
		if(UnicodeAppearance->UnicodeCharacter == ReadChar)
		{
			UnicodeAppearance->NumberOfAppearance++;
			return;
		}	
		
		//Insert a new entry into the List, when the new Character is smaller than another
		if(UnicodeAppearance->UnicodeCharacter > ReadChar)
		{
			memmove(UnicodeAppearance + 1, UnicodeAppearance, (Tweet->NumberOfDifferentUnicodes - i) * sizeof(UNICODE_APPEARANCE));
			
			UnicodeAppearance->UnicodeCharacter = ReadChar;
			UnicodeAppearance->NumberOfAppearance = 1;
			return;
		}
	}

	//If no entry was found and it is the biggest Unicode so far, append it
	PUNICODE_APPEARANCE UnicodeAppearance = &(Tweet->UnicodeAppearance[Tweet->NumberOfDifferentUnicodes]);
	Tweet->NumberOfDifferentUnicodes++;
	
	UnicodeAppearance->UnicodeCharacter = ReadChar;
	UnicodeAppearance->NumberOfAppearance = 1;
}