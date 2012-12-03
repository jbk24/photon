#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{

	//Instantiate classes
	PhotonMPIClass PhotonMPI;
	
	//Create map of chunks
	//ChunkClass chunkMap[rows][cols];
	
	
	//Initialize MPI
	if(PhotonMPI.initialize(argc,argv))
		printf("Error occured in MPI initialization.");
	
	
	
	printf("hello world. Rank = %d, Size = %d. \n", PhotonMPI.rank, PhotonMPI.size);
	
	
	//Terminate MPI
	if(PhotonMPI.finalize())
		printf("Error occured in MPI finalize.");
	
	return 0;
	
	
}
