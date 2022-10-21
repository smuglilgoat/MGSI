#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include <time.h>
#include <string.h>
#include <sstream>
#include <string>
#include <vector>

int width = 800;
int height = 600;
int b_spline = 0;
int bezier = 1;
int subdivisions = 10;

struct Point {
  float x;
  float y;
  float z;
  Point(float a, float b, float c) : x(a), y(b), z(c) {}
  Point() : x(0), y(0), z(0) {}
};

Point control_points[4 * 4] = {
    Point(0, 0, 0),       Point(0.3, -0.5, 0),  Point(0.8, 0, 0),
    Point(1.1, 0.5, 0),     Point(0, 0, 0),       Point(0.3, 0.3, 0.3),
    Point(0.6, 0.4, 0.3), Point(1, 0.5, 0.3),   Point(0, 0.5, 0.5),
    Point(0.5, 0.5, 0.5), Point(0.6, 0.4, 0.6), Point(1, 0, 0.6),
    Point(0.3, 0.4, 1.1),     Point(0.3, 0.4, 1),   Point(0.6, 0., 1),
    Point(1, 0.2, 1)};
;
Point points_bezier[4][50];
Point points_spline[4][50];

bool mouseLeftDown;
bool mouseRightDown;
bool mouseMiddleDown;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;
float cameraDistance = 0;

void draw_points() {
  glColor3f(0.0, 1.0, 0.0);
  glBegin(GL_POINTS);
  for (int i = 0; i < 16; i++) {
    glVertex3f(control_points[i].x, control_points[i].y, control_points[i].z);
  }
  glEnd();
  glColor3f(1.0, 1.0, 1.0);
}

void draw_bspline() {
  float b_spline[4][4] = {
      {-1, 3, -3, 1}, {3, -6, 3, 0}, {-3, 0, 3, 0}, {1, 4, 1, 0}};

  glColor3f(0.0, 0.0, 1.0);

  for (int courbe = 0; courbe < 4; courbe++) {
    Point spline_c[4];
    float x, y, z, xx, yy, zz;
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 4; i++) {
      x = y = z = 0;
      for (int j = 0; j < 4; j++) {
        x += 1.0 / 6.0 * b_spline[i][j] * control_points[j + (courbe * 4)].x;
        y += 1.0 / 6.0 * b_spline[i][j] * control_points[j + (courbe * 4)].y;
        z += 1.0 / 6.0 * b_spline[i][j] * control_points[j + (courbe * 4)].z;
      }
      spline_c[i] = Point(x, y, z);
    }
    float t = 0.1;
    float D[4];
    D[0] = 3 * pow(t, 2);
    D[1] = 2 * t;
    D[2] = 1;
    D[3] = 0;
    for (int j = 0; j <= subdivisions; j++) {
      x = y = z = xx = yy = zz = 0.0;
      for (int i = 0; i < 4; i++) {
        x += spline_c[i].x * pow(t, 3 - i);
        y += spline_c[i].y * pow(t, 3 - i);
        z += spline_c[i].z * pow(t, 3 - i);
        xx += spline_c[i].x * D[i];
        yy += spline_c[i].y * D[i];
        zz += spline_c[i].z * D[i];
      }
      t += 1.0 / subdivisions;
      glVertex3f(x, y, z);
      points_spline[courbe][j] = Point(x, y, z);
    }

    glEnd();
  }
}

void draw_bezier() {
  float bezier[4][4] = {
      {-1, 3, -3, 1}, {3, -6, 3, 0}, {-3, 3, 0, 0}, {1, 0, 0, 0}};

  glColor3f(0.0, 0.0, 1.0);

  for (int courbe = 0; courbe < 4; courbe++) {
    Point points_bezier_[4];
    float x, y, z, xx, yy, zz;
    glBegin(GL_LINE_STRIP);

    for (int i = 0; i < 4; i++) {
      x = y = z = 0;
      for (int j = 0; j < 4; j++) {
        x += bezier[i][j] * control_points[j + (courbe * 4)].x;
        y += bezier[i][j] * control_points[j + (courbe * 4)].y;
        z += bezier[i][j] * control_points[j + (courbe * 4)].z;
      }
      points_bezier_[i] = Point(x, y, z);
    }

    float t = 0.0;
    float D[4];
    D[0] = 3 * pow(t, 2);
    D[1] = 2 * t;
    D[2] = 1;
    D[3] = 0;

    for (int j = 0; j <= subdivisions; j++) {
      x = y = z = xx = yy = zz = 0.0;
      for (int i = 0; i < 4; i++) {
        x += points_bezier_[i].x * pow(t, 3 - i);
        y += points_bezier_[i].y * pow(t, 3 - i);
        z += points_bezier_[i].z * pow(t, 3 - i);

        xx += points_bezier_[i].x * D[i];
        yy += points_bezier_[i].y * D[i];
        zz += points_bezier_[i].z * D[i];
      }
      t += 1.0 / subdivisions;
      glVertex3f(x, y, z);
      points_bezier[courbe][j] = Point(x, y, z);
    }

    glEnd();
  }
}

void display(void) {
  glMatrixMode(GL_MODELVIEW);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
  glTranslatef(0, 0, cameraDistance);
  glRotatef(cameraAngleX, 1., 0., 0.);
  glRotatef(cameraAngleY, 0., 1., 0.);
  draw_points();
  if (bezier == 1) {
    draw_bezier();
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POINTS);
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < subdivisions; j++) {
        glVertex3f(points_bezier[i][j].x, points_bezier[i][j].y,
                   points_bezier[i][j].z);
      }
    }
    glEnd();
    glColor3f(1.0, 1.0, 1.0);
    float col = 0.0f;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < subdivisions; j++) {
        glBegin(GL_POLYGON);
        col += 1.0f / (3 * subdivisions);
        glColor3f(0.0, 0.0, col);
        glVertex3f(points_bezier[i][j].x, points_bezier[i][j].y,
                   points_bezier[i][j].z);
        glVertex3f(points_bezier[i + 1][j].x, points_bezier[i + 1][j].y,
                   points_bezier[i + 1][j].z);
        glVertex3f(points_bezier[i + 1][j + 1].x, points_bezier[i + 1][j + 1].y,
                   points_bezier[i + 1][j + 1].z);
        glVertex3f(points_bezier[i][j + 1].x, points_bezier[i][j + 1].y,
                   points_bezier[i][j + 1].z);

        glEnd();
      }
    }
  } else if (b_spline == 1) {
    draw_bspline();
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POINTS);
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < subdivisions; j++) {
        glVertex3f(points_spline[i][j].x, points_spline[i][j].y,
                   points_spline[i][j].z);
      }
    }
    glEnd();
    glColor3f(1.0, 1.0, 1.0);
    float col = 0.0f;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < subdivisions; j++) {
        glBegin(GL_POLYGON);
        col += 1.0f / (3 * subdivisions);
        glColor3f(0.0, 0.0, col);
        glVertex3f(points_spline[i][j].x, points_spline[i][j].y,
                   points_spline[i][j].z);
        glVertex3f(points_spline[i + 1][j].x, points_spline[i + 1][j].y,
                   points_spline[i + 1][j].z);
        glVertex3f(points_spline[i + 1][j + 1].x, points_spline[i + 1][j + 1].y,
                   points_spline[i + 1][j + 1].z);
        glVertex3f(points_spline[i][j + 1].x, points_spline[i][j + 1].y,
                   points_spline[i][j + 1].z);
        glEnd();
      }
    }
  }
  glPopMatrix();
  glFlush();
  glutSwapBuffers();
  glutPostRedisplay();
}

void key_down(unsigned char touche, int x, int y) {
  switch (touche) {
  case '+':
    if (subdivisions < 30)
      subdivisions += 1;
    break;
  case '-':
    if (subdivisions > 3)
      subdivisions -= 1;
    break;
  case 'q':
    bezier = 1 - bezier;
    break;
  case 'w':
    b_spline = 1 - b_spline;
    break;
  case 'x':
    exit(0);
  }
  glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
  mouseX = x;
  mouseY = y;

  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      mouseLeftDown = true;
    } else if (state == GLUT_UP)
      mouseLeftDown = false;
  }

  else if (button == GLUT_RIGHT_BUTTON) {
    if (state == GLUT_DOWN) {
      mouseRightDown = true;
    } else if (state == GLUT_UP)
      mouseRightDown = false;
  }

  else if (button == GLUT_MIDDLE_BUTTON) {
    if (state == GLUT_DOWN) {
      mouseMiddleDown = true;
    } else if (state == GLUT_UP)
      mouseMiddleDown = false;
  }
}

void mouseMotion(int x, int y) {
  if (mouseLeftDown) {
    cameraAngleY += (x - mouseX);
    cameraAngleX += (y - mouseY);
    mouseX = x;
    mouseY = y;
  }
  if (mouseRightDown) {
    cameraDistance += (y - mouseY) * 0.2f;
    mouseY = y;
  }

  glutPostRedisplay();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB);
  glutInitWindowPosition(200, 200);
  glutInitWindowSize(width, height);
  glutCreateWindow("tp3 spline nurb_spline");
  glColor3f(1.0, 1.0, 1.0);
  glPointSize(10.0);
  glutDisplayFunc(display);
  glutKeyboardFunc(key_down);
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMotion);
  glClearColor(1, 1, 1, 1);
  GLfloat l_pos[] = {0, 3.5, 3.0, 1.0};
  glLightfv(GL_LIGHT0, GL_POSITION, l_pos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, l_pos);
  glLightfv(GL_LIGHT0, GL_SPECULAR, l_pos);
  glEnable(GL_COLOR_MATERIAL);
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0f, (GLfloat)height / (GLfloat)width, 0.1f, 10000.0f);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(4., 2., 3., 0., 0., 0., 0., 1., 0.);
  glutMainLoop();
  return 0;
}