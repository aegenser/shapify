#include "imageTools.h"
#include "image.h"
#include "rectangle.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

double recDegToRad(int degree) {
  return degree / 180.0 * M_PI;
}

int recIsInside(int x, int y, Rectangle *rectangle) {
  int translatedX = x - rectangle->centerX;
  int translatedY = y - rectangle->centerY;
  double rotatedX = translatedX * cos(-1.0 * recDegToRad(rectangle->angle)) \
                    - translatedY * sin(-1.0 * recDegToRad(rectangle->angle));
  double rotatedY = translatedY * cos(-1.0 * recDegToRad(rectangle->angle)) \
                    + translatedX * sin(-1.0 * recDegToRad(rectangle->angle));
  if(abs(rotatedX) <= rectangle->width/2.0 && abs(rotatedY) <= rectangle->height/2.0) {
    return 1;
  }
  return 0;
}

void recSetMinMax(Rectangle *rectangle) {
  int verticesX[4];
  int verticesY[4];
  verticesX[0] = (int)(rectangle->width / 2.0 * cos(recDegToRad(rectangle->angle)) \
                    - rectangle->height / 2.0 * sin(recDegToRad(rectangle->angle)));
  verticesY[0] = (int)(rectangle->height / 2.0 * cos(recDegToRad(rectangle->angle)) \
                    + rectangle->width / 2.0 * sin(recDegToRad(rectangle->angle)));
  verticesX[1] = (int)(-1 * rectangle->width / 2.0 * cos(recDegToRad(rectangle->angle)) \
                    - rectangle->height / 2.0 * sin(recDegToRad(rectangle->angle)));
  verticesY[1] = (int)(rectangle->height / 2.0 * cos(recDegToRad(rectangle->angle)) \
                    - rectangle->width / 2.0 * sin(recDegToRad(rectangle->angle)));
  verticesX[2] = (int)(rectangle->width / 2.0 * cos(recDegToRad(rectangle->angle)) \
                    + rectangle->height / 2.0 * sin(recDegToRad(rectangle->angle)));
  verticesY[2] = (int)(-1 * rectangle->height / 2.0 * cos(recDegToRad(rectangle->angle)) \
                    + rectangle->width / 2.0 * sin(recDegToRad(rectangle->angle)));
  verticesX[3] = (int)(-1 * rectangle->width / 2.0 * cos(recDegToRad(rectangle->angle)) \
                    + rectangle->height / 2.0 * sin(recDegToRad(rectangle->angle)));
  verticesY[3] = (int)(-1 * rectangle->height / 2.0 * cos(recDegToRad(rectangle->angle)) \
                    - rectangle->width / 2.0 * sin(recDegToRad(rectangle->angle)));

  rectangle->minX = verticesX[0] + rectangle->centerX;
  rectangle->maxX = verticesX[0] + rectangle->centerX;
  rectangle->minY = verticesY[0] + rectangle->centerY;
  rectangle->maxY = verticesY[0] + rectangle->centerY;

  for(int i = 1; i < 4; i++) {
    if(verticesX[i] + rectangle->centerX < rectangle->minX) {
      rectangle->minX = verticesX[i] + rectangle->centerX;
    }
    if(verticesX[i] + rectangle->centerX > rectangle->maxX) {
      rectangle->maxX = verticesX[i] + rectangle->centerX;
    }
    if(verticesY[i] + rectangle->centerY < rectangle->minY) {
      rectangle->minY = verticesY[i] + rectangle->centerY;
    }
    if(verticesY[i] + rectangle->centerY > rectangle->maxY) {
      rectangle->maxY = verticesY[i] + rectangle->centerY;
    }
  }
  rectangle->minX -= 5;
  rectangle->minY -= 5;
  rectangle->maxX += 5;
  rectangle->maxY += 5;
}

float recRandomV() {
  return 1.0 * rand() / RAND_MAX;
}

void addRectangle(Image *canvas, Image *original, Error *error, int iter, double opacity) {
  Rectangle rectangle;
  //triangle.x1 = (int) (recRandomV() * (canvas->hor));
  rectangle.centerX = (int) (recRandomV() * (canvas->hor));
  rectangle.centerY = (int) (recRandomV() * (canvas->vert));
  rectangle.width = (int) (recRandomV() * 300);
  rectangle.height = (int) (recRandomV() * 300);
  rectangle.angle = (int) (recRandomV() * 180);

  long unsigned int totalRed = 0ul;
  long unsigned int totalGreen = 0ul;
  long unsigned int totalBlue = 0ul;
  long unsigned int total = 0ul;

  recSetMinMax(&rectangle);

  for(int i = MAX(0, rectangle.minY); i < MIN(canvas->vert, rectangle.maxY); i++) {
    for(int j = MAX(0, rectangle.minX); j < MIN(canvas->hor, rectangle.maxX); j++) {
      if(recIsInside(j, i, &rectangle)) {
        total++;
        totalRed += original->Pixels[i/2][j/2].Red;
        totalGreen += original->Pixels[i/2][j/2].Green;
        totalBlue += original->Pixels[i/2][j/2].Blue;
      }
    }
  }

  //printf("%lu\n", total);


  unsigned char avgRed = 0;
  unsigned char avgGreen = 0;
  unsigned char avgBlue = 0;
  if(total != 0) {
    avgRed = totalRed / total;
    avgGreen = totalGreen / total;
    avgBlue = totalBlue / total;
  }

  //printf("%d\n", avgRed);


  Pixel avgPixel;
  avgPixel.Red = avgRed;
  avgPixel.Green = avgGreen;
  avgPixel.Blue = avgBlue;
  long unsigned int currentError = error->totalError;

  //printf("original - %lu\n", currentError);

  for(int i = MAX(0, rectangle.minY); i < MIN(canvas->vert, rectangle.maxY); i++) {
    for(int j = MAX(0, rectangle.minX); j < MIN(canvas->hor, rectangle.maxX); j++) {
      if(recIsInside(j, i, &rectangle)) {
        Pixel specificPixel;
        specificPixel.Red =  (int)(canvas->Pixels[i][j].Red * (1-opacity) + \
                                   avgPixel.Red * opacity);
        specificPixel.Green =  (int)(canvas->Pixels[i][j].Green * (1-opacity) + \
                                   avgPixel.Green * opacity);
        specificPixel.Blue =  (int)(canvas->Pixels[i][j].Blue * (1-opacity)  + \
                                   avgPixel.Blue * opacity);
        //printf("%d + %d + %d\n", findDistance(&canvas->Pixels[i][j]), &original->Pixels[i/2][j/2]), findDistance(&avgPixel, &original->Pixels[i/2][j/2]), error->pixelError[i][j]);
        currentError = currentError - error->pixelError[i][j] + \
                       findDistance(&specificPixel, &original->Pixels[i/2][j/2]);
      }
    }
  }

 //printf("%d\n", currentError);

  Rectangle newRectangle;
  long unsigned int newError;

  for(int k = 0; k < iter; k++) {
    //printf("current - %lu\n", currentError);

    int valid = 0;
    while(!valid) {
      newRectangle = rectangle;
      valid = 0;
      char whichMutation = (char)(14 * recRandomV());
      char howExtreme = (char)(recRandomV() * 3);
      int extreme = 0;
      if (howExtreme == 0) {
        extreme = 20;
      } else if (howExtreme == 1) {
        extreme = 50;
      } else if (howExtreme == 2) {
        extreme = 200;
      }
      int halfExtreme = extreme / 2;
      switch(whichMutation) {
        case 0:
          newRectangle.centerX = (int) (recRandomV() * extreme - halfExtreme + newRectangle.centerX);
          break;
        case 1:
          newRectangle.centerY = (int) (recRandomV() * extreme - halfExtreme + newRectangle.centerY);
          break;
        case 2:
          newRectangle.angle = (int) (recRandomV() * extreme - halfExtreme + newRectangle.angle) % 180;
          break;
        case 3:
          newRectangle.centerX = (int) (recRandomV() * extreme - halfExtreme + newRectangle.centerX);
          newRectangle.centerY = (int) (recRandomV() * extreme - halfExtreme + newRectangle.centerY);
          break;
        case 4:
          newRectangle.centerX = (int) (recRandomV() * (canvas->hor));
          newRectangle.centerY = (int) (recRandomV() * (canvas->vert));
          newRectangle.width = (int) (recRandomV() * 300);
          newRectangle.height = (int) (recRandomV() * 300);
          newRectangle.angle = (int) (recRandomV() * 180);
        case 5:
          newRectangle.width = (int) abs(recRandomV() * newRectangle.width);
          break;
        case 6:
          newRectangle.height = (int) abs(recRandomV() * newRectangle.height);
          break;
        case 7:
          newRectangle.width = (int) (recRandomV() * 300);
          newRectangle.height = (int) (recRandomV() * 300);
          break;
        case 8:
          newRectangle.angle = (int) (recRandomV() * 180);
          break;
        case 9:
          newRectangle.centerX = (int) (recRandomV() * (canvas->hor));
          newRectangle.centerY = (int) (recRandomV() * (canvas->vert));
          break;
        case 10:
          newRectangle.width = (int) abs((recRandomV() + 1) * newRectangle.width);
          break;
        case 11:
          newRectangle.height = (int) abs((recRandomV() + 1) * newRectangle.height);
          break;
        case 12:
          newRectangle.width = (int) abs((recRandomV() + 1) * newRectangle.width);
          newRectangle.height = (int) abs((recRandomV() + 1) * newRectangle.height);
          break;
        case 13:
          newRectangle.width = (int) abs(recRandomV() * newRectangle.width);
          newRectangle.height = (int) abs(recRandomV() * newRectangle.height);
          break;
        default:
          printf("Something went wrong\n");
      }
      if(newRectangle.centerX < -100 || newRectangle.centerX > (canvas->hor + 100) || \
         newRectangle.centerY < -100 || newRectangle.centerY > (canvas->vert + 100) || \
         newRectangle.height <= 0 || newRectangle.width <= 0) {
        valid = 0;
      } else {
        valid = 1;
      }
    }

    recSetMinMax(&newRectangle);

    totalRed = 0ul;
    totalGreen = 0ul;
    totalBlue = 0ul;
    total = 0ul;

    for(int i = MAX(0, newRectangle.minY); i < MIN(canvas->vert, newRectangle.maxY); i++) {
      for(int j = MAX(0, newRectangle.minX); j < MIN(canvas->hor, newRectangle.maxX); j++) {
        if(recIsInside(j, i, &newRectangle)) {
          total++;
          totalRed += original->Pixels[i/2][j/2].Red;
          totalGreen += original->Pixels[i/2][j/2].Green;
          totalBlue += original->Pixels[i/2][j/2].Blue;
        }
      }
    }

    newError = error->totalError;
    unsigned char newAvgRed;
    unsigned char newAvgGreen;
    unsigned char newAvgBlue;
    Pixel newAvgPixel;

    if(total != 0) {
      unsigned char newAvgRed = totalRed / total;
      unsigned char newAvgGreen = totalGreen / total;
      unsigned char newAvgBlue = totalBlue / total;

      newAvgPixel.Red = newAvgRed;
      newAvgPixel.Green = newAvgGreen;
      newAvgPixel.Blue = newAvgBlue;

      for(int i = MAX(0, newRectangle.minY); i < MIN(canvas->vert, newRectangle.maxY); i++) {
        for(int j = MAX(0, newRectangle.minX); j < MIN(canvas->hor, newRectangle.maxX); j++) {
          if(recIsInside(j, i, &newRectangle)) {
            Pixel specificPixel;
            specificPixel.Red =  (int)(canvas->Pixels[i][j].Red * (1-opacity) + \
                                       newAvgPixel.Red * opacity);
            specificPixel.Green =  (int)(canvas->Pixels[i][j].Green * (1-opacity) + \
                                       newAvgPixel.Green * opacity);
            specificPixel.Blue =  (int)(canvas->Pixels[i][j].Blue * (1-opacity) + \
                                       newAvgPixel.Blue * opacity);
            newError = newError - error->pixelError[i][j] + \
                           findDistance(&specificPixel, &original->Pixels[i/2][j/2]);
          }
        }
      }
    } else {
      newAvgPixel.Red = 0;
      newAvgPixel.Green = 0;
      newAvgPixel.Blue = 0;
    }

    if(currentError > newError) {
      currentError = newError;
      rectangle = newRectangle;
      avgPixel = newAvgPixel;
    }
  }

  for(int i = MAX(0, rectangle.minY); i < MIN(canvas->vert, rectangle.maxY); i++) {
    for(int j = MAX(0, rectangle.minX); j < MIN(canvas->hor, rectangle.maxX); j++) {
      if(recIsInside(j, i, &rectangle)) {
        Pixel specificPixel;
        specificPixel.Red =  (int)(canvas->Pixels[i][j].Red * (1-opacity) + \
                                   avgPixel.Red * opacity);
        specificPixel.Green =  (int)(canvas->Pixels[i][j].Green * (1-opacity) + \
                                   avgPixel.Green * opacity);
        specificPixel.Blue =  (int)(canvas->Pixels[i][j].Blue * (1-opacity) + \
                                   avgPixel.Blue * opacity);
        int distance = findDistance(&specificPixel, &original->Pixels[i/2][j/2]);
        error->totalError = error->totalError - error->pixelError[i][j] + distance;
        error->pixelError[i][j] = distance;
        canvas->Pixels[i][j] = specificPixel;
      }
    }
  }
}

// int main() {
//   //printf("blow up: %d", 1 / 0);
//   fflush(stdout);
//   srand(time(0));
//   Image *original = readImage("trees.bmp");
//   Image *resizedOriginal;
//   if(original->vert > original->hor) {
//     resizedOriginal = verticalResize(original, 540);
//   } else {
//     resizedOriginal = horizontalResize(original, 540);
//   }
//   freeImage(original);
//   long unsigned int totalRed = 0ul;
//   long unsigned int totalGreen = 0ul;
//   long unsigned int totalBlue = 0ul;
//   long unsigned int total = resizedOriginal->vert * resizedOriginal->hor;
//
//   for(int i = 0; i < resizedOriginal->vert; i++) {
//     for(int j = 0; j < resizedOriginal->hor; j++) {
//       totalRed += resizedOriginal->Pixels[i][j].Red;
//       totalGreen += resizedOriginal->Pixels[i][j].Green;
//       totalBlue += resizedOriginal->Pixels[i][j].Blue;
//     }
//   }
//
//   unsigned char avgRed = totalRed / total;
//   unsigned char avgGreen = totalGreen / total;
//   unsigned char avgBlue = totalBlue / total;
//
//   Image *canvas = malloc(sizeof(Image));
//   Error *error = malloc(sizeof(Error));
//   error->totalError = 0;
//   canvas->vert = resizedOriginal->vert * 2;
//   canvas->hor = resizedOriginal->hor * 2;
//   error->vert = canvas->vert;
//   error->hor = canvas->hor;
//   canvas->Pixels = malloc(sizeof(Pixel*) * canvas->vert);
//   error->pixelError = malloc(sizeof(unsigned int*) * error->vert);
//   for(int i = 0; i < canvas->vert; i++) {
//     canvas->Pixels[i] = malloc(sizeof(Pixel) * canvas->hor);
//     error->pixelError[i] = malloc(sizeof(unsigned int) * error->hor);
//   }
//
//
//
//   for(int i = 0; i < canvas->vert; i++) {
//     for(int j = 0; j < canvas->hor; j++) {
//       canvas->Pixels[i][j].Red = avgRed;
//       canvas->Pixels[i][j].Green = avgGreen;
//       canvas->Pixels[i][j].Blue = avgBlue;
//       int distance = findDistance(&(canvas->Pixels[i][j]), \
//                                   &(resizedOriginal->Pixels[i/2][j/2]));
//       error->pixelError[i][j] = distance;
//       error->totalError += distance;
//     }
//   }
//
//   //printf("%ld\n", error->totalError);
//   for(int i = 0; i < 340; i++) {
//     printf("%d\n", i);
//     char filename[14];
//     if(i<10) {
//       sprintf(filename, "result00%d.bmp", i);
//     } else if (i < 100) {
//       sprintf(filename, "result0%d.bmp", i);
//     } else {
//       sprintf(filename, "result%d.bmp", i);
//     }
//     writeImage(canvas, filename);
//     addRectangle(canvas, resizedOriginal, error, 1000, .7);
//   }
//   writeImage(canvas, "result340.bmp");
//   //writeImage(resizedOriginal, "result2.bmp");
//
// }
