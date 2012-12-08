PhotonMPIClass PhotonMPI;
SimulationClass Simulation;
	
#include "utilities.h"
	
//Instantiate chunk map 
void instantiateChunkMap()
{

	//Instantiate array of ChunkClass of size xChunks by yChunks
	ChunkClass ChunkMap[Simulation.numChunks.x][Simulation.numChunks.y];
	
	//Initialize chunkMap by defining chunk neighbors
	updateChunkMap();
}