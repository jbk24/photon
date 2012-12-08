/*This file contains functions for background operations not directly associated with any class */
#include global.h

//Build or rebuild map of which processor a given chunk resides on
void initalizeChunkMap()
{
	//Loop over chunk map
	for(int x = 0; x<Simulation.numChunks.x; x++)
	{
		for(int y = 0; y<Simulation.numChunks.y; y++)
		{
			int processorX = x%Simulation.numChunks.x; //X position of current processor
			int processorY = y%Simulation.numChunks.y;S; //Y position of current processor
			ChunkMap[x][y].processor = xy2gid(processorX, processorY, Simulation.processors.x); //get rank of processor
		}
	}
}

void updateChunkMap()
{
	//Loop over chunk map
	for(int x = 0; x<Simulation.numChunks.x; x++)
	{
		for(int y = 0; y<Simulation.numChunks.y; y++)
		{
			/*FIX THIS CODE*/
			
			int processorX = x%Simulation.numChunks.x; //X position of current processor
			int processorY = y%Simulation.numChunks.y;S; //Y position of current processor
			ChunkMap[x][y].processor = xy2gid(processorX, processorY, Simulation.processors.x); //get rank of processor
		}
	}
	
}


int gid2x(int gid, int sizeX) //Convert gid (such as processor rank) to x position, using the size of the array in the x direction
{
	return gid%sizeX; //y
}

int gid2y(int gid, int sizeX) //Convert gid (such as processor rank) to y position, using the size of the array in the x direction
{
	int y = gid%x;
	return gid - y*sizeX; //x
}

int xy2gid(int x, int y, int sizeX) //Convert x-y position to gid (such as processor rank), using the size of the array in the x direction
{
	return y*sizeX + x; //gid
}

