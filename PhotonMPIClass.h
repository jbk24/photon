#ifndef PHOTONMPICLASS_H
#define PHOTONMPICLASS_H
/*This class abstracts out calls to MPI, as well as storing MPI related data (rank, size etc. etc.) for use elsewhere*/
#include <mpi.h>

class PhotonMPIClass
{
public:
	PhotonMPIClass();
	~PhotonMPIClass();

	//Parameters
	
	int rank; //Rank of current process
	int size; //Total number of processes in MPI_COMM_WORLD

	int tagXn; //positive-x going messages
	int tagXp; //negative-x going messages
	int tagYn; //positive y going messages
	int tagYp; //negative-y going messages

	//Comm
	MPI_Request* sendRequest; //array of send message requests
	MPI_Request* recvRequest; //array of recv message requests
	MPI_Status* sendStatus; //status buffer for sent mesages
	MPI_Status* recvStatus; //status buffer for recieved messages
	int messageCount; //number of chunk messages sent on current timestep
	int edgeCount; //Number of comm edges on current chunk, upated during AMR
	
	//Functions
	int initialize(int argc, char *argv[]); //Initialize MPI system, call once at beginning
	int finalize(); //Terminates MPI system, other MPI cleanup, call once at end
	int createMPIType(int count, int array_of_blocklengths[], int array );
	int symChunkCommInit(); //Initializes buffers and counters for symChunkComm, call once per timestep
	int symChunkComm(double *sendData, double *recvData, int count, int origin, int dest); //Symmetric chunk communication using MPI_Isend and MPI_Irecv
	int symChunkCommWait(); //Waits on all outstanding symChunkComm asynchronous sends and recieves
	void allocateChunkTags(); //Determine base tags for chunk message addressing 
	
};

#endif // PHOTONMPICLASS_H
