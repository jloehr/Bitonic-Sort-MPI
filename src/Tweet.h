#pragma once

#include <stdint.h>
#define TWEET_ENTROPY_SIZE 200


typedef struct _TWEET {
	
	uint8_t SearchTermValue;
	uint8_t Size;
	double Entropy[TWEET_ENTROPY_SIZE];
	
	uint8_t FileID;
	uint64_t PositionInFile;
	
} TWEET, * PTWEET;

void PrintTweet(PTWEET Tweet);
