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

TARGETS = as1

OBJECTS = as1.o

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

as1: as1.o
	$(CC) as1.o $(LDOPTS) -o as1 

as1.o: as1.cpp
	$(CC) as1.cpp -c $(CCOPTS)


#export LD_LIBRARY_PATH="/usr/sww/pkg/Mesa/lib:/usr/lib:/usr/sww/lib"

