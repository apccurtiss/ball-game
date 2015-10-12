#ifndef terrain
#define terrain

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <GL/glut.h>
int aone = 1;
float alpha = 0.75;
int inc = 10;

#define Cos(th) cos(3.1415927/180*(th))
#define Sin(th) sin(3.1415927/180*(th))

static void blok(float pos[]) {
   float x = pos[0];
   float y = pos[1];
   float z = pos[2];
   float d = pos[4];
   
   glEnable(GL_BLEND);
      glColor4f(1,1,1,alpha);
      if (aone)
         glBlendFunc(GL_SRC_ALPHA,GL_ONE);
      else
         glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glDepthMask(0);
      glEnable(GL_TEXTURE_2D);
      
   glBegin(GL_QUADS);
   //glColor3f(0.5 , 0.5 , 0.5);
   
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
   
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_BLEND);
      glDepthMask(1);
      glDisable(GL_TEXTURE_2D);
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

/*
 *  Draw vertex in polar coordinates with normal
 *  Code for Vertex and ball functions shamelessly stolen from ex13.c
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void player(double x,double y,double z,double r)
{
    int th,ph;
   
    glPushMatrix();
    glTranslated(x,y,z);
    glScaled(r,r,r);
   
   
    glEnable(GL_BLEND);
    glColor4f(1,1,1,0.75);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(0);
    glEnable(GL_TEXTURE_2D);

   
    //glColor3f(.5,.5,1);
   
    for (ph=-90;ph<90;ph+=inc)
    {
        glBegin(GL_QUAD_STRIP);
        for (th=0;th<=360;th+=2*inc)
        {
            Vertex(th,ph);
            Vertex(th,ph+inc);
        }
        glEnd();
    }
    
    glDisable(GL_BLEND);
    glDepthMask(1);
    glDisable(GL_TEXTURE_2D);
    //  Undo transofrmations
    glPopMatrix();
}