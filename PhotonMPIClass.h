#ifndef PHOTONMPICLASS_H
#define PHOTONMPICLASS_H
/*This class abstracts out calls to MPI, as well as storing MPI related data (rank, size etc. etc.) for use elsewhere*/
#include <mpi.h>
#include <vector>
#include "RecvDataClass.h"

using namespace std;

class PhotonMPIClass
{
public:
	PhotonMPIClass();
	~PhotonMPIClass();

	//Parameters
	int rank; //Rank of current process
	int size; //Total number of processes in MPI_COMM_WORLD

	//Addressing
	unsigned int tagXn; //positive-x going messages
	unsigned int tagXp; //negative-x going messages
	unsigned int tagYn; //positive y going messages
	unsigned int tagYp; //negative-y going messages
	unsigned int tagAddSpace; //space required for sending in each direction
	
	unsigned int dataTypeAdd [7]; //base address for  messages, depending on data type:
	//0 : epsilon
	//1 : sigmaX
	//2 : sigmaY
	//3 : Ezx
	//4 : Ezy
	//5 : Hx
	//6 : Hy
	
	//Comm
	MPI_Request* sendRequest; //array of send message requests
	MPI_Request* recvRequest; //array of recv message requests
	MPI_Status* sendStatus; //status buffer for sent mesages
	MPI_Status* recvStatus; //status buffer for recieved messages
	int msgSendCount; //number of chunk messages sent on current timestep
	int msgRecvCount; //number of chunk messages recieved on current timestep
	int edgeCount; // Toal number of comm edges on current chunk, upated during AMR
	vector<RecvDataClass> recievedData; //List of all data being recived in current MPI cycle
	
	
	
	//Functions
	int initialize(int argc, char *argv[]); //Initialize MPI system, call once at beginning
	int finalize(); //Terminates MPI system, other MPI cleanup, call once at end
	int createMPIType(int count, int array_of_blocklengths[], int array );
	int chunkCommInit(); //Initializes buffers and counters for symChunkComm, call once per timestep
	int sendChunkComm(int gid, int dataType, int edge); //Send data to another chunk
	int recvChunkComm(int gid, int dataType, int edge); //Recieve data from another chunk
	int chunkCommWait(); //Waits on all outstanding symChunkComm asynchronous sends and recieves
	void allocateAddressSpace(); //Determine base tags for chunk message addressing 
	
};

#endif // PHOTONMPICLASS_H
