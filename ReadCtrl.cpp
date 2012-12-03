/******************************************************************************\
	This cpp function will parse an input file and grab the parameters from
	said file and store them in the Simulation class structure. 
\******************************************************************************/

#include <iostream>
using std::cout;
using std::endl;

#include <fstream>
using std::ifstream;

#include <vector>
#include <cstring>
const int MAX_CHARS_PER_LINE = 512;
const char* const DELIMITER = ": ";

unsigned field_index( string& s )
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
			// read each line as "field : data\eof" 
			std::getline(input_file,line);
			
			int n = 0; //a for-loop index
			vector <string> fields;	
			split( fields)
			//parse the line with respect to ":" or space
			field = strtok(buf, DELIMITER);
			if (field)	//zero if line is blank or doesn't have ":" or space
			{
				data = strtok(0, )
			}
			switch (field_index( field ))
			{
				case 0: cout << field << " is not a valid field, please check your spelling or syntax.\n";
				case 1: 
			}
			
	}