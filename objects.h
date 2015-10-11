#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <GL/glut.h>

#define Cos(th) cos(3.1415927/180*(th))
#define Sin(th) sin(3.1415927/180*(th))

static void blok(float pos[]) {
   float x = pos[0];
   float y = pos[1];
   float z = pos[2];
   float d = pos[4];
   glBegin(GL_QUADS);
   glColor3f(0.5 , 0.5 , 0.5);
   
   // Cap on the bottom
   glNormal3d(0,-1,0);
   glVertex3d(x - d, y - d, z + d);
   glVertex3d(x + d, y - d, z + d);
   glVertex3d(x + d, y - d, z - d);
   glVertex3d(x - d, y - d, z - d);
   
   glNormal3d(0,1,0);
   glVertex3d(x - d, y + d, z + d);
   glVertex3d(x + d, y + d, z + d);
   glVertex3d(x + d, y + d, z - d);
   glVertex3d(x - d, y + d, z - d);
   
   // Sides
   glNormal3d(0,0,1);
   glVertex3d(x - d, y - d, z + d);
   glVertex3d(x - d, y + d, z + d);
   glVertex3d(x + d, y + d, z + d);
   glVertex3d(x + d, y - d, z + d);
   
   glNormal3d(0,0,-1);
   glVertex3d(x + d, y - d, z - d);
   glVertex3d(x + d, y + d, z - d);
   glVertex3d(x - d, y + d, z - d);
   glVertex3d(x - d, y - d, z - d);
   
   glNormal3d(1,0,0);
   glVertex3d(x + d, y - d, z - d);
   glVertex3d(x + d, y + d, z - d);
   glVertex3d(x + d, y + d, z + d);
   glVertex3d(x + d, y - d, z + d);
   
   glNormal3d(-1,0,0);
   glVertex3d(x - d, y - d, z - d);
   glVertex3d(x - d, y + d, z - d);
   glVertex3d(x - d, y + d, z + d);
   glVertex3d(x - d, y - d, z + d);
   glEnd();
}

static void hoop(float pos[]) {
   float x = pos[0];
   float y = pos[1];
   float z = pos[2];
   float d = pos[4];
   
   glBegin(GL_QUAD_STRIP);
   int k;
   for (k=0;k<=360;k+=1)
   {
      if (d) {
          glVertex3d(x + Sin(k) / 2,y + Cos(k) / 2,z);
          glVertex3d(x + Sin(k) / 2,y + Cos(k) / 2,z + 0.2);
      } else {
          glVertex3d(x,y + Sin(k) / 2,z + Cos(k) / 2);
          glVertex3d(x + 0.2,y + Sin(k) / 2,z + Cos(k) / 2);
      }
   }
   glEnd();
}