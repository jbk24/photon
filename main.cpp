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
	
	//Create map of chunks
	if(instantiateChunkMap())
		cout << "Rank: " << PhotonMPI.rank << "Error occured in chunk map initialization.\n";
		
	//Set up source in chunk region
	if(setupSourceInChunks())
		cout << "Rank: " << PhotonMPI.rank << "Problem setting up source in chunks." << endl;
	
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
	
	timer FDTD_time;
	
	//Call FDTD
	FDTD_timestep();
	
	cout << "Total FDTD computation time: " << FDTD_time.get_ms() << endl;
	cout << "Average time/step: " << FDTD_time.get_ms()/Simulation.curT << endl;
	
	
	//Write all PFA_Ezx chunks to file
	//writeAllChunkstoCSV(8);
	
	//Write all epsilon chunks to file
	writeAllChunkstoCSV(0);
	
	//Write all Ezx chunks to file
	//writeAllChunkstoCSV(3);
	
	//Perform global cleanup
	globalCleanUp();
	
	//Terminate MPI
	if(PhotonMPI.finalize())
		cout << "Rank: " << PhotonMPI.rank << "Error occured in MPI finalize.\n";
	
	return 0;
	
	
}
