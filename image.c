#include <stdio.h>
#include <stdlib.h>
#include "image.h"

int getOffset(int hor) {
  return (4 - ((hor * 3) % 4)) % 4;
}

int getSizeOfPixelArray(int hor, int vert) {
  return ((((hor * 3) + 3) >> 2) << 2) * vert;
}

Image *readImage(char *filename) {
  Image *newImage = malloc(sizeof(Image));
  FILE *input = fopen(filename, "rp");
  unsigned char getChar;
  int vert = 0;
  int hor = 0;
  char skip[30];

  // while(1) {
  //    getChar = fgetc(input);
  //    if( feof(input) ) {
  //       break;
  //    }
  //    char q = getchar();
  //    if(q == 'q') break;
  //    printf(" - %02X - %u -", getChar, getChar);
  // }

  fgets(skip, 19, input);
  getChar = fgetc(input);
  hor += getChar;
  getChar = fgetc(input);
  hor += getChar * 256;
  getChar = fgetc(input);
  hor += getChar * 256 * 256;
  getChar = fgetc(input);
  hor += getChar * 256 * 256 * 256;
  getChar = fgetc(input);
  vert += getChar;
  getChar = fgetc(input);
  vert += getChar * 256;
  getChar = fgetc(input);
  vert += getChar * 256 * 256;
  getChar = fgetc(input);
  vert += getChar * 256 * 256 * 256;

  newImage->vert = vert;
  newImage->hor = hor;
  newImage->Pixels = malloc(sizeof(Pixel*) * vert);
  for(int i = 0; i < vert; i++) {
    newImage->Pixels[i] = malloc(sizeof(Pixel) * hor);
  }
  char offset = getOffset(hor);
  //printf("%d\n", offset);

  //printf("%d\n", offset);
  fgets(skip, 29, input);

  for(int i = 0; i < vert; i++) {
    for(int j = 0; j < hor; j++) {
      getChar = fgetc(input);
      //printf("%d\n", getChar);
      newImage->Pixels[i][j].Blue = getChar;
      getChar = fgetc(input);
      //printf("%d\n", getChar);
      newImage->Pixels[i][j].Green = getChar;
      getChar = fgetc(input);
      //printf("%d\n", getChar);
      newImage->Pixels[i][j].Red = getChar;
    }
    fgets(skip, offset + 1, input);
  }

  fclose(input);
  return(newImage);
}

void writeImage(Image *image, char *filename) {
  FILE *output = fopen(filename, "wp+");
  unsigned char header[] = {66, 77, 0 /* filesize starts here (2-5)*/, 0, 0, 0, \
                            0, 0, 0, 0, 54, 0, 0, 0, 40, 0, 0, 0, 0 /* width starts here (18-21)*/, \
                            0, 0, 0, 0 /* height starts here (22-25)*/, 0, 0, 0, \
                            1, 0, 24, 0, 0, 0, 0, 0, 0 /* size of pixel matrix starts here (34-37)*/, \
                            0, 0, 0, 116, 18, 0, 0, 116, 18, 0, 0, 0, 0, 0, 0, \
                            0, 0, 0, 0};
  int size = getSizeOfPixelArray(image->hor, image->vert);
  int totalSize = size + 54;
  header[2] = totalSize % 256;
  header[3] = (totalSize >> 8) % 256;
  header[4] = (totalSize >> 16) % 256;
  header[5] = (totalSize >> 24) % 256;
  header[18] = image->hor % 256;
  header[19] = (image->hor >> 8) % 256;
  header[20] = (image->hor >> 16) % 256;
  header[21] = (image->hor >> 24) % 256;
  header[22] = image->vert % 256;
  header[23] = (image->vert >> 8) % 256;
  header[24] = (image->vert >> 16) % 256;
  header[25] = (image->vert >> 24) % 256;
  header[34] = size % 256;
  header[35] = (size >> 8) % 256;
  header[36] = (size >> 16) % 256;
  header[37] = (size >> 24) % 256;
  fwrite(header, sizeof(char), sizeof(header), output);

  for(int i = 0; i < image->vert; i++) {
    for(int j = 0; j < image->hor; j++) {

      fwrite(&(image->Pixels[i][j].Blue), sizeof(char), 1, output);
      //printf("%d\n", image->Pixels[i][j].Blue);
      fwrite(&(image->Pixels[i][j].Green), sizeof(char), 1, output);
      fwrite(&(image->Pixels[i][j].Red), sizeof(char), 1, output);
    }
    unsigned char offset = getOffset(image->hor);
    unsigned char zero = 0;
    for(int j = 0; j < offset; j++) {
      fwrite(&(zero), sizeof(char), 1, output);
    }
  }

  fclose(output);
}

void freeImage(Image *image) {
  for(int i = 0; i < image->vert; i++) {
    free(image->Pixels[i]);
  }
  free(image->Pixels);
  free(image);
}

// int main () {
//
//    // printf("Dimensions: %d x %d \n", hor, vert);
//    //
//    // int size = (((hor * 3) >> 2) << 2) * vert + 54;
//    //
//    // printf("Size: %d\n", size);
//
//
//    // while(1) {
//    //    getChar = fgetc(input);
//    //    if( feof(input) ) {
//    //       break;
//    //    }
//    //    char q = getchar();
//    //    if(q == 'q') break;
//    //    printf(" - %02X - %u -", getChar, getChar);
//    // }
//
//    Image *newImage = readImage("mountain.bmp");
//    writeImage(newImage, "result.bmp");
//
//    return(0);
// }
