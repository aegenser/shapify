#include "imageTools.h"
#include "image.h"
#include "ellipse.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

double ellDegToRad(int degree) {
  return degree / 180.0 * M_PI;
}

int ellIsInside(int x, int y, Ellipse *ellipse) {
  int translatedX = x - ellipse->centerX;
  int translatedY = y - ellipse->centerY;
  double rotatedX = translatedX * cos(-1.0 * ellDegToRad(ellipse->angle)) \
                    - translatedY * sin(-1.0 * ellDegToRad(ellipse->angle));
  double rotatedY = translatedY * cos(-1.0 * ellDegToRad(ellipse->angle)) \
                    + translatedX * sin(-1.0 * ellDegToRad(ellipse->angle));
  if(pow(rotatedX, 2)/pow(ellipse->width/2.0, 2) +  \
     pow(rotatedY, 2)/pow(ellipse->height/2.0, 2) <= 1) {
    return 1;
  }
  return 0;
}

void ellSetMinMax(Ellipse *ellipse) {
  int verticesX[4];
  int verticesY[4];
  verticesX[0] = (int)(ellipse->width / 2.0 * cos(ellDegToRad(ellipse->angle)) \
                    - ellipse->height / 2.0 * sin(ellDegToRad(ellipse->angle)));
  verticesY[0] = (int)(ellipse->height / 2.0 * cos(ellDegToRad(ellipse->angle)) \
                    + ellipse->width / 2.0 * sin(ellDegToRad(ellipse->angle)));
  verticesX[1] = (int)(-1 * ellipse->width / 2.0 * cos(ellDegToRad(ellipse->angle)) \
                    - ellipse->height / 2.0 * sin(ellDegToRad(ellipse->angle)));
  verticesY[1] = (int)(ellipse->height / 2.0 * cos(ellDegToRad(ellipse->angle)) \
                    - ellipse->width / 2.0 * sin(ellDegToRad(ellipse->angle)));
  verticesX[2] = (int)(ellipse->width / 2.0 * cos(ellDegToRad(ellipse->angle)) \
                    + ellipse->height / 2.0 * sin(ellDegToRad(ellipse->angle)));
  verticesY[2] = (int)(-1 * ellipse->height / 2.0 * cos(ellDegToRad(ellipse->angle)) \
                    + ellipse->width / 2.0 * sin(ellDegToRad(ellipse->angle)));
  verticesX[3] = (int)(-1 * ellipse->width / 2.0 * cos(ellDegToRad(ellipse->angle)) \
                    + ellipse->height / 2.0 * sin(ellDegToRad(ellipse->angle)));
  verticesY[3] = (int)(-1 * ellipse->height / 2.0 * cos(ellDegToRad(ellipse->angle)) \
                    - ellipse->width / 2.0 * sin(ellDegToRad(ellipse->angle)));

  ellipse->minX = verticesX[0] + ellipse->centerX;
  ellipse->maxX = verticesX[0] + ellipse->centerX;
  ellipse->minY = verticesY[0] + ellipse->centerY;
  ellipse->maxY = verticesY[0] + ellipse->centerY;

  for(int i = 1; i < 4; i++) {
    if(verticesX[i] + ellipse->centerX < ellipse->minX) {
      ellipse->minX = verticesX[i] + ellipse->centerX;
    }
    if(verticesX[i] + ellipse->centerX > ellipse->maxX) {
      ellipse->maxX = verticesX[i] + ellipse->centerX;
    }
    if(verticesY[i] + ellipse->centerY < ellipse->minY) {
      ellipse->minY = verticesY[i] + ellipse->centerY;
    }
    if(verticesY[i] + ellipse->centerY > ellipse->maxY) {
      ellipse->maxY = verticesY[i] + ellipse->centerY;
    }
  }
  ellipse->minX -= 5;
  ellipse->minY -= 5;
  ellipse->maxX += 5;
  ellipse->maxY += 5;
}

float ellRandomV() {
  return 1.0 * rand() / RAND_MAX;
}

void addEllipse(Image *canvas, Image *original, Error *error, int iter, double opacity) {
  Ellipse ellipse;
  //triangle.x1 = (int) (ellRandomV() * (canvas->hor));
  ellipse.centerX = (int) (ellRandomV() * (canvas->hor));
  ellipse.centerY = (int) (ellRandomV() * (canvas->vert));
  ellipse.width = (int) (ellRandomV() * 20);
  ellipse.height = (int) (ellRandomV() * 20);
  ellipse.angle = (int) (ellRandomV() * 180);

  long unsigned int totalRed = 0ul;
  long unsigned int totalGreen = 0ul;
  long unsigned int totalBlue = 0ul;
  long unsigned int total = 0ul;

  ellSetMinMax(&ellipse);

  for(int i = MAX(0, ellipse.minY); i < MIN(canvas->vert, ellipse.maxY); i++) {
    for(int j = MAX(0, ellipse.minX); j < MIN(canvas->hor, ellipse.maxX); j++) {
      if(ellIsInside(j, i, &ellipse)) {
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

  for(int i = MAX(0, ellipse.minY); i < MIN(canvas->vert, ellipse.maxY); i++) {
    for(int j = MAX(0, ellipse.minX); j < MIN(canvas->hor, ellipse.maxX); j++) {
      if(ellIsInside(j, i, &ellipse)) {
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

  Ellipse newEllipse;
  long unsigned int newError;

  for(int k = 0; k < iter; k++) {
    //printf("current - %lu\n", currentError);

    int valid = 0;
    while(!valid) {
      newEllipse = ellipse;
      valid = 0;
      char whichMutation = (char)(14 * ellRandomV());
      char howExtreme = (char)(ellRandomV() * 3);
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
          newEllipse.centerX = (int) (ellRandomV() * extreme - halfExtreme + newEllipse.centerX);
          break;
        case 1:
          newEllipse.centerY = (int) (ellRandomV() * extreme - halfExtreme + newEllipse.centerY);
          break;
        case 2:
          newEllipse.angle = (int) (ellRandomV() * extreme - halfExtreme + newEllipse.angle) % 180;
          break;
        case 3:
          newEllipse.centerX = (int) (ellRandomV() * extreme - halfExtreme + newEllipse.centerX);
          newEllipse.centerY = (int) (ellRandomV() * extreme - halfExtreme + newEllipse.centerY);
          break;
        case 4:
          newEllipse.centerX = (int) (ellRandomV() * (canvas->hor));
          newEllipse.centerY = (int) (ellRandomV() * (canvas->vert));
          newEllipse.width = (int) (ellRandomV() * 300);
          newEllipse.height = (int) (ellRandomV() * 300);
          newEllipse.angle = (int) (ellRandomV() * 180);
        case 5:
          newEllipse.width = (int) abs(ellRandomV() * newEllipse.width);
          break;
        case 6:
          newEllipse.height = (int) abs(ellRandomV() * newEllipse.height);
          break;
        case 7:
          newEllipse.width = (int) (ellRandomV() * 300);
          newEllipse.height = (int) (ellRandomV() * 300);
          break;
        case 8:
          newEllipse.angle = (int) (ellRandomV() * 180);
          break;
        case 9:
          newEllipse.centerX = (int) (ellRandomV() * (canvas->hor));
          newEllipse.centerY = (int) (ellRandomV() * (canvas->vert));
          break;
        case 10:
          newEllipse.width = (int) abs((ellRandomV() + 1) * newEllipse.width);
          break;
        case 11:
          newEllipse.height = (int) abs((ellRandomV() + 1) * newEllipse.height);
          break;
        case 12:
          newEllipse.width = (int) abs((ellRandomV() + 1) * newEllipse.width);
          newEllipse.height = (int) abs((ellRandomV() + 1) * newEllipse.height);
          break;
        case 13:
          newEllipse.width = (int) abs(ellRandomV() * newEllipse.width);
          newEllipse.height = (int) abs(ellRandomV() * newEllipse.height);
          break;
        default:
          printf("Something went wrong\n");
      }
      if(newEllipse.centerX < -100 || newEllipse.centerX > (canvas->hor + 100) || \
         newEllipse.centerY < -100 || newEllipse.centerY > (canvas->vert + 100) || \
         newEllipse.height <= 0 || newEllipse.width <= 0) {
        valid = 0;
      } else {
        valid = 1;
      }
    }

    ellSetMinMax(&newEllipse);

    totalRed = 0ul;
    totalGreen = 0ul;
    totalBlue = 0ul;
    total = 0ul;

    for(int i = MAX(0, newEllipse.minY); i < MIN(canvas->vert, newEllipse.maxY); i++) {
      for(int j = MAX(0, newEllipse.minX); j < MIN(canvas->hor, newEllipse.maxX); j++) {
        if(ellIsInside(j, i, &newEllipse)) {
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

      for(int i = MAX(0, newEllipse.minY); i < MIN(canvas->vert, newEllipse.maxY); i++) {
        for(int j = MAX(0, newEllipse.minX); j < MIN(canvas->hor, newEllipse.maxX); j++) {
          if(ellIsInside(j, i, &newEllipse)) {
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
      ellipse = newEllipse;
      avgPixel = newAvgPixel;
    }
  }

  for(int i = MAX(0, ellipse.minY); i < MIN(canvas->vert, ellipse.maxY); i++) {
    for(int j = MAX(0, ellipse.minX); j < MIN(canvas->hor, ellipse.maxX); j++) {
      if(ellIsInside(j, i, &ellipse)) {
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
//   Image *original = readImage("boat.bmp");
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
//   for(int i = 0; i < 400; i++) {
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
//     addEllipse(canvas, resizedOriginal, error, 1000, .8);
//   }
//   writeImage(canvas, "result400.bmp");
//   //writeImage(resizedOriginal, "result2.bmp");
//
// }
