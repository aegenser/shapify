#include "imageTools.h"
#include "image.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

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

typedef struct Error_s {
  int vert;
  int hor;
  long int totalError;
  unsigned int **pixelError;
} Error;

float findArea(int x1, int y1, int x2, int y2, int x3, int y3) {
  return abs( (float)(1.0 * x1 * (y2-y3) + x2 * (y3-y1) + x3 * (y1-y2)) / 2);
}

int isInside(int x, int y, Triangle *triangle) {
  float a1 = findArea(x, y, triangle->x2, triangle->y2, triangle->x3, triangle->y3);
  float a2 = findArea(x, y, triangle->x1, triangle->y1, triangle->x3, triangle->y3);
  float a3 = findArea(x, y, triangle->x2, triangle->y2, triangle->x1, triangle->y1);
  float a = findArea(triangle->x1, triangle->y1, triangle->x2, triangle->y2, \
                     triangle->x3, triangle->y3);

  //printf("%f - %f - %f - %f - %f\n", a1, a2, a3, a, a1 + a2 + a3 - a);
  if(abs(a1 + a2 + a3 - a) < 1.1) {
    return 1;
  }
  return 0;
}

void setMinMax(Triangle *triangle) {
  if(triangle->x1 < triangle->x2 && triangle->x1 < triangle->x3) {
    triangle->minX = triangle->x1;
  } else if(triangle->x2 < triangle->x3 && triangle->x2 < triangle->x1) {
    triangle->minX = triangle->x2;
  } else {
    triangle->minX = triangle->x3;
  }
  if(triangle->y1 < triangle->y2 && triangle->y1 < triangle->y3) {
    triangle->minY = triangle->y1;
  } else if(triangle->y2 < triangle->y3 && triangle->y2 < triangle->y1) {
    triangle->minY = triangle->y2;
  } else {
    triangle->minY = triangle->y3;
  }
  if(triangle->x1 > triangle->x2 && triangle->x1 > triangle->x3) {
    triangle->maxX = triangle->x1;
  } else if(triangle->x2 > triangle->x3 && triangle->x2 > triangle->x1) {
    triangle->maxX = triangle->x2;
  } else {
    triangle->maxX = triangle->x3;
  }
  if(triangle->y1 > triangle->y2 && triangle->y1 > triangle->y3) {
    triangle->maxY = triangle->y1;
  } else if(triangle->y2 > triangle->y3 && triangle->y2 > triangle->y1) {
    triangle->maxY = triangle->y2;
  } else {
    triangle->maxY = triangle->y3;
  }
}

int findDistance(Pixel *pixel1, Pixel *pixel2) {
  double red = sqrt(abs(pow(pixel1->Red, 2) - pow(pixel2->Red, 2)));
  double green = sqrt(abs(pow(pixel1->Green, 2) - pow(pixel2->Green, 2)));
  double blue = sqrt(abs(pow(pixel1->Blue, 2) - pow(pixel2->Blue, 2)));
  return (int)(red + green + blue);
}

float randomV() {
  return 1.0 * rand() / RAND_MAX;
}

void addTriangle(Image *canvas, Image *original, Error *error, int iter, double opacity) {
  Triangle triangle;
  triangle.x1 = (int) (randomV() * (canvas->hor));
  triangle.x2 = (int) (randomV() * (canvas->hor));
  triangle.x3 = (int) (randomV() * (canvas->hor));
  triangle.y1 = (int) (randomV() * (canvas->vert));
  triangle.y2 = (int) (randomV() * (canvas->vert));
  triangle.y3 = (int) (randomV() * (canvas->vert));

  long unsigned int totalRed = 0ul;
  long unsigned int totalGreen = 0ul;
  long unsigned int totalBlue = 0ul;
  long unsigned int total = 0ul;

  setMinMax(&triangle);

  for(int i = MAX(0, triangle.minY); i < MIN(canvas->vert, triangle.maxY); i++) {
    for(int j = MAX(0, triangle.minX); j < MIN(canvas->hor, triangle.maxX); j++) {
      if(isInside(j, i, &triangle)) {
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

  for(int i = MAX(0, triangle.minY); i < MIN(canvas->vert, triangle.maxY); i++) {
    for(int j = MAX(0, triangle.minX); j < MIN(canvas->hor, triangle.maxX); j++) {
      if(isInside(j, i, &triangle)) {
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

  Triangle newTriangle;
  long unsigned int newError;

  for(int k = 0; k < iter; k++) {
    //printf("current - %lu\n", currentError);

    int valid = 0;
    while(!valid) {
      newTriangle.x1 = triangle.x1;
      newTriangle.x2 = triangle.x2;
      newTriangle.x3 = triangle.x3;
      newTriangle.y1 = triangle.y1;
      newTriangle.y2 = triangle.y2;
      newTriangle.y3 = triangle.y3;
      valid = 0;
      char whichMutation = (char)(6 * randomV());
      char whichVertex = (char)(randomV() * 3);
      char howExtreme = (char)(randomV() * 3);
      int extreme = 0;
      if (howExtreme == 0) {
        extreme = 20;
      } else if (howExtreme == 1) {
        extreme = 50;
      } else if (howExtreme == 2) {
        extreme = 200;
      }
      int halfExtreme = extreme / 2;
      int shiftX;
      int shiftY;
      //printf("mutation - %d\n", whichMutation);
      //printf("vertex - %d\n", whichVertex);
      switch(whichMutation) {
        case 0: // move one vertex a little on the x axis
          if (whichVertex == 0) {
            newTriangle.x1 = (int) (randomV() * extreme - halfExtreme + newTriangle.x1);
          } else if (whichVertex == 1) {
            newTriangle.x2 = (int) (randomV() * extreme - halfExtreme + newTriangle.x2);
          } else if (whichVertex == 2) {
            newTriangle.x3 = (int) (randomV() * extreme - halfExtreme + newTriangle.x3);
          }
          break;
        case 1: // move one vertex a little on the y axis
          if (whichVertex == 0) {
            newTriangle.y1 = (int) (randomV() * extreme - halfExtreme + newTriangle.y1);
          } else if (whichVertex == 1) {
            newTriangle.y2 = (int) (randomV() * extreme - halfExtreme + newTriangle.y2);
          } else if (whichVertex == 2) {
            newTriangle.y3 = (int) (randomV() * extreme - halfExtreme + newTriangle.y3);
          }
          break;
        case 2: // move one vertex to a random location
          if (whichVertex == 0) {
            newTriangle.x1 = (int) (randomV() * (canvas->hor));
            newTriangle.y1 = (int) (randomV() * (canvas->vert));
          } else if (whichVertex == 1) {
            newTriangle.x2 = (int) (randomV() * (canvas->hor));
            newTriangle.y2 = (int) (randomV() * (canvas->vert));
          } else if (whichVertex == 2) {
            newTriangle.x3 = (int) (randomV() * (canvas->hor));
            newTriangle.y3 = (int) (randomV() * (canvas->vert));
          }
          break;
        case 3: // move both vertices close to another
          if (whichVertex == 0) {
            newTriangle.x2 = (int) (randomV() * extreme - halfExtreme + newTriangle.x1);
            newTriangle.x3 = (int) (randomV() * extreme - halfExtreme + newTriangle.x1);
            newTriangle.y2 = (int) (randomV() * extreme - halfExtreme + newTriangle.y1);
            newTriangle.y3 = (int) (randomV() * extreme - halfExtreme + newTriangle.y1);
          } else if (whichVertex == 1) {
            newTriangle.x1 = (int) (randomV() * extreme - halfExtreme + newTriangle.x2);
            newTriangle.x3 = (int) (randomV() * extreme - halfExtreme + newTriangle.x2);
            newTriangle.y1 = (int) (randomV() * extreme - halfExtreme + newTriangle.y2);
            newTriangle.y3 = (int) (randomV() * extreme - halfExtreme + newTriangle.y2);
          } else if (whichVertex == 2) {
            newTriangle.x2 = (int) (randomV() * extreme - halfExtreme + newTriangle.x3);
            newTriangle.x1 = (int) (randomV() * extreme - halfExtreme + newTriangle.x3);
            newTriangle.y2 = (int) (randomV() * extreme - halfExtreme + newTriangle.y3);
            newTriangle.y1 = (int) (randomV() * extreme - halfExtreme + newTriangle.y3);
          }
          break;
        case 4: // new random triangle
          newTriangle.x1 = (int) (randomV() * (canvas->hor));
          newTriangle.y1 = (int) (randomV() * (canvas->vert));
          newTriangle.x2 = (int) (randomV() * (canvas->hor));
          newTriangle.y2 = (int) (randomV() * (canvas->vert));
          newTriangle.x3 = (int) (randomV() * (canvas->hor));
          newTriangle.y3 = (int) (randomV() * (canvas->vert));
          break;
        case 5: // shift triangle
          shiftX = (randomV() * extreme - halfExtreme);
          shiftY = (randomV() * extreme - halfExtreme);
          newTriangle.x1 = (newTriangle.x1 + shiftX);
          newTriangle.y1 = (newTriangle.y1 + shiftY);
          newTriangle.x2 = (newTriangle.x2 + shiftX);
          newTriangle.y2 = (newTriangle.y2 + shiftY);
          newTriangle.x3 = (newTriangle.x3 + shiftX);
          newTriangle.y3 = (newTriangle.y3 + shiftY);
          break;
        // case 0:
        // case 19:
        //   newTriangle.x1 = (int)(newTriangle.x1 + (randomV() * 100) - 50);
        //   newTriangle.y1 = (int)(newTriangle.y1 + (randomV() * 100) - 50);
        //   break;
        // case 9:
        // case 20:
        //   //printf("%d\n", whichMutation);
        //   //printf("case before 0/9 - %d\n", newTriangle.x1);
        //   newTriangle.x1 = (int)(newTriangle.x1 + (randomV() * 20) - 10);
        //   newTriangle.y1 = (int)(newTriangle.y1 + (randomV() * 20) - 10);
        //   //printf("case after 0/9 - %d\n", newTriangle.x1);
        //   break;
        // case 1:
        // case 21:
        //   newTriangle.x2 = (int)(newTriangle.x2 + (randomV() * 100) - 50);
        //   newTriangle.y2 = (int)(newTriangle.y2 + (randomV() * 100) - 50);
        //   break;
        // case 10:
        // case 22:
        //   //printf("%d\n", whichMutation);
        //   newTriangle.x2 = (int)(newTriangle.x2 + (randomV() * 20) - 10);
        //   newTriangle.y2 = (int)(newTriangle.y2 + (randomV() * 20) - 10);
        //   break;
        // case 2:
        // case 23:
        //   newTriangle.x3 = (int)(newTriangle.x3 + (randomV() * 100) - 50);
        //   newTriangle.y3 = (int)(newTriangle.y3 + (randomV() * 100) - 50);
        //   break;
        // case 11:
        // case 24:
        //   //printf("%d\n", whichMutation);
        //   newTriangle.x3 = (int)(newTriangle.x3 + (randomV() * 20) - 10);
        //   newTriangle.y3 = (int)(newTriangle.y3 + (randomV() * 20) - 10);
        //   break;
        // case 3:
        //   newTriangle.x1 = (int)(newTriangle.x1 + (randomV() * 100) - 50);
        //   newTriangle.y1 = (int)(newTriangle.y1 + (randomV() * 100) - 50);
        //   newTriangle.x2 = (int)(newTriangle.x2 + (randomV() * 100) - 50);
        //   newTriangle.y2 = (int)(newTriangle.y2 + (randomV() * 100) - 50);
        //   newTriangle.x3 = (int)(newTriangle.x3 + (randomV() * 100) - 50);
        //   newTriangle.y3 = (int)(newTriangle.y3 + (randomV() * 100) - 50);
        //   break;
        // case 12:
        //   //printf("%d\n", whichMutation);
        //   newTriangle.x1 = (int)(newTriangle.x1 + (randomV() * 20) - 10);
        //   newTriangle.y1 = (int)(newTriangle.y1 + (randomV() * 20) - 10);
        //   newTriangle.x2 = (int)(newTriangle.x2 + (randomV() * 20) - 10);
        //   newTriangle.y2 = (int)(newTriangle.y2 + (randomV() * 20) - 10);
        //   newTriangle.x3 = (int)(newTriangle.x3 + (randomV() * 20) - 10);
        //   newTriangle.y3 = (int)(newTriangle.y3 + (randomV() * 20) - 10);
        //   break;
        // case 4:
        //   //printf("%d\n", whichMutation);
        //   shiftX = (int)(randomV() * 150 - 75);
        //   shiftY = (int)(randomV() * 150 - 75);
        //   newTriangle.x1 = (newTriangle.x1 + shiftX);
        //   newTriangle.y1 = (newTriangle.y1 + shiftY);
        //   newTriangle.x2 = (newTriangle.x2 + shiftX);
        //   newTriangle.y2 = (newTriangle.y2 + shiftY);
        //   newTriangle.x3 = (newTriangle.x3 + shiftX);
        //   newTriangle.y3 = (newTriangle.y3 + shiftY);
        //   break;
        // case 5:
        //   newTriangle.x1 = (int) (randomV() * (canvas->hor));
        //   newTriangle.y1 = (int) (randomV() * (canvas->vert));
        //   break;
        // case 6:
        //   newTriangle.x2 = (int) (randomV() * (canvas->hor));
        //   newTriangle.y2 = (int) (randomV() * (canvas->vert));
        //   break;
        // case 7:
        //   newTriangle.x3 = (int) (randomV() * (canvas->hor));
        //   newTriangle.y3 = (int) (randomV() * (canvas->vert));
        //   break;
        // case 8:
        //   //printf("case before - %d\n", newTriangle.x1);
        //   newTriangle.x1 = (int) (randomV() * (canvas->hor));
        //   newTriangle.y1 = (int) (randomV() * (canvas->vert));
        //   newTriangle.x2 = (int) (randomV() * (canvas->hor));
        //   newTriangle.y2 = (int) (randomV() * (canvas->vert));
        //   newTriangle.x3 = (int) (randomV() * (canvas->hor));
        //   newTriangle.y3 = (int) (randomV() * (canvas->vert));
        //   //printf("case before - %d\n", newTriangle.x1);
        //   break;
        // case 13:
        //   newTriangle.x2 = (int) (randomV() * 20 - 10 + newTriangle.x1);
        //   newTriangle.x3 = (int) (randomV() * 20 - 10 + newTriangle.x1);
        //   newTriangle.y2 = (int) (randomV() * 20 - 10 + newTriangle.y1);
        //   newTriangle.y3 = (int) (randomV() * 20 - 10 + newTriangle.y1);
        //   break;
        // case 14:
        //   newTriangle.x2 = (int) (randomV() * 50 - 25 + newTriangle.x1);
        //   newTriangle.x3 = (int) (randomV() * 50 - 25 + newTriangle.x1);
        //   newTriangle.y2 = (int) (randomV() * 50 - 25 + newTriangle.y1);
        //   newTriangle.y3 = (int) (randomV() * 50 - 25 + newTriangle.y1);
        //   break;
        // case 15:
        //   newTriangle.x2 = (int) (randomV() * 20 - 10 + newTriangle.x1);
        //   newTriangle.x3 = (int) (randomV() * 20 - 10 + newTriangle.x1);
        //   newTriangle.y2 = (int) (randomV() * 20 - 10 + newTriangle.y1);
        //   newTriangle.y3 = (int) (randomV() * 20 - 10 + newTriangle.y1);
        //   break;
        // case 16:
        //   newTriangle.x2 = (int) (randomV() * 50 - 25 + newTriangle.x1);
        //   newTriangle.x3 = (int) (randomV() * 50 - 25 + newTriangle.x1);
        //   newTriangle.y2 = (int) (randomV() * 50 - 25 + newTriangle.y1);
        //   newTriangle.y3 = (int) (randomV() * 50 - 25 + newTriangle.y1);
        //   break;
        // case 17:
        //   newTriangle.x2 = (int) (randomV() * 20 - 10 + newTriangle.x1);
        //   newTriangle.x3 = (int) (randomV() * 20 - 10 + newTriangle.x1);
        //   newTriangle.y2 = (int) (randomV() * 20 - 10 + newTriangle.y1);
        //   newTriangle.y3 = (int) (randomV() * 20 - 10 + newTriangle.y1);
        //   break;
        // case 18:
        //   newTriangle.x2 = (int) (randomV() * 50 - 25 + newTriangle.x1);
        //   newTriangle.x3 = (int) (randomV() * 50 - 25 + newTriangle.x1);
        //   newTriangle.y2 = (int) (randomV() * 50 - 25 + newTriangle.y1);
        //   newTriangle.y3 = (int) (randomV() * 50 - 25 + newTriangle.y1);
        //     break;
        default:
          printf("Something went wrong\n");
      }
      if(newTriangle.x1 < -100 || newTriangle.x1 > (canvas->hor + 100) || \
         newTriangle.x2 < -100 || newTriangle.x2 > (canvas->hor + 100) || \
         newTriangle.x3 < -100 || newTriangle.x3 > (canvas->hor + 100) || \
         newTriangle.y1 < -100 || newTriangle.y1 > (canvas->vert + 100) || \
         newTriangle.y2 < -100 || newTriangle.y2 > (canvas->vert + 100) || \
         newTriangle.y3 < -100 || newTriangle.y3 > (canvas->vert + 100)) {
        valid = 0;
        //printf("x1 = %d, x2 = %d, x3 = %d, y1 = %d, y2 = %d, y3 = %d\n", newTriangle.x1, newTriangle.x2, newTriangle.x3, newTriangle.y1, newTriangle.y2, newTriangle.y3);
      } else {
        valid = 1;
      }
    }

    setMinMax(&newTriangle);
    //printf("x1 = %d, x2 = %d, x3 = %d, y1 = %d, y2 = %d, y3 = %d\n", newTriangle.x1, newTriangle.x2, newTriangle.x3, newTriangle.y1, newTriangle.y2, newTriangle.y3);

    totalRed = 0ul;
    totalGreen = 0ul;
    totalBlue = 0ul;
    total = 0ul;

    for(int i = MAX(0, newTriangle.minY); i < MIN(canvas->vert, newTriangle.maxY); i++) {
      for(int j = MAX(0, newTriangle.minX); j < MIN(canvas->hor, newTriangle.maxX); j++) {
        if(isInside(j, i, &newTriangle)) {
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

      for(int i = MAX(0, newTriangle.minY); i < MIN(canvas->vert, newTriangle.maxY); i++) {
        for(int j = MAX(0, newTriangle.minX); j < MIN(canvas->hor, newTriangle.maxX); j++) {
          if(isInside(j, i, &newTriangle)) {
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
    //printf("new - %lu\n", newError);

    if(currentError > newError) {
      currentError = newError;
      triangle = newTriangle;
      avgPixel = newAvgPixel;
    }
  }

  for(int i = MAX(0, triangle.minY); i < MIN(canvas->vert, triangle.maxY); i++) {
    for(int j = MAX(0, triangle.minX); j < MIN(canvas->hor, triangle.maxX); j++) {
      if(isInside(j, i, &triangle)) {
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

int main() {
  //printf("blow up: %d", 1 / 0);
  fflush(stdout);
  srand(time(0));
  Image *original = readImage("lbj.bmp");
  Image *resizedOriginal;
  if(original->vert > original->hor) {
    resizedOriginal = verticalResize(original, 540);
  } else {
    resizedOriginal = horizontalResize(original, 540);
  }
  freeImage(original);
  long unsigned int totalRed = 0ul;
  long unsigned int totalGreen = 0ul;
  long unsigned int totalBlue = 0ul;
  long unsigned int total = resizedOriginal->vert * resizedOriginal->hor;

  for(int i = 0; i < resizedOriginal->vert; i++) {
    for(int j = 0; j < resizedOriginal->hor; j++) {
      totalRed += resizedOriginal->Pixels[i][j].Red;
      totalGreen += resizedOriginal->Pixels[i][j].Green;
      totalBlue += resizedOriginal->Pixels[i][j].Blue;
    }
  }

  unsigned char avgRed = totalRed / total;
  unsigned char avgGreen = totalGreen / total;
  unsigned char avgBlue = totalBlue / total;

  Image *canvas = malloc(sizeof(Image));
  Error *error = malloc(sizeof(Error));
  error->totalError = 0;
  canvas->vert = resizedOriginal->vert * 2;
  canvas->hor = resizedOriginal->hor * 2;
  error->vert = canvas->vert;
  error->hor = canvas->hor;
  canvas->Pixels = malloc(sizeof(Pixel*) * canvas->vert);
  error->pixelError = malloc(sizeof(unsigned int*) * error->vert);
  for(int i = 0; i < canvas->vert; i++) {
    canvas->Pixels[i] = malloc(sizeof(Pixel) * canvas->hor);
    error->pixelError[i] = malloc(sizeof(unsigned int) * error->hor);
  }



  for(int i = 0; i < canvas->vert; i++) {
    for(int j = 0; j < canvas->hor; j++) {
      canvas->Pixels[i][j].Red = avgRed;
      canvas->Pixels[i][j].Green = avgGreen;
      canvas->Pixels[i][j].Blue = avgBlue;
      int distance = findDistance(&(canvas->Pixels[i][j]), \
                                  &(resizedOriginal->Pixels[i/2][j/2]));
      error->pixelError[i][j] = distance;
      error->totalError += distance;
    }
  }

  //printf("%ld\n", error->totalError);
  for(int i = 0; i < 400; i++) {
    printf("%d\n", i);
    char filename[14];
    if(i<10) {
      sprintf(filename, "result00%d.bmp", i);
    } else if (i < 100) {
      sprintf(filename, "result0%d.bmp", i);
    } else {
      sprintf(filename, "result%d.bmp", i);
    }
    writeImage(canvas, filename);
    addTriangle(canvas, resizedOriginal, error, 1000, .7);
  }
  writeImage(canvas, "result400.bmp");
  //writeImage(resizedOriginal, "result2.bmp");

}
