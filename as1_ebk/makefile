#!gmake 

#-----------------------------------------
#Basic Stuff -----------------------------
CC          = g++ 
cc          = gcc

#-----------------------------------------
#Optimization ----------------------------
OPTMAC = -fast

TARGETS = a1_ebk

OBJECTS = a1_ebk.o

#-----------------------------------------
# Mac specific stuff
FRAMEWORK = -framework GLUT
FRAMEWORK += -framework OpenGL
MACLIBS = -lGL -lGLU -lm -lstdc++ 
MACINCS = -L"/System/Library/Frameworks/OpenGL.framework/Libraries"

#-----------------------------------------
CCOPTSMAC = $(OPTMAC) $(MACINCS) -DOSX
LDOPTSMAC = $(OPTMAC) $(MACINCS) $(MACLIBS) -DOSX

#-----------------------------------------
#-----------------------------------------

default: $(TARGETS)

clean:
	/bin/rm -f *.o $(TARGETS)

#-----------------------------------------
#-----------------------------------------

a1_ebk: a1_ebk.o
	$(CC) a1_ebk.o $(FRAMEWORK) $(LDOPTSMAC) -o a1_ebk

a1_ebk.o: a1_ebk.cpp
	$(CC) a1_ebk.cpp -c $(CCOPTSMAC)

