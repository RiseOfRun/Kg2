
#include <stdlib.h>
#include <iostream>
#include <vector>	
#include <fstream>
#include <math.h>
#include <algorithm>
#include <unordered_map>
#include <GL/freeglut.h>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define KEY_ESCAPE 27
using namespace std;
typedef glm::vec3 vec;

glm::vec4 pos = { 0, 5, 10, 0 };
float rotate_x = 0, rotate_y = 0;
float old_x = -1;
float old_y = -1;

float  PI_F = 3.14159265358979f;

struct color
{
	unsigned char R = 0, G = 0, B = 0;
};

vec operator * (color vec, float num) {
	return { vec.R/255. * num, vec.G/255. * num, vec.B/255 * num };
}

struct Material 
{
	Material(const double& ka, const double& kd, const double& ks, int r, int g, int b, const double& spec) :ka(ka), kd(kd), ks(ks), specularExponent(spec) { Color.R = r, Color.G = g, Color.B = b; }
	Material() {}
	double ka;// фоновое 
	double kd;// диффузное
	double ks;//блики
	color Color;
	double specularExponent;
};


struct
{
	bool smoothNormals = false, light = true, viewNormals = true, filled = true, viewAxes = true;
	int cameraMode = 1;
} viewsettings;

struct light
{
	vec pos = { -5,-5,-5 };
	float intense = 1;
} Light_0;

struct camera
{
	vec position = { 0,0,0 };
	float fov = 60;
	float N = 1;
	float AspectR = 3.0 / 4.0;
	vec w = { 0,0,1 }, v = { -1, 0, 0 }, u = { 0,-1,0 };
	int width = 640, high = 480;
};


camera mainCamera;

//утилити функции, для задания хэша и сравнений
struct KeyFuncs
{
	size_t operator()(const vec& k)const
	{
		return std::hash<float>()(k.x) ^ std::hash<float>()(k.y) ^ std::hash<float>()(k.z);
	}

	bool operator()(const vec& a, const vec& b)const
	{
		return a.x == b.x && a.y == b.y && a.z == b.z;
	}
};

class Shape
{
public:
	virtual float GetRayDistance(vec e, vec d)
	{
		return 0;
	}

private:

};


class Sphere : Shape
{
public:
	Material essense;
	vec C;
	float r;
	
	float GetRayDistance(vec e, vec d)
	{
		vec s = e-C;
		float a = glm::dot(d,d);
		float b = 2 * glm::dot(s, d);
		float c = glm::dot(s, s)-r*r;
		
		float D = b * b - 4 * a * c;

		if (D < 0)
		{
			return 0;
		}


		float t = (-b-sqrt(D))/a;
		if (t<1)
		{
			return t = (-b + sqrt(D)) / a;
		}
		return t;
	}

	Sphere(vec C, float r)
	{
		this->C = C;
		this->r = r;
	}

	Sphere()
	{
	}
	~Sphere()
	{
	}

private:

};

Sphere MySphere(vec(0,0,3), 1);
typedef struct {
	int width;
	int height;
	char* title;

	float field_of_view_angle;
	float z_near;
	float z_far;
} glutWindow;

glutWindow win;



void setCamera()
{
	glMatrixMode(GL_PROJECTION);	// select projection matrix
	//glViewport(0, 0, win.width, win.height);									// set the viewport	
	glLoadIdentity();															// reset projection matrix
	GLfloat aspect = (GLfloat)win.width / win.height;

	if (viewsettings.cameraMode == 1)
	{
		glOrtho(-10, 10, -10, 10, 0, 1000);
	}
	else
	{
		gluPerspective(win.field_of_view_angle, aspect, win.z_near, win.z_far);

	}

	glTranslatef(0.0f, 0.0f, -20.0f);
	glMatrixMode(GL_MODELVIEW);
}

vec Reflect(vec L, vec N) {  //отраженный луч h
	return L - glm::cross(2.f*glm::cross(L, N), N);
}

vec Fong(vec e, vec d, float t)
{
	float diffuseLightIntensity = 0, specularLightIntensity = 0, ambientLigthIntensity = 0.5;

	vec pointHit = e + d * t;
	color curColor = MySphere.essense.Color;


	vec lightDir = glm::normalize(-Light_0.pos + pointHit);//падающий луч l
	vec PointToCenter = glm::normalize((pointHit - MySphere.C)); //нормаль в обратную сторону n
		ambientLigthIntensity *= MySphere.essense.ka;
		float test = glm::dot(lightDir, PointToCenter);
		diffuseLightIntensity += Light_0.intense * max(0.f, glm::dot(lightDir, PointToCenter)) * MySphere.essense.kd;

		specularLightIntensity += powf(max(0.f, (glm::dot(PointToCenter, Reflect(lightDir, PointToCenter)))), MySphere.essense.specularExponent) * Light_0.intense * MySphere.essense.ks;

		return curColor * ambientLigthIntensity + curColor * diffuseLightIntensity + vec(1.,1.,1.) * specularLightIntensity; //+ReflectColor * MySphere.essense.kr;
}

vec GetPixelColor(int r, int c)
{
	//Getting ray
	vec e = mainCamera.position;
	float H = mainCamera.AspectR * tan(mainCamera.fov * PI_F / 360.), W = H * mainCamera.AspectR;
	int nc = win.height, nr = win.width;
	vec w = mainCamera.w;
	vec u = mainCamera.u;
	vec v = mainCamera.v;

	float uc = -W + W * 2 * c / nc;
	float vr = -H + H * 2 * r / nr;
	vec d = mainCamera.N * w + uc * u + vr * v;

	//пересечение луча со сферой
	float t = MySphere.GetRayDistance(e, d);
	MySphere.essense = Material(0.9, 0.7, 0.5, 0, 80, 9, 30.);
	color pixelcolor;

	vec newColor;
	if (t != 0)
	{
		newColor = Fong(e, d, t);
	}
	else
	{
		newColor = { 0,0,0 };

	}


	return newColor;
}

void RotateCamera(float r_y, float r_x)
{
	vector<glm::vec4> tmp;
	tmp.push_back({mainCamera.w,1 });
	tmp.push_back({ mainCamera.u,1 });
	tmp.push_back({ mainCamera.v,1 });
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::rotate(mat, glm::radians(r_y), { 0,1,0 });
	mat = glm::rotate(mat, glm::radians(r_x), { 1,0,0 });
	mainCamera.w = tmp[0] * mat;
	mainCamera.u = tmp[1] * mat;
	mainCamera.v = tmp[2] * mat;
}

void display() 
{
	glClearColor(0.5, 0.5, 0.5, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	color Col;
	mainCamera = camera();
	RotateCamera(rotate_y,rotate_x);

	for (int i = 0; i < win.width; i++)
	{
		for (int j = 0; j < win.height; j++)
		{
			if (i==320&&j==240)
			{
				int tmp = 1;
			}
			vec NewCol = GetPixelColor(i, j);

			
			//расчет цвета и света
				



			if (Col.G == 1)
			{
				int t = 1;
			}
			glBegin(GL_POINTS);
			 glColor3fv(glm::value_ptr(NewCol));
			 glVertex2i(i,j);
			glEnd();
		}
	}
	glFlush();
	glutSwapBuffers();
}



void MouseButton(int button, int state, int x, int y) {


	//if (button == 3)
	//{
	//	scale += 0.05;
	//	if (scale < 0)
	//	{
	//		scale = 0;
	//	}
	//	return;
	//}

	//if (button == 4)
	//{
	//	scale -= 0.05;
	//	if (scale < 0)
	//	{
	//		scale = 0;
	//	}
	//	return;
	//}
	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			old_x = -1;
			old_y = -1;
		}
		else {// state = GLUT_DOWN
			old_x = x;
			old_y = y;
		}
	}
}

void RotateTheCamera(int x, int y)
{

	if (old_x >= 0) {
		int dx = old_x - x;
		int dy = old_y - y;

		rotate_y -= dx;
		rotate_x -= dy;
		/* do something with dx and dy */
	}
	old_x = x;
	old_y = y;
}

void initialize()
{
	glEnable(GL_NORMALIZE);
	glMatrixMode(GL_PROJECTION);	// select projection matrix
	glLoadIdentity();															// reset projection matrix
	GLfloat aspect = (GLfloat)win.width / win.height;
	gluPerspective(win.field_of_view_angle, aspect, win.z_near, win.z_far);		
	glTranslatef(0.0f, 0.0f, -10.0f);
	glMatrixMode(GL_MODELVIEW);													// specify which matrix is the current matrix
	glClearColor(0.23, 0.23, 0.23, 1.0);											// specify clear values for the color buffers								
}


void specialKeys(int key, int x, int y) {

	//  Right arrow - increase rotation by 5 degree
	//if (key == GLUT_KEY_RIGHT)
	//	rotate_y += 5;

	////  Left arrow - decrease rotation by 5 degree
	//else if (key == GLUT_KEY_LEFT)
	//	rotate_y -= 5;

	//else if (key == GLUT_KEY_UP)
	//	rotate_x += 5;

	//else if (key == GLUT_KEY_DOWN)
	//{
	//	rotate_x -= 5;												// specify which matrix is the current matrix
	//}

	//else if (key == 32)
	//{
	//	rotate_x = 0;
	//	rotate_y = 0;
	//	scale = 1;
	//}

	//  Request display update
	glutPostRedisplay();

}

void Keyboard(unsigned char key, int x, int y)
{
#define ESCAPE '\033'
	if (key == ESCAPE) exit(0);
	switch (key)
	{
	case 'l':
		viewsettings.light = !viewsettings.light;
		break;
	case 'n':
		viewsettings.viewNormals = !viewsettings.viewNormals;
		break;
	case 'a':
		viewsettings.viewAxes = !viewsettings.viewAxes;
		break;
	case 'p':
		viewsettings.cameraMode = abs(viewsettings.cameraMode - 1);
		setCamera();
		break;
	case 'c':
		viewsettings.filled = !viewsettings.filled;
	default:
		break;
	}
	glutPostRedisplay();
}



int main(int argc, char** argv)
{
	fstream secF, trajF;
	secF.open("2D.txt");
	trajF.open("3D.txt");

	win.width = 640;
	win.height = 480;
	win.field_of_view_angle = 45;

	glEnable(GL_NORMALIZE);
	// initialize and run program
	glutInit(&argc, argv);                                      // GLUT initialization
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );  // Display Mode используем режим двойной буфферизации, режим глубины и мультисемплинг
	glutInitWindowSize(win.width, win.height);					// set window size


	glutCreateWindow(win.title);
	glutDisplayFunc(display);									// register Display Function
	
	glViewport(0, 0, win.width, win.height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, win.width, 0, win.height, -10000, 10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glutSpecialFunc(specialKeys);							// register Keyboard Handler
	glutKeyboardFunc(Keyboard);



	//cameraRotationFunctions
	glutMouseFunc(MouseButton);
	glutMotionFunc(RotateTheCamera);

	glutMainLoop();												// run GLUT mainloop
	return 0;
}