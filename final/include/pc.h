#ifndef _pc_H
#define _pc_H

#include <vector>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstring>

#ifdef _WIN32
#	include <windows.h>
#else
#	include <sys/time.h>
#endif

#include <time.h>
#include <math.h>

#include "algebra3.h"
#include "FreeImage.h"

#include "classes.h"
#include "imagewriter.h"
#include "trees.h"
#include "scene.h"
#include "util.h"

//Simple function to display a message and exit the program with status 1
void Error(string);
void Warning(string);

enum {POINT, DIR};

#endif
