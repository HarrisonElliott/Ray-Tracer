#ifndef MOVINGSPHEREH
#define MOVINGSPHEREH

#include "hitable.h"

class MovingSphere : public Hitable {
public:
	MovingSphere() {}
	MovingSphere(Vec3 cen0, Vec3 cen1, float t0, float t1, float r, Material *m) : center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m) {};
	virtual bool hit(const Ray& ray, float tmin, float tmax, Hit_Record& record) const;
	virtual bool boundingBox(float t0, float t1, AABB& box) const;
	Vec3 center(float time) const;
	Vec3 center0, center1;
	float time0, time1;
	float radius;
	Material *mat_ptr;
};

Vec3 MovingSphere::center(float time) const {
	return center0 + ((time - time0) / (time1 - time0))*(center1 - center0);
}

bool MovingSphere::boundingBox(float t0, float t1, AABB& box) const {
	AABB box0(center(t0) - Vec3(radius, radius, radius), center(t0) + Vec3(radius, radius, radius));
	AABB box1(center(t1) - Vec3(radius, radius, radius), center(t1) + Vec3(radius, radius, radius));
	box = surroundingBox(box0, box1);
	return true;
}

bool MovingSphere::hit(const Ray& ray, float t_min, float t_max, Hit_Record& record) const {
	Vec3 oc = ray.origin() - center(ray.time());
	float a = dot(ray.direction(), ray.direction());
	float b = dot(oc, ray.direction());
	float c = dot(oc, oc) - (radius*radius);
	float discriminant = (b*b) - (a*c);
	if (discriminant > 0) {
		float temp = (-b - sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			record.t = temp;
			record.p = ray.pointAtParameter(record.t);
			record.normal = (record.p - center(ray.time())) / radius;
			record.mat_ptr = mat_ptr;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			record.t = temp;
			record.p = ray.pointAtParameter(record.t);
			record.normal = (record.p - center(ray.time())) / radius;
			record.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}


#endif // MOVINGSPHEREH