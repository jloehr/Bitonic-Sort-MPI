#include "Tweet.h"

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <mpi.h>
#include <stddef.h>
#include <string.h>

#include "Program.h"
#include "ErrorCodes.h"

void GetNextBiggerUnicode(PPROGRAM_CONTEXT ProgramContext, PUNICODE_APPEARANCE UnicodeAppearancePointer, const TWEET * Tweet, wchar_t PreviousUnicode);
uint16_t ParseTweetForUnicodeAppearance(PPROGRAM_CONTEXT ProgramContext, PUNICODE_APPEARANCE UnicodeAppearancePointer, const TWEET * Tweet);
void AddCharacterToUnicodeAppearance(wint_t ReadChar, PUNICODE_APPEARANCE UnicodeAppearance, uint16_t * NumberOfDifferentUnicodes);

int InitMPITweetType(PPROGRAM_CONTEXT ProgramContext)
{
	int Result = NO_ERROR;
	
	int BlockLength[4] = {2, 1, 1, 1};
	MPI_Datatype MPITypes[4] = { MPI_UINT8_T, MPI_WCHAR, MPI_UINT16_T, MPI_UINT64_T };
	MPI_Aint Offsets[4] = {offsetof(TWEET, SearchTermValue), offsetof(TWEET, SmallestUnicode) + offsetof(UNICODE_APPEARANCE, UnicodeCharacter), offsetof(TWEET, SmallestUnicode) + offsetof(UNICODE_APPEARANCE, NumberOfAppearance), offsetof(TWEET, TweetStringOffset)};
	
	Result = MPI_Type_create_struct(4, BlockLength, Offsets, MPITypes, &(ProgramContext->MPITweetType));
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
	
	UNICODE_APPEARANCE UnicodeAppearanceBuffer[ProgramContext->MaxTweetSize];
	uint16_t NumberOfDifferentUnicodes = ParseTweetForUnicodeAppearance(ProgramContext, UnicodeAppearanceBuffer, Tweet);
	
	for(PUNICODE_APPEARANCE UnicodeAppearance = UnicodeAppearanceBuffer; UnicodeAppearance != (UnicodeAppearanceBuffer + NumberOfDifferentUnicodes); UnicodeAppearance++)
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
		//Calculate Unicode Appearance
		UNICODE_APPEARANCE UnicodeAppearanceA = A->SmallestUnicode;
		UNICODE_APPEARANCE UnicodeAppearanceB = B->SmallestUnicode;
		
		while((UnicodeAppearanceA.UnicodeCharacter != WCHAR_MAX) && (UnicodeAppearanceB.UnicodeCharacter != WCHAR_MAX))
		{
			if(UnicodeAppearanceA.UnicodeCharacter != UnicodeAppearanceB.UnicodeCharacter)
			{
				return UnicodeAppearanceA.UnicodeCharacter - UnicodeAppearanceB.UnicodeCharacter;
			}
			else if(UnicodeAppearanceA.NumberOfAppearance != UnicodeAppearanceB.NumberOfAppearance)
			{
				return UnicodeAppearanceB.NumberOfAppearance - UnicodeAppearanceA.NumberOfAppearance;
			}
			
			GetNextBiggerUnicode(ProgramContext, &UnicodeAppearanceA, A, UnicodeAppearanceA.UnicodeCharacter);
			GetNextBiggerUnicode(ProgramContext, &UnicodeAppearanceB, B, UnicodeAppearanceB.UnicodeCharacter);
		}
		
		return UnicodeAppearanceA.UnicodeCharacter - UnicodeAppearanceB.UnicodeCharacter;	
	}
}

void GetNextBiggerUnicode(PPROGRAM_CONTEXT ProgramContext, PUNICODE_APPEARANCE UnicodeAppearancePointer, const TWEET * Tweet, wchar_t PreviousUnicode)
{
	PWSTRING ReadPointer = ProgramContext->TweetStrings + Tweet->TweetStringOffset;
	wchar_t UnicodeCharacter = WCHAR_MAX;
	uint16_t NumberOfAppearance = 0;
	
	for(wchar_t CurrentCharachter = (*ReadPointer);
		CurrentCharachter != '\0'; 
		CurrentCharachter = (*++ReadPointer))
	{
		if(CurrentCharachter <= PreviousUnicode)
		{
			continue;
		}
		
		if(CurrentCharachter < UnicodeCharacter)
		{
			UnicodeCharacter = CurrentCharachter;
			NumberOfAppearance = 1;
		} else if(CurrentCharachter == UnicodeCharacter)
		{
			NumberOfAppearance++;
		}
	}
	
	UnicodeAppearancePointer->UnicodeCharacter = UnicodeCharacter;
	UnicodeAppearancePointer->NumberOfAppearance = NumberOfAppearance;
}

uint16_t ParseTweetForUnicodeAppearance(PPROGRAM_CONTEXT ProgramContext, PUNICODE_APPEARANCE UnicodeAppearancePointer, const TWEET * Tweet)
{
	uint16_t NumberOfDifferentUnicodes = 0;
	PWSTRING ReadPointer = ProgramContext->TweetStrings + Tweet->TweetStringOffset;
	
	for(wchar_t CurrentCharachter = (*ReadPointer);
		CurrentCharachter != '\0'; 
		CurrentCharachter = (*++ReadPointer))
	{
		//Unicode Appearance
		AddCharacterToUnicodeAppearance(CurrentCharachter, UnicodeAppearancePointer, &NumberOfDifferentUnicodes);	
	}
	
	return NumberOfDifferentUnicodes;
}

void AddCharacterToUnicodeAppearance(wint_t ReadChar, PUNICODE_APPEARANCE UnicodeAppearance, uint16_t * NumberOfDifferentUnicodes)
{
	PUNICODE_APPEARANCE End = UnicodeAppearance + (*NumberOfDifferentUnicodes);

	for(uint16_t i = 0; 
		UnicodeAppearance < End; 
		UnicodeAppearance++, i++) 
	{	
		//If the exact Unicode is already in the List, increase its Appearance Number
		if(UnicodeAppearance->UnicodeCharacter == ReadChar)
		{
			UnicodeAppearance->NumberOfAppearance++;
			return;
		}	
		
		//Insert a new entry into the List, when the new Character is smaller than another
		if(UnicodeAppearance->UnicodeCharacter > ReadChar)
		{
			memmove(UnicodeAppearance + 1, UnicodeAppearance, ((*NumberOfDifferentUnicodes) - i) * sizeof(UNICODE_APPEARANCE));
			
			UnicodeAppearance->UnicodeCharacter = ReadChar;
			UnicodeAppearance->NumberOfAppearance = 1;
			(*NumberOfDifferentUnicodes)++;
			return;
		}
	}

	//If no entry was found and it is the biggest Unicode so far, append it
	UnicodeAppearance->UnicodeCharacter = ReadChar;
	UnicodeAppearance->NumberOfAppearance = 1;
	(*NumberOfDifferentUnicodes)++;
	
	return;
}
