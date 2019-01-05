#ifndef IMAGE_H
#define IMAGE_H


typedef struct Pixel_s {
  unsigned char Red;
  unsigned char Blue;
  unsigned char Green;
} Pixel;

typedef struct Image_s {
  int vert;
  int hor;
  Pixel **Pixels;
} Image;

int getOffset(int hor);

int getSizeOfPixelArray(int hor, int vert);

Image *readImage(char *filename);

void writeImage(Image *image, char *filename);

void freeImage(Image *image);

#endif
