#ifndef GLOBAL_H
#define GLOBAL_H

#include "SimulationClass.h"
#include "PhotonMPIClass.h"
#include "ChunkClass.h"

extern PhotonMPIClass PhotonMPI;
extern SimulationClass Simulation;
extern ChunkClass ChunkMap[][];

void instantiateChunkMap(); //Initalize chunk map with processor ranks

#endif //GLOBAL_H