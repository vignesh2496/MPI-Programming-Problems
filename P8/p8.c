#include <stdio.h>
#include <mpi.h>

int main()
{
	// Initialize MPI
	MPI_Init(NULL, NULL);
	int ID, num_proc, I, position, i, count;
	float F;
	double D[4];
	char buff[1000];

	// Get rank and size  
	MPI_Comm_rank(MPI_COMM_WORLD, &ID);
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

	// Process 0 packs data and sends it point to point to each process
	if(ID == 0)
	{
		printf("\nProcess 0 sending point to point......\n");
		position = 0;
		I = 4; F = 0.2, D[0] = 1.1, D[1] = 1.2, D[2] = 1.3, D[3] = 1.4;
		MPI_Pack(&I, 1, MPI_INT, buff, 100, &position, MPI_COMM_WORLD);
		MPI_Pack(&F, 1, MPI_FLOAT, buff, 100, &position, MPI_COMM_WORLD);
		MPI_Pack(D, 4, MPI_DOUBLE, buff, 100, &position, MPI_COMM_WORLD);
		for(i = 1; i < num_proc; i++)
			MPI_Send(buff, position, MPI_PACKED, i, 0, MPI_COMM_WORLD);
	}
	// Each receiving process unpacks the data and prints it
	else
	{
		position = 0;
		MPI_Status status;
		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_PACKED, &count);
		MPI_Recv(buff, count, MPI_PACKED, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Unpack(buff, count, &position, &I, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buff, count, &position, &F, 1, MPI_FLOAT, MPI_COMM_WORLD);
		MPI_Unpack(buff, count, &position, D, 4, MPI_DOUBLE, MPI_COMM_WORLD);
		printf("Process %d received data : I = %d, F = %f, D[0] = %lf, D[1] = %lf, D[2] = %lf, D[3] = %lf\n", ID, I, F, D[0], D[1], D[2], D[3]);
	}

	// Clean up and finish
	MPI_Finalize();
	return 0;
}