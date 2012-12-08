#ifndef UTILITIES_H
#define UTILITIES_H
/*This file contains functions for background operations not directly associated with any class */


void initializeChunkMap() //Build or rebuild map of which processor a given chunk resides on
void updateChunkMap() //Process chunk map to get neighbor information
int gid2x(int gid, int sizeX); //Convert gid (such as processor rank) to x position, using the size of the array in the x direction
int gid2y(int gid, int sizeX); //Convert gid (such as processor rank) to y position, using the size of the array in the x direction
int xy2gid(int x, int y, int sizeX); //Convert x-y position to gid (such as processor rank), using the size of the array in the x direction


#endif //UTILITIES_H