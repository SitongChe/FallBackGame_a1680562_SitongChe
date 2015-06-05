
CC = g++ 

LDLIBS= -framework Glut -framework OpenGL -framework CoreFoundation SOIL.o image_helper.o image_DXT.o stb_image_aug.o
#LDLIBS=-lglut -lGLEW -lGL -lm

COMP_FLAGS = -DMACOSX -I.

all: cube

cube: shader_utils.o cube.cpp SOIL.o image_helper.o image_DXT.o stb_image_aug.o
	$(CC) cube.cpp -o cube shader_utils.o $(COMP_FLAGS) $(LDLIBS)

shader_utils.o: shader_utils.cpp
	$(CC) shader_utils.cpp -c $(COMP_FLAGS)

SOIL.o: SOIL.c
	$(CC) SOIL.c -c $(COMP_FLAGS)

image_helper.o: image_helper.c
	$(CC) image_helper.c -c $(COMP_FLAGS)

image_DXT.o: image_DXT.c
	$(CC) image_DXT.c -c $(COMP_FLAGS)

stb_image_aug.o: stb_image_aug.c
	$(CC) stb_image_aug.c -c $(COMP_FLAGS)

clean:
	rm -f *.o cube

