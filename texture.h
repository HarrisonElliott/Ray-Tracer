#ifndef TEXTUREH
#define TEXTUREH

#include "perlin.h"

class Texture 
{
public:
	virtual Vec3 value(float u, float v, const Vec3& p) const = 0;
};

class ConstantTexture : public Texture
{
public:
	ConstantTexture() {}
	ConstantTexture(Vec3 c) : colour(c) { }
	virtual Vec3 value(float u, float v, const Vec3& p) const {
		return colour;
	}
	Vec3 colour;
};

class CheckerTexture : public Texture {
public:
	CheckerTexture() { }
	CheckerTexture(Texture *t0, Texture *t1) : even(t0), odd(t1) { }
	virtual Vec3 value(float u, float v, const Vec3& p) const {
		float sines = sin(10.0f * p.x())*sin(10.0f * p.y())*sin(10.0f * p.z());
		if (sines < 0)
			return odd->value(u, v, p);
		else
			return even->value(u, v, p);
	}
	Texture *odd;
	Texture *even;
};

class NoiseTexture : public Texture {
public:
	NoiseTexture() {}
	NoiseTexture(float sc) : scale(sc) {}
	virtual Vec3 value(float u, float v, const Vec3& p) const
	{
		return Vec3(1.0f, 1.0f, 1.0f)*0.5f*(1.0f + sin(scale*p.x() + 5.0f*noise.turb(scale*p)));
		//return Vec3(1, 1, 1);
	}
	Perlin noise;
	float scale;
};

#endif // TEXTUREH
