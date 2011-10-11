#ifndef _AS4_H
#define _AS4_H

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

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>

#include "algebra3.h"
#include "FreeImage.h"


#include "classes.h"
#include "imagewriter.h"

#ifdef _WIN32
static DWORD lastTime;
#else
static struct timeval lastTime;
#endif

#define PI 3.14159265
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define FRAMERATE 10
#define EPSILON 0.001
#define DEBUG false
#define T_MAX 400

#define MAXRECURSION 4
#define MAXLINE 255

//Simple function to display a message and exit the program with status 1
void Error(string);

#endif