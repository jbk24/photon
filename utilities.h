#ifndef UTILITIES_H
#define UTILITIES_H
/*This file contains functions for background operations not directly associated with any class */
#include <string>

void initializeChunkMap(); //Build map of which processor a given chunk resides on
void updateChunkBounds(); //Get min x,y and max x,y chunk for processor
void ChunkMap2csv(string filePath); //Write chunk map to csv file

inline int gid2x(int gid, int sizeX) //Convert gid (such as processor rank) to x position, using the size of the array in the x direction
{
	return gid%sizeX; //y
}

inline int gid2y(int gid, int sizeX) //Convert gid (such as processor rank) to y position, using the size of the array in the x direction
{
	return gid - (gid%sizeX)*sizeX; //x
}

inline int xy2gid(int x, int y, int sizeX) //Convert x-y position to gid (such as processor rank), using the size of the array in the x direction
{
	return y*sizeX + x; //gid
}

#endif //UTILITIES_H