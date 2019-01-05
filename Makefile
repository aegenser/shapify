
CFLAGS ?= -fsanitize=undefined
LDFLAGS ?= -lm

shapify: main.c triangle.c rectangle.c ellipse.c image.c imageTools.c
	 $(CC) $^ $(LDFLAGS) -o $@

debug: main.c triangle.c rectangle.c ellipse.c image.c imageTools.c
	 $(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@
	 
clean:
	rm shapify
