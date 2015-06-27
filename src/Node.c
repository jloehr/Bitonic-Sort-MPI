#include "Node.h"

#include <stdlib.h>
#include <mpi.h>

#include "Tweet.h"

void InitNodeContext(PNODE_CONTEXT NodeContext)
{
  InitBenchmark(&NodeContext->BenchmarkContext);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &NodeContext->NodeID);
  MPI_Comm_size(MPI_COMM_WORLD, &NodeContext->NumberOfNodes);
  
  NodeContext->ElementsPerNode = 0;
  NodeContext->Data = NULL;
  NodeContext->DataBuffer = NULL;
  NodeContext->DataBufferSize = 0;
}

void FinalizeNodeContext(PNODE_CONTEXT NodeContext)
{ 
  if(NodeContext->Data != NULL)
  {
    free(NodeContext->Data);
    NodeContext->Data = NULL;
  }
}

bool IsMasterNode(PNODE_CONTEXT NodeContext)
{
  return (NodeContext->NodeID == 0);
}