#include "mpi.h"
#include <iostream>

using namespace std;


int main(int argc, char *argv[])
   {
    int myrank = 0;
    int msgtag = 1;
    int counter, innerCounter;
    double time;
    double firstTime,secondTime;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    if( myrank == 0 )
      {
       int x[10000];
       for( counter = 0; counter < 10000; counter++ )
          {
           x[counter] = 1;
          }
       for( counter = 0; counter < 10000; counter+=5 )
              {
               time = 0;
               for( innerCounter = 0; innerCounter < 10; innerCounter++ )
                  {
                   firstTime = MPI_Wtime();
                   MPI_Send(&x, counter, MPI_INT, 1, msgtag, MPI_COMM_WORLD);
                   MPI_Recv(&x, counter, MPI_INT, 1, msgtag, MPI_COMM_WORLD,&status);
                   secondTime = MPI_Wtime();
                   time = time + (secondTime - firstTime);
                  }
               time = time / 10;
               cout << time << endl;
              }
         
      }
    else
        {
         int x[10000];
         for( counter = 0; counter < 10000; counter+=5 )
            {
             for( innerCounter = 0; innerCounter < 10; innerCounter++ )
                {
                 MPI_Recv(&x,counter,MPI_INT,0,msgtag,MPI_COMM_WORLD,&status);
                 MPI_Send(&x,counter,MPI_INT,0,msgtag,MPI_COMM_WORLD);
                }
            }
        }
    MPI_Finalize();
    

   }
