#ifndef LOOP_OVER_CHUNKS_H
#define LOOP_OVER_CHUNKS_H

#include "global.h"
#include "utilities.h"


//Functions
int update_E_from_H(); //Update E-field from H-field
int update_H_from_E(); //Update H-field from E-field
int processChunkComm(int commType); //Process all inter-chunk communication on current processor


#endif //LOOP_OVER_CHUNKS_H