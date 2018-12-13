#ifndef AARECTH
#define AARECTH

#include "hitable.h"

class xyRect : public Hitable {
public:
	xyRect() {}
	xyRect(float _x0, float _x1, float _y0, float _y1, float _k, Material *mat) : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {};
	virtual bool hit(const Ray& ray, float t0, float t1, Hit_Record& record) const;
	virtual bool boundingBox(float t0, float t1, AABB& box) const {
		box = AABB(Vec3(x0, y0, k - 0.0001), Vec3(x1, y1, k + 0.0001));
		return true;
	}
	Material  *mp;
	float x0, x1, y0, y1, k;
};

class xzRect : public Hitable {
public:
	xzRect() {}
	xzRect(float _x0, float _x1, float _z0, float _z1, float _k, Material *mat) : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {};
	virtual bool hit(const Ray& ray, float t0, float t1, Hit_Record& record) const;
	virtual bool boundingBox(float t0, float t1, AABB& box) const {
		box = AABB(Vec3(x0, k - 0.0001, z0), Vec3(x1, k + 0.0001, z1));
		return true;
	}
	Material  *mp;
	float x0, x1, z0, z1, k;
};

class yzRect : public Hitable {
public:
	yzRect() {}
	yzRect(float _y0, float _y1, float _z0, float _z1, float _k, Material *mat) : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};
	virtual bool hit(const Ray& ray, float t0, float t1, Hit_Record& record) const;
	virtual bool boundingBox(float t0, float t1, AABB& box) const {
		box = AABB(Vec3(k - 0.0001, y0, z0), Vec3(k + 0.0001, y1, z1));
		return true;
	}
	Material  *mp;
	float y0, y1, z0, z1, k;
};

bool xyRect::hit(const Ray& ray, float t0, float t1, Hit_Record& record) const {
	float t = (k - ray.origin().z()) / ray.direction().z();
	if (t < t0 || t > t1)
		return false;
	float x = ray.origin().x() + t * ray.direction().x();
	float y = ray.origin().y() + t * ray.direction().y();
	if (x < x0 || x > x1 || y < y0 || y > y1)
		return false;
	record.u = (x - x0) / (x1 - x0);
	record.v = (y - y0) / (y1 - y0);
	record.t = t;
	record.mat_ptr = mp;
	record.p = ray.pointAtParameter(t);
	record.normal = Vec3(0.0f, 0.0f, 1.0f);
	return true;
}

bool xzRect::hit(const Ray& ray, float t0, float t1, Hit_Record& record) const {
	float t = (k - ray.origin().y()) / ray.direction().y();
	if (t < t0 || t > t1)
		return false;
	float x = ray.origin().x() + t * ray.direction().x();
	float z = ray.origin().z() + t * ray.direction().z();
	if (x < x0 || x > x1 || z < z0 || z > z1)
		return false;
	record.u = (x - x0) / (x1 - x0);
	record.v = (z - z0) / (z1 - z0);
	record.t = t;
	record.mat_ptr = mp;
	record.p = ray.pointAtParameter(t);
	record.normal = Vec3(0.0f, 1.0f, 0.0f);
	return true;
}

bool yzRect::hit(const Ray& ray, float t0, float t1, Hit_Record& record) const {
	float t = (k - ray.origin().x()) / ray.direction().x();
	if (t < t0 || t > t1)
		return false;
	float y = ray.origin().y() + t * ray.direction().y();
	float z = ray.origin().z() + t * ray.direction().z();
	if (y < y0 || y > y1 || z < z0 || z > z1)
		return false;
	record.u = (y - y0) / (y1 - y0);
	record.v = (z - z0) / (z1 - z0);
	record.t = t;
	record.mat_ptr = mp;
	record.p = ray.pointAtParameter(t);
	record.normal = Vec3(1.0f, 0.0f, 0.0f);
	return true;
}

#endif // AARECTH