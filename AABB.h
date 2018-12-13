#ifndef AABBH
#define AABBH

#include "ray.h"
#include "hitable.h"

inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }

class AABB
{
public:
	AABB() {}
	AABB(const Vec3& a, const Vec3& b) { _min = a; _max = b; }

	Vec3 min() const { return _min; }
	Vec3 max() const { return _max; }

	bool hit(const Ray& ray, float tmin, float tmax) const
	{
		for (int a = 0; a < 3; a++) {
                float t0 = ffmin((_min[a] - ray.origin()[a]) / ray.direction()[a],  
                                 (_max[a] - ray.origin()[a]) / ray.direction()[a]);
                float t1 = ffmax((_min[a] - ray.origin()[a]) / ray.direction()[a],  
                                 (_max[a] - ray.origin()[a]) / ray.direction()[a]);
                tmin = ffmax(t0, tmin);
                tmax = ffmin(t1, tmax);
                if (tmax <= tmin)
                    return false;
            }
            return true;
	}

	Vec3 _min;
	Vec3 _max;
};

AABB surroundingBox(AABB box0, AABB box1) {
	Vec3 small(fmin(box0.min().x(), box1.min().x()),
			   fmin(box0.min().y(), box1.min().y()),
			   fmin(box0.min().z(), box1.min().z()));
	Vec3 big  (fmax(box0.max().x(), box1.max().x()),
			   fmax(box0.max().y(), box1.max().y()),
			   fmax(box0.max().z(), box1.max().z()));
	return AABB(small, big);
}
#endif // AABBH