#include "BitonicSort.h"

#include <sys/mman.h>
#include <stdlib.h> 

#include "ErrorCodes.h"
#include "Program.h"
#include "Tweet.h"
#include "Node.h"
#include "DataExchanger.h"

void QSort(PPROGRAM_CONTEXT ProgramContext);
int AllocateAndLockMemory(PPROGRAM_CONTEXT ProgramContext);
int UnlockAndFreeMemory(PPROGRAM_CONTEXT ProgramContext);
void BitonicSort(PPROGRAM_CONTEXT ProgramContext);
void BitonicMerge(PPROGRAM_CONTEXT ProgramContext, uint32_t NodesToMerge, bool Descending);
void BitonicCompare(PPROGRAM_CONTEXT ProgramContext, uint32_t NodesToMerge, bool Descending);
void MergeSort(PPROGRAM_CONTEXT ProgramContext, bool Descending);

PPROGRAM_CONTEXT QsortProgramContext = NULL;

void Sort(PPROGRAM_CONTEXT ProgramContext)
{
    AllocateAndLockMemory(ProgramContext);
	
    QSort(ProgramContext);
     
    //Start Bitonic Sort
    BitonicSort(ProgramContext);
    
    UnlockAndFreeMemory(ProgramContext);
}

void QSort(PPROGRAM_CONTEXT ProgramContext)
{
    StartQsort(&ProgramContext->NodeContext.BenchmarkContext);
    QsortProgramContext = ProgramContext;
    
    bool Descending = ((ProgramContext->NodeContext.NodeID % 2) == 0);
    
    //Sort Node local via qsort
	qsort(ProgramContext->NodeContext.Data, ProgramContext->NodeContext.ElementsPerNode, sizeof(TWEET), Descending ? CompareTweetsDescQsort : CompareTweetsAscQsort);
  
    QsortProgramContext = NULL;
    StopQsort(&ProgramContext->NodeContext.BenchmarkContext);
}

int AllocateAndLockMemory(PPROGRAM_CONTEXT ProgramContext)
{
    uint64_t DataBufferSize = ProgramContext->NodeContext.ElementsPerNode * sizeof(TWEET);
   
    ProgramContext->NodeContext.DataBuffer = malloc(DataBufferSize);
    if(ProgramContext->NodeContext.DataBuffer == NULL)
    {
        return ERROR_OUT_OF_MEMORY;
    }
    
    mlock(ProgramContext->NodeContext.DataBuffer, DataBufferSize);
    
    return NO_ERROR;
}

int UnlockAndFreeMemory(PPROGRAM_CONTEXT ProgramContext)
{
    uint64_t DataBufferSize = ProgramContext->NodeContext.ElementsPerNode * sizeof(TWEET);
    
    munlock(ProgramContext->NodeContext.DataBuffer, DataBufferSize);
    
    free(ProgramContext->NodeContext.DataBuffer);
    
    ProgramContext->NodeContext.DataBuffer = NULL;
   
    return NO_ERROR;
}

void BitonicSort(PPROGRAM_CONTEXT ProgramContext)
{
    // Benchmark Start
    StartBitonicCompare(&ProgramContext->NodeContext.BenchmarkContext);
    
    for(uint32_t NodesToMerge = 2; NodesToMerge <= ProgramContext->NodeContext.NumberOfNodes; NodesToMerge *=2)
    {
        bool Descending = (((ProgramContext->NodeContext.NodeID / NodesToMerge) % 2) == 0);
        BitonicMerge(ProgramContext, NodesToMerge, Descending);
    }
    
    // Benchmark Stop
    StopBitonicCompare(&ProgramContext->NodeContext.BenchmarkContext);
}

void BitonicMerge(PPROGRAM_CONTEXT ProgramContext, uint32_t NodesToMerge, bool Descending)
{   
    if(NodesToMerge != 1)
    {
        BitonicCompare(ProgramContext, NodesToMerge, Descending);
        BitonicMerge(ProgramContext, NodesToMerge/2, Descending);
    }
    else
    {
        StartMergeSort(&ProgramContext->NodeContext.BenchmarkContext);
        MergeSort(ProgramContext, Descending);
        StopMergeSort(&ProgramContext->NodeContext.BenchmarkContext);
    }
}

void BitonicCompare(PPROGRAM_CONTEXT ProgramContext, uint32_t NodesToMerge, bool Descending)
{
    // Get Partner NodeID
    bool LeftNode = (ProgramContext->NodeContext.NodeID % NodesToMerge) < (NodesToMerge/2);
    int PartnerNodeID = ProgramContext->NodeContext.NodeID;
    PartnerNodeID += LeftNode ? (NodesToMerge/2) : -(NodesToMerge/2);
    
    // Exchange Data
    ExchangeTweetData(ProgramContext, PartnerNodeID, LeftNode);
    
    // Compare
    PTWEET FirstHalf = LeftNode ? ProgramContext->NodeContext.Data : ProgramContext->NodeContext.DataBuffer;
    PTWEET SecondHalf = LeftNode ? ProgramContext->NodeContext.DataBuffer : ProgramContext->NodeContext.Data + (ProgramContext->NodeContext.ElementsPerNode / 2);
    
    for(uint64_t i = 0; i < (ProgramContext->NodeContext.ElementsPerNode / 2); i++, FirstHalf++, SecondHalf++)
    {
        int CompareResult = CompareTweetsDesc(FirstHalf, SecondHalf, ProgramContext);
        
        if((Descending && (CompareResult > 0)) || (!Descending && (CompareResult < 0)))
        {
            TWEET Temp = (*FirstHalf);
            (*FirstHalf) = (*SecondHalf);
            (*SecondHalf) = Temp;
        }
    }
    
    // Exhcange Data
    ExchangeTweetDataBack(ProgramContext, PartnerNodeID, LeftNode);
}

void MergeSort(PPROGRAM_CONTEXT ProgramContext, bool Descending)
{
    bool EvenNode = ((ProgramContext->NodeContext.NodeID % 2) == 0);
    bool Down = !(EvenNode ^ Descending);
    bool FillBufferFromFront = EvenNode;
    
    PTWEET LeftPointer = ProgramContext->NodeContext.Data;
    PTWEET RightPointer = ProgramContext->NodeContext.Data + ProgramContext->NodeContext.ElementsPerNode - 1;
    PTWEET BufferPointer = ProgramContext->NodeContext.DataBuffer;
      
    if(!FillBufferFromFront)
    {
        BufferPointer += ProgramContext->NodeContext.ElementsPerNode - 1;
    }
    
    for(uint64_t i = 0; i < ProgramContext->NodeContext.ElementsPerNode; i++)
    {
        bool LeftGreater = (CompareTweetsDesc(LeftPointer, RightPointer, ProgramContext) < 0);
        
        if(!(Down ^ LeftGreater))
        {
            //Copy Left
            (*BufferPointer) = (*LeftPointer);
            LeftPointer++;
        }
        else
        {
            //Copy Right
            (*BufferPointer) = (*RightPointer);
            RightPointer--;
        }
        
        if(FillBufferFromFront)
        {
            BufferPointer++;
        }
        else
        {
            BufferPointer--;
        }      
    }
    
    PTWEET Temp = ProgramContext->NodeContext.Data;
    ProgramContext->NodeContext.Data = ProgramContext->NodeContext.DataBuffer;
    ProgramContext->NodeContext.DataBuffer = Temp;
}