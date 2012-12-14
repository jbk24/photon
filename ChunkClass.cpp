#include "ChunkClass.h"
#include "global.h"
#include "utilities.h"
#include <mpi.h>

ChunkClass::ChunkClass()
{
	//Initialize chunk class parameters
	refinement = 1;
	
	//Initalize array size--larger by 2 elements in each direction for overlap with adjacent chunks to facilitate FDTD
	arraySize.x = Simulation.chunkSize.x + 2;
	arraySize.y = Simulation.chunkSize.y + 2;
	
	//Initalize arrays allocated to false
	arraysAllocated = false;
	
	//Initalize all edges to PEC boundary, no comm (0), all neighbors to -1 (no neighbor), comm flags to 0
	for(int k = 0; k<4; k++)
	{
		edge[k] = 0;
		neighbor[k] = -1;
	}
	
	edgeSend[0] = 1; //edge 0 sends to edge 1
	edgeSend[1] = 0; //edge 1 sents to edge 0
	edgeSend[2] = 3; //edge 2 sends to edge 3
	edgeSend[3] = 2; //edge 3 sends to edge 2
	
	
	//Initalize edge bounds
	initalizeEdgeBounds();
	
	//Default to no source
	sourceFlag = false;
	
	//Number of MPI send and recieve data buffers
	numDataBuffers = 15;

}

ChunkClass::~ChunkClass()
{
	//Deallocate dynamically assigned memory if used
	if(arraysAllocated)
		deallocateArrays();
	
}

void ChunkClass::allocateArrays() //Allocate arrays for chunk
{
	unsigned int size = arraySize.x * arraySize.y;
	
	//Geometry
	epsilon = new double[size]();
	sigmaX = new double[size]();
	sigmaY = new double[size]();
	
	//Field arrays
	Ezx = new double[size]();
	Ezy = new double[size]();
	Hx = new double[size]();
	Hy = new double[size]();
	
	//Prefactors
	PFA_Ezx = new double[size]();
	PFB_Ezx = new double[size]();
	PFA_Ezy = new double[size]();
	PFB_Ezy = new double[size]();
	PFA_Hx = new double[size]();
	PFB_Hx = new double[size]();
	PFA_Hy = new double[size]();
	PFB_Hy = new double[size]();
	
	
	//Allocate data buffers for MPI IO
	//Nominally internal chunks don't need these, but under AMR, internal chunks may end up with external boundaries, and the memory for data buffers is small when compared
	//with total chunk memory
	
	sendBuffers = new DataBufferClass[numDataBuffers];
	recvBuffers = new DataBufferClass[numDataBuffers];
	
	//Initalize data buffers to max(arraySize.x, arraySize.y)
	for (int k = 0; k<numDataBuffers; k++)
	{
		sendBuffers[k].initalizeBuffer(arraySize.x,arraySize.y);
		recvBuffers[k].initalizeBuffer(arraySize.x,arraySize.y);
	}
	
	
	//Set arrays allocated flag
	arraysAllocated = true;
}

void ChunkClass::initalizeDataBuffers() //Initalize data buffers for chunk, must be called before each comm phase
{
	//Reset buffer utilization to 0
	curSendBuf = 0;
	curRecvBuf = 0;
}


void ChunkClass::deallocateArrays() //Delete arrays for chunk
{
	delete [] epsilon;
	delete [] sigmaX;
	delete [] sigmaY;
	delete [] Ezx;
	delete [] Ezy;
	delete [] Hx;
	delete [] Hy;
	
	//Prefactor arrays
	delete [] PFA_Ezx;
	delete [] PFB_Ezx;
	delete [] PFA_Ezy;
	delete [] PFB_Ezy;
	delete [] PFA_Hx;
	delete [] PFB_Hx;
	delete [] PFA_Hy;
	delete [] PFB_Hy;
	
	//MPI Data Buffers
	delete [] sendBuffers;
	delete [] recvBuffers;
}


//This function creates an MPI Struct to describe the current chunk, for transmitting or recieving data
int ChunkClass::createMPIStruct()
{ 
	/*
	//Number of blocks
	const unsigned int count = 5; //eps, sigma, Ez, Hx, Hy
	
	//Length of each block
	unsigned int array_of_blocklengths[count]; 
	
	for (unsigned int k =0; k< count; k++)
	{
		array_of_blocklengths[k] = Simulation.chunkSize.x*refinement*Simulation.chunkSize.y*refinement;
	}
	
	//Displacement of address of each block from first
	int array_of_displacements[count];
	array_of_displacements[0]= &eps;
	array_of_displacements[1]= &sigma - array_of_displacements[0];
	array_of_displacements[2]= &Ez- array_of_displacements[0];
	array_of_displacements[3]= &Hx - array_of_displacements[0];
	array_of_displacements[4]= &Hy - array_of_displacements[0];
	
	//Type of each data array
	int array_of_types[count];
	
	for (unsigned int k = 0; k < count; k++)
	{
		array_of_types[k] = MPI_DOUBLE;
	}
	*/
	
	return 0;
	
}

int ChunkClass::computePrefactors() //Compute FDTD timestepping prefactors from constants
{
	//Get Constants
	double dT = Simulation.deltaT/refinement;
	double dX = Simulation.deltaX/refinement;
	double dY = Simulation.deltaY/refinement;
	double mu = 0.00000125663; //u0, placeholder for future inclusion of magnetic materials (4*pi*10^-7 H/m)
	double epsXavg, sigXavg, epsYavg, sigYavg; // Used to compute averages for H prefactors

	
	//Loop over interior of arrays
	for(int x = 1; x<(arraySize.x-1); x++)
	{
		for(int y = 1; y<(arraySize.y-1); y++)
		{
			unsigned int p = xy2gid(x,y,arraySize.x); //Current point
			unsigned int pXp = xy2gid(x+1,y,arraySize.x); //x+1 point
			unsigned int pYp = xy2gid(x,y+1,arraySize.y); //y+1 point
			
			//Compute prefactors
			
			//Ezx
			PFA_Ezx[p] = (2*epsilon[p] - sigmaX[p]*dT)/(2*epsilon[p] + sigmaX[p]*dT); //PFA_Ezx
			PFB_Ezx[p] = (2*dT)/((2*epsilon[p] + sigmaX[p]*dT)*dX); //PFB_Ezx
			
			//Ezy
			PFA_Ezy[p] = (2*epsilon[p] - sigmaY[p]*dT)/(2*epsilon[p] + sigmaY[p]*dT); //PFA_Ezy
			PFB_Ezy[p] = (2*dT)/((2*epsilon[p] + sigmaY[p]*dT)*dY); //PFB_Ezy
			
			//Hx, first compute eps, sigma at y+1/2
			epsYavg = (epsilon[p] + epsilon[pYp])/2;
			sigYavg = (sigmaY[p] + sigmaY[pYp])/2;
			PFA_Hx[p] = (2*epsYavg - sigYavg*dT)/(2*epsYavg + sigYavg*dT); //PFA_Hx
			PFB_Hx[p] = (2*epsYavg*dT/mu)/((2*epsYavg + sigYavg*dT)*dY); //PFB_Hx
			
			//Hy, first compute eps, sigma at x+1/2
			epsXavg = (epsilon[p] + epsilon[pXp])/2;
			sigXavg = (sigmaY[p] + sigmaY[pXp])/2;
			PFA_Hy[p] = (2*epsXavg - sigXavg*dT)/(2*epsXavg + sigXavg*dT); //PFA_Hy
			PFB_Hy[p] = (2*epsXavg*dT/mu)/((2*epsXavg + sigXavg*dT)*dX); //PFB_Hy
			
		}
	}
	
	return 0;
	
}

void ChunkClass::initalizeEdgeBounds() //Initalize edge bounds based on size of array, must be updated under AMR
{
	//Initalize ownEdge
	//X-negative
	ownEdge[0].start.x = 1;
	ownEdge[0].start.y = 1;
	ownEdge[0].end.x = 1;
	ownEdge[0].end.y = arraySize.y-2;
	
	//X-positive
	ownEdge[1].start.x = arraySize.x-2;
	ownEdge[1].start.y = 1;
	ownEdge[1].end.x = arraySize.x-2;
	ownEdge[1].end.y = arraySize.y-2;
	
	//Y-negative
	ownEdge[2].start.x = 1;
	ownEdge[2].start.y = 1;
	ownEdge[2].end.x = arraySize.x-2;
	ownEdge[2].end.y = 1;
	
	//Y-positve
	ownEdge[3].start.x = 1;
	ownEdge[3].start.y = arraySize.y-2;
	ownEdge[3].end.x = arraySize.x-2;
	ownEdge[3].end.y = arraySize.y-2;
	
	//Initalize overlapEdge
	//X-negative
	overlapEdge[0].start.x = 0;
	overlapEdge[0].start.y = 1;
	overlapEdge[0].end.x = 0;
	overlapEdge[0].end.y = arraySize.y-2;
	
	//X-positive
	overlapEdge[1].start.x = arraySize.x-1;
	overlapEdge[1].start.y = 1;
	overlapEdge[1].end.x = arraySize.x-1;
	overlapEdge[1].end.y = arraySize.y-2;
	
	//Y-negative
	overlapEdge[2].start.x = 1;
	overlapEdge[2].start.y = 0;
	overlapEdge[2].end.x = arraySize.x-2;
	overlapEdge[2].end.y = 0;
	
	//Y-positve
	overlapEdge[3].start.x = 1;
	overlapEdge[3].start.y = arraySize.y-1;
	overlapEdge[3].end.x = arraySize.x-2;
	overlapEdge[3].end.y = arraySize.y-1;
	
	//Set array size
	for (int k = 0; k<4;k++)
	{
		ownEdge[k].sizeX = arraySize.x;
		overlapEdge[k].sizeX = arraySize.x;
	}
	
	
 	
}
