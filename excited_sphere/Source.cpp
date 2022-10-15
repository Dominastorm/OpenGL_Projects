#include <GL/glut.h>
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cmath>


int WINDOW_SIZE[] = { 1000, 1000 };
int sphere_radius = 1.2;


void sphere(float radius = sphere_radius) {
	glPushMatrix();
	glColor3f(.5, .5, .5);
	glutWireSphere(radius, 18, 18);
	glPopMatrix();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	sphere();

	glutSwapBuffers();
}

void initLight() {
	GLfloat ambient[] = { 0.3, 0.3, 0.3 };
	GLfloat diffuse[] = { 0.5, 0.5, 0.5 };
	GLfloat specular[] = { 1, 1, 1 };
	GLfloat position[] = { -2, 0.5, 4, 1 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_COLOR_MATERIAL);
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);

	glutInitWindowSize(WINDOW_SIZE[0], WINDOW_SIZE[1]);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	glutCreateWindow("Excited Sphere");
	glEnable(GL_DEPTH_TEST);

	//initLight();

	glClearColor(.1, .1, .1, 0);

	glutDisplayFunc(display);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Orthographic view
	//glOrtho(-3, 3, -3, 3, -3, 3);

	//Perspective view
	gluPerspective(90, 1, 0.5, 1000);

	glutMainLoop();
}