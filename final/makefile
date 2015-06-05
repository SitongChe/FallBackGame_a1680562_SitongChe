
PLATFORM := $(shell uname)

# If you're not using Cygwin or Linux, 
# you will need to add your own setup here
ifneq (, $(findstring CYGWIN, $(PLATFORM)))
    GL_LIBS = -lopengl32 -lglu32 -lglut32
	EXT = .exe
endif

ifneq (, $(findstring Linux, $(PLATFORM)))
    GL_LIBS = -lGL -lGLU -lglut -lGLEW
    GL_LIBS += -L/usr/lib/nvidia-304 # necessary for some NVidia drivers;
	EXT = 
endif

ifneq (, $(findstring Darwin, $(PLATFORM)))
    CPPFLAGS   = -DMacOSX -D__APPLE__ -m32 -arch x86_64    
    CPPFLAGS += -Wno-deprecated
    GL_LIBS = -framework GLUT -framework OpenGL -lGLEW
	EXT = 
endif

CC = g++ 
EXE = main
LDLIBS= -framework Glut -framework OpenGL -framework CoreFoundation SOIL.o image_helper.o image_DXT.o stb_image_aug.o
#LDLIBS=-lglut -lGLEW -lGL -lm

COMP_FLAGS = -DMACOSX -I.

OBJS = AppManager.o Carousel.o Vector3f.o BezierSpline.o BMPLoader.o BoundingBox.o Camera.o  Door.o Constants.o Flash.o Ghost.o GrandfatherClock.o Key.o main.o Material.o Mirror.o Object.o PivotDoor.o SkyBox.o SlideDoor.o Snowman.o SurvCamera.o Table.o TexturedSurface.o Transformation.o  Wall.o Window.o Viewer.o Chair.o shader_utils.o tiny_obj_loader.o particlesystem.o

.PHONY: all clean

# If you haven't seen this before, notice how the rules
# of the Makefile build .o files from c++ source, and then
# build the executable from the .o files. Files are only
# re-made if they've been modified, or something they depend
# on has been modified.

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(GL_LIBS) $(COMP_FLAGS) $(LDLIBS)

main.o: main.cpp AppManager.h Constants.h Viewer.h
	$(CC) $(CPPFLAGS) -c main.cpp

AppManager.o : shader_utils.o cube.cpp SOIL.o image_helper.o image_DXT.o  stb_image_aug.o AppManager.cpp AppManager.h Constants.h Wall.h Door.h PivotDoor.h Mirror.h SlideDoor.h Table.h BMPLoader.h TexturedSurface.h Viewer.h Chair.h Cube.h tiny_obj_loader.h particlesystem.h
	$(CC) $(CPPFLAGS) -c AppManager.cpp
Vector3f.o : Vector3f.cpp Vector3f.h
	$(CC) $(CPPFLAGS) -c Vector3f.cpp
particlesystem.o : particlesystem.cpp particlesystem.h particle.h
	$(CC) $(CPPFLAGS) -c particlesystem.cpp
tiny_obj_loader.o : tiny_obj_loader.cpp tiny_obj_loader.h
	$(CC) $(CPPFLAGS) -c tiny_obj_loader.cpp
shader_utils.o: shader_utils.cpp shader_utils.h
	$(CC) shader_utils.cpp -c $(COMP_FLAGS)
BezierSpline.o : BezierSpline.cpp Constants.h Vector3f.h
	$(CC) $(CPPFLAGS) -c BezierSpline.cpp
BMPLoader.o : BMPLoader.cpp BMPLoader.h
	$(CC) $(CPPFLAGS) -c BMPLoader.cpp
BoundingBox.o : BoundingBox.cpp BoundingBox.h Constants.h
	$(CC) $(CPPFLAGS) -c BoundingBox.cpp
Camera.o : Camera.cpp AppManager.h Constants.h Camera.h Material.h tiny_obj_loader.h
	$(CC) $(CPPFLAGS) -c Camera.cpp
Carousel.o : Carousel.cpp Carousel.h BezierSpline.h Object.h Vector3f.h
	$(CC) $(CPPFLAGS) -c Carousel.cpp
Constants.o : Constants.cpp Constants.h
	$(CC) $(CPPFLAGS) -c Constants.cpp
Door.o : Door.cpp Door.h
	$(CC) $(CPPFLAGS) -c Door.cpp

SOIL.o: SOIL.c SOIL.h stb_image_aug.h image_helper.h image_DXT.h
	$(CC) SOIL.c -c $(COMP_FLAGS)
image_helper.o: image_helper.c image_helper.h
	$(CC) image_helper.c -c $(COMP_FLAGS)

image_DXT.o: image_DXT.c image_DXT.h
	$(CC) image_DXT.c -c $(COMP_FLAGS)

stb_image_aug.o: stb_image_aug.c stb_image_aug.h stbi_DDS_aug.h stbi_DDS_aug_c.h
	$(CC) stb_image_aug.c -c $(COMP_FLAGS)

Chair.o : Chair.cpp Chair.h
	$(CC) $(CPPFLAGS) -c Chair.cpp
Viewer.o : Viewer.cpp Viewer.h InputState.h
	$(CC) $(CPPFLAGS) -c Viewer.cpp
Flash.o : Flash.cpp Flash.h
	$(CC) $(CPPFLAGS) -c Flash.cpp
Ghost.o : Ghost.cpp Ghost.h
	$(CC) $(CPPFLAGS) -c Ghost.cpp
GrandfatherClock.o : GrandfatherClock.cpp GrandfatherClock.h Constants.h BMPLoader.h
	$(CC) $(CPPFLAGS) -c GrandfatherClock.cpp
Key.o : Key.cpp Key.h Constants.h
	$(CC) $(CPPFLAGS) -c Key.cpp
Material.o : Material.cpp Material.h
	$(CC) $(CPPFLAGS) -c Material.cpp
Mirror.o : Mirror.cpp Mirror.h Constants.h
	$(CC) $(CPPFLAGS) -c Mirror.cpp
Object.o : Object.cpp Object.h
	$(CC) $(CPPFLAGS) -c Object.cpp
PivotDoor.o : PivotDoor.cpp PivotDoor.h
	$(CC) $(CPPFLAGS) -c PivotDoor.cpp
SkyBox.o : SkyBox.cpp SkyBox.h
	$(CC) $(CPPFLAGS) -c SkyBox.cpp
SlideDoor.o : SlideDoor.cpp SlideDoor.h
	$(CC) $(CPPFLAGS) -c SlideDoor.cpp
Snowman.o : Snowman.cpp Snowman.h Constants.h
	$(CC) $(CPPFLAGS) -c Snowman.cpp
SurvCamera.o : SurvCamera.cpp SurvCamera.h Constants.h Vector3f.h BMPLoader.h Material.h
	$(CC) $(CPPFLAGS) -c SurvCamera.cpp
Table.o : Table.cpp Table.h
	$(CC) $(CPPFLAGS) -c Table.cpp
TexturedSurface.o : TexturedSurface.cpp TexturedSurface.h
	$(CC) $(CPPFLAGS) -c TexturedSurface.cpp
Transformation.o : Transformation.cpp Transformation.h
	$(CC) $(CPPFLAGS) -c Transformation.cpp
Wall.o : Wall.cpp Wall.h Constants.h
	$(CC) $(CPPFLAGS) -c Wall.cpp
Window.o : Window.cpp Window.h
	$(CC) $(CPPFLAGS) -c Window.cpp
clean:
	rm -f *.o $(EXE)$(EXT)
