#include "FileWriter.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <stdio.h>
#include <math.h> 

#include "ErrorCodes.h"

#include "Tweet.h"



int OpenOutputFile(PPROGRAM_CONTEXT ProgramContext, PFILE_WRITER_CONTEXT FileWriterContext);
void WriteTweetsToFile(PNODE_CONTEXT NodeContext, PFILE_WRITER_CONTEXT FileWriterContext);
int MMapInputFiles(PPROGRAM_CONTEXT ProgramContext, PFILE_WRITER_CONTEXT FileWriterContext);
int MUnMapInputFiles(PPROGRAM_CONTEXT ProgramContext, PFILE_WRITER_CONTEXT FileWriterContext);

int WriteOutResults(PPROGRAM_CONTEXT ProgramContext)
{
	FILE_WRITER_CONTEXT FileWriterContext;
	int Result = NO_ERROR;
	
#ifdef SAVE_TWEET_POSITION
	//mmap all source files
	Result = MMapInputFiles(ProgramContext, &FileWriterContext);
	if(Result != NO_ERROR)
	{
		return Result;
	}	
#endif
	
	//Open File
	Result = OpenOutputFile(ProgramContext, &FileWriterContext);
	if(Result != NO_ERROR)
	{
		return Result;
	}	
	
	//Write all Elements
	WriteTweetsToFile(&ProgramContext->NodeContext, &FileWriterContext);
	
	//close file
	fclose(FileWriterContext.OutputFile);
	
#ifdef SAVE_TWEET_POSITION
	//unmmap all source files
	Result = MUnMapInputFiles(ProgramContext, &FileWriterContext);
	if(Result != NO_ERROR)
	{
		return Result;
	}	
#endif

	return Result;
	
}

int OpenOutputFile(PPROGRAM_CONTEXT ProgramContext, PFILE_WRITER_CONTEXT FileWriterContext)
{	
	//Get Memory for Filename + ".out." + NodeID
	uint32_t NumberOfDigits = log10(ProgramContext->NodeContext.NodeID) + 1;
	uint32_t FilenameLength = strlen(ProgramContext->Filename);
	char * FilenameBuffer = malloc(FilenameLength + NumberOfDigits + 5);
	
	sprintf(FilenameBuffer, "%s.out.%d", ProgramContext->Filename, ProgramContext->NodeContext.NodeID);
	
	//Open File
	FileWriterContext->OutputFile = fopen(FilenameBuffer, "w");
	if(FileWriterContext->OutputFile == NULL)
	{
		wprintf(L"Error on Node %i: Unable to open File %s\n", ProgramContext->NodeContext.NodeID, FilenameBuffer);
		free(FilenameBuffer);
		return ERROR_UNABLE_TO_OPEN_FILE;
	}
	
	free(FilenameBuffer);
	
	return NO_ERROR;
}


void WriteTweetsToFile(PNODE_CONTEXT NodeContext, PFILE_WRITER_CONTEXT FileWriterContext)
#ifdef SAVE_TWEET_POSITION
{
	PINPUT_MMAP_FILE SourceFile;
	char * ReadPointer;
	
	for(PTWEET Tweet = NodeContext->Data; Tweet != &(NodeContext->Data[NodeContext->ElementsPerNode]); Tweet++)
	{	
		
		SourceFile = &FileWriterContext->InputFiles[Tweet->FileID];
		ReadPointer = SourceFile->Start + Tweet->PositionInFile;
			
#ifdef WRITE_DEBUG_INFO_TO_FILE
		PrintTweetDebugInfoToStream(FileWriterContext->OutputFile, Tweet);
#endif	

		do {
			if(ReadPointer == SourceFile->End)
			{
				break;
			}
			
			fputc((*ReadPointer), FileWriterContext->OutputFile);
			
		} while((*ReadPointer++) != '\n');
	}
}
#else
{
	for(PTWEET Tweet = NodeContext->Data; Tweet != &(NodeContext->Data[NodeContext->ElementsPerNode]); Tweet++)
	{		
#ifdef WRITE_DEBUG_INFO_TO_FILE
		PrintTweetDebugInfoToStream(FileWriterContext->OutputFile, Tweet);
#endif	
		fputws(Tweet->Tweet, FileWriterContext->OutputFile);
		fputwc(L'\n', FileWriterContext->OutputFile);
	}	
}
#endif


int MMapInputFiles(PPROGRAM_CONTEXT ProgramContext, PFILE_WRITER_CONTEXT FileWriterContext)
{
	struct stat TmpFileStats;
	PINPUT_MMAP_FILE InputFile;
	
	//Allocate Memory for each input file
	FileWriterContext->InputFiles = calloc(ProgramContext->NumberOfFiles,  sizeof(INPUT_MMAP_FILE));
	if(FileWriterContext->InputFiles == NULL)
	{
		return ERROR_OUT_OF_MEMORY;
	}
	
	//Get Memory for Filename + "." + FileID
	uint32_t NumberOfDigits = log10(ProgramContext->NumberOfFiles) + 1;
	uint32_t FilenameLength = strlen(ProgramContext->Filename);
	char * FilenameBuffer = malloc(FilenameLength + NumberOfDigits + 1);

	for(uint8_t i = 0; i < ProgramContext->NumberOfFiles; i++)
	{
		InputFile = &FileWriterContext->InputFiles[i];
		//Get Filename
		sprintf(FilenameBuffer, "%s.%d", ProgramContext->Filename, i);
			
		//	get file size
		InputFile->FileHandle = open(FilenameBuffer, O_RDONLY);
		if(InputFile->FileHandle == -1)
		{
			wprintf(L"Error on Node %i: Unable to open File %s\n", ProgramContext->NodeContext.NodeID, FilenameBuffer);
			free(FilenameBuffer);
			MUnMapInputFiles(ProgramContext, FileWriterContext);
			return ERROR_UNABLE_TO_OPEN_FILE;
		}
		
		if(fstat(InputFile->FileHandle, &TmpFileStats) == -1)
		{
			wprintf(L"Error on Node %i: Unable to get File Stats %s\n", ProgramContext->NodeContext.NodeID, FilenameBuffer);
			free(FilenameBuffer);
			MUnMapInputFiles(ProgramContext, FileWriterContext);
			return ERROR_UNABLE_TO_GET_FILE_STATS;		
		}
		
		InputFile->Size = TmpFileStats.st_size;
		
		//  mmap
		InputFile->Start = mmap(NULL, InputFile->Size, PROT_READ, MAP_PRIVATE, InputFile->FileHandle, 0);
		if(InputFile->Start == MAP_FAILED)
		{
			wprintf(L"Error on Node %i: Unable map File %s\n", ProgramContext->NodeContext.NodeID, FilenameBuffer);
			free(FilenameBuffer);
			MUnMapInputFiles(ProgramContext, FileWriterContext);
			return ERROR_UNABLE_TO_MAP_FILE;			
		}
		
		InputFile->End = InputFile->Start + InputFile->Size;
		
	}
	
	free(FilenameBuffer);
	
	return NO_ERROR;
}

int MUnMapInputFiles(PPROGRAM_CONTEXT ProgramContext, PFILE_WRITER_CONTEXT FileWriterContext)
{
	PINPUT_MMAP_FILE InputFile;	
	
	for(uint8_t i = 0; i < ProgramContext->NumberOfFiles; i++)
	{
		InputFile = &FileWriterContext->InputFiles[i];
		
		if(InputFile->FileHandle == 0)
		{
			break;
		}
		
		munmap(InputFile->Start, InputFile->Size);
		close(InputFile->FileHandle);
		
	}
	
	return NO_ERROR;
}