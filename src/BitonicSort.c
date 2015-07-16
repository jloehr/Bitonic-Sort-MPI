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
void GetMergeSortStartingPoints(PPROGRAM_CONTEXT ProgramContext, PTWEET * LeftPointer, PTWEET * RightPointer, bool Down);

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
    PTWEET LeftEdge = LeftPointer;
    PTWEET RightPointer = ProgramContext->NodeContext.Data + ProgramContext->NodeContext.ElementsPerNode - 1;
    PTWEET RightEdge = RightPointer;
    PTWEET BufferPointer = ProgramContext->NodeContext.DataBuffer;
    
    if(!FillBufferFromFront)
    {
        BufferPointer += ProgramContext->NodeContext.ElementsPerNode - 1;
    }
    
    //Adjust offset
    GetMergeSortStartingPoints(ProgramContext, &LeftPointer, &RightPointer, Down);
    
    for(uint64_t i = 0; i < ProgramContext->NodeContext.ElementsPerNode; i++)
    {
        bool LeftGreater = (CompareTweetsDesc(LeftPointer, RightPointer, ProgramContext) < 0);
        
        if(!(Down ^ LeftGreater))
        {
            //Copy Left
            (*BufferPointer) = (*LeftPointer);
            
            LeftPointer = (LeftPointer == RightEdge) ? LeftEdge : LeftPointer + 1;
        }
        else
        {
            //Copy Right
            (*BufferPointer) = (*RightPointer);
            
            RightPointer = (RightPointer == LeftEdge) ? RightEdge : RightPointer - 1;
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

void GetMergeSortStartingPoints(PPROGRAM_CONTEXT ProgramContext, PTWEET * LeftPointer, PTWEET * RightPointer, bool Down)
{
    bool LeftEdgeUp = false;
    bool RightEdgeUp = false;
    bool SeekFromLeft;
    bool SeekDirectionUp;
    PTWEET LeftTmpPointer = *LeftPointer;
    PTWEET RightTmpPointer = *RightPointer;
    PTWEET LeftSeekPointer;
    PTWEET RightSeekPointer;
    PTWEET SeekEnd;
    int Result;
    
    //Check if Adjustment is required
    for(; LeftTmpPointer != ((*RightPointer) - 1); LeftTmpPointer++)
    {
        int Result = CompareTweetsDesc(LeftTmpPointer, LeftTmpPointer + 1, ProgramContext);
        
        if(Result != 0)
        {
            LeftEdgeUp = (Result > 0);
            break;
        }
    }
    
    for(; RightTmpPointer != ((*LeftPointer) + 1); RightTmpPointer--)
    {
        int Result = CompareTweetsDesc(RightTmpPointer - 1, RightTmpPointer, ProgramContext);
        
        if(Result != 0)
        {
            RightEdgeUp = (Result < 0);
            break;
        }
    }
    
    if(LeftEdgeUp == RightEdgeUp)
    {
        //Nothing to adjust
        wprintf(L"Nothing to do: %d\n", ProgramContext->NodeContext.NodeID);
        return;
    }
    
    Result = CompareTweetsDesc(LeftTmpPointer, RightTmpPointer, ProgramContext);
    if((LeftEdgeUp && (Result > 0)) || (!LeftEdgeUp && (Result < 0)))
    {
        // Straight Line, so nothing to adjust
        wprintf(L"Simple Line: %d\n", ProgramContext->NodeContext.NodeID);
        return;
    }
    
    //Get Offset Peek
    SeekFromLeft = (LeftEdgeUp == Down);
    SeekDirectionUp = SeekFromLeft ? LeftEdgeUp : RightEdgeUp;
    LeftSeekPointer = SeekFromLeft ? LeftTmpPointer : RightTmpPointer - 1;
    RightSeekPointer = SeekFromLeft ? LeftTmpPointer + 1 : RightTmpPointer;
    SeekEnd = SeekFromLeft ? RightTmpPointer : LeftTmpPointer;
    
    while((SeekFromLeft && (RightSeekPointer != SeekEnd)) || (!SeekFromLeft && (LeftSeekPointer != SeekEnd)))
    {
        //Compare
        int Result = CompareTweetsDesc(LeftSeekPointer, RightSeekPointer, ProgramContext);
        
        //Check if Extrema
        if(Result != 0)
        {
            bool Up = SeekFromLeft ? (Result > 0) : (Result < 0);
            
            if(SeekDirectionUp != Up)
            {
                //Maxima found
                (*LeftPointer) = RightSeekPointer;
                (*RightPointer) = LeftSeekPointer;
                wprintf(L"Starting adjusted: %d\n", ProgramContext->NodeContext.NodeID);
                return;
            }
        }
        
        //Move Pointer
        if(SeekFromLeft)
        {
           LeftSeekPointer++; 
           RightSeekPointer++;
        }
        else
        {
           LeftSeekPointer--; 
           RightSeekPointer--;   
        }
    }
    
    //If no Extrema is found something went wrong
    wprintf(L"Seeking went wrong on Node: %d\n", ProgramContext->NodeContext.NodeID);
    
    
    
    /*
    
Node 2: Start with Up
Node 2: Change to Down at 57
Node 2: Change to Up at 8173
Node 2: End with Up
Node 2: 1 - 0
May be Shifted: 2
Extrema should be Down
*/
/*
Node 1: Start with Down
Node 1: Change to Up at 56
Node 1: Change to Down at 8215
Node 3: Sorted correct: true
Node 1: End with Down
Node 1: 0 - 1
May be Shifted: 1
Extrema should be Up
*/
/*
    
    bool Print = ProgramContext->NodeContext.NodeID == 1;
    
   
    
    if(Print)
    {
        wprintf(L"Node %d: Start with %s\n", ProgramContext->NodeContext.NodeID, LeftEdgeUp ? "Up" : "Down");
        bool TmpUpDirection = LeftEdgeUp;
        for(uint64_t i = 0; i < ProgramContext->NodeContext.ElementsPerNode - 1; i++)
        {
            int Result = CompareTweetsDesc(ProgramContext->NodeContext.Data + i, ProgramContext->NodeContext.Data + i + 1, ProgramContext);
            
            if(Result != 0)
            {
                bool StillUpDirection = (Result > 0);
                
                if(TmpUpDirection != StillUpDirection)
                {
                    TmpUpDirection = StillUpDirection;
                    wprintf(L"Node %d: Change to %s at %d\n", ProgramContext->NodeContext.NodeID, TmpUpDirection ? "Up" : "Down", i);
                }
            }  
        }
        
        wprintf(L"Node %d: End with %s\n", ProgramContext->NodeContext.NodeID, TmpUpDirection ? "Up" : "Down");
       
        wprintf(L"Node %d: %d - %d\n", ProgramContext->NodeContext.NodeID, LeftEdgeUp, RightEdgeUp);
        
        if(LeftEdgeUp != RightEdgeUp)
        {
            wprintf(L"May be Shifted: %d\n", ProgramContext->NodeContext.NodeID);
            int Result = CompareTweetsDesc(LeftTmpPointer, RightTmpPointer, ProgramContext);
            
            if((LeftEdgeUp && (Result > 0)) || (!LeftEdgeUp && (Result < 0)))
            {
                wprintf(L"Simple Line: %d\n", ProgramContext->NodeContext.NodeID);
            }
        } 
        
        wprintf(L"Extrema should be %s\n", Down ? "Down" : "Up");
        
        wprintf(L"--------\n");
    }
*/
}