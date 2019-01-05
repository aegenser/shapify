#ifndef IMAGETOOLS_H
#define IMAGETOOLS_H

#include "image.h"

typedef struct Error_s {
  int vert;
  int hor;
  long int totalError;
  unsigned int **pixelError;
} Error;

Image *horizontalResize(Image *image, int newSize);

Image *verticalResize(Image *image, int newSize);

int findDistance(Pixel *pixel1, Pixel *pixel2);

#endif
