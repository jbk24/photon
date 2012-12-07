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
	ifstream inputFile ("input.txt");
	if (inputFile.is_open())
	{
		while (!inputFile.eof())
		{
			vector <string> field_data;			
			getline (inputFile, line);
			//here we split each line into field and data by ":" & " ".
			split( field_data, line, ": ", split::no_empties );			
			// cout << field_data[0] << endl; //debug, normally commented out

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
					cout << field_data[0] << " = " << field_data[1] << endl;
					break;
				}
				case 2: //directory, in string format
				{
					//we need to add the directory back because the split function splits ":"
					field_data[1].append(":");
					field_data[1].append(field_data[2]);	
					cout << field_data[0] << " = " << field_data[1] << endl;
					break;
				}
				case 3: //maximum resolution, in integer format
				{
					int maxRes;
					stringstream(field_data[1]) >> maxRes;
					cout << field_data[0] << " = " << maxRes << endl;
					break;
				}
				case 4: //minimum resolution, in integer format
				{
					int minRes;
					stringstream(field_data[1]) >> minRes;
					cout << field_data[0] << " = " << minRes << endl;
					break;
				}
				case 5: //block size, in integer format
				{
					int blockSize;
					stringstream(field_data[1]) >> blockSize;
					cout << field_data[0] << " = " << blockSize << endl;
					break;
				}
				case 6: //cell dimension, with x & y steps in (x,y) vector format, x & y are integers
				{
					cout << field_data[0] << " = " << field_data[1] << endl;
					break;
				}				
				case 7: //maximum time steps, in integer format
				{
					int maxT;
					stringstream(field_data[1]) >> maxT;
					cout << field_data[0] << " = " << maxT << endl;
					break;					
				}		
				case 8: //maximum time steps, in integer format
				{
					double freq;
					stringstream(field_data[1]) >> freq;
					cout << field_data[0] << " = " << freq << endl;
					break;					
				}				
			}
		}
		inputFile.close();
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
		if (s == "blockSize")	return 5;
		if (s == "cell")		return 6;		
		if (s == "maxT")		return 7;
		if (s == "freq")		return 8;
		return 0;
	
}