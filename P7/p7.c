#include <stdio.h>
#include <mpi.h>

struct data
{
	int I;
	float F;
	double D[4];
} s;

int main()
{
	// Initialize MPI
	MPI_Init(NULL, NULL);
	int ID, num_proc, i, count, block_lengths[3] = {1, 1, 4};

	// Get rank and size  
	MPI_Comm_rank(MPI_COMM_WORLD, &ID);
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

	// Setup data for new datatype
	MPI_Aint displacements[3], block1_address, block2_address, block3_address;
	MPI_Get_address(&s.I, &block1_address);
	MPI_Get_address(&s.F, &block2_address);
	MPI_Get_address(s.D, &block3_address);
	displacements[0] = (MPI_Aint) 0;
	displacements[1] = block2_address - block1_address;
	displacements[2] = block3_address - block1_address;
	MPI_Datatype types[3] = {MPI_INT, MPI_FLOAT, MPI_DOUBLE}, newtype;
	MPI_Type_create_struct(3, block_lengths, displacements, types, &newtype);

	// Commit the new datatype
	MPI_Type_commit(&newtype);

	// Process 0 initializes a unit of the new datatype
	if(ID == 0)
	{
		s.I = 3; s.F = 4.5; 
		s.D[0] = 1.1; s.D[1] = 1.2; s.D[2] = 1.3; s.D[3] = 1.4; 
		printf("\nProcess 0 broadcasting......\n");
	}

	// Process 0 broadcasts the initialized unit
	MPI_Bcast(&s, 1, newtype, 0, MPI_COMM_WORLD);

	// Each receiving process prints the data it receives
	if(ID != 0)
		printf("Process %d received data : I = %d, F = %f, D[0] = %lf, D[1] = %lf, D[2] = %lf, D[3] = %lf\n", ID, s.I, s.F, s.D[0], s.D[1], s.D[2], s.D[3]);
	sleep(1);

	// Process 0 initializes a unit of the new datatype and sends it point to point to each process
	if(ID == 0)
	{
		s.I = 6; s.F = 7.5; 
		s.D[0] = 2.1; s.D[1] = 2.2; s.D[2] = 2.3; s.D[3] = 2.4; 
		printf("\nProcess 0 sending point to point......\n");
		for(i = 1; i < num_proc; i++)
			MPI_Send(&s, 1, newtype, i, 0, MPI_COMM_WORLD);
	}
	// Other processes receive the data and print it
	else
	{
		MPI_Status status;
		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, newtype, &count);
		MPI_Recv(&s, count, newtype, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Process %d received data : I = %d, F = %f, D[0] = %lf, D[1] = %lf, D[2] = %lf, D[3] = %lf\n", ID, s.I, s.F, s.D[0], s.D[1], s.D[2], s.D[3]);
	}

	// Clean up and finish
	MPI_Finalize();
	return 0;
}