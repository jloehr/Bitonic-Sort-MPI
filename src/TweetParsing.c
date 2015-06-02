#include "TweetParsing.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h> 

#include "Tweet.h"

int AllocateMemory(PPROGRAM_CONTEXT ProgramContext);
int ParseFiles(PPROGRAM_CONTEXT ProgramContext);
int ParseFile(PPROGRAM_CONTEXT ProgramContext, PTWEET * DataPointer, uint64_t FileID, uint64_t StartingLine, uint64_t LastLine);
void ParseTweetLine(PTWEET_PARSING_CONTEXT TweetParsingContext, const char * SearchTerm, PTWEET Tweet);

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
	
	double * EntropyPointer = Tweet->Entropy;
	wint_t ReadChar;
	uint64_t ConvertedChar;
	uint64_t PreviousChar;
	int64_t Difference; 
	uint64_t CurrentEntropy = 0;
	const char * SearchTermPointer = SearchTerm;
	
	while((ReadChar = fgetwc(TweetParsingContext->File)) != WEOF)
	{
		if(ReadChar == '\n')
		{
			break;
		}
		
		ConvertedChar = ReadChar;
		
		//Calculate Entropy
		if((Tweet->Size > 0))
		{
			Difference = (PreviousChar - ConvertedChar);
			CurrentEntropy += (Difference * Difference);
			(*EntropyPointer++) = sqrt(CurrentEntropy);	
		}
		
		
		//SearchTerm Substring
		if((ConvertedChar <= 0x7F) && ((*SearchTermPointer) == ConvertedChar))
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
		PreviousChar = ConvertedChar;
	}
}
