#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
  printf("%d\n", whichShapes);

  return 0;
}
