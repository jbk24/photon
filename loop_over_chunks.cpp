/************************************************************************\
This file contians the field calculations to support the FDTD timestepping.

\*************************************************************************/

#include "loop_over_chunks.h"
#include <vector>

//Update the E-field from the H-field
int update_E_from_H()
{
	//Initalize MPI comm system
	PhotonMPI.chunkCommInit();
	
	//Loop over owned chunks
	for(int g = 0; g<Simulation.ownedChunkList.size(); g++)
	{
		//Get gid for current chunk
		unsigned int gid = Simulation.ownedChunkList.at(g);
		
		//Get pointers to data to simplify code below
		double* Ezx = ChunkMap[gid].Ezx;
		double* Ezy = ChunkMap[gid].Ezy;
		double* Hx = ChunkMap[gid].Hx;
		double* Hy = ChunkMap[gid].Hy;
		
		//Prefactors
		double *PFA_Ezx = ChunkMap[gid].PFA_Ezx;
		double *PFB_Ezx = ChunkMap[gid].PFB_Ezx;
		double *PFA_Ezy = ChunkMap[gid].PFA_Ezy;
		double *PFB_Ezy = ChunkMap[gid].PFB_Ezy;
		
		//Loop over array, as defined by compute bounds
		for(unsigned int x = ChunkMap[gid].computeBounds.start.x; x < ChunkMap[gid].computeBounds.end.x; x++)
		{
			for(unsigned int y = ChunkMap[gid].computeBounds.start.y; y < ChunkMap[gid].computeBounds.end.y; y++)
			{
			
				unsigned int p = xy2gid(x,y,ChunkMap[gid].arraySize.x); //Current point (i+1/2, j+1/2) for H
				unsigned int px = xy2gid(x-1,y,ChunkMap[gid].arraySize.x); //i-1/2 point, for derivative
				unsigned int py = xy2gid(x,y-1,ChunkMap[gid].arraySize.x); //j-1/2 point, for derivative
				
				Ezx[p] = Ezx[p]*PFA_Ezx[p] + PFB_Ezx[p]*(Hy[p]-Hy[px]);
				Ezy[p] = Ezy[p]*PFA_Ezy[p] + PFB_Ezy[p]*(Hx[p]-Hx[py]);
			}
		}
		//Send newly computed data associated with current chunk
		E_updateComm(gid);
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
	
	
	//Loop over owned chunks
	for(int g = 0; g<Simulation.ownedChunkList.size(); g++)
	{
		//Get gid for current chunk
		unsigned int gid = Simulation.ownedChunkList.at(g);
		
		//Get pointers to data to simplify code below
		double* Ezx = ChunkMap[gid].Ezx;
		double* Ezy = ChunkMap[gid].Ezy;
		double* Hx = ChunkMap[gid].Hx;
		double* Hy = ChunkMap[gid].Hy;
		
		//Prefactors
		double *PFA_Hx = ChunkMap[gid].PFA_Hx;
		double *PFB_Hx = ChunkMap[gid].PFB_Hx;
		double *PFA_Hy = ChunkMap[gid].PFA_Hy;
		double *PFB_Hy = ChunkMap[gid].PFB_Hy;
		
		//Loop over array, as defined by compute bounds
		for(unsigned int x = ChunkMap[gid].computeBounds.start.x; x < ChunkMap[gid].computeBounds.end.x; x++)
		{
			for(unsigned int y = ChunkMap[gid].computeBounds.start.y; y < ChunkMap[gid].computeBounds.end.y; y++)
			{
			
				unsigned int p = xy2gid(x,y,ChunkMap[gid].arraySize.x); //Current point (i+1/2, j+1/2) for H
				unsigned int px = xy2gid(x+1,y,ChunkMap[gid].arraySize.x); //i+1 point, for derivative
				unsigned int py = xy2gid(x,y+1,ChunkMap[gid].arraySize.x); //j+1 point, for derivative
				
				Hx[p] = Hx[p]*PFA_Hx[p] - PFB_Hx[p]*(Ezx[py]-Ezx[p]+Ezy[py]-Ezy[p]);
				Hy[p] = Hy[p]*PFA_Hy[p] - PFB_Hy[p]*(Ezx[px]-Ezx[p]+Ezy[px]-Ezx[p]);
			}
		}
		//Send newly computed data associated with current chunk
		H_updateComm(gid);
	}
	
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
