#include <stdlib.h>
#include <iostream>
#include <vector>	
#include <GL/freeglut.h>
#include <GL/glut.h>
using namespace std;


bool drawing = false;
GLint Width = 512, Height = 512;
vector<GLfloat> CurrentCol = { 1,0,0 };
int currentPolygonIndex = -1;
/*������� ����������� �������� ����*/
/*������� ������ �� �����*/
void Display(void)
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3ub(255, 0, 0);
	glFinish();
}

/* ������� ���������� ��� ��������� �������� ���� */
void Reshape(GLint w, GLint h)
{
	/*������������ ����*/
	Width = w; Height = h;
	/* ������������� ������� ������� ����������� */
	glViewport(0, 0, w, h);
	/* ��������������� �������� */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutPostRedisplay();
}

/* ������� ������������ ��������� c ���������� */
void Keyboard(unsigned char key, int x, int y)
{
#define ESCAPE '\033'
	if (key == ESCAPE) exit(0);
	
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("glut");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMainLoop();
}