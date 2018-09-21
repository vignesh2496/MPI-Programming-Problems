#include <stdio.h>
#include <mpi.h>
#include <math.h>
#define N 4

// **************************
// NOTE IMPORTANT           *
// RUN WITH N * N processes *
// **************************

// Structure for each process in the grid
typedef struct
{
  int num_proc, row, col, ID, A, B, C, proc_per_dim;
  // row : Row number of process in 2D grid
  // col : Col number of process in 2D grid
  // ID : Rank of process in MPI_COMM_WORLD
  // A : A-value of process
  // B : B-value of process
  // C : C-value of process
  // proc_per_dim : Number of processes in each dimension
  MPI_Comm comm; // 2D grid communicator group handle
  MPI_Comm row_comm; // Row communicator group handle
  MPI_Comm col_comm; // Column communicator group handle
} grid_info;

// Initialize matrix
void init_matrix(int mat[N][N])
{
  int i, j;
  for(i = 0; i < N; i++)
    for(j = 0; j < N; j++)
      mat[i][j] = (i * 2) + j;
}

// Print matrix
void print_matrix(int mat[N][N])
{
	int i, j;
	for(i = 0; i < N; i++)
    {
      for(j = 0; j < N; j++)
        printf("%d ", mat[i][j]);
      printf("\n");
    }
    printf("\n");
}

int main()
{
  // Initialize MPI
  MPI_Init(NULL, NULL);
  int A[N][N], B[N][N], C[N][N], i, j;

  // Grid information structure variable
  grid_info g;

  // Get rank and size  
  MPI_Comm_rank(MPI_COMM_WORLD, &g.ID);
  MPI_Comm_size(MPI_COMM_WORLD, &g.num_proc);

  g.proc_per_dim = sqrt(g.num_proc);
  g.C = 0;

  // Process 0 initializes matrices A and B
  if(g.ID == 0)
  {
  	init_matrix(A);
  	init_matrix(B);
  	printf("\nA : \n");
  	print_matrix(A);
  	printf("\nB : \n");
  	print_matrix(B);
  }

  // Process 0 scatters the values to the remaining processes
  MPI_Scatter(A, 1, MPI_INT, &g.A, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Scatter(B, 1, MPI_INT, &g.B, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Setup data for the 2D grid creation
  int dims[2] = {N, N}, periods[2] = {0, 0};

  // Make 2D grid of processes
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &(g.comm));

  // Setup data for row and column sub-groups
  int sub_group[2][2]={{1,0}, {0,1}};

  // Make row and column sub-groups
  MPI_Cart_sub(g.comm, sub_group[0], &(g.row_comm));
  MPI_Cart_sub(g.comm, sub_group[1], &(g.col_comm));

  // Get row number from rank in column group
  MPI_Comm_rank(g.col_comm, &g.row);

  // Get col number from rank in row group
  MPI_Comm_rank(g.row_comm, &g.col);

  // Initial skew operation for A and B
  MPI_Sendrecv_replace(&g.A, 1, MPI_INT, (g.col - g.row + g.proc_per_dim) % g.proc_per_dim , 0, (g.col + g.row) % g.proc_per_dim, 0, g.row_comm, MPI_STATUS_IGNORE);
  MPI_Sendrecv_replace(&g.B, 1, MPI_INT, (g.row - g.col + g.proc_per_dim) % g.proc_per_dim , 0, (g.row + g. col) % g.proc_per_dim, 0, g.col_comm, MPI_STATUS_IGNORE);
  g.C += g.A * g.B;
  
  // Remaining shift and add operations
  for(i = 0; i < g.proc_per_dim - 1; i++)
  {
    MPI_Sendrecv_replace(&g.A, 1, MPI_INT, (g.col - 1 + g.proc_per_dim) % g.proc_per_dim , 0, (g.col + 1) % g.proc_per_dim, 0, g.row_comm, MPI_STATUS_IGNORE);
    MPI_Sendrecv_replace(&g.B, 1, MPI_INT, (g.row - 1 + g.proc_per_dim) % g.proc_per_dim , 0, (g.row + 1) % g.proc_per_dim, 0, g.col_comm, MPI_STATUS_IGNORE);
    g.C += g.A * g.B;
  }

  // Gather the results of each process in C
  MPI_Gather(&g.C, 1, MPI_INT, C, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Process 0 prints the product of A and B as C
  if(g.ID == 0)
  {
  	printf("\nC : \n");
  	print_matrix(C);
  }

  // Clean up and finish
  MPI_Finalize();
  return 0;
}