/******************************************************************************\
	This cpp function will parse a csv file and grab the parameters from
	said file and store them in the Simulation class structure. 
	At the moment, however, it is a standalone program for debugging purpose
\******************************************************************************/
#include "csvFileIO.h"
#include "global.h"
#include "utilities.h"
#include <iostream>
using std::cout;
using std::endl;
#include <fstream>
using std::ifstream;
#include <cstring>
#include <string>
//using std::string
#include <sstream>
using namespace std;
#include <vector>
#include <cstddef>

//The following structure, split, is used to split the input string into field and data substrings
//It is a standard structure obtained from the cpp resources library online 
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

int readEpsSigmaCSV()
{
	int chunkX = Simulation.numChunks.x/Simulation.processors.x;	// Number of chunks in x direction per processor
	int chunkY = Simulation.numChunks.y/Simulation.processors.y; // Number of chunks in y direction per processor
	
	const int colSkip = gid2y(PhotonMPI.rank,Simulation.processors.x)*Simulation.cellLocal.y;		//Number of grid columns to skip until reaching processor column
	const int rowSkip = gid2x(PhotonMPI.rank,Simulation.processors.x)*Simulation.cellLocal.x;		//Number of grid rows to skip until reaching processor row
		
	// the csv file xCsv is read line-by-line into string "xLine" until eof
	string epsilon_line;
	string sigmaX_line;
	string sigmaY_line;
	
	ifstream epsilon_CSV ("epsilon.csv");
	ifstream sigmaX_CSV ("sigmaX.csv");
	ifstream sigmaY_CSV ("sigmaY.csv");
	
	vector <string> epsilon_row;
	vector <string> sigmaX_row;
	vector <string> sigmaY_row;
	
	double epsilon_double;
	double sigmaX_double;
	double sigmaY_double;
	
	int locGx = 0;			    // x location of global grid in the loop
	int locGy = 0;				// y location of global grid in the loop
	int locCx;					// x location of chunk wrt global x
	int locCy;					// y location of chunk wrt global y
	int locX;					// x location within chunk
	int locY;					// y location within chunk
	
	if (epsilon_CSV.is_open() && sigmaX_CSV.is_open() && sigmaY_CSV.is_open())
	{
		while (epsilon_CSV.good() && sigmaX_CSV.good() && sigmaY_CSV.good())
		{
			if (locGx < rowSkip) //skip rows prior to processor
			{
				locGx++;
				getline (epsilon_CSV, epsilon_line); //get sigma line and go to next, but just dump it!
				getline (sigmaX_CSV, sigmaX_line); //get sigmaX line and go to next, but just dump it!
				getline (sigmaY_CSV, sigmaY_line); //get sigmaY line and go to next, but just dump it!
				
			}
			else //read row
			{	
				if (locGx < (rowSkip + Simulation.cellLocal.x))	// step through rows of interest for the processor
					{
					getline (epsilon_CSV, epsilon_line);
					getline (sigmaX_CSV, sigmaX_line);
					getline (sigmaY_CSV, sigmaY_line);
					
					//Split each line by ",".
					split( epsilon_row, epsilon_line, ",", split::no_empties );
					split( sigmaX_row, sigmaX_line, ",", split::no_empties );
					split( sigmaY_row, sigmaY_line, ",", split::no_empties );
					
					locCx = locGx/Simulation.chunkSize.x;	// x location of chunk is globalx location / x size of chunk
					locX  = locGx%Simulation.chunkSize.x;	// x location within chunk is global x location % x size of chunk
					locGy = colSkip;		// location of global y

					while(locGy < (colSkip + Simulation.cellLocal.y))
						{

							locCy = locGy/Simulation.chunkSize.y;	// same logic as locCx
							locY  = locGy%Simulation.chunkSize.y;	// same logic as locX
							
							//Write into locX+1, locY+1 b/c arrays are size 2 larger than actual chunk for overlap
							stringstream(epsilon_row[locGy]) >> epsilon_double;
							ChunkMap[xy2gid(locCx,locCy,Simulation.numChunks.x)].epsilon[xy2gid(locX+1,locY+1,Simulation.chunkSize.x+2)]= epsilon_double;				
							stringstream(sigmaX_row[locGy]) >> sigmaX_double;
							ChunkMap[xy2gid(locCx,locCy,Simulation.numChunks.x)].sigmaX[xy2gid(locX+1,locY+1,Simulation.chunkSize.x+2)] = sigmaX_double;
							stringstream(sigmaY_row[locGy]) >> sigmaY_double;
							ChunkMap[xy2gid(locCx,locCy,Simulation.numChunks.x)].sigmaY[xy2gid(locX+1,locY+1,Simulation.chunkSize.x+2)] = sigmaY_double;
							locGy++;
						} 
					locGx++;
					}
				else	break;
			}				
		}
		epsilon_CSV.close();
		sigmaX_CSV.close();
		sigmaY_CSV.close();
	}
	
	else {
		cout << "Can't open one of the csv files.\n";
		return 1;
	}
	 
	return 0;
}


int writeChunkMapCSV()
{

	//Open files
	ofstream procMap ("ChunkMapProcessors.csv"); //Map of grid chunk processors
	ofstream gidMap ("ChunkMapGID.csv");
	ofstream refineMap ("ChunkMapRefinement.csv"); //Map of grid chunk refinement
	
	//Check if files are open
	if(procMap.is_open() && gidMap.is_open() && refineMap.is_open())
	{
		for(int x = 0; x < Simulation.numChunks.x; x++) // Loop over x (rows in file)
		{
			for(int y = 0; y < Simulation.numChunks.y; y++) //Loop over y (columns in file)
			{
				procMap << ChunkMap[xy2gid(x,y,Simulation.numChunks.x)].processor;
				gidMap << xy2gid(x,y,Simulation.numChunks.x);
				refineMap << ChunkMap[xy2gid(x,y,Simulation.numChunks.x)].refinement;
				if(y < (Simulation.numChunks.y - 1)) // Don't write "," on last column
					procMap << ",";
					gidMap << ",";
					refineMap << ",";
			}
			//Go to next line
			procMap << endl;
			gidMap << endl;
			refineMap << endl;
		}
	}
	else return 1; //Error in opening files
	
	//Close files
	procMap.close();
	gidMap.close();
	refineMap.close();
	
	return 0;
}


int writeArraytoCSV(double *array, int sizeX, int sizeY, string fileName)// Write an array to a CSV file
{
	//Open file
	ofstream file; //CSV file
	file.open(fileName.c_str());
	
	//Check if file is open
	if(file.is_open())
	{
		for(int x = 0; x < sizeX; x++) // Loop over x (rows in file)
		{
			for(int y = 0; y < sizeY; y++) //Loop over y (columns in file)
			{
				file << array[xy2gid(x,y,sizeX)];
				
				if(y < (sizeY - 1)) // Don't write "," on last column
					file << ",";
			}
			//Go to next line
			file << endl;
		}
	}
	else return 1; //Error in opening files
	
	//Close file
	file.close();
	
	return 0;
}

int writeAllChunkstoCSV(int dataType) //Write arrays in all owned chunks to files 
{
	//Loop over owned chunks
	for(int g = 0; g<Simulation.ownedChunkList.size(); g++)
	{
		unsigned int gid = Simulation.ownedChunkList.at(g);
		string filename = "";
		stringstream filenameStream;
		filenameStream.clear();
		filenameStream.str("");
		
		switch(dataType)
		{
			case 0: //epsilon
				filenameStream << "Chunk " << gid << " epsilon.csv";
				filename = filenameStream.str();
				writeArraytoCSV(ChunkMap[gid].epsilon, ChunkMap[gid].arraySize.x, ChunkMap[gid].arraySize.y, filename );
				break;
			
			case 1: //sigmaX
				filenameStream << "Chunk " << gid << " sigmaX.csv";
				filename = filenameStream.str();
				writeArraytoCSV(ChunkMap[gid].sigmaX, ChunkMap[gid].arraySize.x, ChunkMap[gid].arraySize.y, filename );
				break;
			case 2: //sigmaY
				filenameStream << "Chunk " << gid << " sigmaY.csv";
				filename = filenameStream.str();
				writeArraytoCSV(ChunkMap[gid].sigmaX, ChunkMap[gid].arraySize.x, ChunkMap[gid].arraySize.y, filename );
				break;
			case 3: //Ezx
				filenameStream << "Chunk " << gid << " Ezx.csv";
				filename = filenameStream.str();
				writeArraytoCSV(ChunkMap[gid].Ezx, ChunkMap[gid].arraySize.x, ChunkMap[gid].arraySize.y, filename );
				break;
			case 4: //Ezy
				filenameStream << "Chunk " << gid << " Ezy.csv";
				filename = filenameStream.str();
				writeArraytoCSV(ChunkMap[gid].Ezy, ChunkMap[gid].arraySize.x, ChunkMap[gid].arraySize.y, filename );
				break;
			case 5: //Hx
				filenameStream << "Chunk " << gid << " Hx.csv";
				filename = filenameStream.str();
				writeArraytoCSV(ChunkMap[gid].Hx, ChunkMap[gid].arraySize.x, ChunkMap[gid].arraySize.y, filename );
				break;
			case 6: //Hy;
				filenameStream << "Chunk " << gid << " Hy.csv";
				filename = filenameStream.str();
				writeArraytoCSV(ChunkMap[gid].Hy, ChunkMap[gid].arraySize.x, ChunkMap[gid].arraySize.y, filename );
				break;
		
		}
		
		
		
		
		
	}
	return 0;
}

