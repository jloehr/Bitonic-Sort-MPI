#include "BitonicSort.h"

#include <sys/mman.h>
#include <stdlib.h> 

#include "Program.h"
#include "Tweet.h"
#include "Node.h"

PPROGRAM_CONTEXT QsortProgramContext = NULL;

void Sort(PPROGRAM_CONTEXT ProgramContext, PNODE_CONTEXT NodeContext)
{
    QsortProgramContext = ProgramContext;
    
    mlock(ProgramContext->UnicodeAppearances, ProgramContext->UnicodeAppearancesSize);
	
    //Sort Node local via qsort
	qsort(NodeContext->Data, NodeContext->ElementsPerNode, sizeof(TWEET), CompareTweets);
    
    munlock(ProgramContext->UnicodeAppearances, ProgramContext->UnicodeAppearancesSize);
    
    QsortProgramContext = NULL;
}
/*
void BitonicSort()
{
	private int[] a = {5,10,2,4,88,6,3,2};
    // sorting direction:
    private final static boolean ASCENDING=true, DESCENDING=false;

    public void sort(int[] a)
    {
        this.a=a;
        bitonicSort(0, a.length, ASCENDING);
    }

    private void bitonicSort(int lo, int n, boolean dir)
    {
        if (n>1)
        {
            int m=n/2;
            bitonicSort(lo, m, ASCENDING);
            bitonicSort(lo+m, m, DESCENDING);
            bitonicMerge(lo, n, dir);
        }
    }

    private void bitonicMerge(int lo, int n, boolean dir)
    {
        if (n>1)
        {
            int m=n/2;
            for (int i=lo; i<lo+m; i++)
                compare(i, i+m, dir);
            bitonicMerge(lo, m, dir);
            bitonicMerge(lo+m, m, dir);
        }
    }

    private void compare(int i, int j, boolean dir)
    {
        if (dir==(a[i]>a[j]))
            exchange(i, j);
    }

    private void exchange(int i, int j)
    {
        int t=a[i];
        a[i]=a[j];
        a[j]=t;
    }
	
	Pfrintf(a)
	
}
*/