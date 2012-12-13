/************************************************************************\
This file contians the field calculations to support the FDTD timestepping.

\*************************************************************************/

#include "loop_over_chunks.h"
#inclue <vector>

//Update the E-field from the H-field
int update_E_from_H()
{
	//Initalize MPI comm system
	PhotonMPI.chunkCommInit();
	
	for(int g = 0; g<Simulation.ownedChunkList.size(); g++)
	{
		//Get gid for current chunk
		unsigned int gid = Simulation.ownedChunkList.at(g);
		
		//Get pointers to data
	}

	//Wait on sends and recieves
	PhotonMPI.chunkCommWait();
	
	//Process recieved data
	processRecievedData();
	
	return 0;
}

//Update the H-field from the E-field
int update_H_from_E()
{
	//Initalize MPI comm system
	PhotonMPI.chunkCommInit();
	
	
	//Wait on sends and recieves
	PhotonMPI.chunkCommWait();
	
	//Process recieved data
	processRecievedData();
	
	return 0;
}

//Process all communications for current chunk after E update, send Ezx and Ezy along negative edges, recieve Ezx and Ezy along positive edges
int E_updateComm(unsigned int gid)
{
		unsigned int gidOther;
		unsigned int destEdge;
		
		//Intialize data buffers for current chunk
		ChunkMap[gid].initalizeDataBuffers();
		
		//Process edges
		for (int k = 0; k<4; k++)
		{
		switch (ChunkMap[gid].edge[k])
			{
				case 0: // PEC boundary, do nothing
					break;
				case 1: //internal comm, matched refinement, send if negative-x (0) or negative-y (2)
					if(k == 0 || k == 2) //Send Ezx and Ezy to adjacent chunk on same processor
					{
						//Get neighbor
						gidOther = ChunkMap[gid].neighbor[k];
						destEdge = ChunkMap[gid].edgeSend[k];
						//Ezx
						oneWayInternalArrayCopy(ChunkMap[gid].Ezx,ChunkMap[gidOther].Ezx, ChunkMap[gid].ownEdge[k], ChunkMap[gidOther].overlapEdge[destEdge]);
						//Ezy
						oneWayInternalArrayCopy(ChunkMap[gid].Ezy,ChunkMap[gidOther].Ezy,  ChunkMap[gid].ownEdge[k], ChunkMap[gidOther].overlapEdge[destEdge]);
					}
					break;
				case 2: //internal comm, decreasing refinement
					break;
				case 3: //internal comm, increasing refinement
					break;
				case 4: //external comm, matched refinement
					
					if( k == 0 || k == 2) //Send Ezx and Ezy to adjacent chunk on different processor
					{
						PhotonMPI.sendChunkComm(gid,3,k); //Send Ezx
						PhotonMPI.sendChunkComm(gid,4,k); //Send Ezy
					}
					else //Recieve Ezx, Ezy
					{
						PhotonMPI.recvChunkComm(gid,3,k);//Recive Ezx;
						PhotonMPI.recvChunkComm(gid,4,k); //Recive Ezy;
					}
					break;
				case 5: //external comm, decreasing refinement
					break;
				case 6: //external comm, increasing refinement
					break;
			}
		}
	return 0;
}

//Process all communications for current chunk after H update, send Hx and Hy along positive edges, recieve Hx and Hy along negative edges
int H_updateComm(unsigned int gid)
{
		unsigned int gidOther;
		unsigned int destEdge;
		
		//Intialize data buffers for current chunk
		ChunkMap[gid].initalizeDataBuffers();
		
		//Process edges
		for (int k = 0; k<4; k++)
		{
		switch (ChunkMap[gid].edge[k]) //Switch on edge type
			{
				case 0: // PEC boundary, do nothing
					break;
				case 1: //internal comm, matched refinement, send Hy if positive-x (1), Hx if positive-y (3)
					if(k == 1) //Send Hy
					{
						//Get neighbor
						gidOther = ChunkMap[gid].neighbor[k];
						destEdge = ChunkMap[gid].edgeSend[k];
						//Hx
						oneWayInternalArrayCopy(ChunkMap[gid].Hx,ChunkMap[gidOther].Hx, ChunkMap[gid].ownEdge[k], ChunkMap[gidOther].overlapEdge[destEdge]);
					}
					else if (k == 3)//Send Hx
					{
						//Get neighbor
						gidOther = ChunkMap[gid].neighbor[k];
						destEdge = ChunkMap[gid].edgeSend[k];
						oneWayInternalArrayCopy(ChunkMap[gid].Hx,ChunkMap[gidOther].Hx,  ChunkMap[gid].ownEdge[k], ChunkMap[gidOther].overlapEdge[destEdge]);
					}
					break;
				case 2: //internal comm, decreasing refinement
					break;
				case 3: //internal comm, increasing refinement
					break;
				case 4: //external comm, matched refinement
					switch (k) //Switch on edege, send Hy on e1, Hx on e3, recv Hy on e0, recv Hx on e2
					{
						case 0:
							PhotonMPI.recvChunkComm(gid,6,k);//Recive Hy
							break;
						case 1:
							PhotonMPI.sendChunkComm(gid,6,k); //Send Hy
							break;
						case 2:
							PhotonMPI.recvChunkComm(gid,5,k);//Recive Hx
							break;
						case 3:
							PhotonMPI.sendChunkComm(gid,5,k); //Send Hx
							break;
					}
					break;
				case 5: //external comm, decreasing refinement
					break;
				case 6: //external comm, increasing refinement
					break;
			}
		}
	return 0;
}
