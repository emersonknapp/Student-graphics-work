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
X_INC  =   -I/usr/X11R6/include -I/sw/include -I/usr/sww/include -I/usr/sww/pkg/Mesa/include -I./fisolaris
X_LIB  =   -L/usr/X11R6/lib -L/sw/lib -L/usr/sww/lib -L/usr/sww/bin -L/usr/sww/pkg/Mesa/lib -L./fisolaris

#-----------------------------------------

#-----------------------------------------
# GL      --------------------------------
GL_LIB  =   -lglut -lGLU -lGL -lX11 -lfreeimage

#-----------------------------------------

TARGETS = as3

OBJECTS = as3.o

#-----------------------------------------

LIBS = $(X_LIB) $(GL_LIB)
INCS = $(X_INC)

CCOPTS = $(OPT) $(INCS)
LDOPTS = $(OPT) $(INCS) $(LIBS)

#-----------------------------------------
#-----------------------------------------

default: $(TARGETS)

clean:
	/bin/rm -f *.o $(TARGETS)

#-----------------------------------------
#-----------------------------------------

as3: as3.o
	$(CC) as3.o $(LDOPTS) -o as3 

as3.o: as3.cpp
	$(CC) as3.cpp -c $(CCOPTS)


#export LD_LIBRARY_PATH="/usr/sww/pkg/Mesa/lib:/usr/lib:/usr/sww/lib"

