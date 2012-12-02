#ifndef CHUNKCLASS_H
#define CHUNKCLASS_H
/* This class describes individual "chunks" of the computational grid. Each processor will own more than one chunk*/

class ChunkClass
{
public:
	ChunkClass();
	~ChunkClass();
	
	//Properties
	VectorIntClass size; //Initial size of grid chunk
	VectorIntClass loc; //X-Y location of chunk, in terms of chunks, not grid points
	int processor; //Rank of processor that currently owns chunk--will change under adaptive grid
	int refinement; //Integer indicating refinement level. Current dimesnions are original dimensions multiplied by refinement
	
	
	//Arrays: All arrays have are of size "size*refinement", as above
	int *epsPtr; //Pointer to epsilon array
	int *sigmaPtr; //Pointer to sigma array
	
	int *EzPtr; //Pointer to Ez array
	int *HxPtr; //Pointer to Hx array
	int *HyPtr; //Pointer to Hy array
	
};

#endif // CHUNKCLASS_H
