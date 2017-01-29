#pragma once
#include"vessel_segmentation_system.h"
#define WindowWidth 400
#define WindowHeight 400
static GLint ImageWidth;
static GLint ImageHeight;
static GLint PixelLength;
static GLubyte* PixelData;
using namespace std;
#define N 40
int num = 10;
int cnt = 0;
int ratable[60];
int ra38[300];
int ra170190[100];
int ra5565[100];
int ra15[300];
int g_mousePosition[2] = { 0, 0 };
float g_centerRotate[] = { 0, 1, 1 };
bool g_leftMouseButtonDown = false;
GLUquadric *qObj;
GLuint theLeaf;
float R[N + 1], H[N + 1], k, p, q, m, alpha, z[N + 1], a = 6;
float Gauss(float min, float max)
{
	int a = rand(), b = rand(), c = rand(), d = rand();
	int x = a + b + c + d;
	double y = (x*sqrt(3)) / RAND_MAX - sqrt(12);
	double mid = (min + max) / 2;
	float z = (float)(mid + (max - mid)*y / (2 * sqrt(3)));
	return z;

}
int ra(int min, int max)
{

	return min + rand() % (max - min);

}
void setData(void)
{
	R[N] = 0.10*N / 40;
	H[N] = 0.4;
	z[N] = H[N];
	k = 0.8;
	p = 0.6;
	q = 0.94;
	m = 0.95;
	alpha = 45;
	for (int i = N - 1; i >= 0; i--)
	{
		R[i] = R[i + 1] * q;
		H[i] = H[i + 1] * m;
		z[i] = z[i + 1] * p;
	}
	H[N] = 0.03;
}
void init(void)
{
	GLfloat white_light[] = { 0.4, 0.5, 0.5, 1.0 };
	GLfloat light_position[] = { 4, 4, 4, 0 };
	qObj = gluNewQuadric();

	glClearColor(0.8, 0.8, 0.8, 0.0);
	glClearDepth(1.0);
	glShadeModel(GL_SMOOTH);

	glLightfv(GL_LIGHT0, GL_AMBIENT, white_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_BLEND);


	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
}
void drawTree(int n)
{

	GLfloat no_mat[] = { 0.8, 0.0, 0.0, 0.0 };
	GLfloat diffuse[] = { 0.8, 0.15, 0.1, 0.0 };
	GLfloat specular[] = { 0.8, 0.1, 0.1, 0.0 };
	if (n == 0)
	{

		float h = Gauss(0.4, 0.7);


		gluCylinder(qObj, R[n], k*R[n], H[n] * h, 20, 20);

		glTranslatef(0, 0, H[n] * h);
		glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		glMaterialf(GL_FRONT, GL_SHININESS, 5);
		glRotatef(40, 1, 0, 0);
		glRotatef(45, 0, 1, 0);
		glRotatef(90, 1, 0, 0);
		glScalef(1, 1, 0.1);

		return;
	}


	else
	{
		gluCylinder(qObj, R[n], k*R[n], H[n], 20, 20);

		glTranslatef(0, 0, H[n]);


		if ((ratable[n] == 0) && (n % 8 == 1))
		{
			glPushMatrix();
			glTranslatef(0, 0, -H[n] * 0.2);
			glRotatef(ra170190[n], 0, 0, 1);
			glRotatef(ra5565[n], 0, 1, 0);
			drawTree(n - 1);
			glPopMatrix();
		}

		if ((ratable[99 - n] == 0) && (n % 2 == 1))
		{
			glPushMatrix();
			glTranslatef(0, 0, -H[n] * 0.2);
			glRotatef(ra170190[99 - n], 0, 0, 1);
			glRotatef(ra5565[99 - n], 0, 1, 0);
			if (n>8)
				drawTree(n - 8);
			glPopMatrix();
		}

		glRotatef(ra15[cnt], 1, 0, 0);

		glRotatef(ra38[cnt], 0, -1, 0);
		glRotatef(ra15[299 - cnt], 0, 0, 1);
		cnt = (cnt + 1) % 300;
		drawTree(n - 1);

	}
}
void display(void)
{
	GLfloat no_mat[] = { 0.9, 0.1, 0.0, 1.0 };
	GLfloat tree_diffuse[] = { 0, 0.25, 0.1, 1.0 };
	GLfloat tree_specular[] = { 0, 0.1, 0.1, 1.0 };
	cnt = 0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawPixels(ImageWidth, ImageHeight,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, PixelData);
	glutSolidSphere(0.3*num / 40, 30, 30);


	glLoadIdentity();
	gluLookAt(0.0, -4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);

	glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, tree_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, tree_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 8);

	glRotatef(g_centerRotate[0], 1, 0, 0);
	glRotatef(g_centerRotate[1], 0, 1, 0);
	glRotatef(g_centerRotate[2], 0, 0, 1);
	glTranslatef(0, 0, -3.0);

	setData();
	glPushMatrix();
	glRotatef(-5, 0, 0, 1);
	glRotatef(-5, 0, 1, 0);
	drawTree(num);
	glPopMatrix();

	glPushMatrix();
	glRotatef(5, 0, 0, 1);
	glRotatef(5, 0, 1, 0);
	drawTree(num);
	glPopMatrix();

	glPushMatrix();
	glRotatef(0, 0, 0, 1);
	glRotatef(75, 0, 1, 0);
	drawTree(num);
	glPopMatrix();

	glPushMatrix();
	glRotatef(120, 0, 0, 1);
	glRotatef(75, 0, 1, 0);
	drawTree(num);
	glPopMatrix();

	glPushMatrix();
	glRotatef(240, 0, 0, 1);
	glRotatef(75, 0, 1, 0);
	drawTree(num);
	glPopMatrix();


	glutSwapBuffers();
}
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-a, a, -a, a, 0.01, 1.0e3);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void keyboardKeyPressed(unsigned char key, int x, int y)
{
	cnt = 0;
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'a':
	case 'A':
		g_centerRotate[2] -= 5;
		glutPostRedisplay();
		break;

	case 'G':
	case 'g':
		if (num<N)
			++num;
		glutPostRedisplay();
		break;

	case 'B':
	case 'b':
		if (num>5)
			--num;
		glutPostRedisplay();
		break;


	case 'd':
	case 'D':
		g_centerRotate[2] += 5;
		glutPostRedisplay();
		break;
	case 'w':
	case 'W':
		g_centerRotate[0] -= 5;
		glutPostRedisplay();
		break;
	case 's':
	case 'S':
		g_centerRotate[0] += 5;
		glutPostRedisplay();
		break;
	case 'r':
	case 'R':
		g_centerRotate[0] = 0;
		g_centerRotate[1] = 0;
		g_centerRotate[2] = 0;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}
void mouseMotionActive(int x, int y)
{
	cnt = 0;
	int mouseDelta[2] = { x - g_mousePosition[0], y - g_mousePosition[1] };

	if (g_leftMouseButtonDown)
	{
		g_centerRotate[1] += 1.0 * mouseDelta[0];
		g_centerRotate[0] += 1.0 * mouseDelta[1];
	}

	g_mousePosition[0] = x;
	g_mousePosition[1] = y;

	glutPostRedisplay();
}
void mouseMotionPassive(int x, int y)
{
	cnt = 0;
	g_mousePosition[0] = x;
	g_mousePosition[1] = y;
}
void mouseButtonPressed(int button, int state, int x, int y)
{
	cnt = 0;
	if (button == GLUT_LEFT_BUTTON)
		g_leftMouseButtonDown = (state == GLUT_DOWN);

	g_mousePosition[0] = x;
	g_mousePosition[1] = y;
}
//int vessel_grow()
//{
//	for (int i = 0; i<100; ++i)
//	{
//		ratable[i] = ra(0, 2);
//		ra15[i] = ra(-15, 15);
//		ra15[i + 100] = ra(-15, 15);
//		ra15[i + 200] = ra(-15, 15);
//		ra38[i] = ra(3, 8);
//		ra38[i + 100] = ra(3, 8);
//		ra38[i + 200] = ra(3, 8);
//		ra170190[i] = ra(170, 190);
//		ra5565[i] = ra(55, 65);
//	}
//	FILE* pFile = fopen("C:\\Users\\Yuxin\\Desktop\\background.bmp", "rb");
//	if (pFile == 0)exit(0);
//	fseek(pFile, 0x0012, SEEK_SET);
//	fread(&ImageWidth, sizeof(ImageWidth), 1, pFile);
//	fread(&ImageHeight, sizeof(ImageHeight), 1, pFile);
//	PixelLength = ImageWidth * 3;
//	while (PixelLength % 4 != 0)
//		++PixelLength;
//	PixelLength *= ImageHeight;
//	PixelData = (GLubyte*)malloc(PixelLength);
//	if (PixelData == 0)exit(0);
//	fseek(pFile, 54, SEEK_SET);
//	fread(PixelData, PixelLength, 1, pFile);
//	fclose(pFile);
//	int seed;
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
//	glutInitWindowSize(800, 700);
//	glutInitWindowPosition(50, 50);
//	glutCreateWindow("Blood Vessel");
//	init();
//	glutDisplayFunc(display);
//	glutReshapeFunc(reshape);
//	glutKeyboardFunc(keyboardKeyPressed);
//	glutMouseFunc(mouseButtonPressed);
//	glutMotionFunc(mouseMotionActive);
//	glutPassiveMotionFunc(mouseMotionPassive);
//	glutMainLoop();
//}
