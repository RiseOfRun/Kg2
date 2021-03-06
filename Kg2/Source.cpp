
#include <stdlib.h>
#include <iostream>
#include <vector>	
#include <fstream>
#include <unordered_map>
#include <GL/freeglut.h>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>


#define KEY_ESCAPE 27
using namespace std;
typedef glm::vec3 vec;

struct
{
	bool smoothNormals = false, light = true, viewNormals = true, filled = true, viewAxes = true;
	int cameraMode = 1;
} viewsettings;

struct settings
{
	vector<vec> section;
	vec position;
	vec diraction;
	float rotation;
} editionSettings;

glm::vec4 pos = { 0, 5, 10, 0 };
float rotate_x = 0, rotate_y = 0;
int old_x = -1;
int old_y = -1;
float scale = 1;
float texCoords[] = {
	0.0f, 0.0f,  // lower-left corner  
	1.0f, 0.0f,  // lower-right corner
	0.5f, 1.0f   // top-center corner
};

void Textures()
{

} //ToDo

void ReadEditionSettings(fstream& section, fstream& trajectory)
{
	vec point;
	while (section >> point.x >> point.y >> point.z)
	{
		editionSettings.section.push_back(point);
	}
	trajectory >> point.x >> point.y >> point.z;
	editionSettings.position = point;
	trajectory >> point.x >> point.y >> point.z;
	editionSettings.diraction = point;
	trajectory >> editionSettings.rotation;

}

//������� �������, ��� ������� ���� � ���������
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


class Primal
{
public:
	vector<vec> points;
	vector<vec> normals;
	vector<vector<vec>> edges;
	vec norm;
	void Norm()
	{
		vec a = points[0];
		vec b = points[1];
		vec c = points[2];
		vec ba = b - a;
		vec ca = c - a;
		norm = -glm::cross(ba, ca);
		normals = vector<vec>(points.size());
		for (size_t i = 0; i < points.size(); i++)
		{
			normals[i] = norm;
		}
	}



	void ReBuildEdges()
	{
		edges = vector<vector<vec>>(0);
		for (size_t i = 0; i < points.size() - 1; i++)
		{
			vector<vec> edge;
			edge.push_back(points[i]);
			edge.push_back(points[i + 1]);
			edges.push_back(edge);
		}
		vector<vec> edge;
		edge.push_back(points[points.size() - 1]);
		edge.push_back(points[0]);
		edges.push_back(edge);
	}

	void DrawNormals()
	{

		glColor3f(0, 0, 0);
		glBegin(GL_LINES);

		for (size_t i = 0; i < points.size(); i++)
		{
			vec path = normals[i];
			vec point = points[i];
			glVertex3fv(glm::value_ptr(point));
			glVertex3fv(glm::value_ptr(point + 3.0f * path));
		}

		glEnd();
	}

	void Draw()
	{
		if (points.size() > 3)
		{
			glColor3f(0, 1, 0);
		}
		else
		{
			glColor3f(0, 1, 0);
		}

		glBegin(GL_POLYGON);
		for (size_t i = 0; i < points.size(); i++)
		{
			glNormal3fv(glm::value_ptr(normals[i]));
			glVertex3fv(glm::value_ptr(points[i]));
		}
		glEnd();
	}

	void DrawCarcass()
	{
		//glTranslatef(0,0,7);
		if (points.size() > 3)
		{
			glColor3f(1, 0, 0);
		}
		else
		{
			glColor3f(1, 1, 1);
		}

		glBegin(GL_LINE_LOOP);
		for (size_t i = 0; i < points.size(); i++)
		{
			glNormal3fv(glm::value_ptr(normals[i]));
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
		ReBuildEdges();
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
	float rotation;
	vector<Primal*> primals;


	void BuildSmoothNormals()
	{
		std::unordered_map<vec, vec, KeyFuncs, KeyFuncs> norms;
		for (int i = 2; i < primals.size(); i++)
		{
			for (size_t j = 0; j < primals[i]->points.size(); j++)
			{
				if (!norms.count(primals[i]->points[j]))
				{
					norms[primals[i]->points[j]] = primals[i]->normals[j];
				}
				else
				{
					norms[primals[i]->points[j]] += primals[i]->normals[j];
				}
			}
		}

		for (int i = 2; i < primals.size(); i++)
		{
			for (size_t j = 0; j < primals[i]->points.size(); j++)
			{
				primals[i]->normals[j] = glm::normalize(norms[primals[i]->points[j]]);
				printf("primal: %d, point (%f, %f, %f), normal(%f, %f, %f)\n", i, primals[i]->points[j].x, primals[i]->points[j].y, primals[i]->points[j].z,
					primals[i]->normals[j].x, primals[i]->normals[j].y, primals[i]->normals[j].z);
			}
		}
	}

	void BuildDefaultNormals()
	{
		for (size_t i = 2; i < primals.size(); i++)
		{
			primals[i]->Norm();
		}
	}

	Primal* BuildPolygon(vec a, vec b, vec c)
	{
		Primal* Edge = new Primal({ a, b, c });
		Edge->Norm();
		return Edge;
	}

	void Build(Primal* section)
	{
		//������������ ������� � �����������
		glm::mat4 mat = glm::mat4(1.0f);
		vec norm = glm::normalize(way);
		vec rotationAxe = glm::cross({ 0,0,1 }, norm);
		if (way != vec{ 0, 0, 1 })
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

		//�������� ������� �� ��������� �������
		for (size_t i = 0; i < section->points.size(); i++)
		{
			section->points[i] += position;
		}

		section->norm = norm;
		section->ReBuildEdges();
		primals.push_back(new Primal(*section));

		//���������� ������:

		//�������� �������
		for (size_t i = 0; i < section->points.size(); i++)
		{
			section->points[i] += norm;
		}

		//������������ �� �������� ����

		mat = glm::mat4(1.0f);
		mat = glm::rotate(mat, (float)glm::radians(rotation), norm);
		for (size_t i = 0; i < section->points.size(); i++)
		{
			glm::vec4 tmp = { section->points[i],1 };
			section->points[i] = mat * tmp;
		}
		section->ReBuildEdges();
		primals.push_back(new Primal(*section));

		//������ �������������� �����
		for (size_t i = 0; i < section->edges.size(); i++)
		{
			vec a1 = primals[0]->edges[i][0];
			vec a2 = primals[0]->edges[i][1];
			vec b1 = primals[1]->edges[i][0];
			vec b2 = primals[1]->edges[i][1];
			primals.push_back(BuildPolygon(a1, a2, b1));
			primals.push_back(BuildPolygon(a2, b2, b1));
		}

		for (size_t i = 0; i < primals.size(); i++)
		{
			primals[i]->Norm();
		}

		for (size_t i = 0; i < primals[0]->normals.size(); i++)
		{
			primals[0]->normals[i] *= -1;
		}
	}

	void DrawNormals()
	{
		for (size_t i = 0; i < primals.size(); i++)
		{

			primals[i]->DrawNormals();
		}
	}

	void Draw()
	{
		for (size_t i = 0; i < primals.size(); i++)
		{
			primals[i]->Draw();
		}
	}

	void DrawCarcass()
	{
		for (size_t i = 0; i < primals.size(); i++)
		{
			primals[i]->DrawCarcass();
		}
	}



	Edition(vec pos, vec way, float rotation)
	{
		position = pos;
		this->way = way;
		this->rotation = rotation;
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

void DrawAxis()
{
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(-10, 0, 0);
	glVertex3f(10, 0, 0);
	glColor3f(0, 1, 0);
	glVertex3f(0, -10, 0);
	glVertex3f(0, 10, 0);
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, -10);
	glVertex3f(0, 0, 10);
	glEnd();
}

void Lightning()
{
	if (viewsettings.light && viewsettings.filled)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_NORMALIZE);
	}
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}

	float mat[] = { 0.2, 0.2, 0.2, 1 };
	glm::vec4 curPosition = { pos.x * scale,pos.y * scale,pos.z * scale, pos.w };
	glLightfv(GL_LIGHT0, GL_POSITION, glm::value_ptr(curPosition));
	float LighSpec[] = { 0.0f,0.0f,0.0f,0.0f };
	//glLightfv(GL_LIGHT0, GL_SPECULAR, LighSpec);
	glEnable(GL_COLOR_MATERIAL);

	//��������� �������  ���������� �����

	//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT); // Adjust ambient
	//glColor4f(0.5, 0.5, 0.5, 1.0);
	//glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
	//glColor4f(0.0, 0.0, 0.0, 1.0);
	//glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
	//glColor4f(0.25, 0.25, 0.25, 1.0);
	//glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	//glColor4f(1.0, 1.0, 1.0, 1.0);
	//glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 80.0);

}

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

void display() {
	GLfloat params[16];

	//  Clear screen and Z-buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Reset transformations
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(rotate_x, 1, 0, 0);
	glRotatef(rotate_y, 0, 1, 0);
	glScalef(scale, scale, scale);
	glColor3f(0, 1, 0);

	Lightning();

	if (viewsettings.filled)
	{
		toDraw->Draw();
	}
	else
	{
		toDraw->DrawCarcass();
	}

	if (viewsettings.viewNormals)
	{
		toDraw->DrawNormals();
	}

	if (viewsettings.viewAxes)
	{
		DrawAxis();
	}

	float curPos[] = { pos.x,pos.y,pos.z };
	glTranslatef(pos.x, pos.y, pos.z);
	glColor3f(1, 0, 0);
	//glutSolidSphere(1, 256, 256);

	glFlush();
	glutSwapBuffers();
}




void MouseButton(int button, int state, int x, int y) {


	if (button == 3)
	{
		scale += 0.05;
		if (scale < 0)
		{
			scale = 0;
		}
		return;
	}

	if (button == 4)
	{
		scale -= 0.05;
		if (scale < 0)
		{
			scale = 0;
		}
		return;
	}
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
	glOrtho(-10, 10, -10, 10, 0, 1000);
	//gluPerspective(win.field_of_view_angle, aspect, win.z_near, win.z_far);		
	glTranslatef(0.0f, 0.0f, -10.0f);
	glMatrixMode(GL_MODELVIEW);													// specify which matrix is the current matrix
	//��������� �������� ���������, ������� �� �������

	//glShadeModel(GL_SMOOTH);
	//glClearDepth(1.0f);														// specify the clear value for the depth buffer
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);						// specify implementation-specific hints
	//Ligt

	glClearColor(0.23, 0.23, 0.23, 1.0);											// specify clear values for the color buffers								
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
		rotate_x -= 5;												// specify which matrix is the current matrix
	}

	else if (key == 32)
	{
		rotate_x = 0;
		rotate_y = 0;
		scale = 1;
	}

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
	case 's':
		viewsettings.smoothNormals = !viewsettings.smoothNormals;
		if (viewsettings.smoothNormals)
		{
			toDraw->BuildDefaultNormals();
			toDraw->BuildSmoothNormals();
		}
		else
		{
			toDraw->BuildDefaultNormals();
		}
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

	ReadEditionSettings(secF, trajF);
	Primal* sec = new Primal(
		editionSettings.section
		/*{
			{-1,0,0},
			{-0.5,1,0},
			{0.5,1,0},
			{1,0,0},
			{0.5,-1,0},
			{-0.5,-1,0}

		}*/
		);
	Edition myEdition(editionSettings.position, editionSettings.diraction, editionSettings.rotation);

	myEdition.Build(sec);
	toDraw = &myEdition;
	toDraw->BuildSmoothNormals();
	win.width = 640;
	win.height = 480;
	win.field_of_view_angle = 45;
	win.z_near = 1.0f;
	win.z_far = 500.0f;

	// initialize and run program
	glutInit(&argc, argv);                                      // GLUT initialization
	glutSetOption(GLUT_MULTISAMPLE, 8);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);  // Display Mode ���������� ����� ������� ������������, ����� ������� � ��������������

	glutInitWindowSize(win.width, win.height);					// set window size
	glutCreateWindow(win.title);								// create Window
	glEnable(GL_DEPTH_TEST);
	glutDisplayFunc(display);									// register Display Function
	glutIdleFunc(display);									// register Idle Function
	glutSpecialFunc(specialKeys);							// register Keyboard Handler
	glutKeyboardFunc(Keyboard);

	//cameraRotationFunctions
	glutMouseFunc(MouseButton);
	glutMotionFunc(RotateTheCamera);
	initialize();
	glutMainLoop();												// run GLUT mainloop
	return 0;
}