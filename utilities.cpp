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
			
			//Allocate arrays if on current processor
			if(ChunkMap[curGid].processor == PhotonMPI.rank)
				ChunkMap[curGid].allocateArrays();
			
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
					
					switch(k) //Determine compute bounds from edge conditions, for use in FDTD loop
					{
						case 0: //negative-x edge
							if (!ChunkMap[curGid].edge[k]) //Test if edge is PEC
								ChunkMap[curGid].computeBounds.start.x = 2;
							else
								ChunkMap[curGid].computeBounds.start.x = 1;
							break;
						case 1: //positive-x edge
							if (!ChunkMap[curGid].edge[k]) //Test if edge is PEC
								ChunkMap[curGid].computeBounds.end.x = ChunkMap[curGid].arraySize.x - 2;
							else
								ChunkMap[curGid].computeBounds.end.x = ChunkMap[curGid].arraySize.x - 1;
							break;
						case 2: //negative-y edge
							if (!ChunkMap[curGid].edge[k]) //Test if edge is PEC
								ChunkMap[curGid].computeBounds.start.y = 2;
							else
								ChunkMap[curGid].computeBounds.start.y = 1;
							break;
						case 3: //positive-y edge
							if (!ChunkMap[curGid].edge[k]) //Test if edge is PEC
								ChunkMap[curGid].computeBounds.end.y = ChunkMap[curGid].arraySize.y - 2;
							else
								ChunkMap[curGid].computeBounds.end.y = ChunkMap[curGid].arraySize.y - 1;
							break;
					}
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
	//Initalize MPI comm system
	PhotonMPI.chunkCommInit();
	
	//Loop over list of owned chunks
	for(int g = 0; g<Simulation.ownedChunkList.size(); g++)
	{
		//Get gid for current chunk
		unsigned int gid = Simulation.ownedChunkList.at(g);
		unsigned int gidOther;
		unsigned int destEdge;
		
		//Intialize data buffers for current chunk
		ChunkMap[gid].initalizeDataBuffers();
		
		//Process edges
		for (int k = 0; k<4; k++)
		{
		switch (ChunkMap[gid].edge[k])
			{
				
				case 0: // PEC boundary, duplicate epsilon and sigma
				//	cout << "PEC on edge " << k << " of chunk " << gid << endl;
					
					//Epsilon
					oneWayIntraArrayCopy(ChunkMap[gid].epsilon, ChunkMap[gid].ownEdge[k], ChunkMap[gid].overlapEdge[k]);
					//SigmaX
					oneWayIntraArrayCopy(ChunkMap[gid].sigmaX, ChunkMap[gid].ownEdge[k], ChunkMap[gid].overlapEdge[k]);
					//SigmaY
					oneWayIntraArrayCopy(ChunkMap[gid].sigmaY, ChunkMap[gid].ownEdge[k], ChunkMap[gid].overlapEdge[k]);
					break;
					 
				case 1: //internal comm, matched refinement, 
					//Get neighbor
					
					gidOther = ChunkMap[gid].neighbor[k];
					destEdge = ChunkMap[gid].edgeSend[k];
					//Epsilon
					oneWayInternalArrayCopy(ChunkMap[gid].epsilon,ChunkMap[gidOther].epsilon, ChunkMap[gid].ownEdge[k], ChunkMap[gidOther].overlapEdge[destEdge]);
					//SigmaX
				 	oneWayInternalArrayCopy(ChunkMap[gid].sigmaX,ChunkMap[gidOther].sigmaX,  ChunkMap[gid].ownEdge[k], ChunkMap[gidOther].overlapEdge[destEdge]);
					//SigmaY
					oneWayInternalArrayCopy(ChunkMap[gid].sigmaY,ChunkMap[gidOther].sigmaY, ChunkMap[gid].ownEdge[k], ChunkMap[gidOther].overlapEdge[destEdge]);
					break;
					 
				case 2: //internal comm, decreasing refinement
					break;
				case 3: //internal comm, increasing refinement
					break;
				case 4: //external comm, matched refinement

					PhotonMPI.sendChunkComm(gid,0,k); //Send Epsilon
					PhotonMPI.recvChunkComm(gid,0,k); //Recive Epsilon;
					PhotonMPI.sendChunkComm(gid,1,k); //Send sigmaX
					PhotonMPI.recvChunkComm(gid,1,k); //Recive sigmaX
					PhotonMPI.sendChunkComm(gid,2,k); //Send sigmaY
					PhotonMPI.recvChunkComm(gid,2,k); //Recive sigmaY
					break;
				case 5: //external comm, decreasing refinement
					break;
				case 6: //external comm, increasing refinement
					break;
			}
		}
	}
	
	//Wait on sends and recieves
	PhotonMPI.chunkCommWait();
	
	//Process recieved data
	processRecievedData();
	
	return 0;
}

//Copy a chunk of elements from one part of an array to another
int oneWayIntraArrayCopy(double *array, ArraySectorClass origin, ArraySectorClass dest)
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
			
			array[xy2gid(x+xOffset,y+yOffset,dest.sizeX)] = array[xy2gid(x,y,origin.sizeX)];
			
			//cout << "(x+xOffest,y+yOffset): (" << x+xOffset << "," << y+yOffset << ") = " << array[xy2gid(x+xOffset,y+yOffset,sizeX)] << endl;
			
		}
	}	
	return 0;
}

//Copy a chunk of elements from orign array to dest array
int oneWayInternalArrayCopy(double *originArray, double *destArray, ArraySectorClass origin, ArraySectorClass dest) 
{
	//Calculate X-Y offests
	int xOffset = dest.start.x - origin.start.x;
	int yOffset = dest.start.y - origin.start.y;
	
	//cout << "xOffset: " << xOffset << endl;
	//cout << "yOffset: " << yOffset << endl;
	//cout << "Origin Start (x,y) : (" << origin.start.x << "," << origin.start.y << ")" << endl;
	//cout << "Origin End (x,y)   : (" << origin.end.x << "," << origin.end.y << ")" << endl;
	//cout << "Dest Start (x,y)   : (" << dest.start.x << "," << dest.start.y << ")" << endl;
	//cout << "Dest End (x,y)     : (" << dest.end.x << "," << dest.end.y << ")" << endl;
	
	//Check if data are oriented in same way
	//Loop over origin array, within bounds defined by origin
		for(int x = origin.start.x; x <= origin.end.x; x++)
		{
			for(int y = origin.start.y; y <= origin.end.y; y++)
			{
				//cout << "(x,y)				  : (" << x << "," << y << ") = " << originArray[xy2gid(x,y,origin.sizeX)] << endl;
				//cout << "Dest gid:" << xy2gid(x+xOffset,y+yOffset,dest.sizeX) << endl;
				destArray[xy2gid(x+xOffset,y+yOffset,dest.sizeX)] = originArray[xy2gid(x,y,origin.sizeX)];
					
				//cout << "(x+xOffest,y+yOffset): (" << x+xOffset << "," << y+yOffset << ") = " << destArray[xy2gid(x+xOffset,y+yOffset,dest.sizeX)] << endl;
			}
		}	
	

	return 0;
}

//Bidirectionally copy elements between arrays
int twoWayInternalArrayCopy(double *array1, double *array2, int array1SizeX, int array2SizeX, ArraySectorClass array1send, ArraySectorClass array1recv, ArraySectorClass array2send, ArraySectorClass array2recv)
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

int vectorIntoArrayCopy(double *vector, double *destArray, ArraySectorClass dest)
{
	int index = 0; //Index along vector
	for(int x = dest.start.x; x <= dest.end.x; x++)
		{
			for(int y = dest.start.y; y <= dest.end.y; y++)
			{

				destArray[xy2gid(x,y,dest.sizeX)] = vector[index];
				index++;
			}
		}	
		return 0;
}
int arrayIntoVectorCopy(double *vector, double *sourceArray, ArraySectorClass source)
{
	int index = 0; //Index along vector
	for(int x = source.start.x; x <= source.end.x; x++)
		{
			for(int y = source.start.y; y <= source.end.y; y++)
			{

				 vector[index] = sourceArray[xy2gid(x,y,source.sizeX)]; 
				index++;
			}
		}	
		return 0;
}

int processRecievedData() //Process all data recieves currently indicated in PhotonMPI.recievedData vector
{
	//Loop over number of data recieves
	for(unsigned int k = 0; k < PhotonMPI.recievedData.size(); k++)
	{
		unsigned int gid = PhotonMPI.recievedData.at(k).chunk;
		unsigned int recvBufNum = PhotonMPI.recievedData.at(k).dataBuffer;
		double* destArray;
		
		//Get destination array, based on data type
	    destArray = getDataPointerFromType(gid, ChunkMap[gid].recvBuffers[recvBufNum].dataType);
		
		//Copy data from recieve bufferto target data array
		vectorIntoArrayCopy(ChunkMap[gid].recvBuffers[recvBufNum].dataBuf, destArray, ChunkMap[gid].recvBuffers[recvBufNum].remote);

	}
	return 0;
}

int setupSourceInChunks() //Using input data for source, set-up source information in relevant chunks
{
	
	//Loop across specified source region
	for(int x = Simulation.sourceLocation.start.x; x <= Simulation.sourceLocation.end.x; x++)
	{
		for(int y = Simulation.sourceLocation.start.y; y <= Simulation.sourceLocation.end.y; y++)
		{
				//Determine what chunk this point resides in
				int chunkX = x/Simulation.chunkSize.x;
				int chunkY = y/Simulation.chunkSize.y;
				int gid = xy2gid(chunkX,chunkY, Simulation.numChunks.x);

				
				//See if this is the first point for this chunk
				if(!ChunkMap[gid].sourceFlag)
				{
					//Set sourceFlag to indicate presence of source
					ChunkMap[gid].sourceFlag = true;
					//First point in chunk, set start point
					int locX = x%Simulation.chunkSize.x + 1; //Arrays are padded
					int locY = y%Simulation.chunkSize.y + 1; //Arrays are padded
				
					ChunkMap[gid].source.start.x = locX;
					ChunkMap[gid].source.start.y = locY;
					cout << "Source in: " << gid << endl;
				}
				else //Set end point, keep updating until we leave the chunk
				{
					int locX = x%Simulation.chunkSize.x + 1; //Arrays are padded
					int locY = y%Simulation.chunkSize.y + 1; //Arrays are padded
					
					ChunkMap[gid].source.end.x = locX;
					ChunkMap[gid].source.end.y = locY;
				}
		}
	}	
	
	return 0;
}

