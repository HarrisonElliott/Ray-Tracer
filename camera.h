#ifndef CAMERAH
#define CAMERAH

#include "ray.h"

#define PI 3.14159265358979323846 // PI
#define RANDOM_FLOAT ((double)rand() / RAND_MAX)

Vec3 randomInUnitDisk() {
	Vec3 p;
	do {
		p = 2.0f*Vec3(RANDOM_FLOAT, RANDOM_FLOAT, 0.0f) - Vec3(1.0f, 1.0f, 0.0f);
	} while (dot(p, p) >= 1.0f);
	return p;
}

class Camera {
public:
	Camera(Vec3 lookfrom, Vec3 lookat, Vec3 vup, float vfov, float aspect, float aperture, float focusDist,
		float t0, float t1) {
		time0 = t0;
		time1 = t1;
		lens_radius = aperture / 2;
		float theta = vfov *(PI/180.0f);
		float half_height = tan(theta / 2.0f);
		float half_width = aspect * half_height;
		origin = lookfrom;
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);
		lower_left_corner = origin - (half_width*focusDist*u) - (half_height*focusDist*v) - (focusDist*w);
		horizontal = 2.0f*(half_width*focusDist*u);
		vertical = 2.0f*(half_height*focusDist*v);
	}

	Ray GetRay(float s, float t) {
		Vec3 rd = lens_radius * randomInUnitDisk();
		Vec3 offset = (u*rd.x()) + (v*rd.y());
		float time = time0 + RANDOM_FLOAT *(time1 - time0);
		return Ray(origin + offset, lower_left_corner + (s*horizontal) + (t*vertical) - origin - offset, time);
	}

	Vec3 origin;
	Vec3 lower_left_corner;
	Vec3 horizontal;
	Vec3 vertical;
	Vec3 u, v, w;
	float time0, time1;
	float lens_radius;
};

#endif