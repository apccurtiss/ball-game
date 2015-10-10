#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <GL/glut.h>

static void blok(float pos[]) {
   float x = pos[0];
   float y = pos[1];
   float z = pos[2];
   glBegin(GL_QUADS);
   glColor3f(0.5 , 0.5 , 0.5);
   
   // Cap on the bottom
   glNormal3d(0,-1,0);
   glVertex3d(x - 0.5, y - 0.5, z + 0.5);
   glVertex3d(x + 0.5, y - 0.5, z + 0.5);
   glVertex3d(x + 0.5, y - 0.5, z - 0.5);
   glVertex3d(x - 0.5, y - 0.5, z - 0.5);
   
   glNormal3d(0,1,0);
   glVertex3d(x - 0.5, y + 0.5, z + 0.5);
   glVertex3d(x + 0.5, y + 0.5, z + 0.5);
   glVertex3d(x + 0.5, y + 0.5, z - 0.5);
   glVertex3d(x - 0.5, y + 0.5, z - 0.5);
   
   // Sides
   glNormal3d(0,0,1);
   glVertex3d(x - 0.5, y - 0.5, z + 0.5);
   glVertex3d(x - 0.5, y + 0.5, z + 0.5);
   glVertex3d(x + 0.5, y + 0.5, z + 0.5);
   glVertex3d(x + 0.5, y - 0.5, z + 0.5);
   
   glNormal3d(0,0,-1);
   glVertex3d(x + 0.5, y - 0.5, z - 0.5);
   glVertex3d(x + 0.5, y + 0.5, z - 0.5);
   glVertex3d(x - 0.5, y + 0.5, z - 0.5);
   glVertex3d(x - 0.5, y - 0.5, z - 0.5);
   
   glNormal3d(1,0,0);
   glVertex3d(x + 0.5, y - 0.5, z - 0.5);
   glVertex3d(x + 0.5, y + 0.5, z - 0.5);
   glVertex3d(x + 0.5, y + 0.5, z + 0.5);
   glVertex3d(x + 0.5, y - 0.5, z + 0.5);
   
   glNormal3d(-1,0,0);
   glVertex3d(x - 0.5, y - 0.5, z - 0.5);
   glVertex3d(x - 0.5, y + 0.5, z - 0.5);
   glVertex3d(x - 0.5, y + 0.5, z + 0.5);
   glVertex3d(x - 0.5, y - 0.5, z + 0.5);
   glEnd();
}