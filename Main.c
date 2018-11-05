#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

//#define SIZE 1000000      // trabalho final com o valores 10.000, 100.000, 1.000.000

// BUBBLE SORT
void BubbleSort(int n, int * vetor) {
  int c =0, d, troca, trocou =1;

  while (c < (n-1) && trocou ) {
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vetor[d] > vetor[d+1]) {
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
            }
        c++;
  }
}

int main(int argc, char *argv[]) {
  int array[SIZE], p, id,i ;
  double elapsed_time ;

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  if (id != 0) {
     MPI_Finalize();
     exit(0);
  }

  // INICIALIZA O ARRAY A SER ORDENADO (COM O PIOR CASO)
  for (i=0 ; i<SIZE; i++)
      array[i] = SIZE-i;

  elapsed_time = - MPI_Wtime ();
  // ORDENA
  BubbleSort(SIZE, &array[0]);
  elapsed_time += MPI_Wtime ();

  // VERIFICA SE A ORDENAÇÃO FUNCIONOU
  for (i=0 ; i<SIZE-1; i++)
      if (array[i] > array[i+1])
         return 1;

  printf("%lf",elapsed_time);
  MPI_Finalize();
  return 0;
}