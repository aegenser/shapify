#include "triangle.h"
#include "ellipse.h"
#include "rectangle.h"
#include "imageTools.h"
#include "image.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main() {

  printf("Welcome to Shapify! [q to quit]\n");
  char input[100];
  char file[100];
  int valid = 0;
  while(!valid) {
    printf("Input image: ");
    fgets(input, 100, stdin);
    int length = strlen(input);
    if(input[0] == 'q' && (input[1] == ' ' || input[1] == '\n')) {
      exit(1);
    } else if(input[length-1] != '\n') {
      printf("Error: input too long\n");
      while(input[length-1] != '\n') {
        fgets(input, 100, stdin);
        length = strlen(input);
      }
    } else if(input[length-5] != '.' || input[length-4] != 'b' || \
              input[length-3] != 'm' || input[length-2] != 'p') {
      printf("Error: input must be of type .bmp\n");
    } else {
      valid = 1;
      strncpy(file, input, length-1);
    }
  }
  char whichShapes;
  valid = 0;
  printf("Choose shape: \n1 - Triangles \n2 - Ellipses \n3 - Rectangles\n");
  printf("[1,2 or 3 or q to quit]\n");
  while(!valid) {
    printf("Input: ");
    fgets(input, 100, stdin);
    int length = strlen(input);
    if(input[0] == 'q' && (input[1] == ' ' || input[1] == '\n')) {
      exit(1);
    } else if(input[length-1] != '\n') {
      printf("Error: must be just the integer 1, 2, or 3\n");
      while(input[length-1] != '\n') {
        fgets(input, 100, stdin);
        length = strlen(input);
      }
    } else if((input[0] != '1' && input[0] != '2' && input[0] != '3') || \
              (length > 2)) {
      printf("Error: must be just the integer 1, 2, or 3\n");
    } else {
      valid = 1;
      if(input[0] == '1') {
        whichShapes = 1;
      } else if(input[0] == '2') {
        whichShapes = 2;
      } else {
        whichShapes = 3;
      }
    }
  }
  int howManyShapes;
  valid = 0;
  printf("Choose how many shapes: \n1 - 100 \n2 - 200 \n3 - 300 \n4 - 400\n");
  printf("[1,2,3 or 4 or q to quit]\n");
  while(!valid) {
    printf("Input: ");
    fgets(input, 100, stdin);
    int length = strlen(input);
    if(input[0] == 'q' && (input[1] == ' ' || input[1] == '\n')) {
      exit(1);
    } else if(input[length-1] != '\n') {
      printf("Error: must be just the integer 1, 2, 3, or 4\n");
      while(input[length-1] != '\n') {
        fgets(input, 100, stdin);
        length = strlen(input);
      }
    } else if((input[0] != '1' && input[0] != '2' && input[0] != '3' && input[0] != '4') || \
              (length > 2)) {
      printf("Error: must be just the integer 1, 2, 3, or 4\n");
    } else {
      valid = 1;
      if(input[0] == '1') {
        howManyShapes = 100;
      } else if(input[0] == '2') {
        howManyShapes = 200;
      } else if(input[0] == '3') {
        howManyShapes = 300;
      } else {
        howManyShapes = 400;
      }
    }
  }
  int howManyMutations;
  valid = 0;
  printf("Choose how many mutations: \n");
  printf("1 - Imprecise (100) \n2 - Fairly Precise (500) \n3 - Precise (1000) \n4 - Very Precise (1500)\n");
  printf("[1,2,3 or 4 or q to quit]\n");
  while(!valid) {
    printf("Input: ");
    fgets(input, 100, stdin);
    int length = strlen(input);
    if(input[0] == 'q' && (input[1] == ' ' || input[1] == '\n')) {
      exit(1);
    } else if(input[length-1] != '\n') {
      printf("Error: must be just the integer 1, 2, 3, or 4\n");
      while(input[length-1] != '\n') {
        fgets(input, 100, stdin);
        length = strlen(input);
      }
    } else if((input[0] != '1' && input[0] != '2' && input[0] != '3' && input[0] != '4') || \
              (length > 2)) {
      printf("Error: must be just the integer 1, 2, 3, or 4\n");
    } else {
      valid = 1;
      if(input[0] == '1') {
        howManyMutations = 100;
      } else if(input[0] == '2') {
        howManyMutations = 500;
      } else if(input[0] == '3') {
        howManyMutations = 1000;
      } else {
        howManyMutations = 1500;
      }
    }
  }

  //fflush(stdout);
  srand(time(0));
  Image *original = readImage(file);
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

  if(whichShapes == 1) {
    for(int i = 0; i < howManyShapes; i++) {
      printf("%d\n", i+1);
      // char filename[14];
      // if(i<10) {
      //   sprintf(filename, "result00%d.bmp", i);
      // } else if (i < 100) {
      //   sprintf(filename, "result0%d.bmp", i);
      // } else {
      //   sprintf(filename, "result%d.bmp", i);
      // }
      // writeImage(canvas, filename);
      addTriangle(canvas, resizedOriginal, error, howManyMutations, .7);
    }
  } else if (whichShapes == 2) {
    for(int i = 0; i < howManyShapes; i++) {
      printf("%d\n", i+1);
      addEllipse(canvas, resizedOriginal, error, howManyMutations, .7);
    }
  } else {
    for(int i = 0; i < howManyShapes; i++) {
      printf("%d\n", i+1);
      addRectangle(canvas, resizedOriginal, error, howManyMutations, .7);
    }
  }

  writeImage(canvas, "result.bmp");
  //printf("%s %d %d %d\n", file, whichShapes, howManyShapes, howManyMutations);
  return 0;
}
