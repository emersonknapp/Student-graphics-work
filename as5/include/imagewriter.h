#ifndef _IMAGEWRITER_H
#define _IMAGEWRITER_H

#include "as5.h"

#define BITSPERPIXEL 24

class ImageWriter {
public:
	ImageWriter();
	void init(int, int);
	void setPixel(int, int, unsigned char[3]);
	void setSize(int, int);
	bool drawing;
	bool glOn;
	int width;
	int height;
	string fileName;
	FIBITMAP* bitmap;
	void printScreen();
};

#endif