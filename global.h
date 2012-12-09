#ifndef GLOBAL_H
#define GLOBAL_H

#include "SimulationClass.h"
#include "PhotonMPIClass.h"
#include "ChunkClass.h"

extern PhotonMPIClass PhotonMPI;
extern SimulationClass Simulation;
extern ChunkClass* ChunkMap; //Pointer to map of chunks, uses linear indexing

int instantiateChunkMap(); //Initalize chunk map with processor ranks
void globalCleanUp(); //Perform all cleanup actions for variables allocated on the heap

#endif //GLOBAL_H