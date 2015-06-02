#include "Tweet.h"

#include <stdio.h>

void PrintTweet(PTWEET Tweet)
{
	double OverallEntropy = (Tweet->Size > 1) ? Tweet->Entropy[Tweet->Size - 2] : 0.f;
	printf("%llu\t%u\t%u\t%f\n", Tweet->PositionInFile, Tweet->Size, Tweet->SearchTermValue, OverallEntropy);
}