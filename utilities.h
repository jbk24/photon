#ifndef UTILITIES_H
#define UTILITIES_H
/*This file contains functions for background operations not directly associated with any class */
#include <string>

void initializeChunkMap(); //Build map of which processor a given chunk resides on
void updateChunkList(); //Update list of chunks owned by current processor
int getChunkEdgeConditions(int gid, int gidOther); //Get edge conditions for edge between chunk (x,y) and chunk (otherX, otherY)
void ChunkMap2csv(string filePath); //Write chunk map to csv file

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