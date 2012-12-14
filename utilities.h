#ifndef UTILITIES_H
#define UTILITIES_H
/*This file contains functions for background operations not directly associated with any class */
#include <string>
#include "ArraySectorClass.h"

void initializeChunkMap(); //Build map of which processor a given chunk resides on
void updateChunkList(); //Update list of chunks owned by current processor
int getChunkEdgeConditions(int gid, int gidOther); //Get edge conditions for edge between chunk (x,y) and chunk (otherX, otherY)
void ChunkMap2csv(string filePath); //Write chunk map to csv file
int epsilonSigmaOverlap(); //Communicate epsilon and sigma along edges to achieve array overlap
int vectorIntoArrayCopy(double *vector, double *destArray, ArraySectorClass dest);
int arrayIntoVectorCopy(double *vector, double *sourceArray, ArraySectorClass source);
int oneWayIntraArrayCopy(double *array, ArraySectorClass origin, ArraySectorClass dest); //Copy a chunk of elements from one part of an array to another
int oneWayInternalArrayCopy(double *originArray, double *destArray, ArraySectorClass origin, ArraySectorClass dest); //Copy a chunk of elements from one array to another
//Copy data between two arrays
int twoWayInternalArrayCopy(double *array1, double *array2, int array1SizeX, int array2SizeX, ArraySectorClass array1send, ArraySectorClass array1recv, ArraySectorClass array2send, ArraySectorClass array2recv);
int processRecievedData(); //Process all data recieves currently indicated in PhotonMPI.recievedData
int setupSourceInChunks(); //Using input data for source, set-up source information in relevant chunks
inline double* getDataPointerFromType(int gid, int dataType) //Get pointer to array in chunk based on data type
{
		switch (dataType)
		{
			case 0: //epsilon
				return ChunkMap[gid].epsilon;
			case 1: //sigmaX
				return ChunkMap[gid].sigmaX;
			case 2: //sigmaY
				return ChunkMap[gid].sigmaY;
			case 3: //Ezx
				return ChunkMap[gid].Ezx;
			case 4: //Ezy
				return ChunkMap[gid].Ezy;
			case 5: //Hx
				return ChunkMap[gid].Hx;
			case 6: //Hy;
				return ChunkMap[gid].Hy;
		}
		
		//Bad data type
		return 0;
		
}

inline int gid2x(int gid, int sizeX) //Convert gid (such as processor rank) to x position, using the size of the array in the x direction
{
	return gid%sizeX; //y
}

inline int gid2y(int gid, int sizeX) //Convert gid (such as processor rank) to y position, using the size of the array in the x direction
{
	return (gid/sizeX); //x
}

inline int xy2gid(int x, int y, int sizeX) //Convert x-y position to gid (such as processor rank), using the size of the array in the x direction
{
	return y*sizeX + x; //gid
}

#endif //UTILITIES_H