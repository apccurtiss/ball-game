/*
 *  Alexander Curtiss is awesome
 */
#include "CSCIx229.h"
#include "terrain.h"

//  Width and height, used for calculating point size
int w = 600;
int h = 600;
int win=0;        //  Used to determine if game is won
int starting = 1; //  Used to determine if game has just begun
int axes=1;       //  Display axes
int mode=1;       //  Projection mode
int move=1;       //  Move light
int th=0;         //  Azimuth of view angle
int ph=-45;       //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int light=1;      //  Lighting
double asp=1;     //  Aspect ratio
double dim=3.0;   //  Size of world

// Used to tell when keys are pressed
int up = 0, down = 0, left = 0, right = 0;
int emission =   0;  // Emission intensity (%)
int ambient  =  30;  // Ambient intensity (%)
int diffuse  = 100;  // Diffuse intensity (%)
int specular =   0;  // Specular intensity (%)
float viewy  =  .6;  // Elevation of light
//  Textures
unsigned int rockTex;
unsigned int tntTex;
unsigned int ballTex;
unsigned int starTex;
float acc    = 0; //  Ball vertical motion
float deform = 100; //  Ball deformation factor
float charsize = 0.2; //  Ball size
float charpos[]={0,2,0,0.2}; //  Ball position
//  Position of animations
float collidepos[]={0,0,0};
float explodepos[]={0,0,0};
float tick  = 0; //  Game tick
//  Frames for animations - they start at different values for... reasons
int collidetick = 1000;
int explodetick = 100;
int wintick     = 0;
int deformtick  = 0;

typedef struct
{
   char type; //  Type of block
   float pos[4]; //  x, y, z, radius
   float prop[3]; //  Only the first one is used to deal with shrinking... maybe more will be added later
} block;

//  Used to generate the map
block terrain[] = {
//  Type, Dimensions, Properties
   {'s', {-4,7,-4,0.5}, {0,0,0}}, //  The star block is always the first one, for... reasons.
   {'b', {0,0,0,0.5}, {0,0,0}}, //  'b', "for block"
   {'b', {1,0,0,0.5}, {0,0,0}},
   {'b', {2,0,-2,1}, {0,0,0}},
   {'b', {0,1,1,0.5}, {0,0,0}},
   {'b', {1,0,1,0.5}, {0,0,0}},
   {'b', {2,0,0,0.5}, {0,0,0}},
   {'b', {4,0,0,0.25}, {0,0,0}},
   {'t', {6,-0.5,0,0.5}, {0,0,0}}, //  't' for "TNT block"
   {'t', {6,-0.5,1,0.5}, {0,0,0}},
   {'b', {7.75,5,0,0.75}, {0,0,0}},
   {'b', {7.75,5,1.5,0.75}, {0,0,0}},
   {'b', {4,3.5,-1.5,0.5}, {0,0,0}},
   {'b', {5,4,-2,0.5}, {0,0,0}},
   {'b', {4,4,-3,0.25}, {0,0,0}},
   {'b', {3,4,-3,0.5}, {0,0,0}},
   {'t', {3,4.5,-4,0.5}, {0,0,0}},
   {'t', {3,3.5,-4,0.5}, {0,0,0}},
   {'t', {3,3,-3,0.5}, {0,0,0}},
   {'b', {1.5,7,-3,1}, {0,0,0}},
   {'b', {-0.5,6.5,-2,0.5}, {0,0,0}},
   {'b', {-2,6,-2.5,1}, {0,0,0}},
};
int terrainsize = sizeof(terrain) / sizeof(*terrain); //  Used so often, it helps to figure it out now

void display()
{
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   glLoadIdentity();
   
   //  Deal with viewing from a first person perspective
   viewy = 0.6 + charpos[1] * 0.5;
   if(mode && !starting)
      gluLookAt(charpos[0] - Cos(th) * 2,charpos[1] + viewy * 1.5,charpos[2] - Sin(th) * 2, charpos[0],charpos[1],charpos[2], 0,1,0);
   else
      gluLookAt(-Sin(th) * Cos(ph) * 10,-Sin(ph) * 10,-Cos(th) * Cos(ph) * 10, 0,0,0, 0,Cos(ph),0);
   
   glShadeModel(GL_SMOOTH);
   
   //  Fade when the player falls off
   if(charpos[1] < 0) {
       ambient = ambient + charpos[1];
       diffuse = diffuse + charpos[1];
       specular = specular + charpos[1];
   }
   
   //  Fade when the game is over
   ambient = ambient - (wintick / 100.);
   diffuse = diffuse - (wintick / 100.);
   specular = specular - (wintick / 100.);
   ambient = ambient > 0?ambient:0;
   diffuse = diffuse > 0?diffuse:0;
   specular = specular > 0?specular:0;
   
   float Ambient[]  = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
   float Diffuse[]  = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
   float Specular[] = {0.01*specular,0.01*specular,0.01*specular,1.0};
   float Position[] = {charpos[0],charpos[1],charpos[2],1.0};
   float white[]    = {1,1,1,1};
   glColor3f(1,1,1);
   glEnable(GL_LIGHT0);
   glEnable(GL_NORMALIZE);
   glEnable(GL_LIGHTING);
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);
   glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
   glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
   glLightfv(GL_LIGHT0,GL_POSITION,Position);
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,32);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   
   //  Draw the blocks
   int k;
   for(k=0; k < terrainsize; k++) {
       if(terrain[k].type == 'b' && (terrain[k].pos[3] - terrain[k].prop[0]) > 0.1) {
           blok(terrain[k].pos, terrain[k].prop,rockTex);
       } else if(terrain[k].type == 't' && !terrain[k].prop[0]) {
            blok(terrain[k].pos, terrain[k].prop,tntTex);
       } else if(terrain[k].type == 's' && !terrain[k].prop[0]) {
            blok(terrain[k].pos, terrain[k].prop,starTex);
       }
   }
   
   //  Draw the player
   player(charpos[0], charpos[1], charpos[2], charsize, th, deform, ballTex);
   glDisable(GL_LIGHTING);
   glDisable(GL_NORMALIZE);
   glDisable(GL_LIGHT0);
   
   //  Display any text
   glBindTexture(GL_TEXTURE_2D, 0);
   if (charpos[1] < -5 || wintick > 100)
   {  
      glColor3f(1,1,1);
      glWindowPos2i((w/2)-50, (h/2)+20);
      Print(win?"You win!":"Game over!");
      glWindowPos2i((w/2)-100, h/2);
      Print("Press spacebar to restart");
   }
   
   if (starting)
   {  
      glColor3f(1,1,1);
      glWindowPos2i(w / 2 - 40,h / 4);
      Print("Press any key to begin.");
      glWindowPos2i(w / 2 - 30,h / 4 - 20);
      Print("Arrow keys to move.");
      glWindowPos2i(w / 2 - 50,h / 4 - 40);
      Print("Find the star block to win!");
      
   }
   
   //  Draw explosions and debris animations
   int size = (min(w,h) / 20) - collidetick - (!mode * 35);
   size = size > 1?size:1;
   if(collidetick < 100)
      debris(collidepos, collidetick, size);
   if(explodetick < 45)
      explosion(explodepos, explodetick * 2, 0);
   if(wintick > 0 && wintick < 180)
      explosion(terrain[0].pos, wintick / 2, 1);
   
   //  Clean up
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 * Controls game actions such as collisions and motion
 */
void idle()
{
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   int obstructedx = 0, obstructedz = 0;
   //  Used to cap game speed - only tick if it's been 0.01 second since the last tick.
   if(t > tick + .01) {
      tick = t;
      //  Used to control animations and other events
      collidetick++; //  Collision with normal block
      explodetick++; //  Collision with exploding block
      if(win)
         wintick++;  //  When game ends
      if (deformtick <= 100) {
         deformtick++;  //  When the ball collides, have it bounce slightly
         deformtick %= 360;
         if(win)
            //  Bounce slightly less if game is over
            deform = Cos(deformtick * 10.8) * (100 - deformtick) / 2000; 
         else
            deform = Cos(deformtick * 10.8) * (100 - deformtick) / 600;
      }
       
      //  Slowly rotate if the game has just started.
      if(starting)
         th++;
      
      float step = 0.035;  //  How far to move per tick
      int k;
      //  Checks horizontal clipping
      if (up || down) {
         for(k=0; k < terrainsize; k++) {
            float bdim=terrain[k].pos[3] - terrain[k].prop[0];
            //  Only clip on blocks or TNT blocks.
            if(((terrain[k].type == 'b' && bdim > 0.1)
            || (terrain[k].type == 't' && !terrain[k].prop[0]))
            && fabs(terrain[k].pos[1] - charpos[1]) < bdim + charsize) //  Checks y-axis
            {
               //  Checks for x-axis clipping
               if(fabs(terrain[k].pos[0] - charpos[0]) > bdim + charsize
               && fabs(terrain[k].pos[0] - charpos[0] - Cos(th)*step) < bdim + charsize
               && fabs(terrain[k].pos[2] - charpos[2]) <= bdim + charsize)
                  obstructedx = 1;
               //  Checks for z-axis clipping
               if(fabs(terrain[k].pos[2] - charpos[2]) > bdim + charsize
               && fabs(terrain[k].pos[2] - charpos[2] - Sin(th)*step) < bdim + charsize
               && fabs(terrain[k].pos[0] - charpos[0]) <= bdim + charsize)
                  obstructedz = 1;
            }
         }
      }
      
      //  Only move if not clipping and in first-person mode
      if (right) {
         if(!starting && mode && !win) {
            th += 2;
         } else {
            th += 2;
         }
      } if (left) {
         if(!starting && mode && !win) {
            th -= 2;
         } else {
            th -= 2;
         }
      } if (up) {
         if(!starting && mode && !win) {
            if(!obstructedx)
               charpos[0] += Cos(th)*step;
            if(!obstructedz)
               charpos[2] += Sin(th)*step;
         } else {
            ph -= 2;
         }
      }
      if (down) {
         if(!starting && mode && !win) {
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
      //  Used to determine vertical clipping... only while going downward.
      for(k=0; k < terrainsize; k++) {
         //  Dimension of block - don't ask why it's so complecated
         float bdim=terrain[k].pos[3] - terrain[k].prop[0];
         //  This part isn't actually nessessary, but might be in the future and can't hurt...
         if((terrain[k].type == 'b' && bdim > 0.1)
         || (terrain[k].type == 't' && !terrain[k].prop[0])
         || (terrain[k].type == 's' && !terrain[k].prop[0])) {
            //  Checks that the ball is over the block
            if(fabs(charpos[0] - terrain[k].pos[0]) <= bdim + charsize
            && fabs(charpos[2] - terrain[k].pos[2]) <= bdim + charsize)
            {
               //  Checks if the ball is above the block now, but if it will be below after it moves
               if(charpos[1] - charsize >= terrain[k].pos[1] + bdim
               && charpos[1] - charsize + acc <= terrain[k].pos[1] + bdim)
               {
                  //  Only destroy blocks in first-person mode
                  if(!starting && mode)
                     terrain[k].prop[0] = terrain[k].prop[0] + 0.15;
                  //  Set ball to be slightly above block to avoid... problems.
                  charpos[1] = terrain[k].pos[1] + bdim + charsize + 0.01;
                  //  Bounce a little on a block, and create a collision
                  if(terrain[k].type == 'b'){
                     collidepos[0] = charpos[0];
                     collidepos[1] = terrain[k].pos[1] + bdim;
                     collidepos[2] = charpos[2];
                     collidetick = 0;
                     acc = 0.05;
                  }
                  //  Bounce a lot on a bomb, and create an explosion
                  if(terrain[k].type == 't'){
                     explodepos[0] = terrain[k].pos[0];
                     explodepos[1] = terrain[k].pos[1];
                     explodepos[2] = terrain[k].pos[2];
                     explodetick = 0;
                     acc = 0.13;
                  }
                  //  Win if you bounce on a star block
                  if(terrain[k].type == 's')
                     win=1;
                  //  Deform the ball
                  deformtick = 0;
                  break;
               }
               //  Checks collision on bottom - bounce off of blocks, win on the final block
               if(charpos[1] + charsize >= terrain[k].pos[1] - bdim
               && charpos[1] + charsize - acc <= terrain[k].pos[1] - bdim)
               {
                  if(terrain[k].type == 's') {
                     win=1;
                     terrain[k].prop[0] = terrain[k].prop[0] + 0.15;
                  } else
                     acc = -acc;
                  break;
               }
            }
         }
      }
      acc -= 0.001;
      charpos[1] += acc / (1 + wintick);
      
   }
   glutPostRedisplay();
}

/*
 * Checks keydown (used in idle())
 */
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
   //  When the game starts, reset view
   if(starting) {
      charpos[1] = 2;
      starting = 0;
      th = 0;
   }
}

/*
 * Checks keyup (used in idle())
 */
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

/*
 * Checks keyup (used in idle())
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view on spacebar
   else if (ch==' ') {
      charpos[0] = 0;
      charpos[1] = 1.7;
      charpos[2] = 0;
      ambient    = 30;
      diffuse    = 100;
      specular   = 0;
      acc        = 0;
      if(mode)
         th = 0;
      win = 0;
      wintick = 0;
      int k;
      for(k=0; k < terrainsize; k++)
         terrain[k].prop[0] = 0;
   }
   //  Change mode on m
   else if (ch=='m') {
      mode = 1-mode;
      th = th * (1 - mode);
      ph = -45;
      th = 135;
   }
   //  When the game starts, reset view
   if(starting) {
      charpos[1] = 2;
      starting = 0;
      th = 0;
   }
}

/*
 * Taken from ex18, with very minor changes
 */
void reshape(int width,int height)
{
   w = width;
   h = height;
   asp = (height>0) ? (double)width/height : 1;
   glViewport(0,0, width,height);
   Project(fov,asp,dim);
   glutPostRedisplay();
}

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
   //  Loads textures
   rockTex = LoadTexBMP("rockTexture.bmp");
   ballTex = LoadTexBMP("ballTexture.bmp");
   tntTex = LoadTexBMP("tntTexture.bmp");
   starTex = LoadTexBMP("starTexture.bmp");
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   
   glutMainLoop();
   return 0;
}
