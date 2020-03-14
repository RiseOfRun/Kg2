#include <stdlib.h>
#include <iostream>
#include <vector>
#include <freeglut.h>
#include <glut.h>
using namespace std;


bool drawing = false;
GLint Width = 512, Height = 512;
vector<GLfloat> CurrentCol = { 1,0,0 };
int currentPolygonIndex = -1;
class Poly
{
public:
	void Draw()
	{
		glBegin(GL_POLYGON);
		glColor3fv(Col.data());
		for (size_t i = 0; i < Points.size(); i++)
		{
			glVertex2i(Points[i][0], Points[i][1]);
		}
		glEnd();
	}
	void Prediction()
	{
		glBegin(GL_LINE_STRIP);
		glColor3fv(Col.data());
		for (size_t i = 0; i < Points.size(); i++)
		{
			glVertex2i(Points[i][0], Points[i][1]);
		}
		glEnd();
	}

	void Contour()
	{
		glLineWidth(1);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_LINE_STIPPLE); // разрешаем рисовать
							// прерывистую линию
		glLineStipple(2, 58360);
		glBegin(GL_LINE_LOOP);
		glColor3f(0, 0, 0);
		glLineWidth(5);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_LINE_STIPPLE); // разрешаем рисовать
							// прерывистую линию
		glLineStipple(2, 58360);    // устанавливаем маску
								   // пояснения см. ниже
		for (size_t i = 0; i < Points.size(); i++)
		{
			glVertex2i(Points[i][0], Points[i][1]);
		}
		glEnd();
		glDisable(GL_LINE_STIPPLE);
	}
	vector<vector<GLint>>Points;
	vector<GLfloat> Col = { 1,0,0 };
};

Poly ColorType;
Poly* currentPoly = new Poly();
vector<Poly> primitives(0);

/*Функция для окончания рисования текущего объекта*/
void StopDrawing()
{
	drawing = false;
	primitives.push_back(*currentPoly);
	delete currentPoly;
	currentPoly = &primitives.back();
	glutPostRedisplay();
}

/*Функция обрабатывет действия мыши*/
void Mouse(int button, int state, int x, int y)
{
	if (state != GLUT_DOWN) return;		//если кнопка мыши не была отпущена
	if (button == GLUT_LEFT_BUTTON)
	{
		if (!drawing)
		{
			currentPoly = new Poly();
			drawing = true;
		}
		vector<GLint> cord = { x,Height - y };

		currentPoly->Points.push_back(cord);
	}
}

/*Функция вывода на экран*/
void Display(void)
{
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3ub(255, 0, 0);
	if (drawing)
	{
		currentPoly->Col = CurrentCol;
		currentPoly->Prediction();
	}
	//currentPoly->Prediction();
	for (size_t i = 0; i < primitives.size(); i++)
	{
		primitives[i].Draw();
	}
	ColorType.Draw();
	if (!drawing && currentPoly != NULL)
	{
		currentPoly->Contour();
	}
	glFinish();
}

/* Функция вызывается при изменении размеров окна */
void Reshape(GLint w, GLint h)
{
	/*Отображаемый цвет*/
	Width = w; Height = h;
	ColorType.Points = {
		{Width,Height},
		{Width - 10,Height},
		{Width - 10,Height - 10},
		{Width,Height - 10}
	};
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
void
Keyboard(unsigned char key, int x, int y)
{
#define ESCAPE '\033'
	if (key == ESCAPE) exit(0);
	switch (key)
	{
	case 'q':
		if (!drawing)
			currentPoly = NULL;
		glutPostRedisplay();
		currentPolygonIndex = -1;
		break;

	case '[':
		if (drawing)
		{
			return;
		}
		if (currentPoly == NULL || !currentPolygonIndex)
		{
			currentPoly = &primitives.back();
			currentPolygonIndex = primitives.size() - 1;
		}
		else
		{
			currentPoly = &primitives[currentPolygonIndex - 1];
			currentPolygonIndex--;
		}

		glutPostRedisplay();
		break;
	case ']':
		if (drawing)
		{
			return;
		}
		if (currentPolygonIndex == primitives.size() - 1 || currentPolygonIndex == -1)
		{
			currentPoly = &primitives[0];
			currentPolygonIndex = 0;
		}
		else
		{
			currentPoly = &primitives[currentPolygonIndex + 1];
			currentPolygonIndex++;
		}

		glutPostRedisplay();
		break;
	case 'w':
		if (!drawing)
		{
			for (size_t i = 0; i < currentPoly->Points.size(); i++)
			{
				currentPoly->Points[i][1] += 10;
			}
		}
		glutPostRedisplay();
		break;
	case 's':
		if (!drawing)
		{
			for (size_t i = 0; i < currentPoly->Points.size(); i++)
			{
				currentPoly->Points[i][1] -= 10;
			}
		}
		glutPostRedisplay();
		break;
	case 'a':
		if (!drawing)
		{
			for (size_t i = 0; i < currentPoly->Points.size(); i++)
			{
				currentPoly->Points[i][0] -= 10;
			}
		}
		glutPostRedisplay();
		break;
	case 'd':
		if (!drawing)
		{
			for (size_t i = 0; i < currentPoly->Points.size(); i++)
			{
				currentPoly->Points[i][0] += 10;
			}
		}
		glutPostRedisplay();
		break;
	case 'e':
		currentPoly->Col = CurrentCol;
		StopDrawing();
		currentPolygonIndex = primitives.size() - 1;
		glutPostRedisplay();
		break;
	case 'c':
		if (!drawing)
			return;
		delete currentPoly;
		currentPoly = new Poly();
		glutPostRedisplay();
		break;
	case 'g':
		CurrentCol = { 0,1,0 };
		if (currentPoly != NULL && !drawing)
		{
			currentPoly->Col = CurrentCol;
		}
		ColorType.Col = CurrentCol;
		glutPostRedisplay();
		break;
	case 'b':
		CurrentCol = { 0,0,1 };
		if (currentPoly != NULL && !drawing)
		{
			currentPoly->Col = CurrentCol;
		}
		ColorType.Col = CurrentCol;
		glutPostRedisplay();
		break;
	case 'r':
		CurrentCol = { 1,0,0 };
		if (currentPoly != NULL && !drawing)
		{
			currentPoly->Col = CurrentCol;
		}
		ColorType.Col = CurrentCol;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void main(int argc, char* argv[])
{
	ColorType.Col = CurrentCol;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("Draw");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutMainLoop();
}