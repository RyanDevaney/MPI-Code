#include "mpi.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

void bucketSort(int arr[], int number );
void insertionSort( int arr[], int length );

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
    for( counter = 0; counter < amountToSort; counter++ )
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
    
    /* //this code is used only to print. Uncomment if you wish to see sorted array
    cout << "sorted array " << endl;
    for(int i = 1; i < amountToSort; i++ )
       {
        cout << arr[i] << endl;
       }
   */
   }
       

void bucketSort(int arr[], int number )
   {
    int counter, temp, bucketNum;
    //assume numbers are from 0 to 9999
    int range = 9999 / 10;
    //create  10 buckets
    int **buckets = new int*[ 10 ];
    int numBuckets = 10;
    int bucketIndex;

    //allocate memory for buckets
    for( counter = 0; counter < numBuckets; counter++ )
       {
        buckets[ counter ] = new int[ number ];    
       }
    //set buckets to 0
    for( counter = 0; counter < numBuckets; counter++ )
       {
        buckets[ counter ][0] = 0;
       }
    //put numbers from array into correct bucket
    for( counter = 0; counter < number; counter++ )
       {
        bucketNum = arr[ counter ] / range;
        //check range of bucket
        if( bucketNum >= numBuckets )
          {
           bucketNum = numBuckets - 1;
          }
        //find correct bucket to place number
        bucketIndex = buckets[ bucketNum ][0] + 1;
        //place into bucket
        buckets[ bucketNum ][ bucketIndex ] = arr[ counter ];
        //increment the count for the bucket
        buckets[ bucketNum ][0] = buckets[ bucketNum ][0] + 1;

       } 
      int *tempArr = new int[ number ];
      int innerCounter = 0;
      temp = 1;
      for( counter = 0; counter < numBuckets; counter++ )
         {
          //copy data into temp
          for( innerCounter = 0; innerCounter < buckets[ counter ][0]; innerCounter++ )
             {
              tempArr[ innerCounter ] = buckets[ counter ][ innerCounter + 1 ];
             }
          insertionSort( tempArr, buckets[ counter ][0] );
          for( innerCounter = 0; innerCounter < buckets[ counter ][0]; innerCounter++ )
             {
              arr[ temp ] = tempArr[ innerCounter ];
              temp++;
             }
         }

   }
void insertionSort( int arr[], int length )
   {
    int counter, temp;

    for( int i = 0; i < length; i++ )
       {
        counter = i;
        while( counter > 0 && arr[ counter ] < arr[ counter - 1 ] )
             {
              temp = arr[ counter ];
              arr[ counter ] = arr[ counter - 1 ];
              arr[ counter - 1 ] = temp;
              counter--;
             }
       }

   }
