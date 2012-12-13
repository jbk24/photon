#include "DataBufferClass.h"
#include "global.h"

DataBufferClass::DataBufferClass()
{
	buffersAllocated = false;
}

DataBufferClass::~DataBufferClass()
{
	if(buffersAllocated)
		delete [] dataBuf;
}

//Initalize data buffer size
void DataBufferClass::initalizeBuffer(unsigned int size_X,unsigned int size_Y)
{
	//Set bounds on source data from buffer, will be updated later with message information
	local.start.x=0;
	local.start.y=0;
	local.end.y = 0;
	local.sizeX = 0;
	
	if(size_X>size_Y)
	{
		dataBuf = new double[size_X]();
		local.end.x = size_X;
	}
		
	else
	{
		dataBuf = new double[size_Y]();
		local.end.x = size_Y;
	}	

	buffersAllocated = true;
}
