#include "ChunkClass.h"

ChunkClass::ChunkClass()
{
}

ChunkClass::~ChunkClass()
{
}

//This function creates an MPI Struct to describe the current chunk, for transmitting or recieving data
int ChunkClass::createMPIStruct()
{ 
	
	//Number of blocks
	const unsigned int count = 5; //eps, sigma, Ez, Hx, Hy
	
	//Length of each block
	unsigned int array_of_blocklengths[count]; 
	
	for (unsigned int k =0; k< count; k++)
	{
		array_of_blocklengths[k] = size.x*refinement*size.y*refinement;
	}
	
	//Displacement of address of each block from first
	int array_of_displacements[count];
	array_of_displacements[0]= epsPtr;
	array_of_displacements[1]= sigmaPtr - array_of_displacements[0];
	array_of_displacements[2]= EzPtr- array_of_displacements[0];
	array_of_displacements[3]= HxPtr - array_of_displacements[0];
	array_of_displacements[4]= HyPtr - array_of_displacements[0];
	
	//Type of each data array
	int array_of_types[count];
	
	for (unsigned int k = 0; k < count; k++)
	{
		array_of_types[k] = MPI_DOUBLE;
	}
	
	
	
}