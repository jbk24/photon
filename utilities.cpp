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
			//Get current gid
			unsigned int curGid = xy2gid(x,y,Simulation.numChunks.x);
			
			int processorX = x/(Simulation.numChunks.x/Simulation.processors.x); //X position of current processor
			int processorY = y/(Simulation.numChunks.y/Simulation.processors.y); //Y position of current processor
			ChunkMap[curGid].processor = xy2gid(processorX, processorY, Simulation.processors.x); //get rank of processor
			
			//Process neighbors
			if(x>0) //x-negative
				ChunkMap[curGid].neighbor[0] = xy2gid(x-1,y,Simulation.numChunks.x);
			if(x<(Simulation.numChunks.x - 1)) //x-positive
				ChunkMap[curGid].neighbor[1] = xy2gid(x+1,y,Simulation.numChunks.x);
			if(y>0) //y-negative
				ChunkMap[curGid].neighbor[2] = xy2gid(x,y-1,Simulation.numChunks.x);
			if(y<(Simulation.numChunks.y - 1)) //y-positive
				ChunkMap[curGid].neighbor[3] = xy2gid(x,y+1,Simulation.numChunks.x);
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

				//Loop over neighbors to get edge conditions, count edges with other processors
				for(int k = 0; k<4; k++)
				{
					ChunkMap[curGid].edge[k] = getChunkEdgeConditions(curGid, ChunkMap[curGid].neighbor[k]);
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
	if (gidOther == -1)
		return 0; //PEC boundary
	else if(ChunkMap[gidOther].refinement == ChunkMap[gid].refinement)
		refinementGrad = 1; // matched refinement
							
	else if(ChunkMap[gidOther].refinement < ChunkMap[gid].refinement)
		refinementGrad = 2; // decreasing refinement
							
	else refinementGrad = 3; // increasing refinement
					
	//Check if chunks reside on same processor
		if(ChunkMap[gidOther].processor != PhotonMPI.rank)
		{
			PhotonMPI.edgeCount++; //Count edge
			return refinementGrad + 3; //external comm
		}
		else //Adjacent chunk is from same processor
			return refinementGrad; //internal comm
}

int epsilonSigmaOverlap() //Communicate epsilon and sigma along edges to achieve array overlap
{
	//Loop over list of owned chunks
	for(int g = 0; g<Simulation.ownedChunkList.size(); g++)
	{
		//Get gid for current chunk
		unsigned int gid = Simulation.ownedChunkList.at(g);
		unsigned int gidOther;
		unsigned int destEdge;
		
		if(PhotonMPI.rank ==0)
			cout << "Current Chunk: " << gid << endl;
		
		//Process edges
		for (int k = 0; k<4; k++)
		{
		switch (ChunkMap[gid].edge[k])
			{
				
				case 0: // PEC boundary, duplicate epsilon and sigma
				//	cout << "PEC on edge " << k << " of chunk " << gid << endl;
					//Epsilon
					oneWayIntraArrayCopy(ChunkMap[gid].epsilon, ChunkMap[gid].arraySize.x, ChunkMap[gid].ownEdge[k], ChunkMap[gid].overlapEdge[k]);
					//SigmaX
					oneWayIntraArrayCopy(ChunkMap[gid].sigmaX, ChunkMap[gid].arraySize.x, ChunkMap[gid].ownEdge[k], ChunkMap[gid].overlapEdge[k]);
					//SigmaY
					oneWayIntraArrayCopy(ChunkMap[gid].sigmaY, ChunkMap[gid].arraySize.x, ChunkMap[gid].ownEdge[k], ChunkMap[gid].overlapEdge[k]);
					break;
					
				case 1: //internal comm, matched refinement, 
					//Get neighbor
					gidOther = ChunkMap[gid].neighbor[k];
					destEdge = ChunkMap[gid].edgeSend[k];
					//Epsilon
					oneWayInternalArrayCopy(ChunkMap[gid].epsilon,ChunkMap[gidOther].epsilon, ChunkMap[gid].arraySize.x, ChunkMap[gidOther].arraySize.x, ChunkMap[gid].ownEdge[k], ChunkMap[gidOther].overlapEdge[destEdge]);
					/*if(PhotonMPI.rank == 0 && gid ==0)
					{
						cout << "OwnEdge Start: (" << ChunkMap[gid].ownEdge[k].start.x << "," << ChunkMap[gid].ownEdge[k].start.y << ")" << endl;
						cout << "OwnEdge End: (" << ChunkMap[gid].ownEdge[k].end.x << "," << ChunkMap[gid].ownEdge[k].end.y << ")" << endl;
						cout << "OverlapEdge Start: (" << ChunkMap[gid].overlapEdge[k].start.x << "," << ChunkMap[gid].overlapEdge[k].start.y << ")" << endl;
						cout << "OverlapEdge End: (" << ChunkMap[gid].overlapEdge[k].end.x << "," << ChunkMap[gid].overlapEdge[k].end.y << ")" << endl;
						
			
					}
					 **/
					
					//SigmaX
				 	oneWayInternalArrayCopy(ChunkMap[gid].sigmaX,ChunkMap[gidOther].sigmaX, ChunkMap[gid].arraySize.x, ChunkMap[gidOther].arraySize.x, ChunkMap[gid].ownEdge[k], ChunkMap[gidOther].overlapEdge[destEdge]);
					//SigmaY
					oneWayInternalArrayCopy(ChunkMap[gid].sigmaY,ChunkMap[gidOther].sigmaY, ChunkMap[gid].arraySize.x, ChunkMap[gidOther].arraySize.x, ChunkMap[gid].ownEdge[k], ChunkMap[gidOther].overlapEdge[destEdge]);
					break;
				case 2: //internal comm, decreasing refinement
					break;
				case 3: //internal comm, increasing refinement
					break;
				case 4: //external comm, matched refinement
					break;
				case 5: //external comm, decreasing refinement
					break;
				case 6: //external comm, increasing refinement
					break;
			}
		}
	}
	
	return 0;
}

//Copy a chunk of elements from one part of an array to another
int oneWayIntraArrayCopy(double *array, int sizeX, EdgeBoundsClass origin, EdgeBoundsClass dest)
{
	//Calculate X-Y offests
	int xOffset = dest.start.x - origin.start.x;
	int yOffset = dest.start.y - origin.start.y;
	
	//cout << "xOffset: " << xOffset << endl;
	//cout << "yOffset: " << yOffset << endl;
	
	
	//Loop over origin array, within bounds defined by origin
	for(int x = origin.start.x; x <= origin.end.x; x++)
	{
		for(int y = origin.start.y; y <= origin.end.y; y++)
		{
			//cout << "(x,y)				  : (" << x << "," << y << ") = " << array[xy2gid(x,y,sizeX)] << endl;
			//cout << "(x+xOffest,y+yOffset): (" << x+xOffset << "," << y+yOffset << ") = " << array[xy2gid(x+xOffset,y+yOffset,sizeX)] << endl;
			
			array[xy2gid(x+xOffset,y+yOffset,sizeX)] = array[xy2gid(x,y,sizeX)];
			
			//cout << "(x+xOffest,y+yOffset): (" << x+xOffset << "," << y+yOffset << ") = " << array[xy2gid(x+xOffset,y+yOffset,sizeX)] << endl;
			
		}
	}	
	return 0;
}



//Copy a chunk of elements from orign array to dest array
int oneWayInternalArrayCopy(double *originArray, double *destArray, int originSizeX, int destSizeX, EdgeBoundsClass origin, EdgeBoundsClass dest) 
{
	//Calculate X-Y offests
	int xOffset = dest.start.x - origin.start.x;
	int yOffset = dest.start.y - origin.start.y;
	
	//cout << "xOffset: " << xOffset << endl;
	//cout << "yOffset: " << yOffset << endl;
	
	//Loop over origin array, within bounds defined by origin
	for(int x = origin.start.x; x <= origin.end.x; x++)
	{
		for(int y = origin.start.y; y <= origin.end.y; y++)
		{
			//cout << "(x,y)				  : (" << x << "," << y << ") = " << originArray[xy2gid(x,y,originSizeX)] << endl;
			//cout << "(x+xOffest,y+yOffset): (" << x+xOffset << "," << y+yOffset << ") = " << destArray[xy2gid(x+xOffset,y+yOffset,destSizeX)] << endl;
			
			destArray[xy2gid(x+xOffset,y+yOffset,destSizeX)] = originArray[xy2gid(x,y,originSizeX)];
		}
	}	
	return 0;
}

//Bidirectionally copy elements between arrays
int twoWayInternalArrayCopy(double *array1, double *array2, int array1SizeX, int array2SizeX, EdgeBoundsClass array1send, EdgeBoundsClass array1recv, EdgeBoundsClass array2send, EdgeBoundsClass array2recv)
{
	//Calculate X-Y offests
	int xRecvOffset1to2 = array2recv.start.x - array1send.start.x;
	int yRecvOffset1to2 = array2recv.start.y - array1send.start.y;
	int xRecvOffset2to1 = array1recv.start.x - array2send.start.x;
	int yRecvOffset2to1 = array1recv.start.y - array2send.start.y;
	int xSendOffset2to1 = array2send.start.x - array1send.start.x;
	int ySendOffset2to1 = array2send.start.y - array1send.start.y;
	
	//Loop over array1, within bounds defined by array1send
	for(int x = array1send.start.x; x <= array1send.end.x; x++)
	{
		for(int y = array1send.start.y; y <= array1send.end.y; y++)
		{
			array2[xy2gid(x+xRecvOffset1to2,y+yRecvOffset1to2,array2SizeX)] = array1[xy2gid(x,y,array1SizeX)];
			array1[xy2gid(x+xRecvOffset2to1,y+yRecvOffset2to1,array1SizeX)] = array2[xy2gid(x+xSendOffset2to1,y+ySendOffset2to1,array1SizeX)];
		}
	}
	return 0;
}
