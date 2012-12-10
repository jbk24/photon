#include <stdio.h>
#include <iostream>
#include <string.h>
#include "global.h"
#include "utilities.h"
#include "csvFileIO.h"


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
	
	//Write chunk map if rank 0
	if(PhotonMPI.rank == 0)
	{
		if(writeChunkMapCSV())
			cout << "Rank: " << PhotonMPI.rank << "Error occured in writing chunk map files.\n";
	}
	
	//Read epsilon and sigma
	if(readEpsSigmaCSV())
		cout << "Rank: " << PhotonMPI.rank << " Error in reading CSV file.\n";
	
	//Test MPI comm address
	if(PhotonMPI.rank == 0)
	{
		//Override to just do one comm
		PhotonMPI.edgeCount = 1;
		PhotonMPI.symChunkCommInit();
		PhotonMPI.symChunkComm(ChunkMap[xy2gid(5,0,Simulation.numChunks.x)].eps, ChunkMap[xy2gid(5,0,Simulation.numChunks.x)].sigma, Simulation.chunkSize.x*Simulation.chunkSize.y, xy2gid(5,0,Simulation.numChunks.x), xy2gid(6,0,Simulation.numChunks.x));
		cout << "Messages Queued" << endl;
		PhotonMPI.symChunkCommWait();
		writeArraytoCSV(ChunkMap[xy2gid(5,0,Simulation.numChunks.x)].eps, Simulation.chunkSize.x, Simulation.chunkSize.y, "rank0 sent eps.csv");
	}
	
	if(PhotonMPI.rank == 1)
	{
		//Override to just do one comm
		PhotonMPI.edgeCount = 1;
		PhotonMPI.symChunkCommInit();
		PhotonMPI.symChunkComm(ChunkMap[xy2gid(6,0,Simulation.numChunks.x)].eps, ChunkMap[xy2gid(6,0,Simulation.numChunks.x)].sigma, Simulation.chunkSize.x*Simulation.chunkSize.y, xy2gid(6,0,Simulation.numChunks.x), xy2gid(5,0,Simulation.numChunks.x));
		cout << "Messages Queued" << endl;
		PhotonMPI.symChunkCommWait();
		writeArraytoCSV(ChunkMap[xy2gid(6,0,Simulation.numChunks.x)].sigma, Simulation.chunkSize.x, Simulation.chunkSize.y, "rank1 recv eps.csv");
	}
	
	
	//Write field array
	if(PhotonMPI.rank == 0)
	{
		if(writeArraytoCSV(ChunkMap[xy2gid(0,0,Simulation.numChunks.x)].eps,(Simulation.chunkSize.x), (Simulation.chunkSize.y), "Chunk00Eps.csv"))
			cout << "Rank: " << PhotonMPI.rank << " Error in writing CSV file.\n";
	}
	

	//Perform global cleanup
	globalCleanUp();
	
	//Terminate MPI
	if(PhotonMPI.finalize())
		cout << "Rank: " << PhotonMPI.rank << "Error occured in MPI finalize.\n";
	
	return 0;
	
	
}
