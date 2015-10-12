/*
 *  Alexander Curtiss is awesome
 */
#include "CSCIx229.h"
#include "terrain.h"



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
int zh        =  90;  // Light azimuth
float viewy  =   .6;  // Elevation of light
float terrain[][5] = {{1,4,2,2,1},{0,0,0,0,.5},{2,0,0,0,.5},{2,0,2,0,.5},{2,1,2,0,.5},{2,4,4,0,.5},{1,2,3,0,.5},{0,4,0,0,.5},{3,7,1,0,.5}};
unsigned int testtex;
int terrainsize = sizeof(terrain) / sizeof(*terrain);
float charsize = 0.2;
float charpos[]={0,2,0,0.2};
float acc    = 0;
float tick  = 0;


void ReadTerrainFile() {
    FILE *ifp, *ofp;
char *mode = "r";
char outputFilename[] = "out.list";

ifp = fopen("in.list", mode);

if (ifp == NULL) {
  fprintf(stderr, "Can't open input file in.list!\n");
  exit(1);
}

ofp = fopen(outputFilename, "w");

if (ofp == NULL) {
  fprintf(stderr, "Can't open output file %s!\n",
          outputFilename);
  exit(1);
}
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

void display()
{
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   glLoadIdentity();
   viewy = 0.6 + charpos[1] * 0.5;
   gluLookAt(charpos[0] - Cos(th) * 2,charpos[1] + viewy * 1.5,charpos[2] - Sin(th) * 2, charpos[0],charpos[1],charpos[2], 0,Cos(ph),0);
   
   glShadeModel(GL_SMOOTH);

   //  Translate intensity to color vectors
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
   for(k=0; k < terrainsize; k++) {
       if(terrain[k][3] == 0) {
           blok(terrain[k]);
       } else if(terrain[k][3] == 2) {
           hoop(terrain[k]);
       }
       
   }
   player(charpos[0], charpos[1], charpos[2], charsize, testtex);
   glDisable(GL_LIGHT0);
   //  Display parameters
   
   if (charpos[1] < -5)
   {  
      glColor3f(1 , 1 , 1);
      glWindowPos2i((w/2)-100, h/2);
      Print("Press spacebar to restart!");
      glWindowPos2i((w/2)-50, (h/2)+20);
      Print("Game over!");
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
    double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
    if(t > tick + 0.01) {
        tick = t;
       
        float step = 0.02;
   
        if (right) {
           th += 1;
        } else if (left) {
           th -= 1;
        } else if (up) {
           charpos[0] += Cos(th)*step;
           charpos[2] += Sin(th)*step;
        } else if (down) {
           charpos[0] -= Cos(th)*step;
           charpos[2] -= Sin(th)*step;
        }
      
        int k;
        for(k=0; k < terrainsize; k++) {
            if(
            terrain[k][3] < 2
            && fabs(charpos[0] - terrain[k][0]) <= terrain[k][4]
            && fabs(charpos[2] - terrain[k][2]) <= terrain[k][4]
            && charpos[1] - charsize >= terrain[k][1] + terrain[k][4]
            && charpos[1] - charsize + acc <= terrain[k][1] + terrain[k][4])
            {
                acc = -acc;
                acc = acc + 0.001;
                charpos[1] = charsize + terrain[k][1] + terrain[k][4] + 0.01;
                terrain[k][4] = terrain[k][4] * 0.9;
               
                break;
            }
        }
        acc -= 0.001;
        charpos[1] += acc;
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
   if (key == GLUT_KEY_RIGHT) {
      right = 1;
   } else if (key == GLUT_KEY_LEFT) {
      left = 1;
   } else if (key == GLUT_KEY_UP) {
      up = 1;
   } else if (key == GLUT_KEY_DOWN) {
      down = 1;
   }
  
   th %= 360;
   ph %= 360;
   Project(mode?fov:0,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
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
      viewy -= 0.1;
   else if (ch==']')
      viewy += 0.1;
   //  Ambient level
   else if (ch==' ') {
      charpos[0] = 0;
      charpos[1] = 1.7;
      charpos[2] = 0;
      ambient    = 30;
      diffuse    = 100;
      specular   = 0;
      acc        = 0;
   }
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
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
    w = width;
    h = height;
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
   testtex = LoadTexBMP("glass.bmp");
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   
   glutMainLoop();
   return 0;
}
