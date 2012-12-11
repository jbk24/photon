#ifndef UTILITIES_H
#define UTILITIES_H
/*This file contains functions for background operations not directly associated with any class */
#include <string>
#include "EdgeBoundsClass.h"

void initializeChunkMap(); //Build map of which processor a given chunk resides on
void updateChunkList(); //Update list of chunks owned by current processor
int getChunkEdgeConditions(int gid, int gidOther); //Get edge conditions for edge between chunk (x,y) and chunk (otherX, otherY)
void ChunkMap2csv(string filePath); //Write chunk map to csv file
int epsilonSigmaOverlap(); //Communicate epsilon and sigma along edges to achieve array overlap
int oneWayIntraArrayCopy(double *array, int sizeX, EdgeBoundsClass origin, EdgeBoundsClass dest); //Copy a chunk of elements from one part of an array to another
int oneWayInternalArrayCopy(double *originArray, double *destArray, int originSizeX, int destSizeX, EdgeBoundsClass origin, EdgeBoundsClass dest); //Copy a chunk of elements from one array to another
//Copy data between two arrays
int twoWayInternalArrayCopy(double *array1, double *array2, int array1SizeX, int array2SizeX, EdgeBoundsClass array1send, EdgeBoundsClass array1recv, EdgeBoundsClass array2send, EdgeBoundsClass array2recv);


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