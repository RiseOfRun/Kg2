#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <vector>
#include <fstream>
#include <stack>
#include <math.h>
#include "library.h"
#include "GL\glut.h"
#include <soil.h> // ����� Nuget ������������� ���������� ��� �������� �������

int Width = 800, Height = 800;

using namespace std;

vector<Point> points;

vector<Point> true_array_point;

typedef float type;

type scale = 1;

void Display(void) {

    glClearColor(1, 1, 1, 1);

    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();


    //	��������� ���� ���������

    draw_axis(Width, Height, scale);

    //

    //	������ �����, � ������� ����� ������������� ������������ true_array_point.clear();

    sort_points(true_array_point); // ���������� ����� ����� ��� X ����� ��������

    func_init_true_points(points, true_array_point, Width, Height); // �������������� ���-

    �� ������������ ������ ���� int size;

    if ((size = points.size()) != 0) {
        glPointSize(6);

        glBegin(GL_POINTS);

        for (int i = 0; i < size; i++) {
            glColor3ub(0, 0, 0);

            glVertex2i(true_array_point[i].x * scale + Width / 2, true_array_point[i].y * scale + Height / 2);
        }

        glEnd();

    }

    //	


    //	������������

    interpolation_func(points, true_array_point, Width, Height, scale);

    //



    6

        glFinish();

}

void Reshape(GLint w, GLint h)	//������� ��������� �������� ����

{

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    glOrtho(0, w, 0, h, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

}

enum keys

{

    KeyW, KeyA, KeyS, KeyD,

    KeyZ, KeyX, KeyC, KeyV, KeyB, KeyN,

    KeyO, KeyP, Empty

};

void Keyboard(unsigned char key, int xx, int yy)

{

    char message[50];

    if (key == '-') {

        if (n_power_interpolation > 1)

            n_power_interpolation--;

        else {

            sprintf(message, "Power interpolation can't be 0 !!!"); glutSetWindowTitle(message);

            return;

        }

    }

    if (key == '=') {

        n_power_interpolation++;

    }

    int size = points.size();

    if (size > 0) {

        switch (key) {

        case 'w':

            for (int i = size - 1; i > -1; i--)

                points[i].y += 5;

            break;

        case 's':

            for (int i = size - 1; i > -1; i--)

                points[i].y -= 5;

            break;

        case 'a':

            for (int i = size - 1; i > -1; i--)

                points[i].x -= 5;

            break;

        case 'd':

            for (int i = size - 1; i > -1; i--)

                points[i].x += 5;

            break;

        default:

            break;

        }

    }


    7

        //	���������� �������� if (key == '1')
        if (scale - 0.05 >= 0.2) scale -= 0.05;

    //	���������� ��������

    if (key == '2')

        if (scale + 0.05 <= 1.50)

            scale += 0.05;

    //	����� �������� if (key == '3')
    scale = 1;


    sprintf(message, "Power interpolation %d", n_power_interpolation);

    glutSetWindowTitle(message);

    glutPostRedisplay();

}

void Mouse(int button, int state, int x, int y) {
    if (state != GLUT_DOWN) return;


    if (button == GLUT_LEFT_BUTTON) {

        if (scale == 1)

            points.push_back(Point(x, Height - y));

        else {

            system("echo You need to throw off the scale value (press '3' for the action) for adding points! & pause");
        }

    }

    //	������� ����� ��������� � ��������� ������� if (button == GLUT_RIGHT_BUTTON) {

    if (points.size() != 0) {
        int k = 0;
        Point val(1000, 1000);

        for (int i = 0; i < points.size(); i++)

            if (distance(Point(x, Height - y), val) > distance(points[i], Point(x, Height

                - y))) {

                val = points[i];

                k = i;

            }

        points.erase(points.begin() + k);

    }

}

glutPostRedisplay();

}

int main(int argc, char* argv[])

{

    if (!get_info()) n_power_interpolation = 2;

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB);

    glutInitWindowSize(Width, Height);

    glutCreateWindow("������� ���� ���� �����:");

    glutDisplayFunc(Display);

    glutReshapeFunc(Reshape);

    glutKeyboardFunc(Keyboard);

    glutMouseFunc(Mouse);


    8

        glutMainLoop();

    return 1;

}
