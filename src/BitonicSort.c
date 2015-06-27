#include "BitonicSort.h"

#include <sys/mman.h>
#include <stdlib.h> 

#include "Program.h"
#include "Tweet.h"
#include "Node.h"

void QSort(PPROGRAM_CONTEXT ProgramContext);

PPROGRAM_CONTEXT QsortProgramContext = NULL;


void Sort(PPROGRAM_CONTEXT ProgramContext)
{
     mlock(ProgramContext->UnicodeAppearances, ProgramContext->UnicodeAppearancesSize);
	
     QSort(ProgramContext);
     
    //Start Bitonic Sort
    
    munlock(ProgramContext->UnicodeAppearances, ProgramContext->UnicodeAppearancesSize);
    
}

void QSort(PPROGRAM_CONTEXT ProgramContext)
{
    QsortProgramContext = ProgramContext;
    
    bool Ascending = ((ProgramContext->NodeContext.NodeID % 2) == 1);
    
    //Sort Node local via qsort
	qsort(ProgramContext->NodeContext.Data, ProgramContext->NodeContext.ElementsPerNode, sizeof(TWEET), Ascending ? CompareTweetsAscQsort : CompareTweetsDescQsort);
  
    QsortProgramContext = NULL;
}
/*
void BitonicSort()
{
    // MaxMerges = log2(NumberOfNodes)
    // for ( i = MaxMerges; - 1; i > 0; i--)
    // {
    //      BitonicMegre(2^(MaxMerges - i));
    // }
}

void BitonicMerge(uint32_t NodesToMerge)
{
    //if(NodesToMerge != 1)
    {
    //BitonicCompare(NodesToMerge)
    //BitonicMerge(NodesToMerge/2)
    }
    //else
    //{
    //  MergeSort   
    //}
}
*/