#!gmake 

#-----------------------------------------
#Basic Stuff -----------------------------
CC          = g++ 
cc          = gcc

#-----------------------------------------
#Optimization ----------------------------
OPT   = -O3

#-----------------------------------------
# X       --------------------------------
X_INC  =   -I/usr/X11R6/include -I/sw/include -I/usr/sww/include -I/usr/sww/pkg/Mesa/include -I./lib/fisolaris -I./include
X_LIB  =   -L/usr/X11R6/lib -L/sw/lib -L/usr/sww/lib -L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib -L./lib/fisolaris

#-----------------------------------------

#-----------------------------------------
# GL      --------------------------------
GL_LIB  =   -lglut -lGLU -lGL -lX11 -lfreeimage

#-----------------------------------------

TARGET = as5

OBJECTS = imagewriter.o classes.o as5.o

SOURCES = imagewriter.cpp classes.cpp as5.cpp

#-----------------------------------------

LIBS = $(X_LIB) $(GL_LIB)
INCS = $(X_INC)

CCOPTS = $(OPT) $(INCS)
LDOPTS = $(OPT) $(INCS) $(LIBS)

#-----------------------------------------
#-----------------------------------------

default: $(TARGET)

clean:
	/bin/rm -f *.o $(TARGET)

#-----------------------------------------
#-----------------------------------------

as5: as5.o
	$(CC) $(OBJECTS) $(LDOPTS) -o $(TARGET) 

as5.o: as5.cpp
	$(CC) $(SOURCES) -c $(CCOPTS)

imagewriter.o: imagewriter.cpp
	$(CC) $(SOURCES) -c $(CCOPTS)

classes.o: classes.cpp
	$(CC) $(SOURCES) -c $(CCOPTS)


#export LD_LIBRARY_PATH="/usr/sww/pkg/Mesa/lib:/usr/lib:/usr/sww/lib"

