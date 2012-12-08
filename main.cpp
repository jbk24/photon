#include <stdio.h>
#include <iostream>
#include "global.h"


int main(int argc, char *argv[])
{
	

	//Create map of chunks
	//ChunkClass chunkMap[rows][cols];
	
	
	//Initialize MPI
	if(PhotonMPI.initialize(argc,argv))
		printf("Error occured in MPI initialization.");
	
	
	printf("hello bruce. Rank = %d, Size = %d. \n", PhotonMPI.rank, PhotonMPI.size);
	
	
	//Read input file
	Simulation.readControl(argv[argc-1]);
	
	//Terminate MPI
	if(PhotonMPI.finalize())
		printf("Error occured in MPI finalize.");
	
	return 0;
	
	
}
