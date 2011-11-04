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

	
	if (glOn) {
		RGBQUAD color;

		//pRGB holds openGL pixel output
		unsigned char *pRGB = new unsigned char [3* (width+1) * (height+1) + 3];
		glReadBuffer(GL_BACK);
		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pRGB);

		for (int i=0; i<width*height; i++) {
			int index = i*3;

			int x = i%width;
			int y = (i - i%width)/width;
			color.rgbRed = pRGB[index];
			color.rgbGreen = pRGB[index+1];
			color.rgbBlue = pRGB[index+2];
			FreeImage_SetPixelColor(bitmap, x, y, &color);
		}
	}

	FreeImage_Save(FIF_PNG, bitmap, name, 0);
	cout << "Image successfully saved to " << name << endl;
}