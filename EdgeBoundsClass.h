#ifndef EDGEBOUNDSCLASS_H
#define EDGEBOUNDSCLASS_H
#include "VectorIntClass.h"

class EdgeBoundsClass
{
public:
	EdgeBoundsClass();
	~EdgeBoundsClass();

	VectorIntClass start; //Start coordinates for edge
	VectorIntClass end; //End coordinates for edge
	
};

#endif // EDGEBOUNDSCLASS_H
