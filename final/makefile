#-Photon Cannon Makefile
CC=g++
cc=gcc

OPT = -g -Wall -O3

UNAME := $(shell uname)

#-LIBRARIES AND INCLUDES
LIBS = -lm -lstdc++ -lfreeimage
INCS = -I include -fopenmp
ifeq ($(UNAME), Linux)
INCS += -L./lib/filinux -I./lib/filinux
endif
ifeq ($(UNAME), SunOS)
INCS += -L./lib/fisolaris -I./lib/fisolaris
endif
ifeq ($(UNAME), Darwin)
INCS += -L./lib/fiosx -I./lib/fiosx
OPT += -fast
endif

#FLAGS
CCOPTS = $(OPT) $(INCS) -c
LDOPTS = $(OPT) $(INCS) $(LIBS)

#USEFUL INFORMATION
SOURCES=pc.cpp classes.cpp trees.cpp imagewriter.cpp scene.cpp renderable.cpp
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
