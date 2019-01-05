#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "image.h"
#include "imageTools.h"

typedef struct Triangle_s {
  int x1;
  int x2;
  int x3;
  int y1;
  int y2;
  int y3;
  int minX;
  int maxX;
  int minY;
  int maxY;
} Triangle;

void addTriangle(Image *canvas, Image *original, Error *error, int iter, double opacity);

#endif
