#ifndef CHUNKCLASS_H
#define CHUNKCLASS_H
/* This class describes individual "chunks" of the computational grid. Each processor will own more than one chunk*/
#include "VectorIntClass.h"
#include "EdgeBoundsClass.h"

class ChunkClass
{
public:
	ChunkClass();
	~ChunkClass();
	
	//Properties
	int processor; //Rank of processor that currently owns chunk--will change under adaptive grid
	int refinement; //Integer indicating refinement level. Current dimesnions are original dimensions multiplied by refinement
	
	//Edge conditions:
	// 0 = PEC boundary (edge of computational domain, no comm)
	// 1 = internal comm, matched refinement
	// 2 = internal comm, decreasing refinement (scale down array)
	// 3 = internal comm, increasing refinement (scale up array)
	// 4 = external comm, matched refinement
	// 5 = external comm, decreasing refinement (scale down array)
	// 6 = external comm, increasing refinement (scale up array)
	// 0: negative-x edge
	// 1: positive-x edge
	// 2: negative-y edge
	// 3: positive-y edge
	
	//Edges
	int edge [4]; //Condition on each of the 4 edges
	int edgeSend[4]; // get index for matching edge when sending
	int neighbor [4]; //GID of neighbor on each edge
	EdgeBoundsClass ownEdge[4];//Locations of start and end point for each owned edge
	EdgeBoundsClass overlapEdge[4]; //Locations of start and end points for each overlap edge
	
	//Arrays: arrays are of (size.x+2)(size.y+2)*refinement, for overlap with adjacent arrays
	//Eps and sigma arrays
	VectorIntClass arraySize; //Size of actual arrays (size.x+2),(size.y+2)
	
	double *epsilon; //Pointer to epsilon array
	double *sigmaX; //Pointer to sigmaX array
	double *sigmaY; //Pointer to sigmaY array
	
	//Ex, Hx, Hy 
	double *Ezx; //Pointer to Ezx array Ez = Ezx + Ezy
	double *Ezy; //Pointer to Ezy array
	double *Hx; //Pointer to Hx array
	double *Hy; //Pointer to Hy array
	
	//Prefactor arrays--compute these once at start of simulation, recompute for any refined cells duing simultion
	double *PFA_Ezx; 
	double *PFB_Ezx;
	double *PFA_Ezy;
	double *PFB_Ezy;
	double *PFA_Hx;
	double *PFB_Hx;
	double *PFA_Hy;
	double *PFB_Hy;
	
	//Methods
	int createMPIStruct();
	int computePrefactors(); //Compute FDTD timestepping prefactors from constants
	void initalizeEdgeBounds(); //Initalize edge bounds based on size of array, must be updated under AMR
	
};

#endif // CHUNKCLASS_H
