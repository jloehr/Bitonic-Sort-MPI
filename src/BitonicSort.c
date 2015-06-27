#include "BitonicSort.h"

#include <sys/mman.h>
#include <stdlib.h> 

#include "ErrorCodes.h"
#include "Program.h"
#include "Tweet.h"
#include "Node.h"
#include "DataExchanger.h"

void QSort(PPROGRAM_CONTEXT ProgramContext);
int AllocateAndLockMemory(PNODE_CONTEXT NodeContext);
int UnlockAndFreeMemory(PNODE_CONTEXT NodeContext);
void BitonicSort(PPROGRAM_CONTEXT ProgramContext);
void BitonicMerge(PPROGRAM_CONTEXT ProgramContext, uint32_t NodesToMerge);
void BitonicCompare(PPROGRAM_CONTEXT ProgramContext, uint32_t NodesToMerge, bool Descending);

PPROGRAM_CONTEXT QsortProgramContext = NULL;

void Sort(PPROGRAM_CONTEXT ProgramContext)
{
     mlock(ProgramContext->UnicodeAppearances, ProgramContext->UnicodeAppearancesSize);
     AllocateAndLockMemory(&ProgramContext->NodeContext);
	
     QSort(ProgramContext);
     
    //Start Bitonic Sort
    BitonicSort(ProgramContext);
    
    UnlockAndFreeMemory(&ProgramContext->NodeContext);
    munlock(ProgramContext->UnicodeAppearances, ProgramContext->UnicodeAppearancesSize);   
}

void QSort(PPROGRAM_CONTEXT ProgramContext)
{
    QsortProgramContext = ProgramContext;
    
    bool Descending = ((ProgramContext->NodeContext.NodeID % 2) == 0);
    
    //Sort Node local via qsort
	qsort(ProgramContext->NodeContext.Data, ProgramContext->NodeContext.ElementsPerNode, sizeof(TWEET), Descending ? CompareTweetsDescQsort : CompareTweetsAscQsort);
  
    QsortProgramContext = NULL;
}

int AllocateAndLockMemory(PNODE_CONTEXT NodeContext)
{
    NodeContext->DataBufferSize = (NodeContext->ElementsPerNode / 2) * sizeof(TWEET);
    NodeContext->DataBuffer = malloc(NodeContext->DataBufferSize);
    if(NodeContext->DataBuffer == NULL)
    {
        return ERROR_OUT_OF_MEMORY;
    }
    
    NodeContext->BenchmarkContext.TweetDataBufferMemory = NodeContext->DataBufferSize;
    
    mlock(NodeContext->DataBuffer, NodeContext->DataBufferSize);
    
    return NO_ERROR;
}

int UnlockAndFreeMemory(PNODE_CONTEXT NodeContext)
{
    munlock(NodeContext->DataBuffer, NodeContext->DataBufferSize);
    free(NodeContext->DataBuffer);
    
    NodeContext->DataBuffer = NULL;
    NodeContext->DataBufferSize = 0;
    
    return NO_ERROR;
}

void BitonicSort(PPROGRAM_CONTEXT ProgramContext)
{
    for(uint32_t NodesToMerge = 2; NodesToMerge <= ProgramContext->NodeContext.NumberOfNodes; NodesToMerge *=2)
    {
        BitonicMerge(ProgramContext, NodesToMerge);
    }
}

void BitonicMerge(PPROGRAM_CONTEXT ProgramContext, uint32_t NodesToMerge)
{
    bool Descending = (((ProgramContext->NodeContext.NodeID / NodesToMerge) % 2) == 0);
    
    if(NodesToMerge != 1)
    {
        //BitonicCompare(NodesToMerge, ASC/DESC)
        BitonicMerge(ProgramContext, NodesToMerge/2);
    }
    else
    {
        //MergeSort(ASC/DESC);
    }
}

void BitonicCompare(PPROGRAM_CONTEXT ProgramContext, uint32_t NodesToMerge, bool Descending)
{
    // Get Partner NodeID
    bool LeftNode = (ProgramContext->NodeContext.NodeID % NodesToMerge) < (NodesToMerge/2);
    int PartnerNodeID = ProgramContext->NodeContext.NodeID;
    PartnerNodeID += LeftNode ? (NodesToMerge/2) : -(NodesToMerge/2);
    
    // Exchange Data
    
    // Compare
    
    // Exhcange Data
}