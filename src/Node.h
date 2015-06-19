#pragma once

#include "Int.h"
#include <stdbool.h>

#include "ForwardDeclarations.h"

#include "Benchmark.h"

typedef struct _NODE_CONTEXT
{
	int NodeID;
	int NumberOfNodes;
	uint64_t ElementsPerNode;
	
	BENCHMARK_CONTEXT BenchmarkContext;
	
	PTWEET Data;
	
} NODE_CONTEXT, * PNODE_CONTEXT;

void InitNodeContext(PNODE_CONTEXT NodeContext);
void FinalizeNodeContext(PNODE_CONTEXT NodeContext);
bool IsMasterNode(PNODE_CONTEXT NodeContext);