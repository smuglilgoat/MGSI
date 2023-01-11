#include "ifs.h"
#include <GL/glut.h>
#include <iostream>
// #include "../armadillo/include/armadillo"


using namespace arma;
using namespace std;

void triangle(rowvec a, rowvec b, rowvec c){
    glBegin(GL_TRIANGLES);
    glVertex3f(a(0), a(1), a(2));
    glVertex3f(b(0), b(1), b(2));
    glVertex3f(c(0), c(1), c(2));
    glEnd();
}

Ifs::Ifs(void)
{
    mPrimitive.eye(3, 3);
}

Ifs::~Ifs(void)
{
}

void Ifs::display(int level)
{
    mat t0 = {
        {1, 0.5, 0},
        {0.5, 0.5, 0},
        {0, 0, 0.5}
    };
    
    triangle(mPrimitive.row(0) * (t0 * mPrimitive),
             mPrimitive.row(1) * (t0 * mPrimitive),
             mPrimitive.row(2) * (t0 * mPrimitive));
}

void Ifs::ComputeApproximation() // il faut peut être mettre des parametres
{


}
