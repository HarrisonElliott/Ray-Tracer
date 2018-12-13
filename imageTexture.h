#ifndef IMAGETEXTUREH
#define IMAGETEXTUREH

#include "texture.h"

class ImageTexture : public Texture {
public:
	ImageTexture() {}
	ImageTexture(unsigned char *pixels, int A, int B) : data(pixels), xPixels(A), yPixels(B) {}
	virtual Vec3 value(float u, float v, const Vec3& p) const;
	unsigned char *data;
	int xPixels, yPixels;
};

Vec3 ImageTexture::value(float u, float v, const Vec3& p) const {
	int i = (u)*xPixels;
	int j = (1-v)*yPixels - 0.001;
	if (i < 0) i = 0;
	if (j < 0) j = 0;
	if (i > xPixels - 1) i = xPixels - 1;
	if (j > yPixels - 1) j = yPixels - 1;
	float r = int(data[(3*i) + (3*xPixels*j)]) / 255;
	float g = int(data[(3*i) + (3*xPixels*j + 1)]) / 255;
	float b = int(data[(3*i) + (3*xPixels*j + 2)]) / 255;
	return Vec3(r, g, b);
}

#endif // IMAGETEXTUREH