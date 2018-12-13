#ifndef BVHH
#define BVHH

#include "hitable.h"

#define RANDOM_FLOAT ((double)rand() / RAND_MAX)

class BVH_Node : public Hitable
{
public:
	BVH_Node() {}
	BVH_Node(Hitable **l, int n, float time0, float time1);
	virtual bool hit(const Ray& ray, float tmin, float tmax, Hit_Record& record) const;
	virtual bool boundingBox(float t0, float t1, AABB& box) const;
	Hitable *left;
	Hitable *right;
	AABB box;
};

bool BVH_Node::boundingBox(float t0, float t1, AABB& b) const {
	b = box;
	return true;
}

bool BVH_Node::hit(const Ray& ray, float t_min, float t_max, Hit_Record& record) const
{
	if (box.hit(ray, t_min, t_max)) {
        Hit_Record leftRecord, rightRecord;
        bool hitLeft = left->hit(ray, t_min, t_max, leftRecord);
        bool hitRight = right->hit(ray, t_min, t_max, rightRecord);
        if (hitLeft && hitRight) {
            if (leftRecord.t < rightRecord.t) 
                record = leftRecord;
            else 
                record = rightRecord;
            return true;
        }
		else if (hitLeft) {
            record = leftRecord;
            return true;
        }
        else if (hitRight) {
            record = rightRecord;
            return true;
        }
        else 
            return false;
    }
    else return false;
}

int boxXCompare(const void * a, const void * b) {
	AABB boxLeft, boxRight;
	Hitable *ah = *(Hitable**)a;
	Hitable *bh = *(Hitable**)b;
	if (!ah->boundingBox(0.0f, 0.0f, boxLeft) || !bh->boundingBox(0.0f, 0.0f, boxRight))
		std::cerr << "no bounding box in BVH_Node constructor\n";
	if (boxLeft.min().x() - boxRight.min().x() < 0.0f)
		return -1;
	else
		return 1;
}

int boxYCompare(const void * a, const void * b)
{
	AABB boxLeft, boxRight;
	Hitable *ah = *(Hitable**)a;
	Hitable *bh = *(Hitable**)b;
	if (!ah->boundingBox(0.0f, 0.0f, boxLeft) || !bh->boundingBox(0.0f, 0.0f, boxRight))
		std::cerr << "no bounding box in BVH_Node constructor\n";
	if (boxLeft.min().y() - boxRight.min().y() < 0.0f)
		return -1;
	else
		return 1;
}
int boxZCompare(const void * a, const void * b)
{
	AABB boxLeft, boxRight;
	Hitable *ah = *(Hitable**)a;
	Hitable *bh = *(Hitable**)b;
	if (!ah->boundingBox(0.0f, 0.0f, boxLeft) || !bh->boundingBox(0.0f, 0.0f, boxRight))
		std::cerr << "no bounding box in BVH_Node constructor\n";
	if (boxLeft.min().z() - boxRight.min().z() < 0.0f)
		return -1;
	else
		return 1;
}

BVH_Node::BVH_Node(Hitable **l, int n, float time0, float time1) {
	int axis = int(3 * RANDOM_FLOAT);
	if (axis == 0)
		qsort(l, n, sizeof(Hitable *), boxXCompare);
	else if (axis == 1)
		qsort(l, n, sizeof(Hitable *), boxYCompare);
	else
		qsort(l, n, sizeof(Hitable *), boxZCompare);
	if (n == 1) {
		left = right = l[0];
	}
	else if (n == 2) {
		left = l[0];
		right = l[1];
	}
	else {
		left = new BVH_Node(l, n / 2, time0, time1);
		right = new BVH_Node(l + n / 2, n - n / 2, time0, time1);
	}
	AABB boxLeft, boxRight;
	if (!left->boundingBox(time0, time1, boxLeft) || !right->boundingBox(time0, time1, boxRight))
		std::cerr << "no bounding box in BVH_Node constructor\n";
	box = surroundingBox(boxLeft, boxRight);
}
#endif // BVHH