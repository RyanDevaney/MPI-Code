#include "mpi.h"
#include <iostream>
#include "PIMFuncs.h"
#include <cstring>
#include <fstream>
#include <cstdlib>

using namespace std;

//global constants
static const double real_min = -2.0;
static const double real_max = 2.0;
static const double imag_min = -2.0;
static const double imag_max = 2.0;
static const int MPI_TAG = 5;

struct complex
   {
    double real, imag;
   };

void outputFile();
void findInput( int size, char* input[], int &width, int &height, int &numProcessors);
int cal_pixel(complex c);
void setScaleData( double &scale_real, double &scale_imag, int width, int height);
complex calculateC( double scale_real, double scale_imag, int x, int y);
void splitPixelAmount(int numProcessors, int height, int &rowsPerProcessor, int &remain);
bool checkRemainder( int num );


int main(int argc, char* argv[])
   {
    int pixHeight, pixWidth, index, numProcessors, myRank, begining, last, returnedRow;
    double scale_real, scale_imag;
    MPI_Status status;
    int row[2];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank( MPI_COMM_WORLD, &myRank);
    

    findInput(argc, argv, pixWidth, pixHeight, numProcessors);
    setScaleData( scale_real, scale_imag, pixWidth, pixHeight );
    int calcedRow[pixWidth + 1];

    if( myRank == 0 )
      {
       unsigned char** pixels;
       double start, end;
       bool remainStatus;
       int rowsPerProc, remain;
      
       cout << "Dimensions of picture: " << pixHeight << " X " << pixWidth << endl;
       cout << "Statically calculating with " << numProcessors << " processors" << endl;
       outputFile();

       pixels = new unsigned char *[pixHeight];
       for( int i = 0; i < pixHeight; i++ )
          {
           pixels[i] = new unsigned char[pixWidth];
          }
       
       start = MPI_Wtime();
       splitPixelAmount( numProcessors, pixHeight, rowsPerProc, remain );
       remainStatus = checkRemainder( remain );

       begining = 0;
       last = rowsPerProc - 1;
       if( remain > 0 )
         {
          last++;
          remain--;
         }
       for( index = 0; index < numProcessors; index++ )
          {
           row[0] = begining;
           row[1] = last;

           begining = last + 1;
           last = last + rowsPerProc;
           if( remain > 0 )
             {
              last++;
              remain--;
             }

           MPI_Send( &row, 2, MPI_INT, index + 1, MPI_TAG, MPI_COMM_WORLD );
           
          }
       for( index = 0; index < pixHeight; index++ )
          {

           MPI_Recv( &calcedRow, pixWidth + 1, MPI_INT, MPI_ANY_SOURCE, 
                     MPI_ANY_TAG, MPI_COMM_WORLD, &status);

           returnedRow = calcedRow[0];
           for( int i = 0; i < pixWidth; i++ )
              {
               pixels[returnedRow][i] = calcedRow[i+1];
              }

          }

       end = MPI_Wtime();
    
       end = end - start;
      
       cout << "Time taken to calculate pixels: " << end << endl;
       pim_write_black_and_white( "out/static.ppm", pixWidth, 
                                    pixHeight, (const unsigned char**) pixels);
      }

   else
       {
        complex c;
        int innerIndex, rowIndex, pixOut;

        MPI_Recv( &row, 2, MPI_INT, 0, MPI_TAG, MPI_COMM_WORLD, &status);

        begining = row[0];
        last = row[1];

        
        for( index = begining; index < last + 1; index++ )
           {
            calcedRow[0] = index;
            for( innerIndex = 0, rowIndex = 1; innerIndex < pixWidth; innerIndex++, rowIndex++ )
               {
                c = calculateC( scale_real, scale_imag, innerIndex, index );
                pixOut = cal_pixel( c );
                calcedRow[rowIndex] = pixOut;
     
               }

            MPI_Send( &calcedRow, pixWidth + 1, MPI_INT, 0, MPI_TAG, MPI_COMM_WORLD );
           }
       }

    MPI_Finalize();
   }

void outputFile()
   {
    ofstream fout;
    fout.open( "out/static.ppm" );
    fout.close();

   }

void findInput( int size, char* input[], int &width, int &height, int &numProcessors)
   {
    char temp[500];
    int index, strSize, innerIndex;
    

    for( index = 0; index < size; index++ )
       {
        strcpy( temp, input[index] );
        strSize = strlen( temp );

        if( temp[0] == 'H' || temp[0] == 'h' )
          {
           for( innerIndex = 0; innerIndex < strSize; innerIndex++ )
              {
               temp[innerIndex] = temp[innerIndex + 1];
              }
            temp[strSize - 1] = '\0';
            height = atoi(temp);
          }
        if( temp[0] == 'W' || temp[0] == 'w' )
          {
           for( innerIndex = 0; innerIndex < strSize; innerIndex++ )
              {
               temp[innerIndex] = temp[innerIndex + 1];   
              }
            temp[strSize - 1] = '\0';
            width = atoi(temp);
          }
        if( temp[0] == 'P' || temp[0] == 'p' )
          {
           for( innerIndex = 0; innerIndex < strSize; innerIndex++ )
              {
               temp[innerIndex] = temp[innerIndex + 1];
              }
           temp[strSize -1] = '\0';
           numProcessors = atoi(temp);
           numProcessors--;
          }
       }

   }
//taken from textbook
int cal_pixel(complex c)
   {
    int count, max;
    complex z;
    float temp, lengthsq;
    max = 256;
    z.real = 0;
    z.imag = 0;
    count = 0;
    do
      {
       temp = z.real * z.real - z.imag * z.imag + c.real;
       z.imag = 2 * z.real * z.imag + c.imag;
       z.real = temp;
       lengthsq = z.real * z.real + z.imag * z.imag;
       count++;
      }
     while ((lengthsq < 4.0) && (count < max));
     return count;

   }

//taken from textbook
//scales the data using global constants
void setScaleData( double &scale_real, double &scale_imag, int width, int height)
   {
    scale_real = ( real_max - real_min ) / width;
    scale_imag = ( imag_max - imag_min ) / height;

   }

//taken from textbook
//calculates the complex number with the scale
complex calculateC( double scale_real, double scale_imag, int x, int y)
   {
    complex temp;
    temp.real = real_min + ( (float) x * scale_real);
    temp.imag = imag_min + ( (float) y * scale_imag);
    return temp;

   }

//splits the rows into an even distribution for the processors and gives the remainder
void splitPixelAmount(int numProcessors, int height, int &rowsPerProcessor, int &remain)
   {
    rowsPerProcessor = height / numProcessors;
    remain = height % numProcessors;

   }

//used to check if theres a remainder in height / number of processors
bool checkRemainder( int num )
   {
    if( num > 0 )
      {
       return true;
      }
    return false;
   }


