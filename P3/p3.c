#include <stdio.h>
#include <string.h>
#include <mpi.h>

int main()
{
	// Initialize MPI
	MPI_Init(NULL, NULL);
	int ID, num_proc, count, i;
	char message[100];

	// Get rank and size  
	MPI_Comm_rank(MPI_COMM_WORLD, &ID);
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

	// All processes other than Process 0 send their respective messages to Process 0
	if(ID != 0)
	{
		sprintf(message, "Hello from process %d", ID);
		MPI_Send(message, strlen(message) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	}
	// Process 0 receives the messages as they come and prints it
	else
	{
		for(i = 1; i < num_proc; i++)
		{
			MPI_Status status;
			MPI_Probe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			MPI_Get_count(&status, MPI_CHAR, &count);
			MPI_Recv(message, count, MPI_CHAR, status.MPI_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			printf("Message from process %d : %s\n", status.MPI_SOURCE, message);
		}
	}

	// Clean up and finish
	MPI_Finalize();
	return 0;
}