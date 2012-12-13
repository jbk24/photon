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
	
	//cout << "Epsilon and sigma data read." << endl;
	
	if(PhotonMPI.rank == 0)
	{
		unsigned int gid = xy2gid(0,0,Simulation.numChunks.x);
		writeArraytoCSV(ChunkMap[gid].epsilon, ChunkMap[gid].arraySize.x,ChunkMap[gid].arraySize.y, "rank 0 c0 before overlap.csv");
		gid = xy2gid(0,1,Simulation.numChunks.x);
		writeArraytoCSV(ChunkMap[gid].epsilon, ChunkMap[gid].arraySize.x,ChunkMap[gid].arraySize.y, "rank 0 c1 before overlap.csv");
	}
	

	//cout << "Overlap Epsilon and Sigma" << endl;
	
	if(epsilonSigmaOverlap())
		cout << "Problem overlapping epsilon and sigma data." << endl;
	
	//Write all epsilon chunks to file
	writeAllChunkstoCSV(0);
	//writeAllChunkstoCSV(1);
	
	
	/*
	if(PhotonMPI.rank == 0)
	{
		unsigned int gid = xy2gid(0,1,Simulation.numChunks.x);
		writeArraytoCSV(ChunkMap[gid].epsilon, ChunkMap[gid].arraySize.x,ChunkMap[gid].arraySize.y, "rank 0 after overlap.csv");
	}
	 ** /
	
	
	/*
	if(PhotonMPI.rank == 1)
		writeArraytoCSV(ChunkMap[xy2gid(6,0,Simulation.numChunks.x)].eps, Simulation.chunkSize.x, Simulation.chunkSize.y, "rank1 before eps.csv");

	//Test MPI comm
	if(PhotonMPI.rank == 0)
	{
		PhotonMPI.chunkCommInit();
		PhotonMPI.chunkComm(0,ChunkMap[xy2gid(5,0,Simulation.numChunks.x)].sigma, Simulation.chunkSize.x*Simulation.chunkSize.y, xy2gid(5,0,Simulation.numChunks.x), xy2gid(6,0,Simulation.numChunks.x));
		cout << "Messages Queued" << endl;
		PhotonMPI.chunkCommWait();
		writeArraytoCSV(ChunkMap[xy2gid(5,0,Simulation.numChunks.x)].sigma, Simulation.chunkSize.x, Simulation.chunkSize.y, "rank0 recv eps.csv");
	}
	
	if(PhotonMPI.rank == 1)
	{
		PhotonMPI.edgeCount = 1;
		PhotonMPI.chunkCommInit();
		PhotonMPI.chunkComm(1, ChunkMap[xy2gid(6,0,Simulation.numChunks.x)].eps, Simulation.chunkSize.x*Simulation.chunkSize.y, xy2gid(6,0,Simulation.numChunks.x), xy2gid(5,0,Simulation.numChunks.x));
		cout << "Messages Queued" << endl;
		PhotonMPI.chunkCommWait();
		writeArraytoCSV(ChunkMap[xy2gid(6,0,Simulation.numChunks.x)].eps, Simulation.chunkSize.x, Simulation.chunkSize.y, "rank1 send eps.csv");
	}
	
	
	//Write field array
	if(PhotonMPI.rank == 1)
	{
		if(writeArraytoCSV(ChunkMap[xy2gid(11,4,Simulation.numChunks.x)].eps,(Simulation.chunkSize.x), (Simulation.chunkSize.y), "Chunk114Eps.csv"))
			cout << "Rank: " << PhotonMPI.rank << " Error in writing CSV file.\n";
	}
	
  */
	//Perform global cleanup
	globalCleanUp();
	
	//Terminate MPI
	if(PhotonMPI.finalize())
		cout << "Rank: " << PhotonMPI.rank << "Error occured in MPI finalize.\n";
	
	return 0;
	
	
}
