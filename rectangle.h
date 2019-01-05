#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "image.h"
#include "imageTools.h"

typedef struct Rectangle_s {
  int centerX;
  int centerY;
  int height;
  int width;
  int angle; // in degrees
  int minX;
  int maxX;
  int minY;
  int maxY;
} Rectangle;

void addRectangle(Image *canvas, Image *original, Error *error, int iter, double opacity);

#endif
