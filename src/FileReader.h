#pragma once

#include <stdio.h>

#include "ForwardDeclarations.h"

#include "Program.h"

typedef struct _FILE_READER_CONTEXT
{
	FILE * File;	
	void * EndOfBlock;
	PWSTRING WritePointer;
	
} FILE_READER_CONTEXT, * PFILE_READER_CONTEXT;

int ReadInTweets(PPROGRAM_CONTEXT ProgramContext);