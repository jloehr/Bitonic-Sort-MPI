#include "Node.h"

#include <mpi.h>

void InitNodeContext(PNODE_CONTEXT NodeContext)
{
  MPI_Comm_rank(MPI_COMM_WORLD, &NodeContext->NodeID);
  MPI_Comm_size(MPI_COMM_WORLD, &NodeContext->NumberOfNodes);
}

bool IsMasterNode(PNODE_CONTEXT NodeContext)
{
  return (NodeContext->NodeID == 0);
}