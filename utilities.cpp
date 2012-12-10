/*This file contains functions for background operations not directly associated with any class */
#include "global.h"
#include "utilities.h"
#include <vector>

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

void updateChunkList() //Update list of chunks owned by current processor
{
	const unsigned int sizeIncrement = 100;
	Simulation.ownedChunkList.resize(sizeIncrement); //Initalize vector to sizeIncrement elements, will be resized later
	unsigned int count = 0; //Counter for number of elements, used to resize vector
	
	//Reset edge count
	PhotonMPI.edgeCount = 0;
	
	//Loop over chunk map
	for(int x = 0; x<Simulation.numChunks.x; x++)
	{
		for(int y = 0; y<Simulation.numChunks.y; y++)
		{
			unsigned int curGid = xy2gid(x,y,Simulation.numChunks.x);
			if(ChunkMap[curGid].processor == PhotonMPI.rank) //Check if current chunk resides on this processor
			{
				//Add current chunk to list 
				Simulation.ownedChunkList[count] = curGid; 
				count++;
				
				if(count >= Simulation.ownedChunkList.size()) //Check if vector needs to be increased in size
					Simulation.ownedChunkList.resize(Simulation.ownedChunkList.size() + sizeIncrement);
					
				//Get edge conditions, count edges with other processors
				if(x>0) //x-negative
				{
					ChunkMap[curGid].edgeXn = getChunkEdgeConditions(curGid, xy2gid(x-1,y,Simulation.numChunks.x)); 
				}
				
				if(x<(Simulation.numChunks.x-1)) //x-positive
				{
					ChunkMap[curGid].edgeXp = getChunkEdgeConditions(curGid, xy2gid(x+1,y,Simulation.numChunks.x));
				}
				
				if(y>0) //y-negative
				{
					ChunkMap[curGid].edgeYn = getChunkEdgeConditions(curGid, xy2gid(x,y-1,Simulation.numChunks.x));
				}
				
				if(y<(Simulation.numChunks.y-1)) //y-positive
				{
					ChunkMap[curGid].edgeYp = getChunkEdgeConditions(curGid, xy2gid(x,y+1,Simulation.numChunks.x));
				}
			}
		}
	}

	 //Truncate vector to count elements
	 Simulation.ownedChunkList.resize(count);
	
}

int getChunkEdgeConditions(int gid, int gidOther) //Get edge conditions for edge between chunk (x,y) and chunk (otherX, otherY)
{
	unsigned int refinementGrad;
	//Determine edge condition
	if(ChunkMap[gidOther].refinement == ChunkMap[gid].refinement)
		refinementGrad = 0; // matched refinement
							
	else if(ChunkMap[gidOther].refinement < ChunkMap[gid].refinement)
		refinementGrad = 1; // decreasing refinement
							
	else refinementGrad = 2; // increasing refinement
					
	//Check if chunks reside on same processor
		if(ChunkMap[gidOther].processor != PhotonMPI.rank)
		{
			PhotonMPI.edgeCount++; //Count edge
			return refinementGrad + 3; //external comm
		}
		else //Adjacent chunk is from same processor
			return refinementGrad; //internal comm
}
