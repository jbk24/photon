#ifndef CSVFILEIO_H
#define CSVFILEIO_h
#include <string>


int readEpsSigmaCSV(); //Read epsilon and sigma .csv files
int writeChunkMapCSV(); //Write out chunk map csv files (processor and refinement)
int writeArraytoCSV(double *array, int sizeX, int sizeY, std::string fileName); // Write an array to a CSV file
int writeAllChunkstoCSV(bool writeEpsilon, bool writeSigma, bool writeE, bool writeH); //Write arrays in all owned chunks to files 

#endif