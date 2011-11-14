#-Photon Cannon Makefile
CC=g++
cc=gcc

UNAME := $(shell uname)

#-LIBRARIES AND INCLUDES
LIBS = -lm -lstdc++ -lfreeimage
INCS = -I include
ifeq ($(UNAME), Linux)
INCS += -L./lib/filinux -I./lib/filinux 
endif
ifeq ($(UNAME), SunOS)
INCS += -L./lib/fisolaris -I./lib/fisolaris
endif
ifeq ($(UNAME), Darwin)
INCS += -L./lib/fiosx -I./lib/fiosx 
endif

#OPTIMIZATIONS
OPT = -fast -g -Wall
CCOPTS = $(OPT) $(INCS) -c
LDOPTS = $(OPT) $(INCS) $(LIBS)

#USEFUL INFORMATION
SOURCES=pc.cpp classes.cpp imagewriter.cpp scene.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=pc

#RULES
all: $(SOURCES) $(EXECUTABLE)

clean:
	/bin/rm -f *.o $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDOPTS) -o $@

.cpp.o:
	$(CC) $(CCOPTS) $< -o $@
