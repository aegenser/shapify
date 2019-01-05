#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "image.h"
#include "imageTools.h"

typedef struct Ellipse_s {
  int centerX;
  int centerY;
  int height;
  int width;
  int angle; // in degrees
  int minX;
  int maxX;
  int minY;
  int maxY;
} Ellipse;

void addEllipse(Image *canvas, Image *original, Error *error, int iter, double opacity);

#endif
