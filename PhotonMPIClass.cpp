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

void PhotonMPIClass::allocateAddressSpace()
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
	tagAddSpace = totalChunks*4;
	
	//Directional tags
	tagXn = 0; // 0 : (totalChunks-1)
	tagXp = 1*totalChunks; // totalChunks : (2*toalChunks-1)
	tagYn = 2*totalChunks; // 2*totalChunks : (3*totalChunks-1)
	tagYp = 3*totalChunks; //3*totalChunks : (4*totalCHunks-1)
	
	//Base address for message content based on data type, see header for definitions
	for(int k = 0; k<7; k++)
		dataTypeAdd[k] = tagAddSpace*k;

}

int PhotonMPIClass::chunkCommInit() //Initializes buffers and counters for symChunkComm, call once per timestep
{
	//Reset current message counters to 0
	msgSendCount = 0;
	msgRecvCount = 0;
	
	int bufSize = edgeCount * 6;
	
	//Initalize request and status buffers
	sendRequest = new MPI_Request[bufSize];
	recvRequest = new MPI_Request[bufSize];
	sendStatus = new MPI_Status[bufSize];
	recvStatus = new MPI_Status[bufSize];
	
	//Initalize recievedData list
	recievedData.resize(bufSize);
	
	return 0;
	
}

int PhotonMPIClass::sendChunkComm(int gid, int dataType, int edge) //Send data to another chunk
{
	
	//MPI error handling
	int ierr, errorFlag;
	
	//Gid of recieving chunk
	unsigned int remote = ChunkMap[gid].neighbor[edge];
 
	//Message tag
	int sendTag;
	
	//Message size
	int msgSize;
	
	//Get tag for message based on direction
	switch (edge)
	{
		case 0: //Send on negative-x edge, message traveling x-negative
			sendTag = tagXn + gid + dataTypeAdd[dataType];
			msgSize = ChunkMap[gid].arraySize.y-2;
			break;
		case 1: //Send on positive-x edge, message traveling x-positive
			sendTag = tagXp + gid + dataTypeAdd[dataType];
			msgSize = ChunkMap[gid].arraySize.y-2;
			break;
		case 2: //Recieve on negative-y edge, message traveling y-negative
			sendTag = tagYn + gid + dataTypeAdd[dataType];
			msgSize = ChunkMap[gid].arraySize.x-2;
			break;
		case 3: //Recieve on positive-y edge, message traveling y-positive
			sendTag = tagYp + gid + dataTypeAdd[dataType];
			msgSize = ChunkMap[gid].arraySize.x-2;
			break;
	}
		
		
	//Get source array
	double *sourceArray = getDataPointerFromType(gid, dataType);

	//Get current send buffer number
	int bufNum = ChunkMap[gid].curSendBuf;
	//cout << "bufNum: " << bufNum << endl;
	
	//Get data buffer pointer
	double *data = ChunkMap[gid].sendBuffers[bufNum].dataBuf;
	
	//Set number of elements in sendBuffer
	ChunkMap[gid].sendBuffers[bufNum].local.end.x = msgSize;
	
	//cout << "Rank: " << PhotonMPI.rank <<" Transfer data to send buffer "<< bufNum << endl;
	
	//Copy data into buffer
	//oneWayInternalArrayCopy(sourceArray, data, ChunkMap[gid].ownEdge[edge], ChunkMap[gid].sendBuffers[bufNum].local);
	arrayIntoVectorCopy(data, sourceArray,ChunkMap[gid].ownEdge[edge]);
	
	//cout << "Rank: " << PhotonMPI.rank <<" Data transfer to send buffer complete"<< endl;
	
	//Increment current send buffer
	ChunkMap[gid].curSendBuf++;
	
	//cout << "Rank: " << PhotonMPI.rank <<" Sending message size: "<< msgSize << endl;

	//Asynchronous send
	ierr = MPI_Isend(data, msgSize, MPI_DOUBLE, ChunkMap[remote].processor, sendTag, MPI_COMM_WORLD, &sendRequest[msgSendCount]);
	ierr = 0;
	if(ierr != MPI_SUCCESS)
	{
		printf("MPI_Isend error code: %d", ierr);
		errorFlag = 1;
	}
		msgSendCount++;
		
	if(errorFlag) //Handle errors
		return 1;
	else
		return 0;
}


int PhotonMPIClass::recvChunkComm(int gid, int dataType, int edge) //Recieve data from another chunk
{
	//MPI error handling
	int ierr, errorFlag;
	
	//Gid of sending chunk
	unsigned int remote = ChunkMap[gid].neighbor[edge];
 
	//Message tag
	int recvTag;
	
	//Message size
	int msgSize;

	//cout << "Edge: "<< edge << endl;

	
	//Get tag for message based on direction
	switch (edge)
	{
		case 0: //Recieve on negative-x edge, message traveling x-positive
			recvTag = tagXp + remote + dataTypeAdd[dataType];
			msgSize = ChunkMap[gid].arraySize.y-2;
			break;
		case 1: //Recieve on positive-x edge, message traveling x-negative
			recvTag = tagXn + remote + dataTypeAdd[dataType];
			msgSize = ChunkMap[gid].arraySize.y-2;
			break;
		case 2: //Recieve on negative-y edge, message traveling y-positive
			recvTag = tagYp + remote + dataTypeAdd[dataType];
			msgSize = ChunkMap[gid].arraySize.x-2;
			break;
		case 3: //Recieve on positive-y edge, mesage traveling y-negative
			recvTag = tagYn + remote + dataTypeAdd[dataType];
			msgSize = ChunkMap[gid].arraySize.x-2;
			break;
	}
	
	//Get current recieve buffer number
	int bufNum = ChunkMap[gid].curRecvBuf;
	
	//Get data buffer pointer
	double *data = ChunkMap[gid].recvBuffers[bufNum].dataBuf;
	
	//Set number of elements in recvBuffers
	ChunkMap[gid].recvBuffers[bufNum].local.end.x = msgSize;
	
	//Set data type (epsilon, sigmaX, sigmaY, Ezx, Ezy, Hx, Hy)
	ChunkMap[gid].recvBuffers[bufNum].dataType = dataType;
	
	//Set target for recieved data
	ChunkMap[gid].recvBuffers[bufNum].remote = ChunkMap[gid].overlapEdge[edge];
	
	//Increment current recv buffer
	ChunkMap[gid].curRecvBuf++;
	
	//data = &ChunkMap[gid].recvBuffers[bufNum].dataBuf[0];
	
	//cout << "Rank: " << PhotonMPI.rank <<" Recieving message size: "<< msgSize << endl;
	
	
	//Asynchronous recieve
	ierr = MPI_Irecv(data, msgSize, MPI_DOUBLE, ChunkMap[remote].processor, recvTag, MPI_COMM_WORLD, &recvRequest[msgRecvCount]);
	if(ierr != MPI_SUCCESS)
	{
	printf("MPI_Irecv error code: %d", ierr);
	errorFlag = 1;
	}
	
	if(errorFlag) //Handle errors
		return 1;
	else
	{
		//Register recieve in recievedData vector
		recievedData[msgRecvCount].chunk = gid;
		recievedData[msgRecvCount].dataBuffer = bufNum;
		msgRecvCount++;
		return 0;
	}
}


int PhotonMPIClass::chunkCommWait() //Waits on all outstanding sesndComm and recvComm
{
	//Truncate recievedData to only include msgRecvCount elements
	recievedData.resize(msgRecvCount);
	
	int ierr, errorFlag;
	if(msgSendCount)
	{
		//Wait on sends
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
		ierr = MPI_Waitall(msgRecvCount,recvRequest, recvStatus);
		if(ierr != MPI_SUCCESS)
		{
			printf("MPI_Waitall recv error code: %d", ierr);
			errorFlag = 1;
		}
	}
	
	if(errorFlag) //Handle errors
		return 1;
	else
		return 0;
		

}



