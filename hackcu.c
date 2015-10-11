/*
 *  Alexander Curtiss is awesome
 */
 
#include "objects.h"
#define GL_GLEXT_PROTOTYPES


int axes=1;       //  Display axes
int mode=1;       //  Projection mode
int move=1;       //  Move light
int th=0;         //  Azimuth of view angle
int ph=45;        //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int light=1;      //  Lighting
double asp=1;     //  Aspect ratio
double dim=3.0;   //  Size of world

// Light values
int one       =   1;  // Unit value
int inc       =  10;  // Ball increment
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int zh        =  90;  // Light azimuth
float ylight  =   .6;  // Elevation of light
float terrain[][5] = {{1,4,2,2,1},{0,0,0,0,.5},{1,0,0,0,.5},{1,0,1,0,.5},{1,1,1,0,.5},{2,4,4,0,.5},{1,2,3,0,.5},{0,4,0,0,.5},{3,7,1,0,.5}};
int terrainsize = sizeof(terrain) / sizeof(*terrain);
float charsize = 0.2;
float charpos[]={0,1.7,0,0.2};
float acc    = 0;
float tick  = 0;


#define Cos(th) cos(3.1415927/180*(th))
#define Sin(th) sin(3.1415927/180*(th))

/*
 * ErrCheck, Print, and Project functions taken from previous assignments, and reused verbatum
 */
void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}

void Print(const char* format , ...)
{
   char    buf[8192];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,8192,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

void Project(double fov,double asp,double dim)
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (fov)
      gluPerspective(fov,asp,dim/16,16*dim);
   //  Orthogonal transformation
   else
      glOrtho(-asp*dim,asp*dim,-dim,+dim,-dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
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
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,1);
   //  Bands of latitude
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
   //  Undo transofrmations
   glPopMatrix();
}



void display()
{
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   glLoadIdentity();
   
   gluLookAt(charpos[0] - Cos(th) * 2,charpos[1] + ylight * 1.5,charpos[2] - Sin(th) * 2, charpos[0],charpos[1],charpos[2], 0,Cos(ph),0);
   
   glShadeModel(GL_SMOOTH);

   //  Translate intensity to color vectors
   float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
   float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
   float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
   glColor3f(1,1,1);
   glEnable(GL_LIGHT0);
   
   glEnable(GL_NORMALIZE);
   glEnable(GL_LIGHTING);
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,0);
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);
   glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
   glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
   
   player(charpos[0], charpos[1], charpos[2], charsize);
   int k;
   for(k=0; k < terrainsize; k++) {
       if(terrain[k][3] == 0) {
           blok(terrain[k]);
       } else if(terrain[k][3] == 2) {
           hoop(terrain[k]);
       }
       
   }
   glDisable(GL_LIGHT0);
   //  Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",
     th,ph,dim,fov,mode?"Perpective":"Orthogonal");
   if (light)
   {
      glWindowPos2i(5,25);
      Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Elevation=%.1f",ambient,diffuse,specular,emission, ylight);
   }

   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when the window is resized
 *  Taken verbatum from ex13.
 */
void idle()
{

   int k;
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   if(t > tick + 0.01) {
       tick = t;
       for(k=0; k < terrainsize; k++) {
           if(terrain[k][3] < 2
           &&fabs(charpos[0] - terrain[k][0]) <= terrain[k][4]
           && charpos[1] < terrain[k][1] + 0.7
           && charpos[1] > terrain[k][1] + 0.6
           && fabs(charpos[2] - terrain[k][2]) <= terrain[k][4])
           {
               acc = -acc;
               terrain[k][4] = terrain[k][4] * 0.9;
               charpos[1] += 0.5;
               
               break;
           }
           else 
           {
               //printf("%f %f\n", acc, t);
               if (acc < 0.5) {
                   acc = acc - 0.00001;
               }
               
           }
           
           charpos[1] += acc;
       }
   }
   
   
   zh = fmod(90*t,360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   int direction=Cos(ph)>0?1:-1;
   float step = 0.1;
   
   if (key == GLUT_KEY_RIGHT) {
      th += 5;
   } else if (key == GLUT_KEY_LEFT) {
      th -= 5;
   } else if (key == GLUT_KEY_UP) {
      charpos[0] += Cos(th)*step*direction;
      charpos[2] += Sin(th)*step*direction;
   } else if (key == GLUT_KEY_DOWN) {
      charpos[0] -= Cos(th)*step*direction;
      charpos[2] -= Sin(th)*step*direction;
   } else if (key == GLUT_KEY_PAGE_DOWN) {
      dim += 0.1;
   } else if (key == GLUT_KEY_PAGE_UP && dim>1) {
      dim -= 0.1;
   } else if (key == 32)
      move = 1 - move;
  
   th %= 360;
   ph %= 360;
   Project(mode?fov:0,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Switch mode
   else if (ch == 'm' || ch == 'M')
      mode = 1-mode;
   else if (ch=='[')
      ylight -= 0.1;
   else if (ch==']')
      ylight += 0.1;
   //  Ambient level
   else if (ch==' ') {
      charpos[0] = 0;
      charpos[1] = 1.7;
      charpos[2] = 0;
   }
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   else if (ch == ' ')
      move = 1 - move;
   //  Move light
   else if (ch == '<' || ch == ',')
      zh += 1;
   else if (ch == '>' || ch == '.')
      zh -= 1;
   //  Reproject
   Project(mode?fov:0,asp,dim);
   glutIdleFunc(move?idle:NULL);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 *  Taken directly from ex13.
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(mode?fov:0,asp,dim);
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glEnable(GL_CULL_FACE);
   glutInitWindowSize(600,600);
   glutCreateWindow("Alexander Curtiss");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   
   glutMainLoop();
   return 0;
}
