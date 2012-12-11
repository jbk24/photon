/*This file contains the primary FDTD timestepping loop, along with some supporting functions. 
 * The actual update computations are done in loop_over_chunks.cpp to maintain process clairty 
 in the main timestepping loop. */
 
 
 #include "FDTD.h"
 #include "loop_over_chunks.h"
 
 int FDTD_timestep()
 {
	
 //Timestepping
	for(Simulation.curT = 0; Simulation.curT<Simulation.maxT; Simulation.curT++)
	{
		//Initialize comm system for E update
		PhotonMPI.chunkCommInit();
	 
		//Update E field from H field at curT
		update_E_from_H();
	 
		//Wait for all E field comm to complete
		PhotonMPI.chunkCommWait();
	 
	 
		//Initialize comm system for H update
		PhotonMPI.chunkCommInit();
	 
		//Update H field from E field at curT + 1/2
		update_H_from_E();
	 
		//Wait for all H field comm to complete
		PhotonMPI.chunkCommWait();
	 
	}
	
	
	return 0;
 }