#pragma once

#include <stdio.h>

#include "Program.h"

#define WRITE_DEBUG_INFO_TO_FILE

typedef struct _FILE_WRITER_CONTEXT
{	
	FILE * OutputFile;
	
} FILE_WRITER_CONTEXT, * PFILE_WRITER_CONTEXT;

int WriteOutResults(PPROGRAM_CONTEXT ProgramContext);