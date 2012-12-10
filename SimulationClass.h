#ifndef SIMULATIONCLASS_H
#define SIMULATIONCLASS_H
/* This class stores all parameters associated with the current simulation, primarily data loaded from the control file, as well as other runtime variables */

#include <string>
#include "VectorIntClass.h"
#include <vector>
using namespace std;

class SimulationClass
{
public:
	SimulationClass();
	~SimulationClass();
	
	//Parameters
	
	//Administrative
	string name; //Simulation name
	string dir; //Path to simulation directory (containing known sub directories)
	
	//Mesh
	int maxRes; //Maximum mesh resolution, points per unit
	int minRes; //Minimum mesh resolution, points per unit
	
	//Chunks
	VectorIntClass chunkSize; //Initial size of grid chunks, will change under AMR
	VectorIntClass numChunks; //Number of grid chunks in x and y directions
	vector<int> ownedChunkList; //List of gid of chunks owned by current processor, length and constituents will change under AMR
	
	//Computational Cell
	VectorIntClass cellGlobal; //Size of global computational cell, in grid points
	VectorIntClass cellLocal; //Size of local cell at each processor, in grid points
	VectorIntClass processors; //number of processors in x and y direcitons, must multiply to yield total number of cores
	
	//Run Properties
	int maxT; //Maximum number of timesteps 
	
	//Source properties
	double freq;
	
	//Functions
	
	void readControl(string filePath);//Read input control file

private:

	struct split
	{
		enum empties_t { empties_ok, no_empties };
	};
	

		template <typename Container>
	Container& split(
		Container&                            result,
	const typename Container::value_type& s,
	const typename Container::value_type& delimiters,
	split::empties_t                      empties = split::empties_ok )
	{
	result.clear();
	size_t current;
	size_t next = -1;
	do
	{
		if (empties == split::no_empties)
		{
			next = s.find_first_not_of( delimiters, next + 1 );
			if (next == Container::value_type::npos) break;
			next -= 1;
		}
		current = next + 1;
		next = s.find_first_of( delimiters, current );
		result.push_back( s.substr( current, next - current ) );
	}
	while (next != Container::value_type::npos);
	return result;
	}
	
	
		

	//List of possible fields
	unsigned int field_index(string &s);

};

#endif // SIMULATIONCLASS_H

