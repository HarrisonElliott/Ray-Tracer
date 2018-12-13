#include <iostream>
#include <fstream>
#include <random>
#include <vector>

/** MY INCLUDES **/
#include "AARect.h"
#include "box.h"
#include "bvh.h"
#include "material.h"
#include "camera.h"
#include "constantMedium.h"
#include "hitableList.h"
#include "imageTexture.h"
#include "movingSphere.h"
#include "sphere.h"
#include "texture.h"

/** STB IMAGE INCLUDE **/
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define RANDOM_FLOAT ((double)rand() / RAND_MAX)


Vec3 Colour(const Ray& ray, Hitable *world, int depth)
{
	Hit_Record record;
	if (world->hit(ray, 0.001f, FLT_MAX, record)) {
		Ray scattered;
		Vec3 attenuation;
		Vec3 emitted = record.mat_ptr->emitted(record.u, record.v, record.p);
		if (depth < 50 && record.mat_ptr->scatter(ray, record, attenuation, scattered)) {
			return emitted + attenuation * Colour(scattered, world, depth + 1.0f);
		}
		else {
			return emitted;
		}
	}
	else {
		return Vec3(0.0f, 0.0f, 0.0f);
	}
}

void ProcessRay(Camera camera, int i, int j, float xPixels, float yPixels, int numOfSpheres, Hitable* world)
{
	Vec3 colour(0.0f, 0.0f, 0.0f);
	for (int spheres = 0; spheres < numOfSpheres; spheres++) 
	{
		float u = float(i + RANDOM_FLOAT) / float(xPixels);
		float v = float(j + RANDOM_FLOAT) / float(yPixels);
		Ray ray = camera.GetRay(u, v);
		Vec3 p = ray.pointAtParameter(2.0f);
		colour += Colour(ray, world, 0);
	}
	colour /= float(numOfSpheres);
	colour = Vec3(sqrt(colour[0]), sqrt(colour[1]), sqrt(colour[2]));
	int outputR = int(255 * colour[0]);
	int outputG = int(255 * colour[1]);
	int outputB = int(255 * colour[2]);

	/**** Viewable Copy of Output ****/
	std::ofstream output("output.ppm");

	/** The P3 means the colors are in ASCII.
	  * The xPixels is how many collums.
	  * The yPixels is how many rows.
	  * The final is 255 for max colour.
	  * The Next time this is done is for the RGB triplets.
	  * Outputting to a PPM file to view it elsewhere. **/
	if (output.is_open())
	{
		output << "P3\n" << xPixels << " " << yPixels << "\n255\n";
	}
	/** These are those RGB triplets mentioned. **/
	//for (Vec3 pixels : pixels)
	{
		output << outputR << " " << outputG << " " << outputB << "\n";
	}
	/** END **/
}

/** CORNELL WORLDS **/
Hitable *cornell_box()
{
	Hitable **list = new Hitable*[8];
	int i = 0;
	Material *red = new Lambertian(new ConstantTexture(Vec3(0.65, 0.05, 0.05)));
	Material *white = new Lambertian(new ConstantTexture(Vec3(0.73, 0.73, 0.73)));
	Material *green = new Lambertian(new ConstantTexture(Vec3(0.12, 0.45, 0.15)));
	Material *light = new DiffuseLight(new ConstantTexture(Vec3(15, 15, 15)));
	list[i++] = new FlipNormals(new yzRect(0, 555, 0, 555, 555, green));
	list[i++] = new yzRect(0, 555, 0, 555, 0, red);
	list[i++] = new xzRect(213, 343, 227, 332, 554, light);
	list[i++] = new FlipNormals(new xzRect(0, 555, 0, 555, 555, white));
	list[i++] = new xzRect(0, 555, 0, 555, 0, white);
	list[i++] = new FlipNormals(new xyRect(0, 555, 0, 555, 555, white));
	list[i++] = new Translate(new RotateY(new Box(Vec3(0, 0, 0),
		Vec3(165, 165, 165), white), -18), Vec3(130, 0, 65));
	list[i++] = new Translate(new RotateY(new Box(Vec3(0, 0, 0),
		Vec3(165, 330, 165), white), 15), Vec3(265, 0, 295));
	return new HitableList(list, i);
}

Hitable *cornell_smoke()
{
	Hitable **list = new Hitable*[8];
	int i = 0;
	Material *red = new Lambertian(new ConstantTexture(Vec3(0.65, 0.05, 0.05)));
	Material *white = new Lambertian(new ConstantTexture(Vec3(0.73, 0.73, 0.73)));
	Material *green = new Lambertian(new ConstantTexture(Vec3(0.12, 0.45, 0.15)));
	Material *light = new DiffuseLight(new ConstantTexture(Vec3(7, 7, 7)));
	list[i++] = new FlipNormals(new yzRect(0, 555, 0, 555, 555, green));
	list[i++] = new yzRect(0, 555, 0, 555, 0, red);
	list[i++] = new xzRect(113, 443, 127, 432, 554, light);
	list[i++] = new FlipNormals(new xzRect(0, 555, 0, 555, 555, white));
	list[i++] = new xzRect(0, 555, 0, 555, 0, white);
	list[i++] = new FlipNormals(new xyRect(0, 555, 0, 555, 555, white));
	Hitable *b1 = new Translate(new RotateY(new Box(Vec3(0, 0, 0),
		Vec3(165, 165, 165), white), -18), Vec3(130, 0, 65));
	Hitable *b2 = new Translate(new RotateY(new Box(Vec3(0, 0, 0),
		Vec3(165, 330, 165), white), 15), Vec3(265, 0, 295));
	list[i++] = new ConstantMedium(b1, 0.01, new ConstantTexture(Vec3(1.0, 1.0, 1.0)));
	list[i++] = new ConstantMedium(b2, 0.01, new ConstantTexture(Vec3(0.0, 0.0, 0.0)));
	return new HitableList(list, i);
}

Hitable *cornell_balls()
{
	Hitable **list = new Hitable*[9];
	int i = 0;
	Material *red = new Lambertian(new ConstantTexture(Vec3(0.65, 0.05, 0.05)));
	Material *white = new Lambertian(new ConstantTexture(Vec3(0.73, 0.73, 0.73)));
	Material *green = new Lambertian(new ConstantTexture(Vec3(0.12, 0.45, 0.15)));
	Material *light = new DiffuseLight(new ConstantTexture(Vec3(5, 5, 5)));
	list[i++] = new FlipNormals(new yzRect(0, 555, 0, 555, 555, green));
	list[i++] = new yzRect(0, 555, 0, 555, 0, red);
	list[i++] = new xzRect(113, 443, 127, 432, 554, light);
	list[i++] = new FlipNormals(new xzRect(0, 555, 0, 555, 555, white));
	list[i++] = new xzRect(0, 555, 0, 555, 0, white);
	list[i++] = new FlipNormals(new xyRect(0, 555, 0, 555, 555, white));
	Hitable *boundary = new Sphere(Vec3(160, 100, 145), 100, new Dielectric(1.5));
	list[i++] = boundary;
	list[i++] = new ConstantMedium(boundary, 0.1, new ConstantTexture(Vec3(1.0, 1.0, 1.0)));
	list[i++] = new Translate(new RotateY(new Box(Vec3(0, 0, 0), Vec3(165, 330, 165), white), 15), Vec3(265, 0, 295));
	return new HitableList(list, i);
}

Hitable *cornell_final()
{
	Hitable **list = new Hitable*[30];
	Hitable **boxlist = new Hitable*[10000];
	Texture *pertext = new NoiseTexture(0.1);
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("res/earth.jpg", &nx, &ny, &nn, 0);
	Material *mat = new Lambertian(new ImageTexture(tex_data, nx, ny));
	int i = 0;
	Material *red = new Lambertian(new ConstantTexture(Vec3(0.65, 0.05, 0.05)));
	Material *white = new Lambertian(new ConstantTexture(Vec3(0.73, 0.73, 0.73)));
	Material *green = new Lambertian(new ConstantTexture(Vec3(0.12, 0.45, 0.15)));
	Material *light = new DiffuseLight(new ConstantTexture(Vec3(7, 7, 7)));
	//list[i++] = new sphere(vec3(260, 50, 145), 50,mat);
	list[i++] = new FlipNormals(new yzRect(0, 555, 0, 555, 555, green));
	list[i++] = new yzRect(0, 555, 0, 555, 0, red);
	list[i++] = new xzRect(123, 423, 147, 412, 554, light);
	list[i++] = new FlipNormals(new xzRect(0, 555, 0, 555, 555, white));
	list[i++] = new xzRect(0, 555, 0, 555, 0, white);
	list[i++] = new FlipNormals(new xyRect(0, 555, 0, 555, 555, white));
	/*
	Hitable *boundary = new Sphere(Vec3(160, 50, 345), 50, new Dielectric(1.5));
	list[i++] = boundary;
	list[i++] = new ConstantMedium(boundary, 0.2, new ConstantTexture(Vec3(0.2, 0.4, 0.9)));
	list[i++] = new Sphere(Vec3(460, 50, 105), 50, new Dielectric(1.5));
	list[i++] = new Sphere(Vec3(120, 50, 205), 50, new Lambertian(pertext));
	int numOfSpheres = 10000;
	for (int j = 0; j < ns; j++) {
		boxlist[j] = new Sphere(Vec3(165*RANDOM_FLOAT, 330*RANDOM_FLOAT, 165*RANDOM_FLOAT, 10, white);
	}
	list[i++] =   new translate(new RotateY(new BVH_Node(boxlist, numOfSpheres, 0.0, 1.0), 15), Vec3(265,0,295));
	*/
	Hitable *boundary2 = new Translate(new RotateY(new Box(Vec3(0, 0, 0), Vec3(165, 165, 165), new Dielectric(1.5)), -18), Vec3(130, 0, 65));
	list[i++] = boundary2;
	list[i++] = new ConstantMedium(boundary2, 0.2, new ConstantTexture(Vec3(0.9, 0.9, 0.9)));
	return new HitableList(list, i);
}


/** LIGHT TESTING WORLD **/
Hitable *sampleLight()
{
	Texture *pertext = new NoiseTexture();
	Hitable **list = new Hitable*[4];
	list[0] = new Sphere(Vec3(0, -1000, 0), 1000, new Lambertian(pertext));
	list[1] = new Sphere(Vec3(0, 2, 0), 2, new Lambertian(pertext));
	list[2] = new Sphere(Vec3(0, 6, 0), 1, new DiffuseLight(new ConstantTexture(Vec3(4, 4, 4))));
	list[3] = new xyRect(3, 5, 1, 3, -2, new DiffuseLight(new ConstantTexture(Vec3(4, 4, 4))));
	return new HitableList(list, 4);
}


/** SPHERE WORLDS **/
Hitable *twoSpheres()
{
	Texture *checker = new CheckerTexture(new ConstantTexture(Vec3(0.2, 0.3, 0.1)), new ConstantTexture(Vec3(0.9, 0.9, 0.9)));
	int n = 50;
	Hitable **list = new Hitable*[n + 1];
	list[0] = new Sphere(Vec3(0, -10, 0), 10, new Lambertian(checker));
	list[1] = new Sphere(Vec3(0, 10, 0), 10, new Lambertian(checker));

	return new HitableList(list, 2);
}

Hitable *twoPerlinSpheres()
{
	Texture *pertext = new NoiseTexture();
	Hitable **list = new Hitable*[2];
	list[0] = new Sphere(Vec3(0, -1000, 0), 1000, new Lambertian(pertext));
	list[1] = new Sphere(Vec3(0, 2, 0), 2, new Lambertian(pertext));
	return new HitableList(list, 2);
}


/** RANDOM WORLD **/
Hitable *randomScene()
{
	int n = 50000;
	Hitable **list = new Hitable*[n + 1];
	Texture *checker = new CheckerTexture(new ConstantTexture(Vec3(0.2, 0.3, 0.1)), new ConstantTexture(Vec3(0.9, 0.9, 0.9)));
	list[0] = new Sphere(Vec3(0, -1000, 0), 1000, new Lambertian(checker));
	int i = 1;
	for (int a = -10; a < 10; a++) {
		for (int b = -10; b < 10; b++) {
			float choose_mat = RANDOM_FLOAT;
			Vec3 center(a + 0.9*RANDOM_FLOAT, 0.2, b + 0.9*RANDOM_FLOAT);
			if ((center - Vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) {
					/** DIFFUSE **/
					list[i++] = new MovingSphere(center, center + Vec3(0, 0.5*RANDOM_FLOAT, 0), 0.0, 1.0, 0.2,
						new Lambertian(new ConstantTexture(Vec3(RANDOM_FLOAT*RANDOM_FLOAT, RANDOM_FLOAT*RANDOM_FLOAT, RANDOM_FLOAT*RANDOM_FLOAT))));
				}
				else if (choose_mat < 0.95) {
					/** METAL **/
					list[i++] = new Sphere(center, 0.2,
						new Metal(Vec3(0.5*(1 + RANDOM_FLOAT), 0.5*(1 + RANDOM_FLOAT), 0.5*(1 + RANDOM_FLOAT)), 0.5*RANDOM_FLOAT));
				}
				else {
					/** GLASS **/
					list[i++] = new Sphere(center, 0.2, new Dielectric(1.5));
				}
			}
		}
	}

	list[i++] = new Sphere(Vec3(0, 1, 0), 1.0, new Dielectric(1.5));
	list[i++] = new Sphere(Vec3(-4, 1, 0), 1.0, new Lambertian(new ConstantTexture(Vec3(0.4, 0.2, 0.1))));
	list[i++] = new Sphere(Vec3(4, 1, 0), 1.0, new Metal(Vec3(0.7, 0.6, 0.5), 0.0));

	return new BVH_Node(list, i, 0.0, 1.0);
}


/** IMG LOADING WORLD **/
Hitable *earth()
{
	int xPixels, yPixels, nn;
	//unsigned char *tex_data = stbi_load("tiled.jpg", &nx, &ny, &nn, 0);
	unsigned char *tex_data = stbi_load("res/earth.jpg", &xPixels, &yPixels, &nn, 0);
	Material *emat = new Lambertian(new ImageTexture(tex_data, xPixels, yPixels));
	return new Sphere(Vec3(0, 2, 0), 5, emat);
}


/** FINAL WORLD **/
Hitable *final()
{
	int numOfBoxes = 20;
	Hitable **list = new Hitable*[30];
	Hitable **boxlist = new Hitable*[1000];
	Hitable **boxlist2 = new Hitable*[1000];
	Material *white = new Lambertian(new ConstantTexture(Vec3(0.73f, 0.73f, 0.73f)));
	Material *ground = new Lambertian(new ConstantTexture(Vec3(0.48f, 0.83f, 0.53f)));
	int boxes = 0;
	for (int i = 0; i < numOfBoxes; i++) {
		for (int j = 0; j < numOfBoxes; j++) {
			float w = 100.0f;
			float x0 = -1000.0f + i * w;
			float z0 = -1000.0f + j * w;
			float y0 = 0.0f;
			float x1 = x0 + w;
			float y1 = 100.0f * (RANDOM_FLOAT + 0.01f);
			float z1 = z0 + w;
			boxlist[boxes++] = new Box(Vec3(x0, y0, z0), Vec3(x1, y1, z1), ground);
		}
	}
	int l = 0;
	list[l++] = new BVH_Node(boxlist, boxes, 0.0f, 1.0f);
	Material *light = new DiffuseLight(new ConstantTexture(Vec3(7.0f, 7.0f, 7.0f)));
	list[l++] = new xzRect(123.0f, 423.0f, 147.0f, 412.0f, 554.0f, light);
	Vec3 center(400.0f, 400.0f, 200.0f);
	list[l++] = new MovingSphere(center, center + Vec3(30.0f, 0.0f, 0.0f), 0.0f, 1.0f, 50.0f, new Lambertian(new ConstantTexture(Vec3(0.7f, 0.3f, 0.1f))));
	list[l++] = new Sphere(Vec3(260.0f, 150.0f, 45.0f), 50.0f, new Dielectric(1.5f));
	list[l++] = new Sphere(Vec3(0.0f, 150.0f, 145.0f), 50.0f, new Metal(Vec3(0.8f, 0.8f, 0.9f), 10.0f));
	Hitable *boundary = new Sphere(Vec3(360.0f, 150.0f, 145.0f), 70.0f, new Dielectric(1.5f));
	list[l++] = boundary;
	list[l++] = new ConstantMedium(boundary, 0.2f, new ConstantTexture(Vec3(0.2f, 0.4f, 0.9f)));
	boundary = new Sphere(Vec3(0.0f, 0.0f, 0.0f), 5000.0f, new Dielectric(1.5f));
	list[l++] = new ConstantMedium(boundary, 0.0001f, new ConstantTexture(Vec3(1.0f, 1.0f, 1.0f)));
	int xPixels, yPixels, nn;
	unsigned char *tex_data = stbi_load("res/earth.jpg", &xPixels, &yPixels, &nn, 0.0f);
	Material *emat = new Lambertian(new ImageTexture(tex_data, xPixels, yPixels));
	list[l++] = new Sphere(Vec3(400.0f, 200.0f, 400.0f), 100.0f, emat);
	Texture *pertext = new NoiseTexture(0.1f);
	list[l++] = new Sphere(Vec3(220.0f, 280.0f, 300.0f), 80.0f, new Lambertian(pertext));
	int numOfSpheres = 1000;
	for (int j = 0; j < numOfSpheres; j++) {
		boxlist2[j] = new Sphere(Vec3((165.0f*RANDOM_FLOAT), (165.0f*RANDOM_FLOAT), (165.0f*RANDOM_FLOAT)), 10.0f, white);
	}
	list[l++] = new Translate(new RotateY(new BVH_Node(boxlist2, numOfSpheres, 0.0f, 1.0f), 15.0f), Vec3(-100.0f, 270.0f, 395.0f));
	return new HitableList(list, l);
}

Hitable *finalTest()
{
	int numOfBoxes = 20;
	Hitable **list = new Hitable*[30];
	Hitable **boxlist = new Hitable*[10000];
	Hitable **boxlist2 = new Hitable*[10000];
	Material *white = new Lambertian(new ConstantTexture(Vec3(0.73, 0.73, 0.73)));
	Material *ground = new Lambertian(new ConstantTexture(Vec3(0.48, 0.83, 0.53)));
	int b = 0;
	for (int i = 0; i < numOfBoxes; i++) {
		for (int j = 0; j < numOfBoxes; j++) {
			float w = 100;
			float x0 = -1000 + i * w;
			float z0 = -1000 + j * w;
			float y0 = 0;
			float x1 = x0 + w;
			float y1 = 100 * (RANDOM_FLOAT + 0.01);
			float z1 = z0 + w;
			boxlist[b++] = new Box(Vec3(x0, y0, z0), Vec3(x1, y1, z1), ground);
		}
	}
	int l = 0;
	list[l++] = new BVH_Node(boxlist, b, 0, 1);
	Material *light = new DiffuseLight(new ConstantTexture(Vec3(7, 7, 7)));
	list[l++] = new xzRect(123, 423, 147, 412, 554, light);
	Vec3 center(400, 400, 200);
	list[l++] = new MovingSphere(center, center + Vec3(30, 0, 0), 0, 1, 50, new Lambertian(new ConstantTexture(Vec3(0.7, 0.3, 0.1))));
	list[l++] = new Sphere(Vec3(260, 150, 45), 50, new Dielectric(1.5));
	list[l++] = new Sphere(Vec3(0, 150, 145), 50, new Metal(Vec3(0.8, 0.8, 0.9), 10.0));
	Hitable *boundary = new Sphere(Vec3(360, 150, 145), 70, new Dielectric(1.5));
	list[l++] = boundary;
	list[l++] = new ConstantMedium(boundary, 0.2, new ConstantTexture(Vec3(0.2, 0.4, 0.9)));
	boundary = new Sphere(Vec3(0, 0, 0), 5000, new Dielectric(1.5));
	list[l++] = new ConstantMedium(boundary, 0.0001, new ConstantTexture(Vec3(1.0, 1.0, 1.0)));
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("res/earth.jpg", &nx, &ny, &nn, 0);
	Material *emat = new Lambertian(new ImageTexture(tex_data, nx, ny));
	list[l++] = new Sphere(Vec3(400, 200, 400), 100, emat);
	Texture *pertext = new NoiseTexture(0.1);
	list[l++] = new Sphere(Vec3(220, 280, 300), 80, new Lambertian(pertext));
	int numOfSpheres = 100;
	for (int j = 0; j < numOfSpheres; j++) {
		boxlist2[j] = new Sphere(Vec3(165 * RANDOM_FLOAT, 165 * RANDOM_FLOAT, 165 * RANDOM_FLOAT), 10, white);
	}
	list[l++] = new Translate(new RotateY(new BVH_Node(boxlist2, numOfSpheres, 0.0, 1.0), 15), Vec3(-100, 270, 395));
	return new HitableList(list, l);
}

int main()
{
	int xPixels = 800;
	int yPixels = 400;
	int numOfSpheres = 100;
	

	/**** WORLD SELECT ****/
	/** CORNELL **/
	//Hitable *world = cornell_box();
	//Hitable *world = cornell_smoke();
	//Hitable *world = cornell_balls();
	//Hitable *world = cornell_final();

	//Hitable *world = twoSpheres();
	//Hitable *world = twoPerlinSpheres();
	//Hitable *world = earth();
	//Hitable *world = sampleLight();

	/** RANDOM / FINAL TEST**/
	//Hitable *world = randomScene();
	Hitable *world = final();

	Vec3 lookFrom(478.0f, 278.0f, -600.0f);
	Vec3 lookAt(278.0f, 278.0f, 0.0f);

	float DistanceToFocus = 10.0f;
	float Aperture = 0.0f;
	float FOV = 40.0f;

	Camera camera(lookFrom, lookAt, Vec3(0.0f, 1.0f, 0.0f), FOV, float(xPixels) / float(yPixels), Aperture, DistanceToFocus, 0.0f, 1.0f);

	for (int j = (yPixels - 1); j >= 0; j--) {
		for (int i = 0; i < xPixels; i++) {
			ProcessRay(camera, i, j, xPixels, yPixels, numOfSpheres, world);
		}
	}
}