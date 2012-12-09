#include <stdio.h>
#include <iostream>
#include "global.h"
#include "utilities.h"
#include "csvFileIO.h"

int main(int argc, char *argv[])
{
	
	//Initialize MPI
	if(PhotonMPI.initialize(argc,argv))
		printf("Error occured in MPI initialization.");
	
	//Read input file
	Simulation.readControl(argv[argc-1]);
	
	//Create map of chunks
	if(instantiateChunkMap())
		printf("Error occured in chunk map initialization.");
	
	cout << "X chunks: "<< Simulation.numChunks.x << "\n";
	cout << "Y chunks: "<< Simulation.numChunks.y << "\n";
	
	
	
	//Read epsilon and sigma
	readEpsSigmaCSV();
	
	
	//printf("hello bruce. Rank = %d, Size = %d. \n", PhotonMPI.rank, PhotonMPI.size);
	
	

	//Perform global cleanup
	globalCleanUp();
	
	//Terminate MPI
	if(PhotonMPI.finalize())
		printf("Error occured in MPI finalize.");
	
	return 0;
	
	
}
