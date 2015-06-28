#include "DataExchanger.h"

#include "Int.h"

#include "ErrorCodes.h"
#include "Program.h"
#include "Tweet.h"

int ExchangeTweetData(PPROGRAM_CONTEXT ProgramContext, int PartnerNodeID, bool LeftNode)
{
	uint64_t HalfTweetsPerNode = ProgramContext->NodeContext.ElementsPerNode/2;
	
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