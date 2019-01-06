# Shapify
This program approximates an image by placing semitransparent shapes that are then optimized to get as close to original image as possible.
Examples of this process, which originals and results, can be found in the examples folder. 
## Instructions
The program takes only 24 bit .bmp files. Take whichever image you wish to process and make sure it is in the proper format (I usually 
just used Microsoft Paint to do this conversion). Now in the command line run `make` and then `./shapify`. Then input the name of the
image you wish to process. Then choose which shape (triangle, ellipse, rectangle) you wish to use. Then choose how many shapes you want to
be placed, I recommend 300 or 400. Then choose how many mutations you wish to have. Mutations are how the program tries to find a better 
shape, I recommend 1000. As soon as you have chosen all those options, the program will begin to place the shapes, printing which shape it 
on so you can see the progress. Then it will write the result to result.bmp. I definitely recommend converting to a more compressed file 
type after. 
## Details 
This project was heavily inspired by @PrimitivePic on Twitter, however the code is original. \
The program first reads the image into an image struct that contains the images dimensions and a matrix of pixels, each of which has a red,
blue, and green value. Then the original image is resized so that larger side is 540 pixels across. Then another image is created, called 
the "canvas," that is 1080 across along the larger side, so four times larger than the resized original. The canvas is then colored to be 
the average color of the original image. \
Now an "error" matrix is created, that has the same dimensions as the canvas and measures the distance from the color at a pixel to its corresponding pixel in the original 
(if a pixel in the canvas is at (x,y) it corresponds to (x/2,y/2) in the resized original). The total error is the entire matrix added up. \
A random shape is then placed, and it is colored to be the average color of what the shape would cover in the original image. The error is 
recalculated. Now a mutation is made to the shape (e.g. width, height changed, vertex moved), and if the error is smaller with the new 
mutations, the mutation is kept. The program does as many mutations as it was told to, then places the final shape onto the canvas. This is
then repeated until the right amount of shapes have been placed. Then the result is written into a file. 
## To-Do
1. **Clean up code and add documentation**
   - Give functions/variables better names, remove commented out code, add comments for clarity, and add documentation.
2. **Add custom options for amount of shapes/mutations**
   - Currently there are only preset amounts, allow for another option that is to put a custom number.
3. **Add option for amount of opacity**
   - There is already an input variable for opacity, I just need to implement it into the menu.
4. **Allow detection if the input image does not exist or is a non-24 bit bmp file**
   - Right now the program just checks that you give a .bmp file, but will crash if the file does not exist and has weird behavior if the 
   file exists but is not 24 bit.
5. **Add functionality for mixing shapes**
   - Allow for a mixture, perhaps allow inputting a sequence of shape types and then the program adds the shapes in that repeating sequence.
6. **Restructure code to avoid repeating similar code and increase clarity**
   - A lot of the code is very similar, for example rectangle.c and ellipse.c contain almost the same code. Also, the seperation between image.c, 
   imageTools.c and the respective shape files is not necessarily intuitive. 
7. **Add multithreading**
   - There is an opportunity to make the code much faster by splitting the recalculation of error among multiple threads. 
8. **Make mutations better**
   - Not entirely happy with the results of the mutations, so experiment with options to make the mutations better. 
