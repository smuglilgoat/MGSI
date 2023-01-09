#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include <vector>
#include <sstream>
#include <armadillo>
#include <string>

using namespace std;
using namespace arma;
void affichage(void);

void clavier(unsigned char touche, int x, int y);
void affiche_repere(void);

void mouse(int, int, int, int);
void mouseMotion(int, int);
// void reshape(int,int);
float t = .5;

// variables globales pour OpenGL
bool mouseLeftDown;
bool mouseRightDown;
bool mouseMiddleDown;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;
float cameraDistance = 0.;

// constantes pour les materieux
float no_mat[] = {0.0f, 0.0f, 0.0f, 1.0f};
float mat_ambient[] = {0.7f, 0.7f, 0.7f, 1.0f};
float mat_ambient_color[] = {0.8f, 0.8f, 0.2f, 1.0f};
float mat_diffuse[] = {0.1f, 0.5f, 0.8f, 1.0f};
float mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
float no_shininess = 0.0f;
float low_shininess = 5.0f;
float high_shininess = 100.0f;
float mat_emission[] = {0.3f, 0.2f, 0.2f, 0.0f};

// Control Points
mat p = {
    {0, 1, 0},
    {0, 1, 1},
    {1, 0, 1},
    {0, 0, 0}};

mat m = {
    {-1, 3, -3, 1},
    {3, -6, 3, 0},
    {-3, 3, 0, 0},
    {1, 0, 0, 0}};

mat bezier(float t)
{
  rowvec tVec = {pow(t, 3), pow(t, 2), t, 1};
  mat bVec = tVec * m;
  mat tmp = bVec * p;
  return tmp;
}
mat bezierDerivate(float t)
{
  rowvec tVec = {pow(t, 2) * 3, t * 2, 1, 0};
  mat bVec = tVec * m;
  mat tmp = bVec * p;
  return tmp;
}
mat bezier2nDerivate(float t)
{
  rowvec tVec = {t * 6, 2, 0, 0};
  mat bVec = tVec * m;
  mat tmp = bVec * p;
  return tmp;
}
void draw_circle(mat c, mat n, mat b)
{
  float r = 1;
  int num_segments = 100;

  glBegin(GL_LINE_LOOP);
  for (int i = 0; i < num_segments; i++)
  {
    float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
    rowvec p_t = c + b * (r * cosf(theta)) + n * (r * sinf(theta));
    glVertex3f(p_t[0], p_t[1], p_t[2]);
  }
  glEnd();
}
void draw_cylindre(mat c, mat n, mat b)
{
  float r = 0.2;
  int num_segments = 100;

  glBegin(GL_LINE_LOOP);
  for (int i = 0; i < num_segments; i++)
  {
    float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
    rowvec p_t = c + b * (r * cosf(theta)) + n * (r * sinf(theta));
    glVertex3f(p_t[0], p_t[1], p_t[2]);
  }
  glEnd();
}
float frenet = 0.5;
void initOpenGl()
{

  // lumiere

  glClearColor(.5, .5, 0.5, 0.0);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  GLfloat l_pos[] = {3., 3.5, 3.0, 1.0};
  glLightfv(GL_LIGHT0, GL_POSITION, l_pos);

  glLightfv(GL_LIGHT0, GL_DIFFUSE, l_pos);
  glLightfv(GL_LIGHT0, GL_SPECULAR, l_pos);
  glEnable(GL_COLOR_MATERIAL);

  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  // glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  //  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE|GLUT_RGB);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (GLfloat)200 / (GLfloat)200, 0.1f, 10.0f);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0., 0., 4., 0., 0., 0., 0., 1., 0.);
}

//------------------------------------------------------

void displayCourbe(void)
{
  glPointSize(10);
  glColor3f(1.0, 1.0, 1.0);
  rowvec prevPoint = p.row(0);

  for (float t = 0.01; t <= 1; t += 0.01)
  {
    rowvec currPoint = bezier(t);
    glBegin(GL_LINES);
    glVertex3f(prevPoint(0), prevPoint(1), prevPoint(2));
    glVertex3f(currPoint(0), currPoint(1), currPoint(2));
    glEnd();
    prevPoint = currPoint;
  }

  rowvec f = bezier(frenet);

  for (int i = 0; i < 4; i++)
  {
    glBegin(GL_POINTS);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(p.row(i)(0), p.row(i)(1), p.row(i)(2));
    glEnd();
  }

  mat fPrime = bezierDerivate(frenet);
  mat f2Prime = bezier2nDerivate(frenet);
  mat t = normalise(fPrime, 2, 1);
  mat f2PrimeT = fPrime / pow(norm(fPrime), 2) * dot(fPrime, f2Prime);
  mat f2PrimeN = f2Prime - f2PrimeT;
  mat n = normalise(f2PrimeN, 2, 1);
  mat b = cross(t, n);

  glLineWidth(3.0);

  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_LINES);
  glVertex3f(f(0), f(1), f(2));
  glVertex3f(f(0) + t(0), f(1) + t(1), f(2) + t(2));
  glEnd();

  glColor3f(0.0, 1.0, 0.0);
  glBegin(GL_LINES);
  glVertex3f(f(0), f(1), f(2));
  glVertex3f(f(0) + b(0), f(1) + b(1), f(2) + b(2));
  glEnd();

  glColor3f(0.0, 0.0, 1.0);
  glBegin(GL_LINES);
  glVertex3f(f(0), f(1), f(2));
  glVertex3f(f(0) + n(0), f(1) + n(1), f(2) + n(2));
  glEnd();

  glColor3f(0.0, 0.5, 0.5);
  rowvec c = f + n;
  draw_circle(c, n, t);
  glLineWidth(1.0);

  glColor3f(0.58, 0.0, 0.53);
  for (float t = 0.0; t <= 1; t += 0.01)
  {
    rowvec currPoint = bezier(t);
    mat fPrime_bis = bezierDerivate(t);
    mat f2Prime_bis = bezier2nDerivate(t);
    mat t_bis = normalise(fPrime_bis, 2, 1);
    mat f2PrimeT_bis = fPrime_bis / pow(norm(fPrime_bis), 2) * dot(fPrime_bis, f2Prime_bis);
    mat f2PrimeN_bis = f2Prime_bis - f2PrimeT_bis;
    mat n_bis = normalise(f2PrimeN_bis, 2, 1);
    mat b_bis = cross(t_bis, n_bis);
    draw_cylindre(currPoint, n_bis, b_bis);
    prevPoint = currPoint;
  }
}

int main(int argc, char **argv)
{
  /* initialisation de glut et creation
     de la fenetre */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB);
  glutInitWindowPosition(200, 200);
  glutInitWindowSize(600, 600);
  glutCreateWindow("ifs");

  /* Initialisation d'OpenGL */
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glColor3f(1.0, 1.0, 1.0);
  glPointSize(1.0);

  // ifs = new Ifs();
  /* enregistrement des fonctions de rappel */
  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMotion);
  //-------------------------------

  //-------------------------------
  initOpenGl();
  //-------------------------------

  /* Entree dans la boucle principale glut */
  glutMainLoop();
  return 0;
}
//------------------------------------------------------
void affiche_repere(void)
{
  glBegin(GL_LINES);
  glColor3f(1.0, 0.0, 0.0);
  glVertex2f(0., 0.);
  glVertex2f(1., 0.);
  glEnd();

  glBegin(GL_LINES);
  glColor3f(0.0, 1.0, 0.0);
  glVertex2f(0., 0.);
  glVertex2f(0., 1.);
  glEnd();
  glBegin(GL_LINES);
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(0., 0., 0.);
  glVertex3f(0., 0., 1.);
  glEnd();
}

//-----------------------------------------------------

//------------------------------------------------------
void affichage(void)
{
  glMatrixMode(GL_MODELVIEW);
  /* effacement de l'image avec la couleur de fond */
  //	glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //       glClearDepth(10.0f);                         // 0 is near, >0 is far

  glPushMatrix();
  glTranslatef(0, 0, cameraDistance);
  glRotatef(cameraAngleX, 1., 0., 0.);
  glRotatef(cameraAngleY, 0., 1., 0.);
  affiche_repere();
  displayCourbe();
  glPopMatrix();
  /* on force l'affichage du resultat */

  glFlush();
  glutSwapBuffers();
}

//------------------------------------------------------

//------------------------------------------------------
void clavier(unsigned char touche, int x, int y)
{

  switch (touche)
  {
  case '+': //
    frenet += 0.01;
    frenet = frenet > 1 ? 1 : frenet;
    cout << "Frenet =" << frenet << endl;
    glutPostRedisplay();
    break;
  case '-': //* ajustement du t
    frenet -= 0.01;
    frenet = frenet < 0 ? 0 : frenet;
    cout << "Frenet =" << frenet << endl;
    glutPostRedisplay();
    break;
  case 'f': //* affichage en mode fil de fer
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glutPostRedisplay();
    break;
  case 'p': //* affichage du carre plein
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glutPostRedisplay();
    break;
  case 's': //* Affichage en mode sommets seuls
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    glutPostRedisplay();
    break;
  case 'l': //*la touche 'q' permet de quitter le programme
    exit(0);
  }
}
void mouse(int button, int state, int x, int y)
{
  mouseX = x;
  mouseY = y;

  if (button == GLUT_LEFT_BUTTON)
  {
    if (state == GLUT_DOWN)
    {
      mouseLeftDown = true;
    }
    else if (state == GLUT_UP)
      mouseLeftDown = false;
  }

  else if (button == GLUT_RIGHT_BUTTON)
  {
    if (state == GLUT_DOWN)
    {
      mouseRightDown = true;
    }
    else if (state == GLUT_UP)
      mouseRightDown = false;
  }

  else if (button == GLUT_MIDDLE_BUTTON)
  {
    if (state == GLUT_DOWN)
    {
      mouseMiddleDown = true;
    }
    else if (state == GLUT_UP)
      mouseMiddleDown = false;
  }
}

void mouseMotion(int x, int y)
{
  if (mouseLeftDown)
  {
    cameraAngleY += (x - mouseX);
    cameraAngleX += (y - mouseY);
    mouseX = x;
    mouseY = y;
  }
  if (mouseRightDown)
  {
    cameraDistance += (y - mouseY) * 0.2f;
    mouseY = y;
  }

  glutPostRedisplay();
}
