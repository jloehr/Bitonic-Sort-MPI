#pragma once

#include <stdint.h>
#define TWEET_ENTROPY_SIZE 160


typedef struct _TWEET {
	
	uint32_t SearchTermValue;
	uint32_t Size;
	uint32_t Entropy[TWEET_ENTROPY_SIZE];
	
	uint32_t FileID;
	uint32_t PositionInFile;
	
} TWEET, * PTWEET;