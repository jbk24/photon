#ifndef DATABUFFERCLASS_H
#define DATABUFFERCLASS_H

#include "ArraySectorClass.h"

class DataBufferClass
{
public:
	DataBufferClass();
	~DataBufferClass();
	
	double* dataBuf; //Buffer to data, length is max(arraySize.x, arraySize.y) of owner chunk
	ArraySectorClass local; //Describes bounds of source data in buffer, based on size of message being recieved
	ArraySectorClass remote; //Destination for data (if recieved, unused on send)
	int dataType; //Type of data being recieved (0-6), see PhotonMPIClass.h for definitions 
	
	//Methods
	void initalizeBuffer(unsigned int sizeX, unsigned int sizeY); //Initalize data buffers to max(sizeX,sizeY)
	
private:
	bool buffersAllocated; //used on cleanup

};

#endif // DATABUFFERCLASS_H
