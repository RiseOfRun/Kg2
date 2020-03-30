
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

float rotate_x = 30, rotate_y = 30;

class Primal
{
public:
	vector<vec> points;
	vec norm;
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


void display() {
    GLfloat params[16];

    //  Clear screen and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float f[16];
    // Reset transformations
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, params);
    glm::mat4 matrix = glm::make_mat4(params);
   
    vec test;
    test = { 1,2,3 };
    float* test2;
    test2 = glm::value_ptr(test);
    matrix = glm::rotate(matrix,glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glMultMatrixf(glm::value_ptr(matrix));
    //float* test = glm::value_ptr(test);
   // glPushMatrix();

    //glBegin(GL_POLYGON);

    //glColor3f(1.0, 0.0, 0.0);     glVertex3f(0.5, -0.5, -0.5);      // P1 is red
    //glColor3f(0.0, 1.0, 0.0);     glVertex3f(0.5, 0.5, -0.5);      // P2 is green
    //glColor3f(0.0, 0.0, 1.0);     glVertex3f(-0.5, 0.5, -0.5);      // P3 is blue
    //glColor3f(1.0, 0.0, 1.0);     glVertex3f(-0.5, -0.5, -0.5);      // P4 is purple

    //glEnd();

    //// White side - BACK
    //glBegin(GL_POLYGON);
    //glColor3f(1.0, 1.0, 1.0);
    //glVertex3f(0.5, -0.5, 0.5);
    //glVertex3f(0.5, 0.5, 0.5);
    //glVertex3f(-0.5, 0.5, 0.5);
    //glVertex3f(-0.5, -0.5, 0.5);
    //glEnd();

    //// Purple side - RIGHT
    //glBegin(GL_POLYGON);
    //glColor3f(1.0, 0.0, 1.0);
    //glVertex3f(0.5, -0.5, -0.5);
    //glVertex3f(0.5, 0.5, -0.5);
    //glVertex3f(0.5, 0.5, 0.5);
    //glVertex3f(0.5, -0.5, 0.5);
    //glEnd();

    //// Green side - LEFT
    //glBegin(GL_POLYGON);
    //glColor3f(0.0, 1.0, 0.0);
    //glVertex3f(-0.5, -0.5, 0.5);
    //glVertex3f(-0.5, 0.5, 0.5);
    //glVertex3f(-0.5, 0.5, -0.5);
    //glVertex3f(-0.5, -0.5, -0.5);
    //glEnd();

    //// Blue side - TOP
    //glBegin(GL_POLYGON);
    //glColor3f(0.0, 0.0, 1.0);
    //glVertex3f(0.5, 0.5, 0.5);
    //glVertex3f(0.5, 0.5, -0.5);
    //glVertex3f(-0.5, 0.5, -0.5);
    //glVertex3f(-0.5, 0.5, 0.5);
    //glEnd();
    //// Red side - BOTTOM
    //glBegin(GL_POLYGON);
    //glColor3f(1.0, 0.0, 0.0);
    //glVertex3f(0.5, -0.5, -0.5);
    //glVertex3f(0.5, -0.5, 0.5);
    //glVertex3f(-0.5, -0.5, 0.5);
    //glVertex3f(-0.5, -0.5, -0.5);
    //glEnd();

    // Other Transformations
    // glTranslatef( 0.1, 0.0, 0.0 );      // Not included
    // glRotatef( 180, 0.0, 1.0, 0.0 );    // Not included

    // Rotate when user changes rotate_x and rotate_y
    //glGetFloatv(GL_MODELVIEW_MATRIX, params);
    //glRotatef(rotate_x, 1.0, 0.0, 0.0);
    //glGetFloatv(GL_MODELVIEW_MATRIX, params);
    //glRotatef(rotate_y, 0.0, 1.0, 0.0);
    //glGetFloatv(GL_MODELVIEW_MATRIX, params);


    // Other Transformations
    // glScalef( 2.0, 2.0, 0.0 );          // Not included

   //Multi-colored side - FRONT
    glBegin(GL_POLYGON);

    glColor3f(1.0, 0.0, 0.0);     glVertex3f(0.5, -0.5, -0.5);      // P1 is red
    glColor3f(0.0, 1.0, 0.0);     glVertex3f(0.5, 0.5, -0.5);      // P2 is green
    glColor3f(0.0, 0.0, 1.0);     glVertex3f(-0.5, 0.5, -0.5);      // P3 is blue
    glColor3f(1.0, 0.0, 1.0);     glVertex3f(-0.5, -0.5, -0.5);      // P4 is purple

    glEnd();


    // White side - BACK
    glBegin(GL_POLYGON);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(0.5, -0.5, 0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glVertex3f(-0.5, -0.5, 0.5);
    glEnd();

    // Purple side - RIGHT
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 1.0);
    glVertex3f(0.5, -0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(0.5, -0.5, 0.5);
    glEnd();

    // Green side - LEFT
    glBegin(GL_POLYGON);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(-0.5, -0.5, 0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glVertex3f(-0.5, 0.5, -0.5);
    glVertex3f(-0.5, -0.5, -0.5);
    glEnd();

    // Blue side - TOP
    glBegin(GL_POLYGON);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(-0.5, 0.5, -0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glEnd();
    // Red side - BOTTOM
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.5, -0.5, -0.5);
    glVertex3f(0.5, -0.5, 0.5);
    glVertex3f(-0.5, -0.5, 0.5);
    glVertex3f(-0.5, -0.5, -0.5);
    glEnd();

  
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
	glTranslatef(0.0f, 0.0f, -3.0f);
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
        rotate_x -= 5;

    //  Request display update
    glutPostRedisplay();

}

int main(int argc, char** argv)
{
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