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
    int array[SIZE], p, id, i, t;
    int selfSize; //Tamanho recebido
    int halfSize; //Metade do Tamanho
    int aux1, aux2;
    double elapsed_time;
    MPI_Status status;

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
        selfSize = SIZE;
        halfSize = selfSize/2;
        MPI_Send(&halfSize, 1, MPI_INT, (id * 2) + 1, 1, MPI_COMM_WORLD);
        MPI_Send(&halfSize, 1, MPI_INT, (id * 2) + 2, 1, MPI_COMM_WORLD);

        //TODO: Logica para receber os arrays ordenados dos filhos!
        MPI_Recv(&aux1, 1, MPI_INT, (id * 2) + 1, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&aux2, 1, MPI_INT, (id * 2) + 2, 2, MPI_COMM_WORLD, &status);

        printf("\nPAI : %d , %s , %d \n",id,hostname, aux1+aux2);

    }
    else
    {
        //TODO: Logica para receber o array
        MPI_Recv(&selfSize, 1, MPI_INT, (id - 1) / 2, 1, MPI_COMM_WORLD, &status);
        printf("\nFILHO : %d , %s , %d \n",id,hostname, selfSize);
        halfSize = selfSize/2;

        //TODO: Logica para verificar se processo é folha ou tem filhos
        //Tem Filhos
        if (((id * 2) + 1) <= p && ((id * 2) + 2) <= p)
        {

            MPI_Send(&halfSize, 1, MPI_INT, (id * 2) + 1, 1, MPI_COMM_WORLD);
            MPI_Send(&halfSize, 1, MPI_INT, (id * 2) + 2, 1, MPI_COMM_WORLD);

            //TODO: Logica para receber os arrays ordenados dos filhos!

            MPI_Recv(&aux1, 1, MPI_INT, (id * 2) + 1, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&aux2, 1, MPI_INT, (id * 2) + 2, 2, MPI_COMM_WORLD, &status);

            selfSize = aux1 + aux2;
            MPI_Send(&selfSize, 1, MPI_INT, (id - 1) / 2, 2, MPI_COMM_WORLD);
        }
        //Não tem Filhos
        else
        {   
            MPI_Send(&selfSize, 1, MPI_INT, (id - 1) / 2, 2, MPI_COMM_WORLD);
        }

        //TODO: Enviar vetor ordenado para o pai
    }

    MPI_Finalize();

    elapsed_time += MPI_Wtime();

    // VERIFICA SE A ORDENAÇÃO FUNCIONOU
    for (i = 0; i < SIZE - 1; i++)
        if (array[i] > array[i + 1])
            return 1;

    printf("\n%lf", elapsed_time);
    MPI_Finalize();
    return 0;
}