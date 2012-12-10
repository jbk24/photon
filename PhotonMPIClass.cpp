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

int PhotonMPIClass::symChunkCommInit() //Initializes buffers and counters for symChunkComm, call once per timestep
{
	//Reset current message count to 0
	messageCount = 0;
	
	//Initalize request and status buffers
	
	sendRequest = new MPI_Request[edgeCount];
	recvRequest = new MPI_Request[edgeCount];
	sendStatus = new MPI_Status[edgeCount];
	recvStatus = new MPI_Status[edgeCount];
	
	return 0;
	
}

int PhotonMPIClass::symChunkComm(double *sendData, double* recvData, int count, int origin, int dest) //Symmetric chunk communication using MPI_Isend and MPI_Irecv
{
	
	unsigned int originX = gid2x(origin,Simulation.numChunks.x);
	unsigned int originY = gid2y(origin,Simulation.numChunks.x);
	unsigned int destX = gid2x(dest,Simulation.numChunks.x);
	unsigned int destY = gid2y(dest,Simulation.numChunks.x);
	
	//Deltas used to determine which direction message is going
	int deltaX = destX - originX;
	int deltaY = destY - originY;
	int sendTag, recvTag; // Message tags
	int ierr, errorFlag; //Error handling
	
	//Determine tags for outgoing message (send) and incoming message (recieve)
	if (deltaX != 0) //x-traveling message
	{
		switch (deltaX)
		{
			case -1: //negative-x outgoing message
				sendTag = tagXn + origin;
				recvTag = tagXp + dest;
				break;
			case 1: //positive-x outgoing message
				sendTag = tagXp + origin;
				recvTag = tagXn + dest;
				break;
		}
	}
	else //y-traveling message
	{
		switch (deltaY)
		{
			case -1: //negative-y outgoing message
				sendTag = tagYn + origin;
				recvTag = tagYp + dest;
				break;
			case 1: //positive-y oytgoing message
				sendTag = tagYp + origin;
				recvTag = tagYn + dest;
				break;
		}
	}
	
	cout << "Rank: " << rank << endl;
	cout << "Orign: " << origin << "  Dest: " << dest << "  DeltaY: " << deltaY <<  endl;
	cout << "TagYn: " << tagYn << endl;
	cout << "TagYp: " << tagYp << endl;
	cout << "sendTag: " << sendTag << endl;
	cout << "recvTag: " << recvTag << endl;
	cout << endl;
	
	//Asynchronous send
	ierr = MPI_Isend(sendData, count, MPI_DOUBLE, ChunkMap[dest].processor, sendTag, MPI_COMM_WORLD, &sendRequest[messageCount]);
	if(ierr != MPI_SUCCESS)
	{
		printf("MPI_Isend error code: %d", ierr);
		errorFlag = 1;
	}
	
	//Asynchronous recieve
	ierr = MPI_Irecv(recvData, count, MPI_DOUBLE, ChunkMap[dest].processor, recvTag, MPI_COMM_WORLD, &recvRequest[messageCount]);
	if(ierr != MPI_SUCCESS)
	{
		printf("MPI_Irecv error code: %d", ierr);
		errorFlag = 1;
	}
	messageCount++;
	if(errorFlag)
		return 1;
	else
		return 0;
}

int PhotonMPIClass::symChunkCommWait() //Waits on all outstanding symChunkComm asynchronous sends and recieves
{
	int ierr, errorFlag;
	//Wait on sends
	cout << "Rank: " << rank << "Wait for sends..." << endl;
	ierr = MPI_Waitall(edgeCount,sendRequest, sendStatus);
	if(ierr != MPI_SUCCESS)
	{
		printf("MPI_Waitall send error code: %d", ierr);
		errorFlag = 1;
	}
	
	//Wait on recives
	cout << "Rank: " << rank << "Wait for recieves..." << endl;
	ierr = MPI_Waitall(edgeCount,recvRequest, recvStatus);
	//ierr = MPI_Wait(&recvRequest[0], &recvStatus[0]);
	if(ierr != MPI_SUCCESS)
	{
		printf("MPI_Waitall recv error code: %d", ierr);
		errorFlag = 1;
	}
	cout << "Done waiting... " << endl;
	
	if(errorFlag)
		return 1;
	else
		return 0;
}



