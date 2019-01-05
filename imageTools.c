#include "imageTools.h"
#include "image.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

Image *horizontalResize(Image *image, int newSize) {
  float ratio = (float)image->vert / image->hor;
  float ratio2 = (float)image->hor / newSize;
  //printf("ratios: %f %f\n", ratio, ratio2);
  int newVertSize = (int) (newSize * ratio);

  Image *newImage = malloc(sizeof(Image));
  newImage->vert = newVertSize;
  newImage->hor = newSize;
  newImage->Pixels = malloc(sizeof(Pixel*) * newVertSize);
  for(int i = 0; i < newVertSize; i++) {
    newImage->Pixels[i] = malloc(sizeof(Pixel) * newSize);
  }

  for(int i = 0; i < newVertSize; i++) {
    int oldI = (int) (i * ratio2);
    //printf("%d -i- %d\n", i, oldI);
    for(int j = 0; j < newSize; j++) {
      int oldJ = (int) (j * ratio2);
      //printf("%d -j- %d\n", j, oldJ);
      newImage->Pixels[i][j].Blue = image->Pixels[oldI][oldJ].Blue;
      newImage->Pixels[i][j].Green = image->Pixels[oldI][oldJ].Green;
      newImage->Pixels[i][j].Red = image->Pixels[oldI][oldJ].Red;
    }
  }

  return newImage;
}

Image *verticalResize(Image *image, int newSize) {
  float ratio = (float)image->hor / image->vert;
  float ratio2 = (float)image->vert / newSize;
  //printf("ratios: %f %f\n", ratio, ratio2);
  int newHorSize = (int) (newSize * ratio);

  Image *newImage = malloc(sizeof(Image));
  newImage->vert = newSize;
  newImage->hor = newHorSize;
  newImage->Pixels = malloc(sizeof(Pixel*) * newSize);
  for(int i = 0; i < newSize; i++) {
    newImage->Pixels[i] = malloc(sizeof(Pixel) * newHorSize);
  }

  for(int i = 0; i < newSize; i++) {
    int oldI = (int) (i * ratio2);
    //printf("%d -i- %d\n", i, oldI);
    for(int j = 0; j < newHorSize; j++) {
      int oldJ = (int) (j * ratio2);
      //printf("%d -j- %d\n", j, oldJ);
      newImage->Pixels[i][j].Blue = image->Pixels[oldI][oldJ].Blue;
      newImage->Pixels[i][j].Green = image->Pixels[oldI][oldJ].Green;
      newImage->Pixels[i][j].Red = image->Pixels[oldI][oldJ].Red;
    }
  }

  return newImage;
}

int findDistance(Pixel *pixel1, Pixel *pixel2) {
  double red = sqrt(abs(pow(pixel1->Red, 2) - pow(pixel2->Red, 2)));
  double green = sqrt(abs(pow(pixel1->Green, 2) - pow(pixel2->Green, 2)));
  double blue = sqrt(abs(pow(pixel1->Blue, 2) - pow(pixel2->Blue, 2)));
  return (int)(red + green + blue);
}

// int main() {
//   Image *newImage = readImage("mountain.bmp");
//   Image *resizedImage = horizontalResize(newImage, 256);
//   Image *resizedImage2 = verticalResize(newImage, 256);
//   writeImage(newImage, "result.bmp");
//   writeImage(resizedImage, "result2.bmp");
//   writeImage(resizedImage2, "result3.bmp");
// }
