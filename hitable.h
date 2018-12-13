#ifndef HITABLEH
#define HITABLEH

#define PI 3.14159265358979323846 // PI

#include "AABB.h"

class Material;

void GetSphereUV(const Vec3& p, float& u, float& v) {
	float phi = atan2(p.z(), p.x());
	float theta = asin(p.y());
	u = 1.0f - (phi + PI)/(2.0f * PI);
	v = (theta + (PI/2.0f)) / PI;
}

struct Hit_Record
{
	float t;
	float u;
	float v;
	Vec3 p;
	Vec3 normal;
	Material *mat_ptr;
};

class Hitable {
public:
	virtual bool hit(const Ray& ray, float t_min, float t_max, Hit_Record& record) const = 0;
	virtual bool boundingBox(float t0, float t1, AABB& box) const = 0;
};

class FlipNormals : public Hitable {
public:
	FlipNormals(Hitable *p) : ptr(p) {}
	virtual bool hit(const Ray& ray, float t_min, float t_max, Hit_Record& record) const {
		if (ptr->hit(ray, t_min, t_max, record)) {
			record.normal = -record.normal;
			return true;
		}
		else
			return false;
	}
	virtual bool boundingBox(float t0, float t1, AABB& box) const {
		return ptr->boundingBox(t0, t1, box);
	}
	Hitable *ptr;
};

class Translate : public Hitable {
public:
	Translate(Hitable *p, const Vec3& displacement) : ptr(p), offset(displacement) {}
	virtual bool hit(const Ray& ray, float t_min, float t_max, Hit_Record& record) const;
	virtual bool boundingBox(float t0, float t1, AABB& box) const;
	Hitable *ptr;
	Vec3 offset;
};

bool Translate::hit(const Ray& ray, float t_min, float t_max, Hit_Record& record) const {
	Ray movedRay(ray.origin() - offset, ray.direction(), ray.time());
	if (ptr->hit(movedRay, t_min, t_max, record)) {
		record.p += offset;
		return true;
	}
	else
		return false;
}

bool Translate::boundingBox(float t0, float t1, AABB& box) const {
	if (ptr->boundingBox(t0, t1, box)) {
		box = AABB(box.min() + offset, box.max() + offset);
		return true;
	}
	else
		return false;
}

class RotateY : public Hitable {
public:
	RotateY(Hitable *p, float angle);
	virtual bool hit(const Ray& ray, float t_min, float t_max, Hit_Record& record) const;
	virtual bool boundingBox(float t0, float t1, AABB& box) const {
		box = bbox; return hasbox;
	}
	Hitable *ptr;
	float sin_theta;
	float cos_theta;
	bool hasbox;
	AABB bbox;
};

RotateY::RotateY(Hitable *p, float angle) : ptr(p) {
	float radians = (PI/180.0f) * angle;
	sin_theta = sin(radians);
	cos_theta = cos(radians);
	hasbox = ptr->boundingBox(0, 1, bbox);
	Vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
	Vec3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				float x = (i*bbox.max().x()) + ((1.0f - i)*bbox.min().x());
				float y = (j*bbox.max().y()) + ((1.0f - j)*bbox.min().y());
				float z = (k*bbox.max().z()) + ((1.0f - k)*bbox.min().z());
				float newx = cos_theta * x + sin_theta * z;
				float newz = -sin_theta * x + cos_theta * z;
				Vec3 tester(newx, y, newz);
				for (int c = 0; c < 3; c++)
				{
					if (tester[c] > max[c])
						max[c] = tester[c];
					if (tester[c] < min[c])
						min[c] = tester[c];
				}
			}
		}
	}
	bbox = AABB(min, max);
}

bool RotateY::hit(const Ray& ray, float t_min, float t_max, Hit_Record& record) const {
	Vec3 origin = ray.origin();
	Vec3 direction = ray.direction();
	origin[0] = (cos_theta*ray.origin()[0]) - (sin_theta*ray.origin()[2]);
	origin[2] = (sin_theta*ray.origin()[0]) + (cos_theta*ray.origin()[2]);
	direction[0] = (cos_theta*ray.direction()[0]) - (sin_theta*ray.direction()[2]);
	direction[2] = (sin_theta*ray.direction()[0]) + (cos_theta*ray.direction()[2]);
	Ray rotatedRay(origin, direction, ray.time());
	if (ptr->hit(rotatedRay, t_min, t_max, record)) {
		Vec3 p = record.p;
		Vec3 normal = record.normal;
		p[0] = (cos_theta*record.p[0]) + (sin_theta*record.p[2]);
		p[2] = (-sin_theta*record.p[0]) + (cos_theta*record.p[2]);
		normal[0] = (cos_theta*record.normal[0]) + (sin_theta*record.normal[2]);
		normal[2] = (-sin_theta*record.normal[0]) + (cos_theta*record.normal[2]);
		record.p = p;
		record.normal = normal;
		return true;
	}
	else
		return false;
}

#endif