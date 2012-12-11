#include "PhotonMPIClass.h"
#include "global.h"
#include "utilities.h"
#include <mpi.h>
#include <iostream>

PhotonMPIClass::PhotonMPIClass()
{
}

PhotonMPIClass::~PhotonMPIClass()
{
}

//Initailize MPI system. Call once, returns 1 if error, 0 otherwise.
int PhotonMPIClass::initialize(int argc, char *argv[])
{

	int ierr; //Error return code
	int errorFlag = 0; //Flag to indicate error has occured

	//Start MPI
	ierr = MPI_Init (&argc, &argv);	
	if(ierr != MPI_SUCCESS)
	{
		printf("MPI_Init error code: %d", ierr);
		errorFlag = 1;
	}
	
	//Get rank
	ierr = MPI_Comm_rank (MPI_COMM_WORLD, &rank);	
	if(ierr != MPI_SUCCESS)
	{
		printf("MPI_Comm_rank error code: %d", ierr);
		errorFlag = 1;
	}
	
	//Get number of processes
	ierr = MPI_Comm_size (MPI_COMM_WORLD, &size);
	if(ierr != MPI_SUCCESS)
	{
		printf("MPI_Comm_rank error code: %d", ierr);	
		errorFlag = 1;
	}

	
	return errorFlag;
}

//Terminate MPI system, do any other cleanup. Call once at end. 
int PhotonMPIClass::finalize()
{
	int ierr; //Error return code
	int errorFlag = 0; //Flag to indicate error has occured
	
	ierr = MPI_Finalize();
	if(ierr != MPI_SUCCESS)
	{
		printf("MPI_Finalize error code: %d", ierr);	
		errorFlag = 1;
	}
	
	return errorFlag;
}

void PhotonMPIClass::allocateChunkTags()
{
	/* This function allocates the base address for the 
	 * four types of messages that can be transmitted (in 2D).
	 * All chunk-to-chunk comm is tagged by the sender with a
	 * tag unique to the senders chunk gid and the direction
	 * the message is going (x+, x-, y+, y-). Message recipiants
	 * must determine the corresponding tag to set up recv. The
	 * tag for a message is thus the gid for that chunk pluse 
	 * the base address for the direction the message is traveling.*/
	 
	 //Get total number of chunks
	int totalChunks = Simulation.numChunks.x * Simulation.numChunks.y;
	
	tagXn = 0; // 0 : (totalChunks-1)
	tagXp = totalChunks; // totalChunks : (2*toalChunks-1)
	tagYn = 2*totalChunks; // 2*totalChunks : (3*totalChunks-1)
	tagYp = 3*totalChunks; //3*totalChunks : (4*totalCHunks-1)
	 
}

int PhotonMPIClass::chunkCommInit() //Initializes buffers and counters for symChunkComm, call once per timestep
{
	//Reset current message counters to 0
	msgSendCount = 0;
	msgRecvCount = 0;
	
	//Initalize request and status buffers
	sendRequest = new MPI_Request[edgeCount];
	recvRequest = new MPI_Request[edgeCount];
	sendStatus = new MPI_Status[edgeCount];
	recvStatus = new MPI_Status[edgeCount];
	
	return 0;
	
}

int PhotonMPIClass::chunkComm(int sendFlag, double *Data, int count, int local, int remote) //Chunk-to-chunk communication using MPI_Isend and MPI_Irecv
{
	
	unsigned int localX = gid2x(local,Simulation.numChunks.x);
	unsigned int localY = gid2y(local,Simulation.numChunks.x);
	unsigned int remoteX = gid2x(remote,Simulation.numChunks.x);
	unsigned int remoteY = gid2y(remote,Simulation.numChunks.x);
	
	//Deltas used to determine which direction message is going
	int deltaX = remoteX - localX;
	int deltaY = remoteY - localY;
	int sendTag, recvTag; // Message tags
	int ierr, errorFlag; //Error handling
	
	//Determine tags for outgoing message (send) and incoming message (recieve)
	if (deltaX != 0) //x-traveling message
	{
		switch (deltaX)
		{
			case -1: //negative-x outgoing message
				sendTag = tagXn + local;
				recvTag = tagXp + remote;
				break;
			case 1: //positive-x outgoing message
				sendTag = tagXp + local;
				recvTag = tagXn + remote;
				break;
		}
	}
	else //y-traveling message
	{
		switch (deltaY)
		{
			case -1: //negative-y outgoing message
				sendTag = tagYn + local;
				recvTag = tagYp + remote;
				break;
			case 1: //positive-y oytgoing message
				sendTag = tagYp + local;
				recvTag = tagYn + remote;
				break;
		}
	}
	
	cout << "Rank: " << rank << endl;
	cout << "Local: " << local << "  Remote: " << remote << "  DeltaY: " << deltaY <<  endl;
	cout << "TagYn: " << tagYn << endl;
	cout << "TagYp: " << tagYp << endl;
	cout << "sendTag: " << sendTag << endl;
	cout << "recvTag: " << recvTag << endl;
	cout << endl;
	 
	
	if(sendFlag) //send data
	
	{
		//Asynchronous send
		ierr = MPI_Isend(Data, count, MPI_DOUBLE, ChunkMap[remote].processor, sendTag, MPI_COMM_WORLD, &sendRequest[msgSendCount]);
		if(ierr != MPI_SUCCESS)
		{
			printf("MPI_Isend error code: %d", ierr);
			errorFlag = 1;
		}
		msgSendCount++;
	}
	else //recieve data
	{
		//Asynchronous recieve
		ierr = MPI_Irecv(Data, count, MPI_DOUBLE, ChunkMap[remote].processor, recvTag, MPI_COMM_WORLD, &recvRequest[msgRecvCount]);
		if(ierr != MPI_SUCCESS)
		{
		printf("MPI_Irecv error code: %d", ierr);
		errorFlag = 1;
		}
		msgRecvCount++;
	}
	
	if(errorFlag) //Handle errors
		return 1;
	else
		return 0;
}

int PhotonMPIClass::chunkCommWait() //Waits on all outstanding chunkComm asynchronous sends and recieves
{
	int ierr, errorFlag;
	if(msgSendCount)
	{
	//Wait on sends
	cout << "Rank: " << rank << "Wait for sends..." << endl;
	ierr = MPI_Waitall(msgSendCount,sendRequest, sendStatus);
	if(ierr != MPI_SUCCESS)
	{
		printf("MPI_Waitall send error code: %d", ierr);
		errorFlag = 1;
	}
	}

	if(msgRecvCount)
	{
	//Wait on recives
	cout << "Rank: " << rank << "Wait for recieves..." << endl;
	ierr = MPI_Waitall(msgRecvCount,recvRequest, recvStatus);
	if(ierr != MPI_SUCCESS)
	{
		printf("MPI_Waitall recv error code: %d", ierr);
		errorFlag = 1;
	}
	cout << "Done waiting... " << endl;
	
	}
	
	if(errorFlag) //Handle errors
		return 1;
	else
		return 0;
		

}



