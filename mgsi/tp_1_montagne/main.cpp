#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include <vector>
#include <sstream>
#include <cmath>
#include <string>

using namespace std ;
void affichage(void);

void clavier(unsigned char touche,int x,int y);
void affiche_repere(void);

void mouse(int, int, int, int);
void mouseMotion(int, int);

// variables globales pour OpenGL
bool mousegauchebas;
bool mousedroitebas;
bool mouseMiddlebas;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;
float cameraDistance=-30.;
float d =1.0;
float hi=0.1;
int nbptx=10;
int nbpty=10;
bool dispfil=false;
bool debut = true;
bool wireframe = false;


void initOpenGl()
{
	glClearColor(0.8,0.9,0.9, 0.0 );
    // glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(45.0f,(GLfloat)200/(GLfloat)200,0.1f,10000.0f);
	glMatrixMode(GL_MODELVIEW);
    gluLookAt(0.,0.,4., 0.,0.,0., 0.,1.,0.);
}

struct Mat4{
	float m[16];
};

struct Vec4{
   float v[4];
};

Vec4 vec4_mat4_mul(Vec4 b, Mat4 a){
    return (Vec4){
        b.v[0]*a.m[0]+b.v[1]*a.m[4]+b.v[2]*a.m[8]+b.v[3]*a.m[12],
        b.v[0]*a.m[1]+b.v[1]*a.m[5]+b.v[2]*a.m[9]+b.v[3]*a.m[13],
        b.v[0]*a.m[2]+b.v[1]*a.m[6]+b.v[2]*a.m[10]+b.v[3]*a.m[14],
        b.v[0]*a.m[3]+b.v[1]*a.m[7]+b.v[2]*a.m[11]+b.v[3]*a.m[15]
    };
}

struct Vec3{
    float x;
    float y;
    float z;

    Vec3(float x, float y, float z): x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& b){
        return {this->x+b.x, this->y+b.y, this->z+b.z};
    }

    Vec3 operator-(const Vec3& b){
        return {this->x-b.x, this->y-b.y, this->z-b.z};
    }

    Vec3 operator*(const float& b){
        return {b*this->x, b*this->y, b*this->z};
    }


    Vec3 operator/(const float& b){
        return {this->x/b, this->y/b, this->z/b};
    }
};

float magnitude(Vec3 v){
	return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}

Vec3 normalize(Vec3 v){
	float m = magnitude(v);
	return {v.x/m, v.y/m, v.z/m};
}

Vec3 produit_vectorielle(Vec3 u, Vec3 v){
	return {
			u.y*v.z-u.z*v.y,
			u.z*v.x-u.x*v.z,
			u.x*v.y-u.y*v.x
	};
}

float dot_product(Vec3 a, Vec3 b){
    return a.x*b.x+a.y*b.y+a.z*b.z;
}
float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}


std::vector<Vec3> grille = {};

int grille_size = 6;
int nb_points;

int rand_int(int min, int max){
	return rand()%(max-min + 1) + min;
}

float rand_float(float Min, float Max){
    return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
}

void carre(int x, int y, int size, float value){
    int hs = size / 2;
    float a = grille[x-hs+ (y-hs)*nb_points].y;
    float b = grille[x+hs+ (y-hs)*nb_points].y;
    float c = grille[x-hs+ (y+hs)*nb_points].y;
    float d = grille[x+hs+ (y+hs)*nb_points].y;
    grille[x+y*nb_points].y = ((a + b + c + d) / 4.0) + value;
}

void diamant(int x, int y, int size, float value){
    int hs = size / 2;
    float a = grille[x-hs+y*nb_points].y;
    float b = grille[x+hs+y*nb_points].y;
    float c = grille[x+(y - hs)*nb_points].y;
    float d = grille[x+(y + hs)*nb_points].y;
    grille[x+y*nb_points].y = ((a + b + c + d) / 4.0) + value;
}

void carrediamant(int stepsize, double scale){
    int halfstep = stepsize / 2;
    for(int y = halfstep; y < nb_points + halfstep; y += stepsize){
        for(int x = halfstep; x < nb_points + halfstep; x += stepsize){
            carre(x, y, stepsize, rand_float(0, 1)*scale);
        }
    }

    for(int y = 0; y < nb_points; y += stepsize){
        for(int x = 0; x < nb_points; x += stepsize){
            diamant(x + halfstep, y, stepsize, rand_float(0, 1)*scale);
            diamant(x, y + halfstep, stepsize, rand_float(0, 1)*scale);
        }
    }
}

void generate_terrain(){
    nb_points = pow(2, grille_size)+1;
    for(int i = 0; i < nb_points; i++){
        for(int j = 0; j < nb_points; j++){
            grille.push_back(Vec3(float(i-nb_points/2)/2, rand_float(0, 1), float(j-nb_points/2)/2));
        }
    }

    int samplesize = 16;
    float scale = 3.0;

    while(samplesize > 1){
        carrediamant(samplesize, scale);
        samplesize /= 2;
        scale /= 2.0;
    }
}

void draw_triangle(Vec3 a, Vec3 b, Vec3 c){
    glBegin(GL_TRIANGLES);
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glVertex3f(c.x, c.y, c.z);
    glEnd();
}

void draw_lines(Vec3 a, Vec3 b, Vec3 c){
    glBegin(GL_LINES);
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(b.x, b.y, b.z);
    glVertex3f(c.x, c.y, c.z);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(c.x, c.y, c.z);
    glVertex3f(a.x, a.y, a.z);
    glEnd();
}

void display(void){
    if(wireframe){
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }
    else{
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }

    for(int j = 0; j < nb_points-1; j++){
        for(int i = 0; i < nb_points-1; i++){
            int row1 = j*nb_points;
            int row2 = (j+1)*nb_points;
            glColor3f(0.549, 0.223, 0.137);
            if(
grille[row1+i].y>= 3.5 ||
grille[row1+i+1].y >= 3.5 ||
grille[row2+i+1].y >= 3.5 ||
grille[row1+i].y>= 3.5 ||
grille[row2+i+1].y >= 3.5 ||
grille[row2+i].y>= 3.5
                ){
                glColor3f(0.898, 0.898, 0.898);
            }
            draw_triangle(grille[row1+i], grille[row1+i+1], grille[row2+i+1]);
            draw_triangle(grille[row1+i], grille[row2+i+1], grille[row2+i]);
            // glColor3f(1, 1, 1);
            // draw_lines(grille[row1+i], grille[row1+i+1], grille[row2+i+1]);
            // draw_lines(grille[row1+i], grille[row2+i+1], grille[row2+i]);
        }
    }
}

int main(int argc,char **argv)
{
	srand (time(NULL));
	generate_terrain();
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB);
  glutInitWindowPosition(200,200);
  glutInitWindowSize(600,600);
  glutCreateWindow("Montagnes fractales");
  glClearColor(0.0,0.0,0.0,0.0);
  glColor3f(1.0,1.0,1.0);
  glPointSize(1.0);
  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMotion);
  initOpenGl() ;
  glutMainLoop();

  return 0;
}

void affichage(void)
{
	glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
	glTranslatef(0,0.,cameraDistance);
	glRotatef(cameraAngleX+45,1.,0.,0.)	;
	glRotatef(cameraAngleY-45,0.,1.,0.);
    display();
    glPopMatrix();
    glFlush();
    glutSwapBuffers();
}

void clavier(unsigned char touche,int x,int y)
{
    switch (touche){
        case 'f':
          wireframe = !wireframe;
          break;
        case 'q':
            exit(0);
    }
    glutPostRedisplay();
}
void mouse(int button, int state, int x, int y)
{
    mouseX = x;
    mouseY = y;

    if(button == GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mousegauchebas = true;
        }
        else if(state == GLUT_UP)
            mousegauchebas = false;
    }

    else if(button == GLUT_RIGHT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mousedroitebas = true;
        }
        else if(state == GLUT_UP)
            mousedroitebas = false;
    }

    else if(button == GLUT_MIDDLE_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseMiddlebas = true;
        }
        else if(state == GLUT_UP)
            mouseMiddlebas = false;
    }
}


void mouseMotion(int x, int y){
    if(mousegauchebas)
    {
        cameraAngleY += (x - mouseX);
        cameraAngleX += (y - mouseY);
        mouseX = x;
        mouseY = y;
    }
    if(mousedroitebas)
    {
        cameraDistance += (y - mouseY) * 0.2f;
        mouseY = y;
    }

    glutPostRedisplay();
}