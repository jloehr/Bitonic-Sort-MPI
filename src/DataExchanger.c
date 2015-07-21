#include "DataExchanger.h"

#include "Int.h"
#include <stdlib.h>

#include "ErrorCodes.h"
#include "Program.h"
#include "Tweet.h"

int WaitForPartner(PPROGRAM_CONTEXT ProgramContext, int PartnerNodeID, bool LeftNode);

int ExchangeTweetData(PPROGRAM_CONTEXT ProgramContext, int PartnerNodeID, bool LeftNode)
{
	uint64_t HalfTweetsPerNode = ProgramContext->NodeContext.ElementsPerNode/2;
	
	WaitForPartner(ProgramContext, PartnerNodeID, LeftNode);

	StartNetworking(&ProgramContext->NodeContext.BenchmarkContext);
	
	if(LeftNode)
	{
		//Send
		MPI_Send(ProgramContext->NodeContext.Data + HalfTweetsPerNode, HalfTweetsPerNode, ProgramContext->MPITweetType, PartnerNodeID, 0, MPI_COMM_WORLD);
		
		//Recv
		MPI_Recv(ProgramContext->NodeContext.DataBuffer, HalfTweetsPerNode, ProgramContext->MPITweetType, PartnerNodeID, 0, MPI_COMM_WORLD, NULL);		
	}
	else
	{
		//Recv
		MPI_Recv(ProgramContext->NodeContext.DataBuffer, HalfTweetsPerNode, ProgramContext->MPITweetType, PartnerNodeID, 0, MPI_COMM_WORLD, NULL);	
		
		//Send
		MPI_Send(ProgramContext->NodeContext.Data, HalfTweetsPerNode, ProgramContext->MPITweetType, PartnerNodeID, 0, MPI_COMM_WORLD);	
	}
	
	StopNetworking(&ProgramContext->NodeContext.BenchmarkContext);

	return NO_ERROR;
}

int ExchangeTweetDataBack(PPROGRAM_CONTEXT ProgramContext, int PartnerNodeID, bool LeftNode)
{
	uint64_t HalfTweetsPerNode = ProgramContext->NodeContext.ElementsPerNode/2;

	WaitForPartner(ProgramContext, PartnerNodeID, LeftNode);

	StartNetworking(&ProgramContext->NodeContext.BenchmarkContext);
	
	if(LeftNode)
	{
		//Send
		MPI_Send(ProgramContext->NodeContext.DataBuffer, HalfTweetsPerNode, ProgramContext->MPITweetType, PartnerNodeID, 0, MPI_COMM_WORLD);
		
		//Recv
		MPI_Recv(ProgramContext->NodeContext.Data + HalfTweetsPerNode, HalfTweetsPerNode, ProgramContext->MPITweetType, PartnerNodeID, 0, MPI_COMM_WORLD, NULL);		
	}
	else
	{
		//Recv
		MPI_Recv(ProgramContext->NodeContext.Data, HalfTweetsPerNode, ProgramContext->MPITweetType, PartnerNodeID, 0, MPI_COMM_WORLD, NULL);	
		
		//Send
		MPI_Send(ProgramContext->NodeContext.DataBuffer, HalfTweetsPerNode, ProgramContext->MPITweetType, PartnerNodeID, 0, MPI_COMM_WORLD);	
	}
	
	StopNetworking(&ProgramContext->NodeContext.BenchmarkContext);


	return NO_ERROR;
}

int WaitForPartner(PPROGRAM_CONTEXT ProgramContext, int PartnerNodeID, bool LeftNode)
{
	MPI_Group TempGroup;
	MPI_Comm TempComm;

	StartNetworkingOverhead(&ProgramContext->NodeContext.BenchmarkContext);

	int * NodesForGroup = malloc(2 * sizeof(int));
	NodesForGroup[0] = LeftNode ? ProgramContext->NodeContext.NodeID : PartnerNodeID;
	NodesForGroup[1] = LeftNode ? PartnerNodeID : ProgramContext->NodeContext.NodeID;

	MPI_Group_incl(ProgramContext->NodeContext.WorldGroup, 2, NodesForGroup, &TempGroup);
	MPI_Comm_create(MPI_COMM_WORLD, TempGroup, &TempComm);

	MPI_Barrier(TempComm);

	MPI_Comm_free(&TempComm);
	MPI_Group_free(&TempGroup);
	free(NodesForGroup);

	StopNetworkingOverhead(&ProgramContext->NodeContext.BenchmarkContext);

	return 0;
}
