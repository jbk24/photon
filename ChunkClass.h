#ifndef CHUNKCLASS_H
#define CHUNKCLASS_H
/* This class describes individual "chunks" of the computational grid. Each processor will own more than one chunk*/

class ChunkClass
{
public:
	ChunkClass();
	~ChunkClass();
	
	//Properties
	int processor; //Rank of processor that currently owns chunk--will change under adaptive grid
	int refinement; //Integer indicating refinement level. Current dimesnions are original dimensions multiplied by refinement
	
	//Arrays: 
	//Eps and sigma arrays are of size "size.x*size.y*refinement", as above
	double *eps; //Pointer to epsilon array
	double *sigma; //Pointer to sigma array
	
	//Ex, Hx, Hy arrays are of (size.x+2)(size.y+2)*refinement, for overlap with adjacent arrays
	double *Ez; //Pointer to Ez array
	double *Hx; //Pointer to Hx array
	double *Hy; //Pointer to Hy array
	
	//Methods
	int createMPIStruct();
	
};

#endif // CHUNKCLASS_H
