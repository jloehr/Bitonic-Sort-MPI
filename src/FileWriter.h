#pragma once

#include "Int.h"
#include <stdio.h>

#include "ForwardDeclarations.h"

//#define WRITE_DEBUG_INFO_TO_FILE

typedef struct _INPUT_MMAP_FILE
{
	int FileHandle;
	void * Start;
	void * End;
	uint64_t Size;
	
} INPUT_MMAP_FILE, * PINPUT_MMAP_FILE;

typedef struct _FILE_WRITER_CONTEXT
{	
	FILE * OutputFile;
	PINPUT_MMAP_FILE InputFiles;
	
} FILE_WRITER_CONTEXT, * PFILE_WRITER_CONTEXT;

int WriteOutResults(PPROGRAM_CONTEXT ProgramContext);
