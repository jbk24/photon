/******************************************************************************\
	This cpp function will parse an input file and grab the parameters from
	said file and store them in the Simulation class structure. 
\******************************************************************************/
#include <iostream>
using std::cout;
using std::endl;
#include <cctype>
#include <fstream>
using std::ifstream;
#include <cstring>
#include <string>
#include <functional>
#include <algorithm>
const int MAX_CHARS = 512;
const int MAX_TOKENS = 2;
const char* const DELIMITER = ": ";
#include <sstream>
using namespace std;

unsigned field_index(const char* s )
{
	//Here we list all possible fields. This can be expanded in the future
	//original code using "find" didn't compile because "find" was not found, therefore using if's
	if (s == "name")		return 1;
	if (s == "dir")			return 2;
	if (s == "maxRes")		return 3;
	if (s == "minRes")		return 4;
	if (s == "blockSize")	return 5;
	if (s == "cell")		return 6;		
	if (s == "maxT")		return 7;	
	return 0;
}
int main()
{
	// create a file-reading object
	ifstream input_file;
	input_file.open("input.txt");	// open a file
	if (!input_file.good())
		return 1;	// exit if file not found
	
	// read each line of the file
	while (!input_file.eof())
	{
			// read each line as "field : data" and store in vector field_data 
			char line[MAX_CHARS];
			input_file.getline(line,MAX_CHARS);
			const char* field_data[MAX_TOKENS] = {0}; // initialize to 0
			field_data[0] = strtok(line, DELIMITER);
			field_data[1] = strtok(0, "\n");
			int data = 0;
			switch (field_index( field_data[0] ))
			{
				case 0: //no match
				{
					cout << field_data[0] << " is not a valid field, please check your spelling or syntax.\n"; 
					break;
				}
				case 1: //name, in string format
				{
					cout << field_data[0] << " = " << field_data[1] << "\n";
					break;
				}
				case 2: //directory, in string format
				{
					cout << field_data[0] << " = " << field_data[1] << "\n";
					break;
				}
				case 3: //maximum resolution, in integer format
				{
					stringstream(field_data[1]) >> data;
					cout << field_data[0] << " = " << data << "\n";
					break;
				}
				case 4: //minimum resolution, in integer format
				{
					stringstream(field_data[1]) >> data;
					cout << field_data[0] << " = " << data << "\n";
					break;
				}
				case 5: //block size, in integer format
				{
					stringstream(field_data[1]) >> data;
					cout << field_data[0] << " = " << data << "\n";
					break;
				}
				case 6: //cell dimension, with x & y steps in (x,y) vector format, x & y are integers
				{
					
					cout << field_data[0] << " = " << field_data[1] << "\n";
					break;
				}				
				case 7: //maximum time steps, in integer format
				{
					stringstream(field_data[1]) >> data;
					cout << field_data[0] << " = " << data << "\n";
					break;					
				}
			}
			
	}
}