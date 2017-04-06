#include "mpi.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

const int TAG = 5;

int readInFromFile( char file[], int* &data, int &dataSize, int numProcessors );
void moveToSmallBucket( int bucket[], int data[], int size, int numBuckets, int processRank, int total );
void insertionSort( int arr[], int length );
void printSorted( int arr[], int length );

int main(int argc, char* argv[])
   {
    
    MPI_Status status;   
    int myRank, numProcessors, bucketSize, totalSize;
    int *data,*bucket;
    double startTime, endTime;
    int *tempArr;

    //initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size( MPI_COMM_WORLD, &numProcessors );

    //preliminary master work( read in file/send out unsorted big buckets )
    if( myRank == 0 )
      {
       bucketSize = readInFromFile( argv[1], data, totalSize, numProcessors );
       //make bucket
       bucket = new int[ totalSize + 1 ];
       //wait for all processors to recieve unsorted big buckets
       MPI_Barrier( MPI_COMM_WORLD );
      }

    //preliminary slave work (recieve total size, amount per processor etc. )
    else
        {
         MPI_Bcast( &totalSize, 1, MPI_INT, 0, MPI_COMM_WORLD );
         MPI_Bcast( &bucketSize, 1, MPI_INT, 0, MPI_COMM_WORLD );

         //allocate memory
         data = new int[ bucketSize ];
         //recieve unsorted big bucket
         MPI_Recv( data, bucketSize, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status );
         //make bucket
         bucket = new int[ totalSize + 1];
         //wait for all processors
         MPI_Barrier( MPI_COMM_WORLD );
        }

    //actual timed work for master
    if( myRank == 0 )
      {
       startTime = MPI_Wtime();
       //put into small buckets and move around into correct bucket
       moveToSmallBucket( bucket, data, bucketSize, numProcessors, myRank, totalSize );
       tempArr = new int[ totalSize ];

       for( int i = 0; i < bucket[0]; i++ )
          {
           tempArr[ i ] = bucket[ i + 1 ];
           
          }

       //sort each large bucket
       insertionSort(tempArr, bucket[0] );
       //barrier call to show end
       MPI_Barrier( MPI_COMM_WORLD);
       endTime = MPI_Wtime() - startTime;
       cout << "Time taken: " << endTime << endl;
      }
    else
        {
         //move to small bucket
         moveToSmallBucket( bucket, data, bucketSize, numProcessors, myRank, totalSize );
         tempArr = new int[ totalSize ];
         for( int i = 0; i < bucket[0]; i++ )
          {
           tempArr[ i ] = bucket[ i + 1 ];
          }
         //sort each large bucket
         insertionSort(tempArr, bucket[0] );
         //barrier call to show end
         MPI_Barrier( MPI_COMM_WORLD );
          
        }
/*
    //Trying to print using barrier 
    //if master just print then call Barrier
    if( myRank == 0 )
      {
       for( int i = 0; i < bucket[0]; i++ )
          {
           cout << tempArr[ i ] << endl;
          }
       MPI_Barrier(MPI_COMM_WORLD);
       //loop barriers for slaves
       for( int i = 1; i < numProcessors; i++ )
          {
           MPI_Barrier(MPI_COMM_WORLD);
          }
      }
    //have all slaves wait for previous slave
    else
        {
         MPI_Barrier(MPI_COMM_WORLD);
         for( int i = 1; i < numProcessors; i++ )
            {
             if( myRank == i )
               {
                //your turn to print!
                for( int j = 0; j < bucket[0]; j++ )
                   {
                    cout << tempArr[ j ] << endl;
                   }
               }
             MPI_Barrier(MPI_COMM_WORLD);  
            }
        }

*/

     

     delete[] bucket;
     delete[] data;
     delete[] tempArr;
     MPI_Finalize();
   }
int readInFromFile( char file[], int* &data, int &dataSize, int numProcessors )
   {
    ifstream fin;
    int counter, numPerProcessor, remainingWork, currentNum, innerCounter, currentProcessor;

    //open number file
    fin.open( file );
    //find out amount of numbers to sort
    fin >> dataSize;
    //send total size to all processors
    MPI_Bcast( &dataSize, 1, MPI_INT, 0, MPI_COMM_WORLD ); 

    //find out how many items per processor
    numPerProcessor = dataSize / numProcessors;
  
    //send number per processor to all processors
    MPI_Bcast( &numPerProcessor, 1, MPI_INT, 0, MPI_COMM_WORLD );
  
    //find work left(remainder for master processor )
    remainingWork = dataSize - ( numPerProcessor * ( numProcessors - 1) );
    //allocate memory for the numbers
    data = new int[ remainingWork ];
    //create memory to send out to other processors
    int *sendArr = new int[ remainingWork ];

    //read in data and send out
    innerCounter = 0;
    currentProcessor = 1;
    for(counter = 1; counter < dataSize; counter++ )
          {
           fin >> currentNum;
           data[ innerCounter ] = currentNum;
           sendArr[ innerCounter ] = currentNum;
           innerCounter++;
           
           //check if its the correct amount to send out 
           if( innerCounter >= numPerProcessor && currentProcessor < numProcessors  )
             {
              MPI_Send(sendArr,numPerProcessor, MPI_INT, currentProcessor, TAG, MPI_COMM_WORLD );
              currentProcessor++;
              innerCounter = 0;            
             }    
          }
     fin.close();
     delete sendArr;
     //return size of master
     return remainingWork;
   }

void moveToSmallBucket( int bucket[], int data[], int size, int numBuckets, int processRank, int total )
   {
    int counter, bucketNum, rangeSize, bucketIndex, innerCounter, numPerBucket;
    int *buffer = new int[ total + 1 ];
    int **buckets = new int*[ numBuckets ];
    MPI_Status status;
    MPI_Request request;
      
    //allocate the 2d array of buckets
    for( int i = 0; i < numBuckets; i++ )
       {
        buckets[i] = new int[ total + 1 ];
       }
    //get range size ( assume largest of 9999 )
    rangeSize = 9999 / numBuckets;
    
    //set buckets to 0
    for( counter = 0; counter < numBuckets; counter++ )
       {
        buckets[ counter ][0] = 0;
       }

    //get numbers into little buckets
    for( counter = 0; counter < size; counter++ )
       {
        bucketNum = data[ counter ] / rangeSize;
        //check range of bucket
        if( bucketNum >= numBuckets )
          {
           bucketNum = numBuckets - 1;
          }
        //find correct bucket to place number
        bucketIndex = buckets[ bucketNum ][0] + 1;
        //place into bucket
        buckets[ bucketNum ][ bucketIndex ] = data[ counter ];
        //increment the count for the bucket
        buckets[ bucketNum ][0] = buckets[ bucketNum ][0] + 1;

       }
    //send little buckets out to big buckets
    for( counter = 0; counter < numBuckets; counter++ )
       {
        //exclude processors own big bucket
        if( counter != processRank )
          {
           MPI_Isend( buckets[ counter ], buckets[ counter ][0] + 1, MPI_INT, counter,
                      TAG, MPI_COMM_WORLD, &request );
          }
       }
    //manage big buckets
     //set bucket to 0 initally and first index
    numPerBucket = 0;
    bucketIndex = 1;

    //recieve buckets from other processors
    for( counter = 0; counter < numBuckets - 1; counter++ )
       {
        //receive a small bucket
        MPI_Recv( buffer, total + 1, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status );
        //add new amount to bucket count
        numPerBucket = numPerBucket + buffer[0];
        //store the data
        for( innerCounter = 1; innerCounter < buffer[0] + 1; innerCounter++, bucketIndex++ )
           {
            bucket[ bucketIndex ] = buffer[ innerCounter ];
            
           }
       }
       //set numbers in bucket
       bucket[0] = numPerBucket + buckets[ processRank ][0];
       //get the numbers and reset bucket index
       for( counter = 1; counter < buckets[ processRank ][0] + 1; counter++ )
          {
           bucket[ bucketIndex ] = buckets[ processRank ][ counter ];
           bucketIndex++;
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

void printSorted( int arr[], int length )
   {
    

   }









