#include "PhotonMPIClass.h"

PhotonMPIClass::PhotonMPIClass()
{
}

PhotonMPICla

//Initailize MPI system. Call once, returns 1 if error, 0 otherwise.
int PhotonMPIClass::initialize(int argc, char argv[])
{

	int ierr; //Error return code
	int errorFlag = 0; //Flag to indicate error has occured

	//Start MPI
	ierr = MPI_Init (&argc, &rgv);	
	if(ierr != MPI_SUCCESS)
	{
		printf("MPI_Init error code: %d", ierr);
		errorFlag = 1;
	}
	
	//Get rank
	ierr = MPI_Comm_rank (MPI_COMM_WORLD, &rank);	
	if(ierr != MPI_SUCCESS)
	{
		printf("MPI_Comm_rank error code: %d", ierr);
		errorFlag = 1;
	}
	
	//Get number of processes
	ierr = MPI_Comm_size (MPI_COMM_WORLD, &size);
	if(ierr != MPI_SUCCESS)
	{
		printf("MPI_Comm_rank error code: %d", ierr);	
		errorFlag = 1;
	}

	
	return errorFlag;
}

//Terminate MPI system, do any other cleanup. Call once at end. 
int PhotonMPIClass::finalize()
{
	int ierr; //Error return code
	int errorFlag = 0; //Flag to indicate error has occured
	
	ierr = MPI_Finalize();
	if(ierr != MPI_SUCCESS)
	{
		printf("MPI_Finalize error code: %d", ierr);	
		errorFlag = 1;
	}
	
	return errorFlag;
}

