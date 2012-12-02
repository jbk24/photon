#ifndef SIMULATIONCLASS_H
#define SIMULATIONCLASS_H
/* This class stores all parameters associated with the current simulation, primarily data loaded from the control file, as well as other runtime variables */

#include <string>
#include <VectorInt.h>

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
	int blockSize; //Size of grid chunks
	
	//Computational Cell
	VectorIntClass cell; //Size of global computational cell
	
	//Run Properties
	int maxT; //Maximum number of timesteps 
	
	
};

#endif // SIMULATIONCLASS_H
