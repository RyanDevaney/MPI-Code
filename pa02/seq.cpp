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

struct complex
   {
    double real, imag;
   };

void outputFile();
void calculateSize( int size, char* input[], int &width, int &height);
int cal_pixel(complex c);
void setScaleData( double &scale_real, double &scale_imag, int width, int height);
complex calculateC( double scale_real, double scale_imag, int x, int y);

int main(int argc, char* argv[])
   {
    //variables
    int pixHeight, pixWidth, index, innerIndex, pixOut;
    double start, end, scale_real, scale_imag;
    unsigned char** pixels;
    complex c;


    //initialize MPI
    MPI_Init(&argc, &argv);

    outputFile();

    calculateSize(argc, argv, pixWidth, pixHeight);

    cout << "Dimensions of picture: " << pixHeight << " X " << pixWidth << endl;

    setScaleData( scale_real, scale_imag, pixWidth, pixHeight );

    //allocate memory for the pixel array
    pixels = new unsigned char *[pixHeight];

    for( int i = 0; i < pixHeight; i++ )
       {
        pixels[i] = new unsigned char[pixWidth];
       }

    start = MPI_Wtime();
    
    //loop through and calculate the mandelbrot set
    for( index = 0; index < pixHeight; index++ )
       {
        for( innerIndex = 0; innerIndex < pixWidth; innerIndex++ )
           {
            c = calculateC( scale_real, scale_imag, index, innerIndex );
            pixOut = cal_pixel( c );
            pixels[index][innerIndex] = pixOut;
           }
       }

    end = MPI_Wtime();
    
    end = end - start;
      
    cout << "Time taken to calculate pixels: " << end << endl;
    //output image
    pim_write_black_and_white( "out/seq.ppm", pixWidth, pixHeight, (const unsigned char**) pixels);

    //finialize MPI
    MPI_Finalize();
   }

//creates the output file
void outputFile()
   {
    ofstream fout;
    fout.open( "out/seq.ppm" );
    fout.close();

   }
//takes command line arguments and provides the program with information on the dimensions of the
//image and returns those dimensions
void calculateSize( int size, char* input[], int &width, int &height)
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
            temp[strSize - 1 ] = '\0';
            height = atoi(temp);
          }
        if( temp[0] == 'W' || temp[0] == 'w' )
          {
           for( innerIndex = 0; innerIndex < strSize; innerIndex++ )
              {
               temp[innerIndex] = temp[innerIndex + 1];
               
              }
            temp[strSize - 1 ] = '\0';
            width = atoi(temp);
          }
       }

   }
//taken from textbook
//calculates the pixel based on the complex number c
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




















