
#include <stdlib.h>
#include <iostream>
#include <vector>	
#include <GL/freeglut.h>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define KEY_ESCAPE 27
using namespace std;

typedef glm::vec3 vec;

float rotate_x = 0, rotate_y = 0;


class Primal
{
public:
	vector<vec> points;
	vec norm;
	void Draw()
	{
		//glTranslatef(0,0,7);
		glBegin(GL_TRIANGLES);
		glNormal3fv(glm::value_ptr(norm));
		for (size_t i = 0; i < points.size(); i++)
		{
			glVertex3fv(glm::value_ptr(points[i]));
		}
		glEnd();
	}

	vec Central()
	{
		int i = 0;
		vec result = vec(0);
		for (i = 0; i < points.size(); i++)
		{
			result += points[i];
		}
		result = result / (float)i;
		return result;
	}

	Primal(vector<vec> points)
	{
		this->points = points;
	}
	Primal() {

	}
	~Primal() {

	}
};

class Edition
{
public:
	vec position;
	vec way;
	vector<Primal*> primals;
	void Build(Primal* section)
	{
		glm::mat4 mat = glm::mat4(1.0f);
		vec norm = glm::normalize(way);
		vec rotationAxe = -glm::cross({ 0,0,1 }, norm);
		if (way != vec{0, 0, 1})
		{
			rotationAxe = glm::normalize(rotationAxe);
		}
		float angle = acosf(glm::dot({ 0,0,1 }, norm));
		mat = glm::rotate(mat, angle, rotationAxe);
		for (size_t i = 0; i < section->points.size(); i++)
		{
			glm::vec4 tmp = { section->points[i],1 };
			section->points[i] = mat * tmp;
		}
		section->norm = norm;
		float* tmp = glm::value_ptr(section->points[2]);
		primals.push_back(section);
	}
	
	void Draw()
	{
		for (size_t i = 0; i < primals.size(); i++)
		{
			primals[i]->Draw();
		}
	}

	Edition(vec pos, vec way)
	{
		position = pos;
		this->way = way;
	}
	Edition() {}
	~Edition() {}
	
};

typedef struct {
	int width;
	int height;
	char* title;

	float field_of_view_angle;
	float z_near;
	float z_far;
} glutWindow;

glutWindow win;

Edition* toDraw;

void display() {
    GLfloat params[16];

    //  Clear screen and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Reset transformations
    glMatrixMode(GL_MODELVIEW);

	vec center = toDraw->primals[0]->Central();
	glBegin(GL_LINES);
	glColor3f(0, 0, 0);
	glVertex3fv(glm::value_ptr(center));
	center += toDraw->primals[0]->norm;
	glVertex3fv(glm::value_ptr(center));
	glEnd();
	glLoadIdentity();

	glColor3f(0, 1, 0);
	toDraw->Draw();

	glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
	  // Top face (y = 1.0f)
	  // Define vertices in counter-clockwise (CCW) order with normal pointing out
	glColor3f(0.0f, 1.0f, 0.0f);     // Green
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	// Bottom face (y = -1.0f)
	glColor3f(1.0f, 0.5f, 0.0f);     // Orange
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	// Front face  (z = 1.0f)
	glColor3f(1.0f, 0.0f, 0.0f);     // Red
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	// Back face (z = -1.0f)
	glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	// Left face (x = -1.0f)
	glColor3f(0.0f, 0.0f, 1.0f);     // Blue
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	// Right face (x = 1.0f)
	glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();  // End of drawing color-cube


    glFlush();
    glutSwapBuffers();

}


void initialize()
{
	glMatrixMode(GL_PROJECTION);	// select projection matrix
	//glViewport(0, 0, win.width, win.height);									// set the viewport	
	glLoadIdentity();															// reset projection matrix
	GLfloat aspect = (GLfloat)win.width / win.height;
	glOrtho(-10,10,-10,10, 1000,3);
	//gluPerspective(win.field_of_view_angle, aspect, win.z_near, win.z_far);		
	glTranslatef(0.0f, 0.0f, -10.0f);
	glRotatef(90, 1, 0, 0);
	// set up a perspective projection matrix
	glMatrixMode(GL_MODELVIEW);													// specify which matrix is the current matrix
	//glShadeModel(GL_SMOOTH);
	//glClearDepth(1.0f);														// specify the clear value for the depth buffer
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);						// specify implementation-specific hints
	glClearColor(1.0, 1.0, 1.0, 1.0);											// specify clear values for the color buffers								
}


void specialKeys(int key, int x, int y) {

    //  Right arrow - increase rotation by 5 degree
    if (key == GLUT_KEY_RIGHT)
        rotate_y += 5;

    //  Left arrow - decrease rotation by 5 degree
    else if (key == GLUT_KEY_LEFT)
        rotate_y -= 5;

    else if (key == GLUT_KEY_UP)
        rotate_x += 5;

	else if (key == GLUT_KEY_DOWN)
	{
		rotate_x -= 5;
		glMatrixMode(GL_PROJECTION);
		glRotatef(-5, 1, 0, 0);
		glMatrixMode(GL_MODELVIEW);													// specify which matrix is the current matrix

	}

    //  Request display update
    glutPostRedisplay();

}

int main(int argc, char** argv)
{
	Primal* sec = new Primal(
		{
			{-1,0,0},
			{0,1,0},
			{1,0,0}
		}
	);
	Edition myEdition({ 0,0,0 }, { 1,0,1 });
	myEdition.Build(sec);
	toDraw = &myEdition;
	vec tmp = glm::vec4{ 1,1,1,2 };
	float* f = glm::value_ptr(tmp);
	// set window values
	win.width = 640;
	win.height = 480;
	win.field_of_view_angle = 45;
	win.z_near = 1.0f;
	win.z_far = 500.0f;

	// initialize and run program
	glutInit(&argc, argv);                                      // GLUT initialization
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);  // Display Mode
	glutInitWindowSize(win.width, win.height);					// set window size
	glutCreateWindow(win.title);								// create Window
    glEnable(GL_DEPTH_TEST);
	glutDisplayFunc(display);									// register Display Function
	glutIdleFunc(display);									// register Idle Function
    glutSpecialFunc(specialKeys);							// register Keyboard Handler
	initialize();
	glutMainLoop();												// run GLUT mainloop
	return 0;
}