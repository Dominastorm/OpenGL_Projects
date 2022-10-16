#include <iostream>
#include <GL/glut.h>
#include <random>
#include <cmath>
#include <tuple>

using namespace std;

const double pi = 3.14159265358979323846;
const float cube_size = .25, sphere_size = 1.2;
int WINDOW_SIZE[] = { 640, 640 };

random_device device;
mt19937 rng(device());
uniform_real_distribution<>distribution(0.0f, 1.0f);

float random() {
	return distribution(rng);
}

void setMaterial(float ambientR, float ambientG, float ambientB, float diffuseR, float diffuseG, float diffuseB, float specularR, float specularG, float specularB, float shine) {
	GLfloat ambient[] = { ambientR, ambientG, ambientB };
	GLfloat diffuse[] = { diffuseR, diffuseG, diffuseB };
	GLfloat specular[] = { specularR, specularG, specularB };

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shine);
}

void initLight()
{
	GLfloat ambient[] = { 0.3, 0.3, 0.3 };
	GLfloat diffuse[] = { 0.5, 0.5, 0.5 };
	GLfloat specular[] = { 1, 1, 1 };
	GLfloat position[] = { -2,0.5,4,1 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_COLOR_MATERIAL);
}

class Camera {
public:
	float upx = 0, upy = 1, upz = 0;
	float theta = 0, phi = 0, r = 2;
	float x = 0, y = 0, z = r;
	float lax = 0, lay = 0, laz = 0;
};

void sphere(float size = sphere_size) {
	glPushMatrix();
	glColor3f(.6, .6, .6);
	glutWireSphere(size, 18, 18);
	glPopMatrix();
}

class Object3D {
public:
	float x = 0.5 - random(), y = 0.5 - random(), z = 0.5 - random();
	float temp[3] = { 0,0,0 };
	bool should_pause = false;
	bool paused = false;
	float size = 0;
	float color[3] = { random(), random(), random() };
	float vx = (-0.5f + random()) / 2000, vy = (-0.5f + random()) / 2000, vz = (-0.5f + random()) / 2000;
	float angle = 0, rx = random() / 100, ry = random() / 100, rz = random() / 100;

	Object3D(float sz = 0, float stx = -0.5f + random()) {
		x = stx;
		size = sz;
	}
};

void cube(Object3D t) {
	glPushMatrix();
	glColor3f(t.color[0], t.color[1], t.color[2]);
	glTranslatef(t.x, t.y, t.z);
	glRotatef(t.angle, t.rx, t.ry, t.rz);
	setMaterial(0.2, 0.2, 0.2, 0.3, 0.3, 0.3, 0.5, 0.5, 0.5, 64);
	glutSolidCube(t.size);
	glPopMatrix();
}

float dist(float ax, float ay, float az, float bx, float by, float bz) {
	float ans = 0;
	ans += pow(ax - bx, 2);
	ans += pow(ay - by, 2);
	ans += pow(az - bz, 2);
	ans = pow(ans, 0.5);
	return ans;
}

Object3D handleSphereCollision(Object3D t) {
	float n = dist(t.x, t.y, t.z, 0, 0, 0);
	if (n > sphere_size - (float)(t.size / 1.414)) {
		float nx = t.x / n, ny = t.y / n, nz = t.z / n;
		float ndotv = t.vx * nx + t.vy * ny + t.vz * nz;
		t.vx = t.vx - 2 * (ndotv)*nx;
		t.vy = t.vy - 2 * (ndotv)*ny;
		t.vz = t.vz - 2 * (ndotv)*nz;
	}
	return t;
}


tuple<Object3D, Object3D> handleObjectsCollision(Object3D t1, Object3D t2) {
	if (dist(t1.x, t1.y, t1.z, t2.x, t2.y, t2.z) < (float)(t1.size + t2.size) / 2) {
		if (t1.paused and !t2.paused) {
			t2.vx = -t2.vx;
			t2.vy = -t2.vy;
			t2.vz = -t2.vz;
		}
		if (t2.paused and !t1.paused) {
			t1.vx = -t1.vx;
			t1.vy = -t1.vy;
			t1.vz = -t1.vz;
		}
		if (!(t1.paused or t2.paused)) {
			float temp[3];
			temp[0] = t1.vx; temp[1] = t1.vy; temp[2] = t1.vz;
			t1.vx = t2.vx; t1.vy = t2.vy; t1.vz = t2.vz;
			t2.vx = temp[0]; t2.vy = temp[1]; t2.vz = temp[2];
		}
	}
	return make_tuple(t1, t2);
}

Object3D randomMotion(Object3D t) {
	t.x = t.x + t.vx;
	t.y = t.y + t.vy;
	t.z = t.z + t.vz;
	if (!t.paused) t.angle += 0.01;
	t = handleSphereCollision(t);
	glutPostRedisplay();
	return t;
}

Object3D cube1(cube_size, -.5);
Object3D cube2(cube_size, 0);
Object3D cube3(cube_size, .5);
Camera cam;

void display()
{
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(cam.x, cam.y, cam.z, cam.lax, cam.lay, cam.laz, cam.upx, cam.upy, cam.upz);

	glColor3f(.5, .5, .5);

	cube(cube1);
	cube1 = randomMotion(cube1);

	cube(cube2);
	cube2 = randomMotion(cube2);

	cube(cube3);
	cube3 = randomMotion(cube3);

	tie(cube1, cube2) = handleObjectsCollision(cube1, cube2);
	tie(cube2, cube3) = handleObjectsCollision(cube2, cube3);
	tie(cube3, cube1) = handleObjectsCollision(cube3, cube1);

	sphere();

	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitWindowSize(WINDOW_SIZE[0], WINDOW_SIZE[1]);
	glutInitWindowPosition(10, 10);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	glutCreateWindow("3 Bouncing Cubes");

	glEnable(GL_DEPTH_TEST);

	initLight();

	glClearColor(.1, .1, .1, 0);

	glutDisplayFunc(display);


	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();

	gluPerspective(90, 1, 0.5, 1000);

	glutMainLoop();
}