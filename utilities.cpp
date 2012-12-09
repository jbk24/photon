/*This file contains functions for background operations not directly associated with any class */
#include "global.h"
#include "utilities.h"

//Build map of which processor a given chunk resides on
void initializeChunkMap()
{
	//Loop over chunk map
	for(int x = 0; x<Simulation.numChunks.x; x++)
	{
		for(int y = 0; y<Simulation.numChunks.y; y++)
		{
			int processorX = x/(Simulation.numChunks.x/Simulation.processors.x); //X position of current processor
			int processorY = y/(Simulation.numChunks.y/Simulation.processors.y); //Y position of current processor
			ChunkMap[xy2gid(x,y,Simulation.numChunks.x)].processor = xy2gid(processorX, processorY, Simulation.processors.x); //get rank of processor
		}
	}
}

void updateChunkBounds() //Get min x,y and max x,y chunk for processor
{
	int minX,minY,maxX,maxY; //Cooridnates of box bounding chunks for current processor  
	bool noChunksFound = true; //flag to indicate first chunk has been found and bounding box coordinates initialized
	//Loop over chunk map
	for(int x = 0; x<Simulation.numChunks.x; x++)
	{
		for(int y = 0; y<Simulation.numChunks.y; y++)
		{
			if(ChunkMap[xy2gid(x,y,Simulation.numChunks.x)].processor == PhotonMPI.rank) //Check if current chunk resides on this processor
			{
				//Check if this is the first chunk we have found
				if(noChunksFound)
				{
					minX = x;
					minY = y;
					maxX = x;
					maxY = y;
					noChunksFound = false;
				}
				else
				{
					//Update minX
					if(x<minX)
						minX = x;
					if(y<minY)
						minY = y;
					if(x>maxX)
						maxX = x;
					if(y>maxY)
						maxY = y;
				}
			}
					
		}
	}
	//Update bounding box cooridnates in simulation
	Simulation.minChunk.x = minX;
	Simulation.minChunk.y = minY;
	Simulation.maxChunk.x = maxX;
	Simulation.maxChunk.y = maxY;
	
}

