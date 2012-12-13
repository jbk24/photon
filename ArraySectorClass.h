#ifndef ARRAYSECTORCLASS_H
#define ARRAYSECTORCLASS_H

#include "VectorIntClass.h"

class ArraySectorClass
{
public:
	ArraySectorClass();
	~ArraySectorClass();

	VectorIntClass start; //Start coordinates for sector
	VectorIntClass end; //End coordinates for sector
	int sizeX; //x-size of total array, used to compute gid
};

#endif // ARRAYSECTORCLASS_H
