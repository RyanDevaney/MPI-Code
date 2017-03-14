#include <cstdio>
#include <cstring>

#ifdef _WIN32
  #define WRITE_FLAGS "wb"
#else
  #define WRITE_FLAGS "w"
#endif

bool pim_write_black_and_white(const char * const fileName,
                               const int width,
                               const int height,
                               const unsigned char * pixels)
{
  FILE * fp = fopen(fileName, WRITE_FLAGS);

  if (!fp) return false;
  fprintf(fp, "P5\n%i %i 255\n", width, height);
  fwrite(pixels, width * height, 1, fp);
  fclose(fp);

  return true;
}
bool pim_write_black_and_white(const char * const fileName,
                               const int width,
                               const int height,
                               const unsigned char ** pixels)
{
  int i;
  bool ret;
  unsigned char * t = new unsigned char[width * height];

  for (i = 0; i < height; ++i) memcpy(t + width * i, pixels[i], width);
  ret = pim_write_black_and_white(fileName, width, height, t);
  delete [] t;
  return ret;
}
bool pim_write_color(const char * const fileName,
                     const int width,
                     const int height,
                     const unsigned char * pixels)
{
  FILE * fp = fopen(fileName, WRITE_FLAGS);

  if (!fp) return false;
  fprintf(fp, "P6\n%i %i 255\n", width, height);
  fwrite(pixels, width * height * 3, 1, fp);
  fclose(fp);

  return true;
}
bool pim_write_color(const char * const fileName,
                     const int width,
                     const int height,
                     const unsigned char * const * pixels)
{
  int i;
  bool ret;
  unsigned char * t = new unsigned char[width * height * 3];

  for (i = 0; i < height; ++i) memcpy(t + width * i * 3, pixels[i], width * 3);
  ret = pim_write_color(fileName, width, height, t);
  delete [] t;
  return ret;
}
bool pim_write_color(const char * const fileName,
                     const int width,
                     const int height,
                     const unsigned char * red,
                     const unsigned char * green,
                     const unsigned char * blue)
{
  int i, j, ind;
  bool ret;
  unsigned char * p, * t = new unsigned char[width * height * 3];

  p = t;
  for (i = 0; i < height; ++i)
  {
    for (j = 0; j < width; ++j)
    {
      *(p++) = *(red++);
      *(p++) = *(green++);
      *(p++) = *(blue++);
    }
  }
  ret = pim_write_color(fileName, width, height, t);
  delete [] t;
  return ret;
}
bool pim_write_color(const char * const fileName,
                     const int width,
                     const int height,
                     const unsigned char ** red,
                     const unsigned char ** green,
                     const unsigned char ** blue)
{
  int i, j, ind;
  bool ret;
  const unsigned char * r, * g, * b;
  unsigned char * p, * t = new unsigned char[width * height * 3];

  p = t;
  for (i = 0; i < height; ++i)
  {
    r = red[i];
    g = green[i];
    b = blue[i];
    for (j = 0; j < width; ++j)
    {
      *(p++) = *(r++);
      *(p++) = *(g++);
      *(p++) = *(b++);
    }
  }
  ret = pim_write_color(fileName, width, height, t);
  delete [] t;
  return ret;
}
