#include "mpi.h"
#include <iostream>

using namespace std;


int main(int argc, char *argv[])
   {
    int myrank = 0;
    int msgtag = 1;
    int counter;
    double timeAvg = 0;
    double firstTime,secondTime;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    if( myrank == 0 )
      {
       int x = 1;
       for( counter = 0; counter < 1000; counter++ )
              {
               firstTime = MPI_Wtime();
               MPI_Send(&x, 1, MPI_INT, 1, msgtag, MPI_COMM_WORLD);
               MPI_Recv(&x, 1, MPI_INT, 1, msgtag, MPI_COMM_WORLD,&status);
               secondTime = MPI_Wtime();
               timeAvg = timeAvg + (secondTime - firstTime);
               
              }
       timeAvg = timeAvg / 1000;
       cout << timeAvg << endl;
      }
    else
        {
         int x;
         for( counter = 0; counter < 1000; counter++ )
            {
             MPI_Recv(&x,1,MPI_INT,0,msgtag,MPI_COMM_WORLD,&status);
             MPI_Send(&x,1,MPI_INT,0,msgtag,MPI_COMM_WORLD);
            }
        
        }
    MPI_Finalize();
    

   }
