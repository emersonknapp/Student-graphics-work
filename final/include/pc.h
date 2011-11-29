#ifndef _pc_H
#define _pc_H

#include <vector>
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



#define VERSION_NUMBER "0.1"
#define PI 3.14159265
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define FRAMERATE 10
#define EPSILON 0.001
#define DEBUG false
#define KDEBUG false
#define T_MAX 400

#define MAXRECURSION 4
#define MAXLINE 255

//Simple function to display a message and exit the program with status 1
void Error(string);
void Warning(string);

enum {POINT, DIR};

#endif