#include "imagewriter.h"

using namespace std;

ImageWriter::ImageWriter(int w, int h) {
	FreeImage_Initialise();
	fileName = "out.png";
	width = w;
	height = h;
	setSize(w, h);
}

void ImageWriter::setSize(int w, int h) {
	bitmap = FreeImage_Allocate(w, h, BITSPERPIXEL);
	if (!bitmap) Error("Could not allocate memory for image.");
}

void ImageWriter::setPixel(int x, int y, unsigned char color[3]) {
	RGBQUAD rgb;
	rgb.rgbRed = color[0];
	rgb.rgbGreen = color[1];
	rgb.rgbBlue = color[2];
	FreeImage_SetPixelColor(bitmap, x, y, &rgb);
}

void ImageWriter::printScreen() {
	const char* name = fileName.c_str();
	//bitmap holds FreeImage Pixels

	FreeImage_Save(FIF_PNG, bitmap, name, 0);
	cout << "Image successfully saved to " << name << endl;
}