#include <stdio.h>
#include <mpi.h>
#define N 6

int main()
{
	// Initialize MPI
	MPI_Init(NULL, NULL);
	int ID, numproc, count, block_lengths[N], displacements[N], A[N][N] = {{0}}, i, j;

	// Get rank and size  
	MPI_Comm_rank(MPI_COMM_WORLD, &ID);
	MPI_Comm_size(MPI_COMM_WORLD, &numproc);

	// New indexed-datatype
	MPI_Datatype newtype;

	// Setup data for new datatype
	for(i = 0; i < N; i++)
	{
		displacements[i] = i * N + i;
		block_lengths[i] = N - i;
	}
	MPI_Type_indexed(N, block_lengths, displacements, MPI_INT, &newtype);

	// Commit new datatype
	MPI_Type_commit(&newtype);

	// Process 0 initializes 2D array and sends it to Process 1
	if(ID == 0)
	{
		for(i = 0; i < N; i++)
			for(j = 0; j < N; j++)
				A[i][j] = i * N + j + 1;
		MPI_Send(A, 1, newtype, 1, 0, MPI_COMM_WORLD);
	}

	// Process 1 receives the data and prints it
	if(ID == 1)
	{
		MPI_Status status;
		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, newtype, &count);
		MPI_Recv(A, count, newtype, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Process 1 received data from Process 0 : \n");
		for(i = 0; i < N; i++)
		{
			for(j = 0; j < N; j++)
				printf("%d  ", A[i][j]);
			printf("\n");
		}
	}

	// Clean up and finish
	MPI_Finalize();
	return 0;
}