/*
 *  Alexander Curtiss is awesome
 */
#include "CSCIx229.h"
#include "terrain.h"


int win=0;
int axes=1;       //  Display axes
int mode=1;       //  Projection mode
int move=1;       //  Move light
int th=0;         //  Azimuth of view angle
int ph=45;        //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int light=1;      //  Lighting
double asp=1;     //  Aspect ratio
double dim=3.0;   //  Size of world
int w = 600;
int h = 600;

// Light values
int up = 0, down = 0, left = 0, right = 0;
int one       =   1;  // Unit value
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
float viewy  =   .6;  // Elevation of light
unsigned int rocktex;
unsigned int tnttex;
unsigned int balltex;
unsigned int hooptex;
float charsize = 0.2;
float charpos[]={0,2,0,0.2};
float collidepos[]={0,0,0};
int collidetick = 1000;
float explodepos[]={0,0,0};
int explodetick = 100;
float deform = 100;
int deformtick = 0;
float acc    = 0;
float tick  = 0;

typedef struct
{
   char type;
   float pos[4]; // x, y, z, radius
   float prop[3];
} block;

block terrain[] = {
//  Type, Dimensions, Properties
   {'b', {0,0,0,0.5}, {0,0,0}},
   {'b', {1,0,0,0.5}, {0,0,0}},
   {'b', {1,0,1,0.5}, {0,0,0}},
   {'b', {2,1,0,1}, {0,0,0}},
   {'b', {1,2,0,0.5}, {0,0,0}},
   {'b', {1,0,-1,0.5}, {0,0,0}},
   {'b', {2,0,0,0.5}, {0,0,0}},
   {'b', {2,0.5,1,0.5}, {0,0,0}},
   {'b', {3,0,3,1}, {0,0,0}},
   {'t', {0,0,1,0.5}, {0,0,0}},
   {'t', {4,1,4,1}, {0,0,0}},
};
int terrainsize = sizeof(terrain) / sizeof(*terrain);

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

void display()
{
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   glLoadIdentity();
   viewy = 0.6 + charpos[1] * 0.5;
   if(mode)
      gluLookAt(charpos[0] - Cos(th) * 2,charpos[1] + viewy * 1.5,charpos[2] - Sin(th) * 2, charpos[0],charpos[1],charpos[2], 0,Cos(ph),0);
   else
      gluLookAt(-Sin(th) * Cos(ph) * 5,-Sin(ph) * 5,-Cos(th) * Cos(ph) * 5, 0,0,0, 0,Cos(ph),0);
   
   glShadeModel(GL_SMOOTH);

   if(charpos[1] < 0) {
       ambient = ambient + charpos[1];
       diffuse = diffuse + charpos[1];
       specular = specular + charpos[1];
   }
   float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
   float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
   float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
   float white[]     = {1,1,1,1};
   float Position[]  = {charpos[0],charpos[1],charpos[2],1.0};
   glColor3f(1,1,1);
   glEnable(GL_LIGHT0);
   glEnable(GL_NORMALIZE);
   glEnable(GL_LIGHTING);
   //glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,0);
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);
   glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
   glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
   glLightfv(GL_LIGHT0,GL_POSITION,Position);
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,32);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   
   int k;
   win = 1;
   for(k=0; k < terrainsize; k++) {
       if(terrain[k].type == 'b' && (terrain[k].pos[3] - terrain[k].prop[0]) > 0.1) {
           win = 0;
           blok(terrain[k].pos, terrain[k].prop,rocktex);
       } else if(terrain[k].type == 't') {
           win = 0;
           if(!terrain[k].prop[0])
              blok(terrain[k].pos, terrain[k].prop,tnttex);
       }
   }
    // << default texture object
   player(charpos[0], charpos[1], charpos[2], charsize, th, deform, balltex);
   glDisable(GL_LIGHTING);
   glDisable(GL_NORMALIZE);
   glDisable(GL_LIGHT0);
   //  Display parameters
   
   glBindTexture(GL_TEXTURE_2D, 0);
   if (charpos[1] < -5)
   {  
      glColor3f(1,1,1);
      glWindowPos2i((w/2)-50, (h/2)+20);
      Print(win?"You win!":"Game over!");
      glWindowPos2i((w/2)-100, h/2);
      Print("Press spacebar to restart");
      
   }
   
   int size = (min(w,h) / 20) - collidetick - (!mode * 35);
   size = size > 1?size:1;
   if(collidetick < 100)
      debris(collidepos, collidetick, size);
   if(explodetick < 45)
      explosion(explodepos, explodetick * 2);

   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

void idle()
{
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   int obstructedx = 0, obstructedz = 0;
   if(t > tick + .01) {
      tick = t;
      collidetick++;
      explodetick++;
      if (deformtick <= 100) {
         deformtick++;
         deformtick %= 360;
         deform = Cos(deformtick * 10.8) * (100 - deformtick) / 200;
      }
       
      float step = 0.025;
        
      int k;
      if (up || down) {
         for(k=0; k < terrainsize; k++) {
            float bdim=terrain[k].pos[3] - terrain[k].prop[0];
            
            if(((terrain[k].type == 'b' && bdim > 0.1)
            || (terrain[k].type == 't' && !terrain[k].prop[0]))
            && fabs(terrain[k].pos[1] - charpos[1]) < bdim + charsize)
            {
               if(fabs(terrain[k].pos[0] - charpos[0]) > bdim + charsize
               && fabs(terrain[k].pos[0] - charpos[0] - Cos(th)*step) < bdim + charsize
               && fabs(terrain[k].pos[2] - charpos[2]) <= bdim + charsize) {
                  obstructedx = 1;
               }
               if(fabs(terrain[k].pos[2] - charpos[2]) > bdim + charsize
               && fabs(terrain[k].pos[2] - charpos[2] - Sin(th)*step) < bdim + charsize
               && fabs(terrain[k].pos[0] - charpos[0]) <= bdim + charsize) {
                  obstructedz = 1;
               }
                  
            }
         }
      }
      
      if (right) {
         if(mode) {
            th += 2;
         } else {
            th += 2;
         }
      } if (left) {
         if(mode) {
            th -= 2;
         } else {
            th -= 2;
         }
      } if (up) {
         if(mode) {
            if(!obstructedx)
               charpos[0] += Cos(th)*step;
            if(!obstructedz)
               charpos[2] += Sin(th)*step;
         } else {
            ph -= 2;
         }
      }
      if (down) {
         if(mode) {
            if(!obstructedx)
               charpos[0] -= Cos(th)*step;
            if(!obstructedz)
               charpos[2] -= Sin(th)*step;
         } else {
            ph += 2;
         }
      }
      
      ph %= 360;
      th %= 360;
      
      for(k=0; k < terrainsize; k++) {
         float bdim=terrain[k].pos[3] - terrain[k].prop[0];
         if((terrain[k].type == 'b' && bdim > 0.1)
         || (terrain[k].type == 't' && !terrain[k].prop[0])) {
            if(fabs(charpos[0] - terrain[k].pos[0]) <= bdim + charsize
            && fabs(charpos[2] - terrain[k].pos[2]) <= bdim + charsize)
            {
               if(charpos[1] - charsize >= terrain[k].pos[1] + bdim
               && charpos[1] - charsize + acc <= terrain[k].pos[1] + bdim)
               {
                   acc = (terrain[k].type == 'b'?0.05:0.13);
                   if(mode)
                     terrain[k].prop[0] = terrain[k].prop[0] + 0.15;
                   charpos[1] = terrain[k].pos[1] + bdim + charsize + 0.01;
                   if(terrain[k].type == 'b'){
                      collidepos[0] = charpos[0];
                      collidepos[1] = terrain[k].pos[1] + bdim;
                      collidepos[2] = charpos[2];
                      collidetick = 0;
                   }
                   if(terrain[k].type == 't'){
                      explodepos[0] = terrain[k].pos[0];
                      explodepos[1] = terrain[k].pos[1];
                      explodepos[2] = terrain[k].pos[2];
                      explodetick = 0;
                   }
                   deformtick = 0;
                   break;
               }
               if(charpos[1] + charsize >= terrain[k].pos[1] - bdim
               && charpos[1] + charsize - acc <= terrain[k].pos[1] - bdim)
               {
                  acc = -acc;
                  break;
               }
            }
         }
      }
      acc -= 0.001;
      charpos[1] += acc;
   }
   glutPostRedisplay();
}

void special(int key,int x,int y)
{   
   if (key == GLUT_KEY_RIGHT)
      right = 1;
   else if (key == GLUT_KEY_LEFT)
      left = 1;
   else if (key == GLUT_KEY_UP)
      up = 1;
   else if (key == GLUT_KEY_DOWN)
      down = 1;
}

void specialup(int key,int x,int y)
{
   if (key == GLUT_KEY_RIGHT) {
      right = 0;
   } else if (key == GLUT_KEY_LEFT) {
      left = 0;
   } else if (key == GLUT_KEY_UP) {
      up = 0;
   } else if (key == GLUT_KEY_DOWN) {
      down = 0;
   }
}

void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   else if (ch==' ') {
      charpos[0] = 0;
      charpos[1] = 1.7;
      charpos[2] = 0;
      ambient    = 30;
      diffuse    = 100;
      specular   = 0;
      acc        = 0;
      th         = 0;
      int k;
      for(k=0; k < terrainsize; k++)
         terrain[k].prop[0] = 0;
   }
   else if (ch=='m') {
      mode = 1-mode;
      ph = -45;
      th = 45;
   }
}

void reshape(int width,int height)
{
   w = width;
   h = height;
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(mode?fov:0,asp,dim);
   glutPostRedisplay();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_ALPHA);
   glEnable(GL_CULL_FACE);
   glutInitWindowSize(w,h);
   glutCreateWindow("Alexander Curtiss");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutSpecialUpFunc(specialup);
   glutKeyboardFunc(key);
   
   glutIdleFunc(idle);
   LoadTexBMP("test.bmp");
   rocktex = LoadTexBMP("rockTexture.bmp");
   balltex = LoadTexBMP("glass.bmp");
   tnttex = LoadTexBMP("tntTexture.bmp");
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   
   glutMainLoop();
   return 0;
}
