#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"

class Sphere : public Hitable {
public:
	Sphere() {}
	Sphere(Vec3 cen, float rad, Material *mat) : center(cen), radius(rad), mat_ptr(mat) {};
	virtual bool hit(const Ray& ray, float t_min, float t_max, Hit_Record& record) const;
	virtual bool boundingBox(float t0, float t1, AABB& box) const;
	Vec3 center;
	float radius;
	Material *mat_ptr;
};

bool Sphere::boundingBox(float t0, float t1, AABB& box) const
{
	box = AABB(center - Vec3(radius, radius, radius), center + Vec3(radius, radius, radius));
	return true;
}

bool Sphere::hit(const Ray& ray, float t_min, float t_max, Hit_Record& record) const {
	Vec3 oc = ray.origin() - center;
	float a = dot(ray.direction(), ray.direction());
	float b = dot(oc, ray.direction());
	float c = dot(oc, oc) - (radius* radius);
	float discriminant = (b*b) - (a*c);
	if (discriminant > 0) {
		float temp = (-b - sqrt(discriminant))/a;
		if (temp < t_max && temp > t_min) {
			record.t = temp;
			record.p = ray.pointAtParameter(record.t);
			GetSphereUV((record.p - center) / radius, record.u, record.v);
			record.normal = (record.p - center) / radius;
			record.mat_ptr = mat_ptr;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			record.t = temp;
			record.p = ray.pointAtParameter(record.t);
			GetSphereUV((record.p - center) / radius, record.u, record.v);
			record.normal = (record.p - center) / radius;
			record.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}

#endif