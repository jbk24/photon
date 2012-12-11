#include "ChunkClass.h"
#include "global.h"
#include <mpi.h>

ChunkClass::ChunkClass()
{
	//Initialize chunk class parameters
	refinement = 1;
	
	//Initalize all edges to PEC boundary, no comm (0)
	edgeXn = 0;
	edgeXp = 0;
	edgeYn = 0;
	edgeYp = 0;
	
	//Instantiate arrays--larger by 2 elements in each direction for overlap with adjacent chunks to facilitate FDTD
	
	//Geometry
	epsilon = new double[(Simulation.chunkSize.x+2)*(Simulation.chunkSize.y+2)]();
	sigmaX = new double[(Simulation.chunkSize.x+2)*(Simulation.chunkSize.y+2)]();
	sigmaY = new double[(Simulation.chunkSize.x+2)*(Simulation.chunkSize.y+2)]();
	
	//Field arrays
	Ezx = new double[(Simulation.chunkSize.x+2)*(Simulation.chunkSize.y+2)]();
	Ezy = new double[(Simulation.chunkSize.x+2)*(Simulation.chunkSize.y+2)]();
	Hx = new double[(Simulation.chunkSize.x+2)*(Simulation.chunkSize.y+2)]();
	Hy = new double[(Simulation.chunkSize.x+2)*(Simulation.chunkSize.y+2)]();
	
	//Prefactors
	PFA_Ezx = new double[(Simulation.chunkSize.x+2)*(Simulation.chunkSize.y+2)]();
	PFB_Ezx = new double[(Simulation.chunkSize.x+2)*(Simulation.chunkSize.y+2)]();
	PFA_Ezy = new double[(Simulation.chunkSize.x+2)*(Simulation.chunkSize.y+2)]();
	PFB_Ezy = new double[(Simulation.chunkSize.x+2)*(Simulation.chunkSize.y+2)]();
	PFA_Hx = new double[(Simulation.chunkSize.x+2)*(Simulation.chunkSize.y+2)]();
	PFB_Hx = new double[(Simulation.chunkSize.x+2)*(Simulation.chunkSize.y+2)]();
	PFA_Hy = new double[(Simulation.chunkSize.x+2)*(Simulation.chunkSize.y+2)]();
	PFB_Hy = new double[(Simulation.chunkSize.x+2)*(Simulation.chunkSize.y+2)]();
}

ChunkClass::~ChunkClass()
{
	//Deallocate dynamically assigned memory
	delete [] eps;
	delete [] sigma;
	delete [] Ez;
	delete [] Hx;
	delete [] Hy;
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
	
}
