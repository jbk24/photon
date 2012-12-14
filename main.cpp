#include <stdio.h>
#include <iostream>
#include <string.h>
#include "global.h"
#include "utilities.h"
#include "csvFileIO.h"
#include "FDTD.h"


int main(int argc, char *argv[])
{
	
	//Initialize MPI
	if(PhotonMPI.initialize(argc,argv))
		cout << "Error occured in MPI initialization.";
	
	//Read input file
	Simulation.readControl(argv[argc-1]);
	
	cout << "deltaT: " << (Simulation.deltaT * 10000 )<< endl;
	
	//Create map of chunks
	if(instantiateChunkMap())
		cout << "Rank: " << PhotonMPI.rank << "Error occured in chunk map initialization.\n";
	
	//Write chunk map if rank 0
	if(PhotonMPI.rank == 0)
	{
		if(writeChunkMapCSV())
			cout << "Rank: " << PhotonMPI.rank << "Error occured in writing chunk map files.\n";
	}
	
	//Read epsilon and sigma
	if(readEpsSigmaCSV())
		cout << "Rank: " << PhotonMPI.rank << " Error in reading CSV file.\n";
		
	
	if(epsilonSigmaOverlap())
		cout << "Problem overlapping epsilon and sigma data." << endl;
	
	
	//Call FDTD
	FDTD_timestep();
	
	//Write all PFA_Ezx chunks to file
	writeAllChunkstoCSV(7);
	
	//Write all epsilon chunks to file
	writeAllChunkstoCSV(0);
	
	//Write all Ezx chunks to file
	writeAllChunkstoCSV(3);
	
	

	//Perform global cleanup
	globalCleanUp();
	
	//Terminate MPI
	if(PhotonMPI.finalize())
		cout << "Rank: " << PhotonMPI.rank << "Error occured in MPI finalize.\n";
	
	return 0;
	
	
}
