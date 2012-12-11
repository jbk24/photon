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
	arraySize.x = Simulation.chunkSize.x + 2;
	arraySize.y = Simulation.chunkSize.y + 2;
	
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
	//Get Constants
	double dT = Simulation.deltaT/refinement;
	double dX = Simulation.deltaX/refinement;
	double dY = Simulation.deltaY/refinement;
	double mu = 1; //Placeholder for future inclusion of magnetic materials
	
	double epsXavg, sigXavg, epsYavg, sigYavg; // Used to compute averages for H prefactors
	//Loop over interior of arrays
	for(int x = 1; x<(arraySize.x-1); x++)
	{
		for(int y = 1; y<(arraySize.y-1); y++)
		{
			p = xy2gid(x,y,arraySize.x); //Current point
			pXp = xy2gid(x+1,y,arraySize.x); //x+1 point
			pYp = xy2gid(x,y+1,arraySize.y); //y+1 point
			
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
