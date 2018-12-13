#ifndef CONSTANTMEDIUMH
#define CONSTANTMEDIUMH

#include "hitable.h"

class ConstantMedium : public Hitable {
public:
	ConstantMedium(Hitable *b, float d, Texture *a) : boundary(b), density(d) { phaseFunction = new Isotropic(a); }
	virtual bool hit(const Ray& ray, float t_min, float t_max, Hit_Record& record) const;
	virtual bool boundingBox(float t0, float t1, AABB& box) const {
		return boundary->boundingBox(t0, t1, box);
	}
	Hitable *boundary;
	float density;
	Material *phaseFunction;
};

bool ConstantMedium::hit(const Ray& ray, float t_min, float t_max, Hit_Record& record) const {
	bool db = (RANDOM_FLOAT < 0.00001);
	db = false;
	Hit_Record record1, record2;
	if (boundary->hit(ray, -FLT_MAX, FLT_MAX, record1)) {
		if (boundary->hit(ray, record1.t + 0.0001, FLT_MAX, record2)) {
			if (db) std::cerr << "\nt0 t1 " << record1.t << " " << record2.t << "\n";
			if (record1.t < t_min)
				record1.t = t_min;
			if (record2.t > t_max)
				record2.t = t_max;
			if (record1.t >= record2.t)
				return false;
			if (record1.t < 0)
				record1.t = 0;
			float distanceInsideBoundary = (record2.t - record1.t)*ray.direction().length();
			float hitDistance = -(1.0f / density)*log(RANDOM_FLOAT);
			if (hitDistance < distanceInsideBoundary) {
				if (db) std::cerr << "hitDistance = " << hitDistance << "\n";
				record.t = record1.t + (hitDistance / ray.direction().length());
				if (db) std::cerr << "record.t = " << record.t << "\n";
				record.p = ray.pointAtParameter(record.t);
				if (db) std::cerr << "record.p = " << record.p << "\n";
				record.normal = Vec3(1.0f, 0.0f, 0.0f);  // arbitrary
				record.mat_ptr = phaseFunction;
				return true;
			}
		}
	}
	return false;
}

#endif