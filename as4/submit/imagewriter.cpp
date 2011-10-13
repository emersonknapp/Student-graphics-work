#include "imagewriter.h"

using namespace std;

ImageWriter::ImageWriter() {
	FreeImage_Initialise();
	drawing = false;
	glOn = true;
}

void ImageWriter::init(int w, int h) {
	drawing = true;
	width = w;
	height = h;
	//bitmap holds FreeImage Pixels
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