#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

//#define SIZE 1000000      // trabalho final com o valores 10.000, 100.000, 1.000.000

// BUBBLE SORT
void BubbleSort(int n, int *vetor)
{
    int c = 0, d, troca, trocou = 1;

    while (c < (n - 1) && trocou)
    {
        trocou = 0;
        for (d = 0; d < n - c - 1; d++)
            if (vetor[d] > vetor[d + 1])
            {
                troca = vetor[d];
                vetor[d] = vetor[d + 1];
                vetor[d + 1] = troca;
                trocou = 1;
            }
        c++;
    }
}

int main(int argc, char *argv[])
{
    int hostsize;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    int array[SIZE], p, id, i, selfSize;
    double elapsed_time;

    // INICIALIZA O ARRAY A SER ORDENADO (COM O PIOR CASO)
    for (i = 0; i < SIZE; i++)
        array[i] = SIZE - i;

    // ORDENA
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Get_processor_name(hostname, &hostsize);
    elapsed_time = -MPI_Wtime();
    //Pai = (n-1)/2
    //Filho = n*2+1 e n*2+2
    if (id == 0)
    {
        int *firstHalf = malloc((SIZE / 2) * sizeof(int));
        if (!firstHalf)
        {
            return 1
        }

        int *secondHalf = malloc((SIZE / 2) * sizeof(int));
        if (!secondHalf)
        {
            return 1
        }

        memcpy(firstHalf, array, (SIZE / 2) * sizeof(int));
        memcpy(secondHalf, array + (SIZE / 2), (SIZE / 2) * sizeof(int));

        MPI_Send((SIZE / 2), 1, MPI_INT, (id * 2) + 1, 1, MPI_COMM_WORLD);
        MPI_Send(firstHalf, (SIZE / 2), MPI_INT, (id * 2) + 1, 2, MPI_COMM_WORLD);
        MPI_Send((SIZE / 2), 1, MPI_INT, (id * 2) + 2, 1, MPI_COMM_WORLD);
        MPI_Send(secondHalf, (SIZE / 2), MPI_INT, (id * 2) + 2, 2, MPI_COMM_WORLD);

        //TODO: Logica para receber os arrays ordenados dos filhos!
        MPI_Recv(&firstHalf, (SIZE / 2), MPI_INT, (id * 2) + 1, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&secondHalf, (SIZE / 2), MPI_INT, (id * 2) + 2, 2, MPI_COMM_WORLD, &status);

        //TODO: Ordenar vetores recebidos em um unico
        memcpy(array, firstHalf, sizeof(firstHalf));
        memcpy(&array[sizeof(firstHalf) / sizeof(int)], secondHalf, sizeof(secondHalf));

        free(firstHalf);
        free(secondHalf)

            BubbleSort(selfSize, &array[0]);
    }
    else
    {
        //TODO: Logica para receber o array
        MPI_Recv(&selfSize, 1, MPI_INT, (id - 1) / 2, 1, MPI_COMM_WORLD, &status);
        int auxArray[selfSize];
        MPI_Recv(auxArray, selfSize, MPI_INT, (id - 1) / 2, 2, MPI_COMM_WORLD, &status);
        printf("\n%d", auxArray);
        //TODO: Logica para verificar se processo é folha ou tem filhos
        //Tem Filhos
        if (((id * 2) + 1) <= p || ((id * 2) + 2) <= p)
        {
            //TODO: Logica para os filhos passarem para baixo
            int *firstHalf = malloc((selfSize / 2) * sizeof(int));
            if (!firstHalf)
            {
                return 1
            }

            int *secondHalf = malloc((selfSize / 2) * sizeof(int));
            if (!secondHalf)
            {
                return 1
            }

            memcpy(firstHalf, auxArray, (selfSize / 2) * sizeof(int));
            memcpy(secondHalf, auxArray + (selfSize / 2), (selfSize / 2) * sizeof(int));

            MPI_Send((selfSize / 2), 1, MPI_INT, (id * 2) + 1, 1, MPI_COMM_WORLD);
            MPI_Send(firstHalf, (selfSize / 2), MPI_INT, (id * 2) + 1, 2, MPI_COMM_WORLD);
            MPI_Send((selfSize / 2), 1, MPI_INT, (id * 2) + 2, 1, MPI_COMM_WORLD);
            MPI_Send(secondHalf, (selfSize / 2), MPI_INT, (id * 2) + 2, 2, MPI_COMM_WORLD);

            //TODO: Logica para receber os arrays ordenados dos filhos!

            MPI_Recv(&firstHalf, (selfSize / 2), MPI_INT, (id * 2) + 1, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&secondHalf, (selfSize / 2), MPI_INT, (id * 2) + 2, 2, MPI_COMM_WORLD, &status);

            //TODO: Ordenar vetores recebidos em um unico
            memcpy(auxArray, firstHalf, sizeof(firstHalf));
            memcpy(&auxArray[sizeof(firstHalf) / sizeof(int)], secondHalf, sizeof(secondHalf));
            free(firstHalf);
            free(secondHalf);

            BubbleSort(selfSize, &auxArray[0]);

            MPI_Send(auxArray, selfSize, MPI_INT, (id - 1) / 2, 2, MPI_COMM_WORLD);
        }
        //Não tem Filhos
        else
        {
            BubbleSort(selfSize, &auxArray[0]);
            MPI_Send(auxArray, selfSize, MPI_INT, (id - 1) / 2, 2, MPI_COMM_WORLD);
        }

        free(auxArray)
        //TODO: Enviar vetor ordenado para o pai
        MPI_Finalize();
        exit(0);
    }

    elapsed_time += MPI_Wtime();

    // VERIFICA SE A ORDENAÇÃO FUNCIONOU
    for (i = 0; i < SIZE - 1; i++)
        if (array[i] > array[i + 1])
            return 1;

    printf("%lf", elapsed_time);
    MPI_Finalize();
    return 0;
}