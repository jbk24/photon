/******************************************************************************\
	This cpp function will parse ChunkMapGID.csv file and grab the Chunk ID
	Then map the field data per chunk back to a full-size plot
\******************************************************************************/
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

int main()
{
	int chunkSizeX = 128;	//Simulation.chunkSize.x	128
	int numChunksY = 8;	//Simulation.numChunks.y	16
	unsigned int lineCount;
	unsigned int gid;
	string gidRow;
	ifstream gidMap ("ChunkMapGID.csv");
	ofstream fullField ("Full_Ezx.csv");
	if (gidMap.is_open() && fullField.is_open())
	{
		while (!gidMap.eof())
		{
			vector <string> rowOfGIDs;
			getline (gidMap, gidRow);
			split(rowOfGIDs,gidRow,", ", split::no_empties);
			//cout << "Row size: " << rowOfGIDs.size() << endl;
			string chunkLine;
			string fieldString[chunkSizeX][numChunksY];
			//string trimmedLine;
		if(rowOfGIDs.size()>1)	//avoid empty line case, not sure why this is happening...
		{
			for (unsigned int i = 0; i < rowOfGIDs.size(); i++)
			{
				string chunkCSV = "Chunk_" + rowOfGIDs[i] + "_Ezx.csv";
				cout << chunkCSV << endl;
				ifstream chunkI (chunkCSV.c_str());
				lineCount = 0;	// initialize every chunk	
				if (chunkI.is_open())
				{
					while (!chunkI.eof())
					{
						if (lineCount == 0 || lineCount > chunkSizeX)
						{
							getline (chunkI,chunkLine);	//do nothing first & last line
							lineCount++;
						} 
						else
						{
							vector <string> rowOfGrids;
							getline (chunkI,chunkLine);
							split(rowOfGrids,chunkLine,", ",split::no_empties);
							for(int j = 1; j<rowOfGrids.size()-1 ;j++)
							{
								fieldString[lineCount-1][i] += rowOfGrids[j] + ",";
							}
							//fieldString[lineCount-1][i] = chunkLine;
							lineCount++;
						}
					}
					chunkI.close();
				}
				else break; //cout << "Can't open " << chunkCSV << endl;
			}
			for(int x = 0; x < chunkSizeX; x++)
			{
				for(int y = 0; y < numChunksY; y++)
				{
					fullField << fieldString[x][y];
				}
				fullField << endl;
			}
		}
		}
		gidMap.close();
	}
	else cout << "Can't open ChunkMapGID.csv file.\n";
	return 0;
}
