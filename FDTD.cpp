/*This file contains the primary FDTD timestepping loop, along with some supporting functions. 
 * The actual update computations are done in loop_over_chunks.cpp to maintain process clairty 
 in the main timestepping loop. */
 
 
 #include "FDTD.h"
 #include "loop_over_chunks.h"
 
 int FDTD_timestep()
 {
	
	 //Compute prefactors by looping over owned chunks
	 for(int g = 0; g<Simulation.ownedChunkList.size(); g++)
	 {
		 //Get gid for current chunk
		unsigned int gid = Simulation.ownedChunkList.at(g);
		
		ChunkMap[gid].computePrefactors();
	 }
	 
 //Timestepping
	for(Simulation.curT = 0; Simulation.curT<Simulation.maxT; Simulation.curT++)
	{

		if(PhotonMPI.rank == 0) //Print timestep if rank 0
			cout << "On timestep: " << Simulation.curT << endl;
		
		//Update E field from H field at curT
		update_E_from_H();

	 
		//Update H field from E field at curT + 1/2
		update_H_from_E();
	 
	}
	
	
	return 0;
 }