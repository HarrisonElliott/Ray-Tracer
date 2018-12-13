#ifndef BOXH
#define BOXH

#include "AARect.h"
#include "hitableList.h"

class Box : public Hitable
{
public:
	Box() {}
	Box(const Vec3& p0, const Vec3& p1, Material *ptr);
	virtual bool hit(const Ray& ray, float t0, float t1, Hit_Record& record) const;
	virtual bool boundingBox(float t0, float t1, AABB& box) const
	{
		box = AABB(pmin, pmax);
		return true;
	}
	Vec3 pmin, pmax;
	Hitable *listPtr;
};

Box::Box(const Vec3& p0, const Vec3& p1, Material *ptr) {
	pmin = p0;
	pmax = p1;
	Hitable **list = new Hitable*[6];
	list[0] = new xyRect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr);
	list[1] = new FlipNormals(new xyRect(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));
	list[2] = new xzRect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr);
	list[3] = new FlipNormals(new xzRect(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));
	list[4] = new yzRect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr);
	list[5] = new FlipNormals(new yzRect(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
	listPtr = new HitableList(list, 6);
}

bool Box::hit(const Ray& ray, float t0, float t1, Hit_Record& record) const {
	return listPtr->hit(ray, t0, t1, record);
}

#endif // BOXH