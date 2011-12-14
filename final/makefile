#-Photon Cannon Makefile
CC=g++
cc=gcc

OPT = -g -Wall
#OPT += -O3

UNAME := $(shell uname)

#-LIBRARIES AND INCLUDES
LIBS = -lm -lstdc++ -lfreeimage
INCS = -I include -fopenmp
ifeq ($(UNAME), Linux)
INCS += -I lib/filinux -Llib/filinux 
endif
ifeq ($(UNAME), SunOS)
INCS += -I lib/fisolaris -Llib/fisolaris 
endif
ifeq ($(UNAME), Darwin)
INCS += -I lib/fiosx -Llib/fiosx
#OPT += -fast
endif

#FLAGS
CCOPTS = $(OPT) $(INCS) -c
LDOPTS = $(OPT) $(INCS) $(LIBS)

#USEFUL INFORMATION
SOURCES=pc.cpp classes.cpp trees.cpp imagewriter.cpp scene.cpp renderable.cpp util.cpp lights.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=pc

#RULES
all: $(SOURCES) $(EXECUTABLE)

clean:
	/bin/rm -f *.o $(EXECUTABLE) out.png

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDOPTS) -o $@

.cpp.o:
	$(CC) $(CCOPTS) $< -o $@
