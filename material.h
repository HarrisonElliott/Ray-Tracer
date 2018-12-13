#ifndef MATERIALH
#define MATERIALH

#include <random>

struct hit_record;

#include "ray.h"
#include "hitable.h"
#include "texture.h"

#define RANDOM_FLOAT ((double)rand() / RAND_MAX)

float Schlick(float cosine, float ref_idx) 
{
	float r0 = (1.0f-ref_idx)/(1.0f+ref_idx);
	r0 = (r0*r0);
	return r0 + (1.0f-r0)*pow((1.0f-cosine), 5.0f);
}

bool Refract(const Vec3& v, const Vec3& n, float ni_over_nt, Vec3& refracted)
{
	Vec3 uv = unit_vector(v);
	float dt = dot(uv, n);
	float discriminant = 1.0f - (ni_over_nt*ni_over_nt)*(1-dt*dt);
	if (discriminant > 0) {
		refracted = ni_over_nt * (uv- (n*dt)) - n*sqrt(discriminant);
		return true;
	}
	else
		return false;
}

Vec3 Reflect(const Vec3& v, const Vec3& n)
{
	return v - (2.0f*dot(v,n)*n);
}

Vec3 RandomInUnitSphere()
{
	Vec3 p;
	do 
	{
		p = (2.0f*Vec3(RANDOM_FLOAT, RANDOM_FLOAT, RANDOM_FLOAT)) - Vec3(1.0f, 1.0f, 1.0f);
	} while (dot(p,p) >= 1.0f);
	return p;
}

class Material 
{
public:
	virtual bool scatter(const Ray& rayIn, const Hit_Record& record, Vec3& attenuation, Ray& scattered) const = 0;
	virtual Vec3 emitted(float u, float v, const Vec3& p) const { return Vec3(0.0f, 0.0f, 0.0f); }
};

class Metal : public Material {
public:
	Metal(const Vec3& a, float f) : albedo(a) { if (f < 1) fuzz = f; else fuzz = 1.0f; }
	virtual bool scatter(const Ray& rayIn, const Hit_Record& record, Vec3& attenuation, Ray& scattered) const {
		Vec3 reflected = Reflect(unit_vector(rayIn.direction()), record.normal);
		scattered = Ray(record.p, reflected + (fuzz*RandomInUnitSphere()));
		attenuation = albedo;
		return (dot(scattered.direction(), record.normal) > 0);
	}
	Vec3 albedo;
	float fuzz;
};

class Lambertian : public Material {
public:
	Lambertian(Texture *a) : albedo(a) {}
	virtual bool scatter(const Ray& rayIn, const Hit_Record& record, Vec3& attenuation, Ray& scattered) const {
		Vec3 target = record.p + record.normal + RandomInUnitSphere();
		scattered = Ray(record.p, target - record.p, rayIn.time());
		attenuation = albedo->value(record.u, record.v, record.p);
		return true;
	}

	Texture *albedo;
};

class Dielectric : public Material {
public:
	Dielectric(float ri) : ref_idx(ri) {}
	virtual bool scatter(const Ray& rayIn, const Hit_Record& record, Vec3& attenuation, Ray& scattered) const {
		Vec3 outward_normal;
		Vec3 reflected = Reflect(rayIn.direction(), record.normal);
		float ni_over_nt;
		attenuation = Vec3(1.0f, 1.0f, 1.0f);
		Vec3 refracted;
		float reflect_prob;
		float cosine;
		if (dot(rayIn.direction(), record.normal) > 0) {
			outward_normal = -record.normal;
			ni_over_nt = ref_idx;
			cosine = ref_idx * dot(rayIn.direction(), record.normal) / rayIn.direction().length();
		}
		else {
			outward_normal = record.normal;
			ni_over_nt = 1.0 / ref_idx;
			cosine = -dot(rayIn.direction(), record.normal) / rayIn.direction().length();
		}
		if (Refract(rayIn.direction(), outward_normal, ni_over_nt, refracted)) {
			reflect_prob = Schlick(cosine, ref_idx);
		}
		else {
			scattered = Ray(record.p, reflected);
			reflect_prob = 1.0;
		}
		if (RANDOM_FLOAT < reflect_prob) {
			scattered = Ray(record.p, reflected);
		}
		else {
			scattered = Ray(record.p, refracted);
		}
		return true;
	}
	float ref_idx;
};

class Isotropic : public Material {
public:
	Isotropic(Texture *a) : albedo(a) {}
	virtual bool scatter(const Ray& rayIn, const Hit_Record& record, Vec3& attenuation, Ray& scattered) const {
		scattered = Ray(record.p, RandomInUnitSphere());
		attenuation = albedo->value(record.u, record.v, record.p);
		return true;
	}
	Texture *albedo;
};

class DiffuseLight : public Material {
public:
	DiffuseLight(Texture *a) : emit(a) {}
	virtual bool scatter(const Ray& rayIn, const Hit_Record& record, Vec3& attenuation, Ray& scattered) const { return false; }
	virtual Vec3 emitted(float u, float v, const Vec3& p) const { return emit->value(u, v, p); }
	Texture *emit;
};

#endif