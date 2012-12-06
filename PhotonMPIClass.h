#ifndef PHOTONMPICLASS_H
#define PHOTONMPICLASS_H
/*This class abstracts out calls to MPI, as well as storing MPI related data (rank, size etc. etc.) for use elsewhere*/


class PhotonMPIClass
{
public:
	PhotonMPIClass();
	~PhotonMPIClass();

	//Parameters
	
	int rank; //Rank of current process
	int size; //Total number of processes in MPI_COMM_WORLD
	int test;

	
	//Functions
	
	int initialize(int argc, char *argv[]); //Initialize MPI system, call once at beginning
	
	int finalize(); //Terminates MPI system, other MPI cleanup, call once at end
	
	int createMPIType(int count, int array_of_blocklengths[], int array );

};

#endif // PHOTONMPICLASS_H
