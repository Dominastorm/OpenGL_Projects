#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <GL/glut.h>
#include <glm/glm/glm.hpp>
#include <random>
#include <cmath>
#include <tuple>

using namespace std;

namespace globals {
	int screen_width = 720;
	int screen_height = 720;

	const int window_start_position_x = 100;
	const int window_start_position_y = 100;

	const glm::vec4 clear_color(0.2f, 0.2f, 0.2f, 1.0f);

	std::vector <glm::vec3> taj_vertices;
	std::vector <std::tuple <int, int, int>> taj_faces;

	std::vector <glm::vec3> big_ben_vertices;
	std::vector <std::tuple <int, int, int>> big_ben_faces;

	std::vector <glm::vec3> laptop_bag_vertices;
	std::vector <std::tuple <int, int, int>> laptop_bag_faces;

	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_real_distribution <> distribution(0.0f, 1.0f);
}

const double pi = 3.14159265358979323846;
const float cube_size = .25, sphere_size = 1.2;
int WINDOW_SIZE[] = { 640, 640 };

random_device device;
mt19937 rng(device());
uniform_real_distribution<>distribution(0.0f, 1.0f);

float random_number() {
	return distribution(rng);
}

// Set the lighting for the material of the objects
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
	float x = 0.5 - random_number(), y = 0.5 - random_number(), z = 0.5 - random_number();
	float temp[3] = { 0,0,0 };
	bool should_pause = false;
	bool paused = false;
	float size = 0;
	float color[10] = { 0 };
	float vx = (-0.5f + random_number()) / 200, vy = (-0.5f + random_number()) / 200, vz = (-0.5f + random_number()) / 200;
	float angle = 0, rx = random_number() / 100, ry = random_number() / 100, rz = random_number() / 100;

	Object3D(float sz = 0, float stx = -0.5f + random_number()) {
		x = stx;
		size = sz;
		for (int _ = 0; _ < 10; ++_) {
			color[_] = random_number();
		}
	}
};

void read_mesh(const std::string& filename, float scale, std::vector <glm::vec3> &vertices, std::vector <std::tuple <int, int, int>> &faces) {
	std::ifstream file(filename);

	std::string line;
	std::stringstream ss;
	glm::vec3 v;

	while (not file.eof()) {
		std::getline(file, line);
		std::string type = line.substr(0, 2);

		if (type == "v ") {
			ss << line.substr(2);
			ss >> v.x >> v.y >> v.z;
			v.x *= scale;
			v.y *= scale;
			v.z *= scale;
			vertices.push_back(v);
			ss.clear();
		}
		else if (type == "f ") {
			int x, y, z;
			ss << line.substr(2);

			ss >> line;
			x = std::stoi(line.substr(0, line.find_first_of('/'))) - 1;
			ss >> line;
			y = std::stoi(line.substr(0, line.find_first_of('/'))) - 1;
			ss >> line;
			z = std::stoi(line.substr(0, line.find_first_of('/'))) - 1;

			faces.push_back({ x, y, z });
			ss.clear();
		}
	}
	file.close();
}

void cube(Object3D t) {
	glPushMatrix();
	glColor3f(t.color[0], t.color[1], t.color[2]);
	glTranslatef(t.x, t.y, t.z);
	glRotatef(t.angle, t.rx, t.ry, t.rz);
	setMaterial(0.2, 0.2, 0.2, 0.3, 0.3, 0.3, 0.5, 0.5, 0.5, 64);
	glutSolidCube(t.size);
	glPopMatrix();
}

void obj(Object3D t, std::vector <glm::vec3> vertices, std::vector <std::tuple <int, int, int>> faces) {
	int c = 0;
	glPushMatrix();
	glTranslatef(t.x, t.y, t.z);
	glRotatef(t.angle, t.rx, t.ry, t.rz);
	setMaterial(0.2, 0.2, 0.2, 0.3, 0.3, 0.3, 0.5, 0.5, 0.5, 64);
	glBegin(GL_TRIANGLES);
	for (auto& i : faces) {
		int x = std::get <0>(i);
		int y = std::get <1>(i);
		int z = std::get <2>(i);
		auto& v1 = vertices[x];
		auto& v2 = vertices[y];
		auto& v3 = vertices[z];
		glColor3f(t.color[c++ % 10], t.color[c++ % 10], t.color[c++ % 10]);
		glVertex3f(v1.x, v1.y, v1.z);
		glVertex3f(v2.x, v2.y, v2.z);
		glVertex3f(v3.x, v3.y, v3.z);
	}
	glEnd();
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

Object3D handlePause(Object3D t) {
	if (t.should_pause == true and t.paused == false) {
		t.temp[0] = t.vx; t.temp[1] = t.vy; t.temp[2] = t.vz;
		t.vx = 0; t.vy = 0; t.vz = 0;
		t.paused = true;
	}
	if (t.should_pause == false and t.paused == true) {
		t.vx = t.temp[0]; t.vy = t.temp[1]; t.vz = t.temp[2];
		t.paused = false;
	}
	return t;
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
	t = handlePause(t);
	glutPostRedisplay();
	return t;
}

Object3D laptop_bag(cube_size, -.5);
Object3D big_ben(cube_size, 0);
Object3D taj(cube_size, .5);
Camera cam;

void rotateCam(float x) {
	cam.theta += x;
	cam.x = cam.r * sin(cam.theta * pi / 180);
	cam.z = cam.r * cos(cam.theta * pi / 180);
}

void translateCamX(float x) {
	cam.x += x;
	cam.lax += x;
}

void translateCamY(float y) {
	cam.y += y;
	cam.lay += y;
}

void translateCamZ(float z) {
	cam.r = abs(cam.r);
	cam.r += z;
	cam.x = cam.r * sin(cam.theta * pi / 180);
	cam.z = cam.r * cos(cam.theta * pi / 180);
}

void resetCam() {
	Camera newCamera;
	cam = newCamera;
}


void handleKeyPress(unsigned char key, int cur_x, int cur_y) {
	switch (key) {
	case 'W':
	case 'w':
		translateCamY(-0.1);
		break;
	case 'A':
	case 'a':
		translateCamX(0.1);
		break;
	case 'S':
	case 's':
		translateCamY(0.1);
		break;
	case 'D':
	case 'd':
		translateCamX(-0.1);
		break;
	case 'E':
	case 'e':
		rotateCam(-10);
		break;
	case 'Q':
	case 'q':
		rotateCam(10);
		break;
	case 'X':
	case 'x':
		translateCamZ(-0.1);
		break;
	case 'Z':
	case 'z':
		translateCamZ(0.1);
		break;
	case 'R':
	case 'r':
		resetCam();
		break;
	case ' ':
		laptop_bag.should_pause = !laptop_bag.should_pause;
		big_ben.should_pause = !big_ben.should_pause;
		taj.should_pause = !taj.should_pause;
		break;
	case '1':
		laptop_bag.should_pause = !laptop_bag.should_pause;
		break;
	case '2':
		big_ben.should_pause = !big_ben.should_pause;
		break;
	case '3':
		taj.should_pause = !taj.should_pause;
		break;
	case '4':
		if (laptop_bag.paused) {
			laptop_bag.angle -= 1;
		}
		if (big_ben.paused) {
			big_ben.angle -= 1;
		}
		if (taj.paused) {
			taj.angle -= 1;
		}
		break;
	case '5':
		if (laptop_bag.paused) {
			laptop_bag.angle += 1;
		}
		if (big_ben.paused) {
			big_ben.angle += 1;
		}
		if (taj.paused) {
			taj.angle += 1;
		}
		break;
	}
	glutPostRedisplay();
}


bool selected = false, waspaused = false, dragaround = false;
int temp;

void currMousePos(int x, int y) { // handles moving selected object around
	float relx, rely, relz;
	if (dragaround) {
		relx = (sphere_size * 7.0f / 6.0f) * (float)cos(cam.theta * pi / 180) * (float)(x - WINDOW_SIZE[0] / 2) / WINDOW_SIZE[0];
		rely = -(sphere_size * 7.0f / 6.0f) * (float)(y - WINDOW_SIZE[1] / 2) / WINDOW_SIZE[1];
		relz = -(sphere_size * 7.0f / 6.0f) * (float)sin(cam.theta * pi / 180) * (float)(x - WINDOW_SIZE[0] / 2) / WINDOW_SIZE[0];
		switch (temp) {
		case 1:
			laptop_bag.x = relx;
			laptop_bag.y = rely;
			laptop_bag.z = relz;
			break;
		case 2:
			big_ben.x = relx;
			big_ben.y = rely;
			big_ben.z = relz;
			break;
		case 3:
			taj.x = relx;
			taj.y = rely;
			taj.z = relz;
			break;
		default:
			break;
		}
	}
}

void onMouse(int button, int state, int x, int y) {
	if (state != GLUT_DOWN) {
		return;
	}

	GLbyte color[4];
	GLfloat depth;
	GLuint index;

	glReadPixels(x, WINDOW_SIZE[1] - y - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
	glReadPixels(x, WINDOW_SIZE[1] - y - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
	glReadPixels(x, WINDOW_SIZE[1] - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

	printf("\nClicked on pixel: (%d, %d), color: %02hhx%02hhx%02hhx%02hhx, depth: %f, stencil index: %u.\n", x, y, color[0], color[1], color[2], color[3], depth, index);

	if (dragaround) {
		dragaround = false;
		switch (temp) {
		case 1:
			printf("LB has been released at (%.2f, %.2f, %.2f) world coordinates.");
			laptop_bag.should_pause = waspaused;
			break;
		case 2:
			big_ben.should_pause = waspaused;
			break;
		case 3:
			taj.should_pause = waspaused;
			break;
		default:
			break;
		}
	}

	switch (index) {
	case 1:
		temp = 1;
		printf("selected lb");
		waspaused = laptop_bag.paused;
		laptop_bag.should_pause = true;
		dragaround = true;
		break;
	case 2:
		temp = 2;
		waspaused = big_ben.paused;
		big_ben.should_pause = true;
		dragaround = true;
		break;
	case 3:
		temp = 3;
		waspaused = taj.paused;
		taj.should_pause = true;
		dragaround = true;
		break;
	}
}

void display()
{
	using namespace globals;

	glClearStencil(0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	gluLookAt(cam.x, cam.y, cam.z, cam.lax, cam.lay, cam.laz, cam.upx, cam.upy, cam.upz);

	glColor3f(.5, .5, .5);

	glStencilFunc(GL_ALWAYS, 1, -1);
	obj(laptop_bag, laptop_bag_vertices, laptop_bag_faces);
	laptop_bag = randomMotion(laptop_bag);

	glStencilFunc(GL_ALWAYS, 2, -1);
	obj(big_ben, big_ben_vertices, big_ben_faces);
	big_ben = randomMotion(big_ben);

	glStencilFunc(GL_ALWAYS, 3, -1);
	obj(taj, taj_vertices, taj_faces);
	taj = randomMotion(taj);

	tie(laptop_bag, big_ben) = handleObjectsCollision(laptop_bag, big_ben);
	tie(big_ben, taj) = handleObjectsCollision(big_ben, taj);
	tie(taj, laptop_bag) = handleObjectsCollision(taj, laptop_bag);

	glStencilFunc(GL_ALWAYS, 4, -1);
	sphere();

	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	read_mesh("res/taj_mahal.obj", 0.005, globals::taj_vertices, globals::taj_faces);
	read_mesh("res/big_ben.obj", 0.1, globals::big_ben_vertices, globals::big_ben_faces);
	read_mesh("res/laptop_bag.obj", 0.5, globals::laptop_bag_vertices, globals::laptop_bag_faces);

	glutInit(&argc, argv);

	glutInitWindowSize(WINDOW_SIZE[0], WINDOW_SIZE[1]);
	glutInitWindowPosition(10, 10);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STENCIL);

	glutCreateWindow("3 Bouncing Objects");

	glEnable(GL_DEPTH_TEST);

	initLight();

	glClearColor(.1, .1, .1, 0);

	glutDisplayFunc(display);

	glutKeyboardFunc(handleKeyPress);

	glutMouseFunc(onMouse);

	glutPassiveMotionFunc(currMousePos);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(90, 1, 0.5, 1000);
	glEnable(GL_DEPTH_TEST);

	glutMainLoop();
}