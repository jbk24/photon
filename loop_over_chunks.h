#ifndef LOOP_OVER_CHUNKS_H
#define LOOP_OVER_CHUNKS_H

#include "global.h"
#include "utilities.h"


//Functions
int update_E_from_H(); //Update E-field from H-field
int update_H_from_E(); //Update H-field from E-field
int E_updateComm(unsigned int gid); //Process all communications for current chunk after E update
int H_updateComm(unsigned int gid); //Process all communications for current chunk after H update

#endif //LOOP_OVER_CHUNKS_H