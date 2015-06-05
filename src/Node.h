#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "Tweet.h"
#include "Benchmark.h"

typedef struct _NODE_CONTEXT
{
	int NodeID;
	int NumberOfNodes;
	
	BENCHMARK_CONTEXT BenchmarkContext;
	
	uint64_t ElementsPerNode;
	
	PTWEET Data;
	
} NODE_CONTEXT, * PNODE_CONTEXT;

void InitNodeContext(PNODE_CONTEXT NodeContext);
void FinalizeNodeContext(PNODE_CONTEXT NodeContext);
bool IsMasterNode(PNODE_CONTEXT NodeContext);