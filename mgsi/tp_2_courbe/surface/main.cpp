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
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat l_pos[] = { 3.,3.5,3.0,1.0 };
    glLightfv(GL_LIGHT0,GL_POSITION,l_pos);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,l_pos);
    glLightfv(GL_LIGHT0,GL_SPECULAR,l_pos);
    glEnable(GL_COLOR_MATERIAL);
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

Vec3 control_points[] = {
    // {0, 0, 0},
    // {1, 2, 0},
    // {2, -2, 0},
    // {3, 1, 0}
{-0.700000, 1.579999, 0.000000},
{1.010000, 2.020000, 0.000000},
{2.510000, 1.499997, 0.000000},
{2.670000 ,-1.129998, 0.000000},
};
int current_selected_control = 0;

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

vector<Vec3> draw_circle(Vec3 p, Vec3 v1, Vec3 v2, float r, int num_segments){
    vector<Vec3> points;
    for(int i = 0; i < num_segments; i++){
        float theta = 2.0f*M_PI*float(i)/float(num_segments);
        Vec3 s = p+v1*(r*cosf(theta))+v2*(r*sinf(theta));
        points.push_back(s);
    }
    return points;
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

    for(int i = 0; i < 4; i++){
        glBegin(GL_POINTS);
        glColor3f(1.0, 1.0, 1.0);
        if(current_selected_control == i) glColor3f(1.0, 0.0, 1.0);
        glVertex3f(control_points[i].x, control_points[i].y, control_points[i].z);
        glEnd();
    }

    glLineWidth(3.0);
    glColor3f(0.0, 0.0, 1.0);
    bool first = true;
    vector<Vec3> previous_points;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for(float frenet_t = 0; frenet_t <= 1.0; frenet_t += 0.01){
        Vec3 p = bezier(frenet_t);
        Vec3 p_prime = bezier_derive(frenet_t);
        Vec3 p_2prime = bezier_2derive(frenet_t);
        Vec3 t = normalize(p_prime);
        Vec3 p_2prime_t = p_prime/pow(magnitude(p_prime), 2)*dot_product(p_prime, p_2prime);
        Vec3 p_2prime_n = p_2prime-p_2prime_t;
        Vec3 n = normalize(p_2prime_n);
        Vec3 b = cross_product(t, n);
        vector<Vec3> current_points = draw_circle(p, b, n, 0.5, 10);
        glColor3f(0.0, 1.0, 1.0);
        if(!first){
            for(int i = 0; i < previous_points.size(); i++){
                int curr = i;
                int next = i+1;
                if(i == previous_points.size()-1) next = 0;
                glBegin(GL_QUADS);
                glVertex3f(previous_points[curr].x, previous_points[curr].y, previous_points[curr].z);
                glVertex3f(current_points[curr].x, current_points[curr].y, current_points[curr].z);
                glVertex3f(current_points[next].x, current_points[next].y, current_points[next].z);
                glVertex3f(previous_points[next].x, previous_points[next].y, previous_points[next].z);
                glEnd();
            }
        }
        previous_points = current_points;
        first = false;
    }

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

    if(keys['g']) control_points[current_selected_control].x -= 0.01;;
    if(keys['j']) control_points[current_selected_control].x += 0.01;;
    if(keys['y']) control_points[current_selected_control].y += 0.01;
    if(keys['h']) control_points[current_selected_control].y -= 0.01;
    glutPostRedisplay();
}

void key_up(unsigned char key,int x,int y){
    keys[key] = false;
}

void key_down(unsigned char key,int x,int y){
    if(key == 'a') current_selected_control--;
    if(key == 'd') current_selected_control++;
    current_selected_control = current_selected_control < 0 ? 0 : current_selected_control;
    current_selected_control = current_selected_control > 3 ? 3 : current_selected_control;
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