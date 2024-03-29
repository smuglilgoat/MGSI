// http://bluh.org/code-the-diamond-square-algorithm/

#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include <vector>
#include <sstream>
#include <armadillo>
#include <string>

using namespace std ;
using namespace arma;

void affichage(void);

void clavier(unsigned char touche, int x, int y);
void affiche_repere(void);

void mouse(int, int, int, int);
void mouseMotion(int, int);
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

void sampleSquare(int x, int y, int size, double value)
{
    int hs = size / 2;

    // a     b 
    //
    //    x
    //
    // c     d

    double a = sample(x - hs, y - hs);
    double b = sample(x + hs, y - hs);
    double c = sample(x - hs, y + hs);
    double d = sample(x + hs, y + hs);

    setSample(x, y, ((a + b + c + d) / 4.0) + value);

}

void sampleDiamond(int x, int y, int size, double value)
{
    int hs = size / 2;

    //   c
    //
    //a  x  b
    //
    //   d

    double a = sample(x - hs, y);
    double b = sample(x + hs, y);
    double c = sample(x, y - hs);
    double d = sample(x, y + hs);

    setSample(x, y, ((a + b + c + d) / 4.0) + value);
}

void DiamondSquare(int stepsize, double scale)
{

    int halfstep = stepsize / 2;

    for (int y = halfstep; y < h + halfstep; y += stepsize)
    {
        for (int x = halfstep; x < w + halfstep; x += stepsize)
        {
            sampleSquare(x, y, stepsize, frand() * scale);
        }
    }

    for (int y = 0; y < h; y += stepsize)
    {
        for (int x = 0; x < w; x += stepsize)
        {
            sampleDiamond(x + halfstep, y, stepsize, frand() * scale);
            sampleDiamond(x, y + halfstep, stepsize, frand() * scale);
        }
    }

}

void triangle(rowvec a, rowvec b, rowvec c){
    glBegin(GL_TRIANGLES);
    glVertex3f(a(0), a(1), a(2));
    glVertex3f(b(0), b(1), b(2));
    glVertex3f(c(0), c(1), c(2));
    glEnd();
}



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

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (GLfloat)200 / (GLfloat)200, 0.1f, 10.0f);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0., 0., 4., 0., 0., 0., 0., 1., 0.);
}

//------------------------------------------------------

void displayCourbe(void)
{
}

int main(int argc, char **argv)
{
  /* initialisation de glut et creation
     de la fenetre */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB);
  glutInitWindowPosition(200, 200);
  glutInitWindowSize(600, 600);
  glutCreateWindow("TP3 - Fractals");

  /* Initialisation d'OpenGL */
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glColor3f(1.0, 1.0, 1.0);
  glPointSize(1.0);

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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

  case 'q': //*la touche 'q' permet de quitter le programme
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
