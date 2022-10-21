#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include <vector>
#include <sstream>
#include <cmath>
#include <string>
#include <map>

using namespace std;
void affichage(void);

void key_down(unsigned char key,int x,int y);
void key_up(unsigned char key,int x,int y);
void affiche_repere(void);
void check_keys();
void mouse(int, int, int, int);
void mouseMotion(int, int);

bool mouseLeftDown;
bool mouseRightDown;
bool mouseMiddleDown;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;
float cameraDistance = -4.;

void initOpenGl(){
	glClearColor( .5, .5, 0.5, 0.0 );
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(45.0f,(GLfloat)200/(GLfloat)200,0.1f,1000.0f);
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

float frenet_t = 0.5;

Vec3 control_points[] = {
    {0, 0, 0},
    {1, 2, 0},
    {2, -2, 0},
    {3, 1, 0}
};
int select = 0;

Mat4 m = {
	-1, 3, -3, 1,
	3, -6, 3, 0,
	-3, 3, 0, 0,
	1, 0, 0, 0
};

Vec3 bezier(float t){
	Vec4 v = {t*t*t, t*t, t, 1};
	Vec4 resultat = vec4_mat4_mul(v, m);
	return control_points[0]*resultat.v[0]+
           control_points[1]*resultat.v[1]+
           control_points[2]*resultat.v[2]+
           control_points[3]*resultat.v[3];
}

Vec3 bezier_derive(float t){
	Vec4 v = {3*t*t, 2*t, 1, 0};
	Vec4 resultat = vec4_mat4_mul(v, m);
	return control_points[0]*resultat.v[0]+
           control_points[1]*resultat.v[1]+
           control_points[2]*resultat.v[2]+
           control_points[3]*resultat.v[3];
}

Vec3 bezier_2derive(float t){
	Vec4 v = {6*t, 2, 0, 0};
	Vec4 resultat = vec4_mat4_mul(v, m);
	return control_points[0]*resultat.v[0]+
           control_points[1]*resultat.v[1]+
           control_points[2]*resultat.v[2]+
           control_points[3]*resultat.v[3];
}

Vec3 cross_product(Vec3 u, Vec3 v){
	return {
		u.y*v.z-u.z*v.y,
		u.z*v.x-u.x*v.z,
		u.x*v.y-u.y*v.x
	};
}

float dot_product(Vec3 a, Vec3 b){
    return a.x*b.x+a.y*b.y+a.z*b.z;
}

void draw_circle(Vec3 p, Vec3 v1, Vec3 v2, float r, int num_segments){
    glBegin(GL_LINE_LOOP);
    for (int ii = 0; ii < num_segments; ii++)   {
        float theta = 2.0f*3.1415926f*float(ii)/float(num_segments);
        Vec3 s = p+v1*(r*cosf(theta))+v2*(r*sinf(theta));
        glVertex3f(s.x, s.y, s.z);
    }
    glEnd();
}

void displayCourbe(void){
    glTranslatef(-1., -0.7, 0);
    glPointSize(15);

    glColor3f(1.0, 1.0, 1.0);
    Vec3 previous_bezier = control_points[0];
    for(float t = 0.01; t <= 1; t+=0.01){
        Vec3 b = bezier(t);
        glBegin(GL_LINES);
        glVertex3f(previous_bezier.x, previous_bezier.y, previous_bezier.z);
        glVertex3f(b.x, b.y, b.z);
        glEnd();
        previous_bezier = b;
    }

	Vec3 p = bezier(frenet_t);

    for(int i = 0; i < 4; i++){
        glBegin(GL_POINTS);
        glColor3f(1.0, 1.0, 1.0);
        if(select == i) glColor3f(0.0, 1.0, 1.0);
        glVertex3f(control_points[i].x, control_points[i].y, control_points[i].z);
        glEnd();
    }

    Vec3 p_prime = bezier_derive(frenet_t);
    Vec3 p_2prime = bezier_2derive(frenet_t);
    Vec3 t = normalize(p_prime);
    Vec3 p_2prime_t = p_prime/pow(magnitude(p_prime), 2)*dot_product(p_prime, p_2prime);
    Vec3 p_2prime_n = p_2prime-p_2prime_t;
    Vec3 n = normalize(p_2prime_n);
    Vec3 b = cross_product(t, n);

    // Vec3 p_prime = bezier_derive(frenet_t);
    // Vec3 p_2prime = bezier_2derive(frenet_t);
    // Vec3 t = normalize(p_prime);
    // Vec3 b = normalize(cross_product(p_prime, p_2prime));
    // Vec3 n = cross_product(b, t);

    glLineWidth(3.0);

    glColor3f(1.0,0.0,0.0);
    glBegin(GL_LINES);
    glVertex3f(p.x, p.y, p.z);
    glVertex3f(p.x+t.x, p.y+t.y, p.z+t.z);
    glEnd();

    glColor3f(0.0,1.0,0.0);
    glBegin(GL_LINES);
    glVertex3f(p.x, p.y, p.z);
    glVertex3f(p.x+b.x, p.y+b.y, p.z+b.z);
    glEnd();

    glColor3f(0.0,0.0,1.0);
    glBegin(GL_LINES);
    glVertex3f(p.x, p.y, p.z);
    glVertex3f(p.x+n.x, p.y+n.y, p.z+n.z);
    glEnd();

    glColor3f(1.0, 0.0, 1.0);

    float r = powf(magnitude(p_prime), 3)/magnitude(cross_product(p_prime, p_2prime));
    Vec3 c = p+n*r;
    draw_circle(c, t, n, r, 100);
    glLineWidth(1.0);
}

int main(int argc,char **argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowPosition(200,200);
    glutInitWindowSize(600,600);
    glutCreateWindow("ifs");
    glClearColor(0.0,0.0,0.0,0.0);
    glColor3f(1.0,1.0,1.0);
    glPointSize(1.0);
    glutDisplayFunc(affichage);
    glutKeyboardFunc(key_down);
    glutKeyboardUpFunc(key_up);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    initOpenGl();
    glutMainLoop();
    return 0;
}

void affichage(void){
    check_keys();
	glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
	glTranslatef(0,0.,cameraDistance);
	glRotatef(cameraAngleX,1.,0.,0.);
	glRotatef(cameraAngleY,0.,1.,0.);
    displayCourbe();
    glPopMatrix();
    glFlush();
    glutSwapBuffers();
}

std::map<char, bool> keys = {
    {'w', false},
    {'s', false},
    {'a', false},
    {'d', false},
};

void check_keys(){
    if(keys['w']) cameraDistance += 0.01;
    if(keys['s']) cameraDistance -= 0.01;
    if(keys['e']){
        frenet_t += 0.001;
        frenet_t = frenet_t > 1 ? 1 : frenet_t;
    }
    if(keys['q']){
        frenet_t -= 0.001;
        frenet_t = frenet_t < 0 ? 0 : frenet_t;
    }

    if(keys['g']) control_points[select].x -= 0.01;;
    if(keys['j']) control_points[select].x += 0.01;;
    if(keys['y']) control_points[select].y += 0.01;
    if(keys['h']) control_points[select].y -= 0.01;
    glutPostRedisplay();
}

void key_up(unsigned char key,int x,int y){
    keys[key] = false;
}

void key_down(unsigned char key,int x,int y){
    if(key == 'a') select--;
    if(key == 'd') select++;
    select = select < 0 ? 0 : select;
    select = select > 3 ? 3 : select;
    keys[key] = true;
}
void mouse(int button, int state, int x, int y){
    mouseX = x;
    mouseY = y;

    if(button == GLUT_LEFT_BUTTON){
        if(state == GLUT_DOWN) mouseLeftDown = true;
        else if(state == GLUT_UP) mouseLeftDown = false;
    }
    else if(button == GLUT_RIGHT_BUTTON){
        if(state == GLUT_DOWN) mouseRightDown = true;
        else if(state == GLUT_UP) mouseRightDown = false;
    }
    else if(button == GLUT_MIDDLE_BUTTON){
        if(state == GLUT_DOWN) mouseMiddleDown = true;
        else if(state == GLUT_UP) mouseMiddleDown = false;
    }
}

void mouseMotion(int x, int y){
    if(mouseLeftDown){
        cameraAngleY += (x - mouseX);
        cameraAngleX += (y - mouseY);
        mouseX = x;
        mouseY = y;
    }
    glutPostRedisplay();
}