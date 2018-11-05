#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 1000000
#define MSG_START_CALC 100
#define MSG_CALC_FINISHED 200

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	size_t task_size = N / (size - 1);
	MPI_Status status;
	float sum = 0;
	if (rank == 0)
	{
		float *data = (float*)malloc(N * sizeof(float));
		size_t i;
		for (i = 0; i < N; i++) data[i] = 1;
		for (i = 1; i < size; i++)
		{
			size_t offset = task_size * (i - 1);
			MPI_Send(data + offset, task_size, MPI_FLOAT, i, MSG_START_CALC, MPI_COMM_WORLD);
		}
		free(data);
		for (i = 1; i < size; i++)
		{
			float part_sum;
			MPI_Recv(&part_sum, 1, MPI_FLOAT, i, MSG_CALC_FINISHED, MPI_COMM_WORLD, &status);
			sum += part_sum;
		}
		printf("Sum is %g\n", sum);
	}
	else
	{
		float *data = (float*)malloc(task_size * sizeof(float));
		MPI_Recv(data, task_size, MPI_FLOAT, 0, MSG_START_CALC, MPI_COMM_WORLD, &status);
		float part_sum = 0;
		size_t i;
		for (i = 0; i < task_size; i++) part_sum += data[i];
		MPI_Send(&part_sum, 1, MPI_FLOAT, 0, MSG_CALC_FINISHED, MPI_COMM_WORLD);
		free(data);
	}
	MPI_Finalize();
	return 0;
}
