#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

int main()
{
	// Initialize MPI
	MPI_Init(NULL, NULL);
	int num_proc, ID, i;
	float *A, *B, buff;

	// Get rank and size  
	MPI_Comm_rank(MPI_COMM_WORLD, &ID);
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

	// Process 0 initializes the array
	if(ID == 0)
	{
		A = (float *)malloc(sizeof(float) * num_proc);
		B = (float *)malloc(sizeof(float) * num_proc);
		printf("The initial array: \n");
		for(i = 0; i < num_proc; i++)
		{
			A[i] = rand() % 100;
			printf("Index %d : %f\n", i, A[i]);
		}
	}

	// Set up data for non-uniform scattering
	int *displacements = (int*)malloc(sizeof(int) * num_proc);
	int *sendcounts = (int*)malloc(sizeof(int) * num_proc);
	for(i = 0; i < num_proc; i++)
	{
		sendcounts[i] = 1;
		displacements[i] = i;
	}

	// Scatter A
	MPI_Scatterv(A, sendcounts, displacements, MPI_FLOAT, &buff, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

	// Find element-wise square root of the received chunk
	float root = sqrt(buff);

	// Gather results in B
	MPI_Gatherv(&root, 1, MPI_FLOAT, B, sendcounts, displacements, MPI_FLOAT, 0, MPI_COMM_WORLD);
	if(ID == 0)
	{
		printf("\nThe square root array:\n");
		for(i = 0; i < num_proc; i++)
			printf("Index %d : %f\n", i, B[i]);
		printf("\n");
	}
	MPI_Barrier(MPI_COMM_WORLD);

	// Clean up and finish
	MPI_Finalize();
	return 0;
}