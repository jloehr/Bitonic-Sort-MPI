#include "Tweet.h"

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <mpi.h>
#include <stddef.h>

#include "Program.h"
#include "ErrorCodes.h"

int InitMPITweetType(PPROGRAM_CONTEXT ProgramContext)
{
	int Result = NO_ERROR;
	
	int BlockLength[3] = {2, 1, 2};
	MPI_Datatype MPITypes[3] = { MPI_UINT8_T, MPI_UINT16_T, MPI_UINT64_T };
	MPI_Aint Offsets[3] = {offsetof(TWEET, SearchTermValue), offsetof(TWEET, NumberOfDifferentUnicodes), offsetof(TWEET, UnicodeAppearanceOffset)};
	
	Result = MPI_Type_create_struct(3, BlockLength, Offsets, MPITypes, &(ProgramContext->MPITweetType));
	if(Result != MPI_SUCCESS)
	{
		return ERROR_MPI_TWEET_DATATYPE;	
	}
	
	Result = MPI_Type_commit(&(ProgramContext->MPITweetType));
	if(Result != MPI_SUCCESS)
	{
		return ERROR_MPI_TWEET_DATATYPE;	
	}	
	
	return NO_ERROR;
}

void PrintTweet(PPROGRAM_CONTEXT ProgramContext, PTWEET Tweet)
{
	PrintTweetDebugInfoToStream(stdout, ProgramContext, Tweet);
	wprintf(L"%S\n", ProgramContext->TweetStrings + Tweet->TweetStringOffset);
}

void PrintTweetDebugInfoToStream(FILE * Stream, PPROGRAM_CONTEXT ProgramContext, PTWEET Tweet)
{
	fwprintf(Stream, L"%3u\t%2u\t", Tweet->Size, Tweet->SearchTermValue);
	
	for(PUNICODE_APPEARANCE UnicodeAppearance = ProgramContext->UnicodeAppearances + Tweet->UnicodeAppearanceOffset; UnicodeAppearance != (ProgramContext->UnicodeAppearances + Tweet->UnicodeAppearanceOffset + Tweet->NumberOfDifferentUnicodes); UnicodeAppearance++)
	{
		fwprintf(Stream, L"%4x: %3u\t", UnicodeAppearance->UnicodeCharacter, UnicodeAppearance->NumberOfAppearance);
	}
}

extern PPROGRAM_CONTEXT QsortProgramContext;

int CompareTweetsAscQsort(const void * a, const void * b)
{
	return CompareTweetsAsc(a, b, QsortProgramContext);
}

int CompareTweetsDescQsort(const void * a, const void * b)
{
	return CompareTweetsDesc(a, b, QsortProgramContext);
}

int CompareTweetsAsc(const void * a, const void * b, PPROGRAM_CONTEXT ProgramContext)
{
	return CompareTweetsDesc(b, a, ProgramContext);
}

int CompareTweetsDesc(const void * a, const void * b, PPROGRAM_CONTEXT ProgramContext)
{	
	const TWEET * A = a;
	const TWEET * B = b;
	
	if(A->SearchTermValue != B->SearchTermValue)
	{
		return B->SearchTermValue - A->SearchTermValue;
	}
	else
	{
		uint8_t MinIndex = A->NumberOfDifferentUnicodes < B->NumberOfDifferentUnicodes ? A->NumberOfDifferentUnicodes : B->NumberOfDifferentUnicodes;	
		
		PUNICODE_APPEARANCE UnicodeAppearanceA, UnicodeAppearanceB;
		
		for(UnicodeAppearanceA = ProgramContext->UnicodeAppearances + A->UnicodeAppearanceOffset,
			UnicodeAppearanceB = ProgramContext->UnicodeAppearances + B->UnicodeAppearanceOffset; 
			UnicodeAppearanceA != (ProgramContext->UnicodeAppearances + A->UnicodeAppearanceOffset + MinIndex);
			UnicodeAppearanceA++, UnicodeAppearanceB++)
		{
			if(UnicodeAppearanceA->UnicodeCharacter != UnicodeAppearanceB->UnicodeCharacter)
			{
				return UnicodeAppearanceA->UnicodeCharacter - UnicodeAppearanceB->UnicodeCharacter;
			}
			else if(UnicodeAppearanceA->NumberOfAppearance != UnicodeAppearanceB->NumberOfAppearance)
			{
				return UnicodeAppearanceB->NumberOfAppearance - UnicodeAppearanceA->NumberOfAppearance;
			}
		}
		
		return 	B->NumberOfDifferentUnicodes - A->NumberOfDifferentUnicodes;
	}
}