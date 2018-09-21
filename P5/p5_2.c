#include <stdio.h>
#include <mpi.h>

int main()
{
	// Initialize MPI
	MPI_Init(NULL, NULL);
	int ID, num_proc, my_val = 1, recv_val;

	// Get rank and size  
	MPI_Comm_rank(MPI_COMM_WORLD, &ID);
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

	MPI_Status status;
	MPI_Request request;

	// Print value of each process	
	printf("Process %d value : %d\n", ID, 1);

	// Perform the send-recv algo using blocking send and recv calls
	while(num_proc > 1 && ID < num_proc)
	{
		if(num_proc % 2 == 1 && ID == num_proc - 1)
			MPI_Isend(&my_val, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
		else if(ID >= num_proc / 2)
			MPI_Isend(&my_val, 1, MPI_INT, ID - num_proc / 2, 0, MPI_COMM_WORLD, &request);
		else 
		{
			MPI_Irecv(&recv_val, 1, MPI_INT, ID + num_proc / 2, 0, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);
			my_val += recv_val;
			if(num_proc % 2 == 1 && ID == 0)
			{
				MPI_Irecv(&recv_val, 1, MPI_INT, num_proc - 1, 0, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				my_val += recv_val;
			}
		}
		num_proc = num_proc >> 1;
	}

	// Process 0 prints the result
	if(ID == 0)
		printf("Final Sum : %d\n", my_val);

	// Clean up and finish
	MPI_Finalize();
	return 0;
}