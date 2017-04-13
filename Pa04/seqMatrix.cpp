#include "mpi.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[] )
 {
  int** matOne, **matTwo, **result;
  int matSize, counter, innerCounter;
  double startTime, endTime;

  MPI_Init(&argc, &argv );

  matSize = atoi( argv[1] );
  
  //allocate memory for matracies
  matOne = new int*[ matSize ];
  matTwo = new int*[ matSize ];
  result = new int*[ matSize ];

  for(counter= 0; counter < matSize; counter++ )
     {
      matOne[ counter ] = new int[ matSize ];
      matTwo[ counter ] = new int[ matSize ];
      result[ counter ] = new int[ matSize ];
      
      //put in random values for first two matracies put in 0 for result
      for(innerCounter = 0; innerCounter < matSize; innerCounter++ )
         {
          matOne[ counter ][ innerCounter ] = rand() % 6 + 1;
          matTwo[ counter ][ innerCounter ] = rand() % 6 + 1;
          result[ counter ][ innerCounter ] = 0;
         }
     }
  startTime = MPI_Wtime();
  //multiply 
  for( counter = 0; counter < matSize; counter++ )
     {
      for( innerCounter = 0; innerCounter < matSize; innerCounter++ )
         {
          for( int i = 0; i < matSize; i++ )
             {
              result[ counter ][ innerCounter ] += 
              matOne[ counter ][ i ] * matTwo[ i ][ innerCounter ];
             }
         }  
     }
   endTime = MPI_Wtime() - startTime;
   cout << "Time taken: " << endTime << endl;
/*
  //display
  for( counter = 0; counter < matSize; counter++ )
     {
      for( innerCounter = 0; innerCounter < matSize; innerCounter++ )
         {
          cout << result[ counter ][ innerCounter ] << " ";
          if( innerCounter == matSize -1 )
            {
             cout << endl;
            }
         }
     }*/
  MPI_Finalize();
 }
