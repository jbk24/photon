#include "SimulationClass.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

using std::cout;
using std::endl;
using std::ifstream;

SimulationClass::SimulationClass()
{
}

SimulationClass::~SimulationClass()
{
}


//Read input control file
SimulationClass::readControl(string filePath)
{
	const int MAX_CHARS_PER_LINE = 512;
	const char* const DELIMITER = ": ";
	
	
	
	
}

SimulationClass::field_index(string &s)
{
	//Here we list all possible fields. This can be expanded in the future
	const char* fields[] = 
	{
		"name",
		"dir",
		"maxRes",
		"minRes",
		"blockSize",
		"cell",
		"maxT",
	};
	unsigned result = find(fields, fields + sizeof(fields), s ) - fields;
	return (result < sizeof(fields)) ? (result + 1) : 0;
}