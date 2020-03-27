#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>	
#include <GL/freeglut.h>
#include <math.h>
#include <windows.h>
#include <GL/glut.h>
using namespace std;
class Triangle
{
public:
	vector <GLfloat> x;
	vector <GLfloat> y;
	vector <GLfloat> z;
	Triangle(vector <GLfloat> x1, vector <GLfloat> y1, vector <GLfloat> z1)
	{
		x = x1;
		y = y1;
		z = z1;
	}
	Triangle() {};
	~Triangle() {};
};

vector <GLfloat> Norm(vector <GLfloat> x, vector <GLfloat> y, vector <GLfloat> z)
{
	vector<GLfloat> v = { y[0]-x[0], y[1] - x[1], y[2] - x[2] };
	vector<GLfloat> w = { z[0] - x[0], z[1] - x[1], z[2] - x[2] };

	vector<GLfloat> norm = {
		v[1] * w[2] - v[2] * w[1],
		v[2] * w[0] - v[0] * w[2],
		v[0] * w[1] - w[1] * v[0]
	};
	return norm;
}
//class Vector3f
//{
//public:
//	float x, y, z;
//
//	Vector3f() {};							//конструктор по умолчанию
//	Vector3f(float _x, float _y, float _z)	//конструктор
//	{
//		x = _x;
//		y = _y;
//		z = _z;
//	}
//
//	//Переопределение оператора +
//	Vector3f operator + (Vector3f _Vector)
//	{
//		return Vector3f(_Vector.x + x, _Vector.y + y, _Vector.z + z);
//	}
//	//Переопределение оператора -
//	Vector3f operator - (Vector3f _Vector)
//	{
//		return Vector3f(x - _Vector.x, y - _Vector.y, z - _Vector.z);
//	}
//
//	//Переопределение оператора *
//	Vector3f operator * (float num)
//	{
//		return Vector3f(x * num, y * num, z * num);
//	}
//
//	//Переопределение оператора /
//	Vector3f operator / (float num)
//	{
//		return Vector3f(x / num, y / num, z / num);
//	}
//};
Triangle * wannaDraw;
bool drawing = false;
GLint Width = 512, Height = 512;
vector<GLfloat> CurrentCol = { 1,0,0 };
int currentPolygonIndex = -1;
/*Функция обрабатывет действия мыши*/
/*Функция вывода на экран*/
void Display(void)
{

	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	int left = (Width - 200) / 2;   // левая граница
	int right = left + 200;          // правая граница
	int bottom = (Height - 200) / 2;  // нижняя граница
	int top = bottom + 200;        // верхняя граница
	glColor3ub(0, 255, 0);
	for (float i = -50; i <= 50; i += 1)
	{
		glBegin(GL_LINES);
		// Ось Х
		glVertex3f(-50, 0, i);
		glVertex3f(50, 0, i);

		// Ось Z
		glVertex3f(i, 0, -50);
		glVertex3f(i, 0, 50);
		glEnd();
	}
	//glBegin(GL_TRIANGLES);
	//vector<GLfloat> norm = Norm(wannaDraw->x, wannaDraw->y, wannaDraw->z);
	////glNormal3f(norm[0],norm[1],norm[2]);
	//glColor3ub(0, 255, 0);
	//glVertex3f(100, 200,0);
	//glVertex3f(200,200,0);
	//glVertex3f(200,100,0);
	//glEnd;
	glBegin(GL_TRIANGLES);
	glColor3ub(255, 0, 0);
	glVertex3f(100,200,0);	glVertex3f(200,200,-50);
	glVertex3f(200,100,0);
	glEnd();

	glFinish();
}

/* Функция вызывается при изменении размеров окна */
void Reshape(GLint w, GLint h)
{
	/*Отображаемый цвет*/
	Width = w; Height = h;
	/* устанавливаем размеры области отображения */
	glViewport(0, 0, w, h);
	/* ортографическая проекция */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutPostRedisplay();
}

/* Функция обрабатывает сообщения c клавиатуры */
void Keyboard(unsigned char key, int x, int y)
{
#define ESCAPE '\033'
	if (key == ESCAPE) exit(0);
	
}

void main(int argc, char* argv[])
{
	wannaDraw = new Triangle({0,0,0}, {0,255,0}, {255,255,0});
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("glut");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMainLoop();
}