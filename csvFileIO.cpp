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
	
	const int colSkip = (PhotonMPI.rank%Simulation.processors.y)*Simulation.cellLocal.y;		//Number of grid columns to skip until reaching processor column
	const int rowSkip = (PhotonMPI.rank/Simulation.processors.y)*Simulation.cellLocal.x;		//Number of grid rows to skip until reaching processor row
		
	// the csv file xCsv is read line-by-line into string "xLine" until eof
	string eLine;
	string zLine;
	ifstream eCsv ("epsilon.csv");
	ifstream zCsv ("sigma.csv");
	vector <string> eRow;
	vector <string> zRow;
	double eDouble;
	double zDouble;
	int locGx = 0;			    // x location of global grid in the loop
	int locGy = 0;				// y location of global grid in the loop
	int locCx;					// x location of chunk wrt global x
	int locCy;					// y location of chunk wrt global y
	int locX;					// x location within chunk
	int locY;					// y location within chunk
	
	if (eCsv.is_open() && zCsv.is_open())
	{
		while (eCsv.good() && zCsv.good())
		{
			if (locGx < rowSkip)	//skip rows prior to processor
			{
				// cout << "locGx increment: " << locGx << " >> "; 	//debug
				locGx++;
				getline (eCsv, eLine);		//get lineE and go to next, but just dump it!
				getline (zCsv, zLine);		//get lineZ and go to next, but just dump it!
				// cout << locGx << endl;	//debug
			}
			else
			{	
				if (locGx < (rowSkip + Simulation.cellLocal.x))	// step through rows of interest for the processor
					{
					getline (eCsv, eLine);
					getline (zCsv, zLine);
					//here we split each line by ",".
					split( eRow, eLine, ",", split::no_empties );
					split( zRow, zLine, ",", split::no_empties );
					
					locCx = locGx/Simulation.chunkSize.x;	// x location of chunk is globalx location / x size of chunk
					locX  = locGx%Simulation.chunkSize.x;	// x location within chunk is global x location % x size of chunk
					//cout << "X loc: " << locX << "\n";
					locGy = colSkip;		// location of global y

					while(locGy < (colSkip + Simulation.cellLocal.y))
						{
							locCy = locGy/Simulation.chunkSize.y;	// same logic as locCx
							locY  = locGy%Simulation.chunkSize.y;	// same logic as locX
							//cout << "Y loc:" << locY << "\n";
							//cout << "global position = " << locGx << "," << locGy << endl;		//debug
							//cout << "chunk ID = " << locCx << "," << locCy << endl;				//debug
							//cout << "within chunk location = " << locX << "," << locY << endl; 	//debug
							//cout << "Z = " << zRow[locGy] << "E = " << eRow[locGy] << endl;		//debug
							stringstream(eRow[locGy]) >> eDouble;
							ChunkMap[xy2gid(locCx,locCy,Simulation.numChunks.x)].eps[xy2gid(locX,locY,Simulation.chunkSize.x)] = eDouble;
							stringstream(zRow[locGy]) >> zDouble;
							ChunkMap[xy2gid(locCx,locCy,Simulation.numChunks.x)].sigma[xy2gid(locX,locY,Simulation.chunkSize.x)]=zDouble;				
							locGy++;
						} 
					locGx++;
					}
				else	break;
			}				
		}
		eCsv.close();
		zCsv.close();
	}
	else cout << "Can't open one of the csv files.\n";

	//the following outputs the result in a per chunk basis, I hope it is easy to understand...	
	cout << "E & Z local CSV with Rank = " << PhotonMPI.rank << endl;
	
	locGx = rowSkip;					// same procedure as before, move row pointer to starting pt
	for (unsigned int i = 0; i < chunkX; i++)	//note we are stepping per x chunk
	{
		locGy = colSkip;				//point column pointer to starting pt
		for (unsigned int j = 0; j < chunkY; j++)	// stepping y chunk
		{
			locCx = locGx/Simulation.chunkSize.x;		// same procedure to comput chunk ID
			locCy = locGy/Simulation.chunkSize.y;		
			cout << "Chunk ID : (" << locCx << "," << locCy << ")\n";
			cout << "Epsilon map: " << endl;
			//simple writing back loop, can be used later for WriteCsv.cpp
			for (unsigned int x = 0; x < Simulation.chunkSize.x; x++)	// unfortunately two nested for loops are needed for E & Z
			{
				for(unsigned int y = 0; y < Simulation.chunkSize.y; y++)
				{
					cout << ChunkMap[xy2gid(locCx,locCy, Simulation.numChunks.x)].eps[xy2gid(x,y,Simulation.chunkSize.x)] << ",";
				}
				cout << endl;
			}
			cout << "Sigma map: " << endl;			
			for (unsigned int x = 0; x < Simulation.chunkSize.x; x++)
			{
				for(unsigned int y = 0; y < Simulation.chunkSize.y; y++)
				{
					cout << ChunkMap[xy2gid(locCx,locCy, Simulation.numChunks.x)].sigma[xy2gid(x,y,Simulation.chunkSize.x)] << ",";
				}
				cout << endl;
			}
			locGy += Simulation.chunkSize.y;	// notice we are stepping by number of grids per chunk, makes sense right?
		}
		locGx += Simulation.chunkSize.x;
	}
	 
	return 0;
}
