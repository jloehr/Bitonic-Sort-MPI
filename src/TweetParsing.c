#include "TweetParsing.h"

#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <stdio.h>
#include <math.h> 

#include "Tweet.h"

int AllocateMemory(PPROGRAM_CONTEXT ProgramContext);
int ParseFiles(PPROGRAM_CONTEXT ProgramContext);
int ParseFile(PPROGRAM_CONTEXT ProgramContext, PTWEET * DataPointer, uint64_t FileID, uint64_t StartingLine, uint64_t LastLine);
void ParseTweetLine(PTWEET_PARSING_CONTEXT TweetParsingContext, const char * SearchTerm, PTWEET Tweet);
#ifdef UNICODE_APPEARANCE_ARRAY
void AddCharacterToUnicodeAppearance(wint_t ReadChar, PTWEET Tweet);
#else
void AddCharacterToUnicodeAppearance(wint_t ReadChar, PTWEET_PARSING_CONTEXT Tweet);
#endif

int ParseTweets(PPROGRAM_CONTEXT ProgramContext)
{	
	int Result = 0;
	
	Result = AllocateMemory(ProgramContext);	
	if(Result != 0)
	{
		return Result;
	}
		
	Result = ParseFiles(ProgramContext);
	if(Result != 0)
	{
		return Result;
	}
	
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
	
	if(IsMasterNode(&(ProgramContext->NodeContext)))
	{
        printf("Total Amount of Tweets: %llu\n", TotalAmountOfTweets);
        printf("Tweets Per Node: %i\n", ProgramContext->NodeContext.ElementsPerNode);
        printf("Size per Tweet: %lu\n", sizeof(TWEET));
		printf("Allocated Space: %llu\n", DataSize);
	}
	
	if(ProgramContext->NodeContext.Data == NULL)
	{
		printf("Node %i: failed to allocate Data Memory\n", ProgramContext->NodeContext.NodeID);
		return 2;
	}
	
	return 0;
}

int ParseFiles(PPROGRAM_CONTEXT ProgramContext)
{
	int Result = 0;
	PTWEET DataPointer = ProgramContext->NodeContext.Data;
	
	uint64_t StartingTweetID = ProgramContext->NodeContext.NodeID * ProgramContext->NodeContext.ElementsPerNode;
	uint64_t EndTweetID = StartingTweetID + ProgramContext->NodeContext.ElementsPerNode - 1;
	
	uint64_t FirstFileID = StartingTweetID / ProgramContext->TweetsPerFile;
	uint64_t LastFileID = EndTweetID / ProgramContext->TweetsPerFile;
	
	for(uint64_t FileID = FirstFileID; FileID <= LastFileID; FileID++)
	{
		uint64_t StartingLine = (FileID == FirstFileID) ? (StartingTweetID - (FirstFileID * ProgramContext->TweetsPerFile)) : 0 ;
		uint64_t LastLine = (FileID == LastFileID) ? (EndTweetID - (LastFileID * ProgramContext->TweetsPerFile)) : ProgramContext->TweetsPerFile - 1;
		
		printf("Node %i: Parsing File %llu from %llu to %llu\n", ProgramContext->NodeContext.NodeID, FileID, StartingLine, LastLine);
		
		Result = ParseFile(ProgramContext, &DataPointer, FileID, StartingLine, LastLine);
		if(Result != 0)
		{
			return Result;
		}
		
	}
	
	return Result;
}

int ParseFile(PPROGRAM_CONTEXT ProgramContext, PTWEET * DataPointer, uint64_t FileID, uint64_t StartingLine, uint64_t LastLine)
{
	TWEET_PARSING_CONTEXT TweetParsingContext;
	TweetParsingContext.FileID = FileID;
	int Result = 0;
	uint64_t LineNumber = 0;
	
	//Get Memory for Filename + the point + the FileID
	uint32_t NumberOfDigits = log10(FileID) + 1;
	uint32_t FilenameLength = strlen(ProgramContext->Filename);
	char * FilenameBuffer = malloc(FilenameLength + NumberOfDigits + 1);
	
	sprintf(FilenameBuffer, "%s.%llu", ProgramContext->Filename, FileID);
	
	//Open File
	TweetParsingContext.File = fopen(FilenameBuffer, "r");
	if(TweetParsingContext.File == NULL)
	{
		printf("Error on Node %i: Unable to open File %s\n", ProgramContext->NodeContext.NodeID, FilenameBuffer);
		free(FilenameBuffer);
		return 3;
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
		ParseTweetLine(&TweetParsingContext, ProgramContext->SearchTerm, (*DataPointer));
		(*DataPointer)++;
	}
	
	//Close File
	fclose(TweetParsingContext.File);
	
	return Result;
}


void ParseTweetLine(PTWEET_PARSING_CONTEXT TweetParsingContext, const char * SearchTerm, PTWEET Tweet)
{
	Tweet->FileID = TweetParsingContext->FileID;
	Tweet->PositionInFile = ftell(TweetParsingContext->File);
	Tweet->Size = 0;
	Tweet->SearchTermValue = 0;
	
	TweetParsingContext->NumberOfDifferentUnicodes = 0;
	
	wint_t ReadChar;
	const char * SearchTermPointer = SearchTerm;
	
	while((ReadChar = fgetwc(TweetParsingContext->File)) != WEOF)
	{
		if(ReadChar == '\n')
		{
			break;
		}
		
		//Unicode Appearance
#ifdef UNICODE_APPEARANCE_ARRAY
		AddCharacterToUnicodeAppearance(ReadChar, Tweet);
#else
		AddCharacterToUnicodeAppearance(ReadChar, TweetParsingContext);
#endif
		
		//SearchTerm Substring
		if((ReadChar <= 0x7F) && ((*SearchTermPointer) == ReadChar))
		{
			SearchTermPointer++;
			if((*SearchTermPointer) == '\0')
			{
				SearchTermPointer = SearchTerm;
				Tweet->SearchTermValue++;
			}
		}
		else
		{
			SearchTermPointer = SearchTerm;
		}
		
		Tweet->Size++;
	}
	
#ifndef UNICODE_APPEARANCE_ARRAY
	Tweet->NumberOfDifferentUnicodes = TweetParsingContext->NumberOfDifferentUnicodes;
	uint64_t NumberOfBytes = TweetParsingContext->NumberOfDifferentUnicodes * sizeof(UNICODE_APPEARANCE);
	Tweet->UnicodeAppearance = malloc(NumberOfBytes);
	if(Tweet->UnicodeAppearance == NULL)
	{
		printf("Out of Memory");
		abort();
	}
	memcpy(Tweet->UnicodeAppearance, TweetParsingContext->UnicodeAppearance, NumberOfBytes);
#endif
}
#ifdef UNICODE_APPEARANCE_ARRAY
void AddCharacterToUnicodeAppearance(wint_t ReadChar, PTWEET Tweet)
#else
void AddCharacterToUnicodeAppearance(wint_t ReadChar, PTWEET_PARSING_CONTEXT Tweet)
#endif
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
		}
	}

	//If no entry was found and it is the biggest Unicode so far, append it
	PUNICODE_APPEARANCE UnicodeAppearance = &(Tweet->UnicodeAppearance[Tweet->NumberOfDifferentUnicodes]);
	Tweet->NumberOfDifferentUnicodes++;
	
	UnicodeAppearance->UnicodeCharacter = ReadChar;
	UnicodeAppearance->NumberOfAppearance = 1;
}