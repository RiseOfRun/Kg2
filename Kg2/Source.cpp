
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
	return { vec.R / 255. * num, vec.G / 255. * num, vec.B / 255 * num };
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


class Shape
{
public:
	bool reflaction = false;
	Material essense = Material(0.9, 0.7, 0.5, 80, 0, 120, 30.);;
	Shape()
	{
	}
	~Shape()
	{
	}
	virtual float GetRayDistance(vec e, vec d,vec& diraction)
	{
		return 0;
	}

private:

};

vector<Shape *> shapes;

class Plane : public Shape
{
public:
	
	vec norm;
	vec position;
	float d;

	Plane()
	{
	}

	Plane(vec position, vec norm)
	{
		this->norm = glm::normalize(norm);
		this->position = position;
		d = glm::dot(position,norm);
	}

	~Plane()
	{
	}

	float GetRayDistance(vec e, vec dir, vec& diraction)
	{
		
		float t = (d - glm::dot(e, norm)) / (glm::dot(dir, norm));

		if (t<0)
		{
			return 0;
		}

		diraction = norm;
		return t;
	}
};


struct
{
	bool smoothNormals = false, light = true, viewNormals = true, filled = true, viewAxes = true;
	int cameraMode = 1;
} viewsettings;

struct light
{
	vec pos = { 2, 0.1, 2 };
	float intense = 1.2;
} Light_0;

struct camera
{
	vec position = { 0,0,-3 };
	float fov = 60;
	float N = 1;
	float AspectR = 3.0 / 4.0;
	vec w = { 0,0,1 }, v = { 1, 0, 0 }, u = { 0,1,0 };
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



class Cube : public Shape
{
public:
	vector<vec> vertexes;
	vector<vector<int>> edges;
	vector<vec> norm;
	vector<float> d;
	void Build(vec position, float a, float b,float c, float r_x, float r_y)
	{
		vec x = { a/2,0,0 };
		vec y = { 0,b/2,0 };
		vec z = { 0,0,c/2 };
		vertexes.push_back(-x - y - z);
		vertexes.push_back(-x + y - z);
		vertexes.push_back(y + x - z);
		vertexes.push_back(-y + x - z);
		vertexes.push_back(-x - y + z);
		vertexes.push_back(+ y - x + z);
		vertexes.push_back(+ y + x + z);
		vertexes.push_back(+ x - y + z);
		glm::mat4 mat = glm::mat4(1.0f);
		mat = glm::rotate(mat, glm::radians(r_y), { 0,1,0 });
		mat = glm::rotate(mat, glm::radians(r_x), { 1,0,0 });
		for (size_t i = 0; i < vertexes.size(); i++)
		{
			glm::vec4 tmp = { vertexes[i], 1 };
			vertexes[i] = tmp * mat;
			vertexes[i] += position;
		}
		Build();
	}
	void Build()
	{
		edges = {
			{0,3,2},
			{2,3,7},
			{4,5,6},
			{0,1,5},
			{1,2,6},
			{0,4,7}
		};
		for (size_t i = 0; i < 6; i++)
		{
			vec v0 = vertexes[edges[i][1]];
			vec v1 = vertexes[edges[i][0]];
			vec v2 = vertexes[edges[i][2]];
			vec v10 = v0 - v1;
			vec v20 = v2 - v1;
			norm.push_back(-glm::normalize(glm::cross(v10,v20)));
			d.push_back(glm::dot(norm[i],v1));
		}
	}
	float GetRayDistance(vec e, vec dir,vec & diraction)
	{
		double fMax = 1;
		double bMin = 1e+25;
		float t = FLT_MAX;
		vec df, db;
		for (int i = 0; i < 6; i++) {
			// Пересечение луча и плоскости
			double s = glm::dot(dir, norm[i]);
			if (s == 0) { // луч и плоскость параллельны
				if (glm::dot(e, norm[i]) > d[i]) {
					return 0; // пересечения нет
				}
			}
			// Если непараллельно плоскости
			t = (d[i] - glm::dot(e, norm[i])) / s;
			if (glm::dot(dir, norm[i]) < 0) { // если пересечение "сверху"
				if (t > fMax) {
					if (t > bMin) {
						return 0; // пересечения нет
					}
					fMax = t;
					df = norm[i];
				}
			}
			else { // если пересечение "снизу"
				if (t < bMin) {
					if (t < 0 || t < fMax) {
						return 0; // пересечения нет
					}
					bMin = t;
					db = norm[i];

				}
			}
		}
		if (fMax>0)
		{
			t = fMax;
			diraction = -df;
		}
		else
		{
			t = bMin;
			diraction = -db;
			
		}
		/*t = fMax > 0 ? fMax : bMin;*/
		return t;
	}
	Cube() {}
	~Cube() {}

private:

};


class Sphere : public Shape
{
public:
	vec C;
	float r;
	
	float GetRayDistance(vec e, vec d, vec & diraction)
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
			t = (-b + sqrt(D)) / a;
		}

		if (t<1)
		{
			return 0;
		}
		vec q = e + d*t;
		diraction = glm::normalize((q - C));
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

vec Fong(vec e, vec d, float t, vec norm, Material essense)
{
	float diffuseLightIntensity = 0, specularLightIntensity = 0, ambientLigthIntensity = 0.5;

	vec pointHit = e + d * t;
	color curColor = essense.Color;


	vec lightDir = glm::normalize(-Light_0.pos + pointHit);//падающий луч l
	vec PointToCenter = norm; //нормаль в обратную сторону n
		ambientLigthIntensity *= essense.ka;
		float test = glm::dot(lightDir, PointToCenter);
		diffuseLightIntensity += Light_0.intense * max(0.f, glm::dot(lightDir, PointToCenter)) * essense.kd;

		specularLightIntensity += powf(max(0.f, (glm::dot(PointToCenter, Reflect(lightDir, PointToCenter)))), essense.specularExponent) * Light_0.intense * essense.ks;

		return curColor * ambientLigthIntensity + curColor * diffuseLightIntensity + vec(1.,1.,1.) * specularLightIntensity; //+ReflectColor * MySphere.essense.kr;
}

int FindRayIntersaction(vec & diraction,float &t, Shape & shape,vec e, vec d)
{
	int count = shapes.size();
	float tmin = FLT_MAX;
	vec minN;
	bool cross = false;
	for (size_t i = 0; i < count; i++)
	{
		t = shapes[i]->GetRayDistance(e, d, diraction);
		if (t < tmin && t != 0)
		{
			cross = true;
			shape = *shapes[i];
			tmin = t;
			minN = diraction;
		}
	}

	if (cross)
	{
		t = tmin;
		diraction = minN;
		return 1;
	}
	else
	{
		return 0;
	}

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

	//пересечение луча

	Shape shape;
	vec diraction;
	float t;

	int cross = FindRayIntersaction(diraction,t, shape, e, d);
	while (cross != 0 && shape.reflaction == true)
	{
		vec point = e + d * t;
		d = -Reflect(d, diraction);
		e = point;
		cross = FindRayIntersaction(diraction, t, shape, e, d);
	}

	color pixelcolor;

	vec newColor;
	if (cross!=0)
	{
		newColor = Fong(e, d, t, diraction, shape.essense);
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
	mat = glm::rotate(mat, glm::radians(-r_y), { 0,1,0 });
	mat = glm::rotate(mat, glm::radians(-r_x), { 1,0,0 });
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
	Cube a;
	
	a.Build({0,1,4},1,1,1,30.f,30.f);
	shapes.push_back(&a);
	/*Plane P({ 0,0,10 }, {0,0,-1});
	P.reflaction = true;
	shapes.push_back(&P);*/
	shapes.push_back(new Sphere({ 1,1.3,4 }, 1));
	shapes[0]->essense.Color = { 0,70,10 };
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
	glOrtho(0, win.width, 0, win.height, 10000, -10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glutKeyboardFunc(Keyboard);



	//cameraRotationFunctions
	glutMouseFunc(MouseButton);
	glutMotionFunc(RotateTheCamera);

	glutMainLoop();												// run GLUT mainloop
	return 0;
}