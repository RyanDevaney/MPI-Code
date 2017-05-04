#include "mpi.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>

using namespace std;

const int TAG = 1;

void matrixAlloc(int** &matrix, int numRows, int numCols);
void zeroOutMatrix(int** matrix, int numRows, int numCols);
void findRowsAndCols( int numProcessors, int rank, int &destRow, int &destCol, int &srcRow, int &srcCol );
void makeMainMatrix(int** &A, int** &B, int** &C, int numRows, int numCols );
void distributeMatrix(int** matrix, int** subMatrix, int numRows, int numCols,
                      int numProcessors, int size );
void rowShift(int** matrix, int numRows, int numCols, int numProcessors, int rank,
              int dest, int src, int first);
void colShift(int** matrix, int numRows, int numCols, int numProcessors, int rank,
              int dest, int src, int first);
void multMatrix(int** A, int** B, int** C, int numRows, int numCols);
void slaveRec(int** matrix, int numRows, int numCols );
void gatherMatrix(int** matrixC, int **subMatrixC, int numRows, int numColumns, int numProcessors);
int readInFile(char* file );
void readInMatrix(int** &A, int** &B, int** &C, int size, char* file1, char* file2 );

int main(int argc, char* argv[])
   {
    //variables
    double startTime, endTime;
    int myRank, numProcessors, numRows, numCols, setSize, dRow, dCol, sRow, sCol, index, sqrtProc;
    int **matrixA, **matrixB, **matrixC, **subMatrixA, **subMatrixB, **subMatrixC;
    bool inputFile = false;

    //////////////////////////////////////////////////////////////////////////////////////////
    //Display variable. Set to 1 if you want to see input and result matrices/////////////////
    int disp = 0; ////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    

    //MPI Initialization
    MPI_Init(&argc, &argv );
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcessors);

    //check to see if there is file input
    if( argc == 3 )
      {
       inputFile = true;
      }    

    if( inputFile )
      {
       numRows = numCols = readInFile( argv[1] );
      }
    //prelim work( allocate memory/get matrix numbers )
    else
        {
         //get the number of rows and columns
         numRows = numCols = atoi(argv[1]);
        }

    //get the square root of the number of processors
    sqrtProc = sqrt( numProcessors);

    //calculate the size of matrix divided by the root of processors
    setSize = numRows / sqrtProc;

    //allocate memory for non main matrices
    matrixAlloc( subMatrixA, setSize, setSize );
    matrixAlloc( subMatrixB, setSize, setSize );
    matrixAlloc( subMatrixC, setSize, setSize );   

    //put 0's in a sub matrix
    zeroOutMatrix( subMatrixC, setSize, setSize);

    //Each processor needs to know its source and destination rows and columns
    findRowsAndCols( numProcessors, myRank, dRow, dCol, sRow, sCol );


    //generate matrix and send out subMatrix to slaves(master only)
    if( myRank == 0 )
      {
       
       cout << "Number of rows/columns: " << numRows << endl;
       if( inputFile)
         {
          readInMatrix( matrixA, matrixB, matrixC, numRows, argv[1], argv[2] );
         }
       else
           {
            makeMainMatrix(matrixA, matrixB, matrixC, numRows, numCols );
           }
       //display if required
       if( disp == 1 )
         {
          cout << "Displaying Matrix A" << endl;

          for( index = 0; index < numRows; index++ )
             {
              for( int i = 0; i < numRows; i++ )
                 {
                  cout << matrixA[ index ][i] << ' '; 
                 }
               cout << endl;
             }
          cout << "Displaying Matrix B" << endl;
          for( index = 0; index < numRows; index++ )
             {
              for( int i = 0; i < numRows; i++ )
                 {
                  cout << matrixB[ index ][i] << ' '; 
                 }
               cout << endl;
             }
          }
       //send out matrices to slave processors
       distributeMatrix( matrixA, subMatrixA, numRows, numCols, numProcessors, sqrtProc );
       distributeMatrix( matrixB, subMatrixB, numRows, numCols, numProcessors, sqrtProc );
      }

    //slave work
    else
        {
   
         //recieve matrix A
         slaveRec( subMatrixA, setSize, setSize );
   
         //recieve matrix B
         slaveRec( subMatrixB, setSize, setSize );

        }
    //Wait for other processors to finish recieving
    MPI_Barrier( MPI_COMM_WORLD);
   
    //begin timer
    startTime = MPI_Wtime();

    rowShift( subMatrixA, setSize, setSize, numProcessors, myRank, dRow, sRow, 0 );
    colShift( subMatrixB, setSize, setSize, numProcessors, myRank, dCol, sCol, 0 );
    //loop through matrix and shift
    for( index = 0; index < sqrtProc; index++ )
       {
        //multiply matrix
        multMatrix( subMatrixA, subMatrixB, subMatrixC, setSize, setSize );
        //shift rows
        rowShift( subMatrixA, setSize, setSize, numProcessors, myRank, dRow, sRow, 1 );
        //shift columns
        colShift( subMatrixB, setSize, setSize, numProcessors, myRank, dCol, sCol, 1 );
        
        
       }
   //wait for all processors to finish
   MPI_Barrier( MPI_COMM_WORLD );
   if( myRank == 0 )
     {
      endTime = MPI_Wtime();
      endTime = endTime - startTime;
      cout << "Time taken: " << endTime << endl;
     }

   

   if( disp == 1 )
     {    
      if(myRank == 0 )
        {
         gatherMatrix( matrixC, subMatrixC, numRows, numCols, numProcessors );
        }      
      else
          {
           for( index = 0; index < setSize; index++ )
              {
               MPI_Send( subMatrixC[ index ], setSize, MPI_INT, 0, TAG, MPI_COMM_WORLD );
              }
          }
      if( myRank == 0 )
        {
         cout << "Displaying final Matrix" << endl;
         for( index = 0; index < numRows; index++ )
            {
             for( int i = 0; i < numRows; i++ )
                {
                 cout << matrixC[ index ][i] << ' '; 
                }
              cout << endl;
            }
        }
   }
  //end of print section
  
    MPI_Finalize();

   }

//Allocates memory for a matrix based on input rows and columns
void matrixAlloc(int** &matrix, int numRows, int numCols)
   {
    int index = 0;

    matrix = new int*[ numRows ];

    for( index = 0; index < numRows; index++ )
       {
        matrix[ index ] = new int[ numCols ];
       }

   }

//sets values to 0 in a matrix
void zeroOutMatrix(int** matrix, int numRows, int numCols)
   {
    for(int i = 0; i < numRows; i++ )
       {
        for( int j = 0; j < numCols; j++ )
           {
            matrix[i][j] = 0;
           }
       }
   }

//Finds the destination for rows/columns and sources for rows/columns
void findRowsAndCols( int numProcessors, int rank, int &destRow, int &destCol, int &srcRow, int &srcCol )
   {
    int myRow, sqrtProc;
    sqrtProc = sqrt(numProcessors);
    //calculate row and col
    myRow = rank / sqrtProc;

    //dest and src rows will be +/- 1 of rank
    srcRow = rank + 1;
    destRow = rank - 1;

    //dest and src cols will be processor rank +/- sqrtProc
    srcCol = rank + sqrtProc;
    destCol = rank - sqrtProc;
   
    //check to see if rows and cols are still in matrix
    if( destRow < 0 || (destRow / sqrtProc) != myRow )
      {
       destRow += sqrtProc;
      }
    if( srcRow / sqrtProc != myRow )
      {
       srcRow -= sqrtProc;
      }
    if( destCol < 0 )
      {
       destCol += numProcessors;
      }
    if( srcCol >= numProcessors )
      {
       srcCol -= numProcessors;
      }
   }

//Allocates memory for the main matrices. Also fills the first two with random numbers and the result
// matrix with 0's
void makeMainMatrix(int** &A, int** &B, int** &C, int numRows, int numCols )
   {
    int randNum;

    //allocate memory for matrices
    matrixAlloc( A, numRows, numCols );
    matrixAlloc( B, numRows, numCols );
    matrixAlloc( C, numRows, numCols );
   
    //fill in matrix A and B with seeded random numbers and C with 0's
    for( int i =0; i < numRows; i++ )
       {
        for( int j = 0; j < numCols; j++ )
           {
            randNum = random() % 10;
            A[i][j] = randNum;
            B[i][j] = randNum;
            C[i][j] = 0;
           }
       }

   }

//Gives out the matrix to all slave processors
void distributeMatrix(int** matrix, int** subMatrix, int numRows, int numCols,
                      int numProcessors, int size )
   {

    // Initialize processors/variables
   int** temp;
   int subRow, subCol, index, innerIndex, processor, sendRow, sendCol, j, k;
    
    //calculate sub rows/columns
    subRow = subCol = numRows / size;
    
    
    //allocate temp
    matrixAlloc( temp, subRow, subCol );
       
    //put the main matrix into the temp
    for( index = 0; index < subRow; index++ )
       {
        for( innerIndex = 0; innerIndex < subCol; innerIndex++ )
           {
            subMatrix[ index ][ innerIndex ] = matrix[ index ][ innerIndex ];
           }
       }
       
    // loop sending the rows out all processes
    for( processor = 1; processor < numProcessors; processor++ )
       {
        // find index for rows
        for( j = 0, sendRow = 0; j < processor / size; j++ )
           {
            sendRow += subRow;
           }
           
        // find the index for columns
        for( k = 0, sendCol = 0; k < processor % size; k++ )
           {
            sendCol += subCol;
           }
         
        // store data in process
        for( j = 0, index = sendRow; index < sendRow + subRow; index++, j++ )
           {
            for( k = 0, innerIndex = sendCol; innerIndex < sendCol + subCol; innerIndex++, k++ )
               {
                temp[ j ][ k ] = matrix[ index ][ innerIndex ];
               }
           }
           
        // send the data 
        for( index = 0; index < subRow; index++ )
           {
            MPI_Send( temp[ index ], subRow, MPI_INT, processor, TAG, MPI_COMM_WORLD );
           }
       }

   }

//Performs row shifting based on cannon's algorithm first variable is used to see if it is the initial shift
void rowShift(int** matrix, int numRows, int numCols, int numProcessors, int rank,
              int dest, int src, int first )
   {
    int row, index, innerIndex;
    int** temp;
    MPI_Request request;
    MPI_Status status, status1;
    
    //allocate temp
    matrixAlloc(temp, numRows, numCols );
    //calculate row
    row = rank / sqrt(numProcessors);

    //if its the first time shifting
    if( first == 0 )
      {
       //loop the appropriate amount of times based on cannon's algorithm
       for( index = 0; index < row; index++ )
          { 
           //send out matrix
           for( innerIndex = 0; innerIndex < numRows; innerIndex++ )
              {
               MPI_Isend( matrix[ innerIndex ], numCols, MPI_INT, dest, row, MPI_COMM_WORLD, &request );
              }
           
           //recieve matrix from other processors
           for( innerIndex = 0; innerIndex < numRows; innerIndex++ )
              {
               MPI_Recv( temp[ innerIndex ], numCols, MPI_INT, src, row, MPI_COMM_WORLD, &status );
              }

           MPI_Wait( &request, &status1 );
              
           //write back to matrix
           for( int i = 0; i < numRows; i++ )
              {
               for( int j = 0 ; j < numCols; j++ )
                  {
                   matrix[i][j] = temp[i][j];
                  }
              }
          }
      }
   //othersize not first time running
   else
   {
    //send out matrix to correct processors
    for( index = 0; index < numRows; index++ )
       {
        MPI_Isend( matrix[ index ], numCols, MPI_INT, dest, row, MPI_COMM_WORLD, &request );
       }

    //recieve matrix from other processors
    for( index = 0; index < numRows; index++ )
       {
        MPI_Recv( temp[ index ], numCols, MPI_INT, src, row, MPI_COMM_WORLD, &status );
       }

    //wait
    MPI_Wait( &request, &status1 );

    //write back to matrix
    for( int i = 0; i < numRows; i++ )
       {
        for( int j = 0; j < numCols; j++ )
           {
            matrix[i][j] = temp[i][j];
           }
       }
    }
   } 

//Performs column shifting based on cannon's algorithm
void colShift(int** matrix, int numRows, int numCols, int numProcessors, int rank,
              int dest, int src, int first)
   {
    int col, index, innerIndex;
    int** temp;
    MPI_Request request;
    MPI_Status status, status1;
    int sqrtProc = sqrt(numProcessors);
    //allocate temp
    matrixAlloc(temp, numRows, numCols );
    //calculate column
    col = rank % sqrtProc;

    //if its the first tiem shifting
    if( first == 0 )
      {
       //loop the appropriate amount of times based on cannon's algorithm
       for( index = 0; index < col; index++ )
          { 
           //send out matrix
           for( innerIndex = 0; innerIndex < numRows; innerIndex++ )
              {
               MPI_Isend( matrix[ innerIndex ], numCols, MPI_INT, dest, col, MPI_COMM_WORLD, &request );
              }
           
           //recieve matrix from other processors
           for( innerIndex = 0; innerIndex < numRows; innerIndex++ )
              {
               MPI_Recv( temp[ innerIndex ], numCols, MPI_INT, src, col, MPI_COMM_WORLD, &status );
              }

           MPI_Wait( &request, &status1 );
              
           //write back to matrix
           for( int i = 0; i < numRows; i++ )
              {
               for( int j = 0 ; j < numCols; j++ )
                  {
                   matrix[i][j] = temp[i][j];
                  }
              }
          }
      }
    
   //not first time shifting
   else
   {

    //send out matrix to correct processors
    for( index = 0; index < numCols; index++ )
       {
        MPI_Isend( matrix[ index ], numCols, MPI_INT, dest, col, MPI_COMM_WORLD, &request );
       }

    //recieve matrix from other processors
    for( index = 0; index < numCols; index++ )
       {
        MPI_Recv( temp[ index ], numCols, MPI_INT, src, col, MPI_COMM_WORLD, &status );
       }

    //wait
    MPI_Wait( &request, &status1 );

    //write back to matrix
    for( int i = 0; i < numRows; i++ )
       {
        for( int j = 0; j < numCols; j++ )
           {
            matrix[i][j] = temp[i][j];
           }
       }
    }
   }

//Multiplies two Matrices and puts the result in the third one.
void multMatrix(int** A, int** B, int** C, int numRows, int numCols )
   {
    //loop through matrices
    for( int i = 0; i < numRows; i++ )
       {
        for( int j = 0; j < numCols; j++ )
           {
            for( int k = 0; k < numCols; k++ )
               {

                C[i][j] += A[i][k] * B[k][j];
               }
           }
       }

   }

//Function for a slave processor to recieve a matrix.
void slaveRec(int** matrix, int numRows, int numCols )
   {
    MPI_Status status;
    for( int i = 0; i < numRows; i++ )
       {
        MPI_Recv(&(matrix[i][0]), numCols, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status );
       }
   }

//Only used for printing. The master gathers all sub matrices from slave processors.
void gatherMatrix(int** matrixC, int **subMatrixC, int numRows, int numColumns, int numProcessors)
   {
    int** temp;
    int subRows, subCols, index, innerIndex, rowIndex, colIndex, k, l, i, j;
    int sqrtProc = sqrt(numProcessors); 
    MPI_Status status, status1;
    
    //calculate sub rows/columns
    
    subRows = numRows / sqrtProc;
    subCols = numColumns / sqrtProc;
    
    //allocate memory for temp
    matrixAlloc(temp, subRows, subCols );
       
    //transfer data to temp
    for( i = 0; i < subRows; i++ )
       {
        for( j = 0; j < subCols; j++ )
           {
            matrixC[i][j] = subMatrixC[i][j];
           }
       }
       
    //Collect the matrix
    for( index = 1; index < numProcessors; index++ )
       {
        //recieve first row
        MPI_Recv( temp[0], subCols, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status );
        
        //get the rest of the rows from the processor
        for( innerIndex = 1; innerIndex < subRows; innerIndex++ )
           {
            MPI_Recv( temp[ innerIndex ], subCols, MPI_INT,
                      status.MPI_SOURCE, TAG, MPI_COMM_WORLD, &status1 );
           }
           
        //calculate the index for the row
        for( i = 0, rowIndex = 0; i < status.MPI_SOURCE / sqrtProc; i++ )
           {
            rowIndex += subRows;
           }
           
        //calculate the index for the column
        for( j = 0, colIndex = 0; j < status.MPI_SOURCE % sqrtProc; j++ )
           {
            colIndex += subCols;
           }
         
        //transfer back to matrixC
        for( i = 0, k = rowIndex; k < rowIndex + subRows; k++, i++ )
           {
            for( j = 0, l = colIndex; l < colIndex + subCols; l++, j++ )
               {
                matrixC[k][l] = temp[i][j];
               }
           } 
       }
   } 
//used to get the size of the matrix from file
int readInFile(char* file )
   {
    int size;
    ifstream fin;
    
    //open file
    fin.open( file );
    //read in size of file
    fin >> size;
    fin.close();
    return size;
   
   }

//read in data from files, allocate memory for matrices, and fill in resulting with 0   
void readInMatrix(int** &A, int** &B, int** &C, int size, char* file1, char* file2  )
   {
    fstream fin;
    int temp;

    //allocate memory for matrices
    matrixAlloc( A, size, size );
    matrixAlloc( B, size, size );
    matrixAlloc( C, size, size );

    //read in matrix A
    fin.open( file1 );
    fin >> temp;
    for(int i = 0; i < size; i++ )
       {
        for(int j =0; j < size; j++ )
           {
            fin >> A[i][j];
           }
       }
   
    fin.close();
    fin.open( file2 );
    fin >> temp;
    //read in matrix b
    for(int i = 0; i < size; i++ )
       {
        for(int j = 0; j < size; j++ )
           {
            fin >> B[i][j];
           }
       }
    
    //set matrix C to all 0
    for(int i = 0; i < size; i++ )
       {
        for(int j = 0; j < size; j++ )
           {
            C[i][j] = 0;
           }
       }
    fin.close();
   }







