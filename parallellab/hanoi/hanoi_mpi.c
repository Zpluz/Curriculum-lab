#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <string.h>

typedef long long ll;
ll a[100];

ll hanoi(int n)
{
    ll step = 0;
    for (int i = n; i > 0; i--)
    {
        step = (step << 1) + 1;
    }
    return step;
}

int main(int argc, char *argv[])
{
    int rank, numprocs, n;
    int start, end;
    MPI_Status status;
    ll step;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    if (rank != 0)
    {
        MPI_Recv(&start, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, &status);
        MPI_Recv(&end, 1, MPI_INT, 0, 100, MPI_COMM_WORLD, &status);
        for (int j = start; j < end; ++j)
        {
            step = hanoi(j);
            MPI_Send(&step, 1, MPI_LONG_LONG_INT, 0, 101, MPI_COMM_WORLD);
        }
    }
    else
    {
        scanf("%d", &n);
        start = 0;
        end = n / 3;
        for (int i = 1; i < numprocs; ++i)
        {
            MPI_Send(&start, 1, MPI_INT, i, 99, MPI_COMM_WORLD);
            MPI_Send(&end, 1, MPI_INT, i, 100, MPI_COMM_WORLD);
            start = end;
            if (i == numprocs - 2)
                end = n;
            else
                end += n / 3;
        }
        for (int i = 1; i < numprocs - 1; ++i)
        {
            for (int j = 0; j < n / 3; ++j)
            {
                MPI_Recv(&a[(i - 1) * (n / 3) + j], 1, MPI_LONG_LONG_INT, i, 101, MPI_COMM_WORLD, &status);
            }
        }
        for (int j = 0; (numprocs - 2) * (n / 3) + j < n; ++j)
        {
            MPI_Recv(&a[(numprocs - 2) * (n / 3) + j], 1, MPI_LONG_LONG_INT, numprocs - 1, 101, MPI_COMM_WORLD, &status);
        }
        for (int i = 0; i < n; ++i)
        {
            printf("%lld ", a[i] + 1);
        }
    }
    MPI_Finalize();
    return 0;
}
