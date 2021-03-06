#include "global.h"
#include "utilities.h"
#include <stdio.h>
#include <iostream>

PhotonMPIClass PhotonMPI;
SimulationClass Simulation;
ChunkClass *ChunkMap;
	
//Instantiate chunk map 
int instantiateChunkMap()
{
	//Instantiate array of ChunkClass of size xChunks by yChunks
	ChunkMap = new ChunkClass[Simulation.numChunks.x*Simulation.numChunks.y];

	//Initialize chunkMap by defining chunk neighbors
	initializeChunkMap();

	//Update chunk bounds for this processor
	updateChunkList();
	
	//Build ChunkMap MPI Tags
	PhotonMPI.allocateAddressSpace();
	
	return 0; //No error handling yet
}

//Perform all cleanup actions for variables allocated on the heap
void globalCleanUp()
{
	//Chunk Map
	//delete [] ChunkMap;
}