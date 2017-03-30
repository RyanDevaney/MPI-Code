#include "mpi.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

void bucketSort(int arr[], int number );

int main(int argc, char* argv[] )
   {
    int amountToSort, counter;
    int* arr;
    ifstream fin;
    double startTime, endTime;


    MPI_Init(&argc, &argv);
    fin.open( argv[1] );        
    fin >> amountToSort;
    arr = new int[ amountToSort ];
    for( counter = 1; counter < amountToSort; counter++ )
       {
        fin >> arr[ counter ];
       }

    //start time
    startTime = MPI_Wtime();

    //bucket sort
     bucketSort(arr, amountToSort );

    //end time
    endTime = MPI_Wtime();
    endTime = endTime - startTime;
    //print out numbers
    cout << "Time taken to compute: " << endTime << endl;
    

   }

void bucketSort(int arr[], int number )
   {
    int counter;
    //assume numbers are from 0 to 10000
    int range = 10002;
    //create buckets
    int buckets[ range ];

    //set buckets to 0
    for( counter = 0; counter < range; counter++ )
       {
        buckets[ counter ] = 0;
       }

    
    //find how many elements are in each bucket
    for( counter = 0; counter < number; counter++ )
       { 
        buckets[ arr[ counter ] ]++;
       }

    for( int i = 0, j = 0; j < range; j++ )
       {
        for(int k = buckets[j]; k > 0; k-- )
           {
            arr[i++] = j;
           }
       }

   }
