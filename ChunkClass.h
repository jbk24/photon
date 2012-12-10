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
	
	//Edge conditions:
	// 0 = PEC boundary (edge of computational domain, no comm)
	// 1 = internal comm, matched refinement
	// 2 = internal comm, decreasing refinement (scale down array)
	// 3 = internal comm, increasing refinement (scale up array)
	// 4 = external comm, matched refinement
	// 5 = external comm, decreasing refinement (scale down array)
	// 6 = external comm, increasing refinement (scale up array)
	
	//Edges
	int edgeXn; //Condition on negative-x edge
	int edgeXp; //Condition on positive-x edge
	int edgeYn; //Condition on negative-y edge
	int edgeYp; //Condition on positive-y edge
	
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
