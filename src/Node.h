#pragma once

#include <stdbool.h>

typedef struct _NODE_CONTEXT
{
	int NodeID;
	int NumberOfNodes;
	
} NODE_CONTEXT, * PNODE_CONTEXT;

void InitNodeContext(PNODE_CONTEXT NodeContext);
bool IsMasterNode(PNODE_CONTEXT NodeContext);