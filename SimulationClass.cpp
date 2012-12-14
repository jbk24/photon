#include "SimulationClass.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sstream>
#include <cstddef>

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
void SimulationClass::readControl(string filePath)
{
	//The following structure, split, is used to split the input string into field and data substrings
	//It is a standard structure obtained from the cpp resources library online 
	
	// the input file is read in line by line into string "line" until eof
	string line;
	ifstream inputFile (filePath.c_str());
	if (inputFile.is_open())
	{
		while (!inputFile.eof())
		{
			vector <string> field_data;			
			getline (inputFile, line);
			//here we split each line into field and data by ":" & " ".
			split( field_data, line, ": ", split::no_empties );			
			//cout << field_data[0] << endl; //debug, normally commented out

			// the following basically performs look up function to determine how to treat the data in each field
			switch (field_index(field_data[0]))
			{
				case 0: //no match
				{
					cout << field_data[0] << " is not a valid field, please check your spelling or syntax.\n"; 
					break;
				}
				case 1: //name, in string format
				{
					name = field_data[1];
					break;
				}
				case 2: //directory, in string format
				{
					//we need to add the directory back because the split function splits ":"
					dir = field_data[1];
					break;
				}
				case 3: //maximum resolution, in integer format
				{
					stringstream(field_data[1]) >> maxRes;
					break;
				}
				case 4: //minimum resolution, in integer format
				{
					stringstream(field_data[1]) >> minRes;
					break;
				}
				case 5: //chunk size, in in (x,y) vector format
				{
					vector <string> xy;
					split(xy, field_data[1], "(,)", split::no_empties);
					stringstream(xy[0]) >> chunkSize.x;
					stringstream(xy[1]) >> chunkSize.y;
					break;
				}
				case 6: //global cell dimension, with x & y steps in (x,y) vector format, x & y are integers
				{
					vector <string> xy;
					split(xy, field_data[1], "(,)", split::no_empties);
					stringstream(xy[0]) >> cellGlobal.x;
					stringstream(xy[1]) >> cellGlobal.y;
					break;
				}			
				case 7: //maximum time steps, in integer format
				{
					stringstream(field_data[1]) >> maxT;
					break;					
				}		
				case 8: //maximum time steps, in integer format
				{
					stringstream(field_data[1]) >> freq;
					break;					
				}
				case 9: //number of processors in x and y directions, in vector (x,y) format
				{
					vector <string> xy;
					split(xy, field_data[1], "(,)", split::no_empties);
					stringstream(xy[0]) >> processors.x;
					stringstream(xy[1]) >> processors.y;
					break;
				}
				case 10: //deltaT: FDTD timestep
				{
					stringstream(field_data[1]) >> deltaT;
					break;	
				}
				case 11: //deltaX: FDTD x-spatial step
				{
					stringstream(field_data[1]) >> deltaX;
					break;	
				}
				case 12: //deltaY: FDTD y-spatial step
				{
					stringstream(field_data[1]) >> deltaY;
					break;	
				}
				case 13: //source start location
				{
					vector <string> xy;
					split(xy, field_data[1], "(,)", split::no_empties);
					stringstream(xy[0]) >> sourceLocation.start.x;
					stringstream(xy[1]) >> sourceLocation.start.y;
					break;
				}
				case 14:
				{
					vector <string> xy;
					split(xy, field_data[1], "(,)", split::no_empties);
					stringstream(xy[0]) >> sourceLocation.end.x;
					stringstream(xy[1]) >> sourceLocation.end.y;
					break;
				}
				case 15:
				{
					stringstream(field_data[1]) >> n0;
					break;	
					
				}
				case 16:
				{
					stringstream(field_data[1]) >> nDecay;
					break;	
				}
			}
		}
		inputFile.close();
		
		// Compute dependant quantities from input data
		cellLocal.x = cellGlobal.x/processors.x;
		cellLocal.y = cellGlobal.y/processors.y;
		
		//Compute number of x and y chunks
		numChunks.x = cellGlobal.x/chunkSize.x;
		numChunks.y = cellGlobal.y/chunkSize.y;
		
	}
	else cout << "Can't open input file.\n";
	
}

unsigned int SimulationClass::field_index(string &s)
{
	//Here we list all possible fields. This can be expanded in the future by appending additional fields.
	//This was originally supposed to be done using "find" function, but if statements will suffice.
		if (s == "name")		return 1;
		if (s == "dir")			return 2;
		if (s == "maxRes")		return 3;
		if (s == "minRes")		return 4;
		if (s == "chunkSize")	return 5;
		if (s == "cellGlobal")	return 6;
		if (s == "maxT")		return 7;
		if (s == "freq")		return 8;
		if (s == "processors")  return 9;
		if (s == "deltaT")		return 10;
		if (s == "deltaX")		return 11;
		if (s == "deltaY")		return 12;
		if (s == "sourceStartXY")return 13;
		if (s == "sourceEndXY") return 14;
		if (s == "n0")			return 15;
		if (s == "nDecay")		return 16;

		return 0;
	
}