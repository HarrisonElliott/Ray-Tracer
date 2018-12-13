#ifndef HITABLELISTH
#define HITABLELISTH

#include "hitable.h"

class HitableList : public Hitable {
public:
	HitableList() {}
	HitableList(Hitable **l, int n) { list = l; listSize = n; }
	virtual bool hit(const Ray& ray, float t_min, float t_max, Hit_Record& record) const;
	virtual bool boundingBox(float t0, float t1, AABB& box) const;
	Hitable **list;
	int listSize;
};

bool HitableList::boundingBox(float t0, float t1, AABB& box) const 
{
	if (listSize < 1) return false;
	AABB tempBox;
	bool firstTrue = list[0]->boundingBox(t0, t1, tempBox);
	if (!firstTrue)
		return false;
	else
		box = tempBox;

	for (int i = 1; i < listSize; i++) {
		if (list[i]->boundingBox(t0, t1, tempBox)) {
			box = surroundingBox(box, tempBox);
		}
		else
			return false;
	}
	return true;
}

bool HitableList::hit(const Ray& ray, float t_min, float t_max, Hit_Record& record) const {
	Hit_Record tempRecord;
	bool hitAnything = false;
	double closestSoFar = t_max;
	for (int i = 0; i < listSize; i++) {
		if (list[i]->hit(ray, t_min, closestSoFar, tempRecord)) {
			hitAnything = true;
			closestSoFar = tempRecord.t;
			record = tempRecord;
		}
	}
	return hitAnything;
}


#endif