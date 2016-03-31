/******************************************************************************/
/******************      Luong Hoang           ********************************/
/****************** Ultimate Gaming Shootem up ********************************/
/******************************************************************************/

#include <stdlib.h>
#include <al/al.h>
#include <al/alc.h>
#include <al/alut.h>
#include <GL/glut.h>
#include <iostream>
#include "Bullets.h"

#define WIDTH 1024
#define HEIGHT 768
#define PI 3.14159

#define TRUE 1
#define FALSE 0

//These are for textures
#define NUM_TEXTURES 26
#define EARTH 0
#define VENUS 1
#define JUPITER 2
#define MARS 3
#define MERCURY 4
#define NEPTUNE 5
#define PLUTO 6
#define SUN 7
#define SATURN 8
#define URANUS 9
#define CROSSHAIR 10
#define GRASSTEXTURE 11
#define GUN_ROCKET 12
#define GUN_AUG 13
#define GUN_GLOCK 14
#define GUN_DEAGLE 15
#define GUN_MP5 16
#define GUN_AK47 17
#define GUN_AWP 18
#define GUN_SHOTGUN 19
#define GUN_G3SG1 20
#define GUN_M4A1 21
#define BLOODSPILLED 22
#define DEAD 23
#define WALL 24
#define SNOW 25

#define NUM_GUNS 10
#define NUM_HITS 30

// Maximum data buffers for loading multiple .wav files.
#define NUM_BUFFERS 20

// Maximum emissions.
#define NUM_SOURCES 20

// These index the sources.
#define MUSIC    0
#define MUSIC_2  1
#define MUSIC_3  2
#define MUSIC_ROCKET 3
#define MUSIC_AUG 4
#define MUSIC_GLOCK 5
#define MUSIC_DEAGLE 6
#define MUSIC_MP5 7
#define MUSIC_AK47 8
#define MUSIC_AWP 9
#define MUSIC_SHOTGUN 10
#define MUSIC_G3SG1 11
#define MUSIC_M4A1 12
#define MUSIC_RELOAD 13
#define MUSIC_EMPTYLOAD 14
#define MUSIC_EARTHHIT 15
#define MUSIC_VICTORY 16
#define MUSIC_PLANETINVASION 17
#define MUSIC_HURT 18
#define MUSIC_URDEAD 19

using namespace std;
int right_button_down = FALSE;
int left_button_down = FALSE;

int prevMouseX;
int prevMouseY;

double view_rotx = 0.0;
double view_roty = 0.0;
double view_rotz = 0.0;
double z_distance;

int texWidth[NUM_TEXTURES], texHeight[NUM_TEXTURES];
GLuint texName[NUM_TEXTURES];

int health=100, dir = 1;

bool attack, firstTime;

//declare parameters to the gluLookAt function call
double ex,ey=0.0,ez=5.0,rx,ry=0.0,rz,ux,uy=1.0,uz;

//these variables are of the saturn and uranus mapping size
double ringRadius=1.3, ringTubeRadius=0.1, zCoeff=0.1;

bool jumpFlag = false, drawEarth = true; 

//jumpI defines coordinates of ey,ry in jumping motion
//alphaText defines the level of blending of text
double jumpI = -2.5, alphaText=0.9;

//these two parameters are used to set level of difficulty
double diffX = 7.0, diffZ = 3.0;

//array to check which gun is picked
bool picked[NUM_GUNS], gotHit;

double dieAlpha=1.0, bloodSpilledAlpha;
/*haveGun is used to check if currently carrying a gun, victory determines     
whether objective has been completed, hit array is used to check whether each
bullet hit the desired destination, instruction is self-explaining*/
bool haveGun, victory, hit[30], pHit[9], instruction = true;

//ii,xx, timer are all variables used to track and change animations
int keyDown[256], ii, xx, xx2, timer, gunIndex, pickedGun, earthHit;

//declare variables to hold bullets instances and other bullets-related objects
int num_bullets = 30, bulletIndex = 0;
Bullets bullets[30], planetBullets[9];
double transBulletX[30], transBulletY[30], transBulletZ[30];
double transPBulletX[9], transPBulletY[9], transPBulletZ[9];

//bb array elements accumulate to simulate each bullet movement
double bb[30], pb[9];

//define speed for rotation and translation of camera
const double eyeSpeed = .05;

//tempRy is used to store the y value of lookat function in order to accurately
//simulate jumping
double tempRy;

//the following are to keep track of each planet's position
double earthX, earthY, earthZ;
double venusX, venusY, venusZ;
double jupiterX, jupiterY, jupiterZ;
double marsX, marsY, marsZ;
double mercuryX, mercuryY, mercuryZ;
double neptuneX, neptuneY, neptuneZ;
double plutoX, plutoY, plutoZ;
double sunX, sunY, sunZ;
double saturnX, saturnY, saturnZ;
double uranusX, uranusY, uranusZ;

//radius for planet
double rPlanet = 1.0;

//zoom
bool zoom = false;
double scaleX=1.0, scaleY=1.0, scaleZ=1.0;
int windowX, windowY, width=WIDTH, height=HEIGHT;

//declare variables to hold texture objects
unsigned char* texture[NUM_TEXTURES];

/******************************************************************************/
/* The OpenAL code implemented in this program has been modified from its     */
/* original which can be found at:                                            */
/* http://www.devmaster.net/articles/openal-tutorials/lesson3.php             */
/******************************************************************************/

// Buffers to hold sound data.
ALuint Buffers[NUM_BUFFERS];
// Sources are points of emitting sound.
ALuint Sources[NUM_SOURCES];
// Position of the source sounds.
ALfloat SourcesPos[NUM_SOURCES][3];
// Velocity of the source sounds.
ALfloat SourcesVel[NUM_SOURCES][3];
// Position of the Listener.
ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
// Velocity of the Listener.
ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
// Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
// Also note that these should be units of '1'.
ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };
/* LoadALData will load the sample song into buffer and store it as source */

ALboolean LoadALData(){
	// Variables to load into.
	ALenum format;
	ALsizei size;
	ALvoid* data;
	ALsizei freq;
	ALboolean loop;
	// Load wav data into a buffer.
	alGenBuffers(NUM_BUFFERS, Buffers);

	if(alGetError() != AL_NO_ERROR)
		return AL_FALSE;
/*This lengthy part is mainly for loading in .wav files to buffers*************/
	alutLoadWAVFile("wavdata/Running.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);

    alutLoadWAVFile("wavdata/Guitar.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_2], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq); 

    alutLoadWAVFile("wavdata/pickup.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_3], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);

	alutLoadWAVFile("wavdata/rocket.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_ROCKET], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);

	alutLoadWAVFile("wavdata/aug.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_AUG], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);

    alutLoadWAVFile("wavdata/glock.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_GLOCK], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq); 

    alutLoadWAVFile("wavdata/deagle.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_DEAGLE], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);

	alutLoadWAVFile("wavdata/mp5.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_MP5], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);

    alutLoadWAVFile("wavdata/ak47.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_AK47], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq); 

    alutLoadWAVFile("wavdata/awp.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_AWP], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);
	
	alutLoadWAVFile("wavdata/shotgun.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_SHOTGUN], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);

    alutLoadWAVFile("wavdata/g3sg1.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_G3SG1], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq); 

    alutLoadWAVFile("wavdata/m4a1.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_M4A1], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);

    alutLoadWAVFile("wavdata/reload.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_RELOAD], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);	
	
    alutLoadWAVFile("wavdata/emptyload.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_EMPTYLOAD], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);
	
    alutLoadWAVFile("wavdata/earthhit.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_EARTHHIT], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);	
	
    alutLoadWAVFile("wavdata/victory.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_VICTORY], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);

    alutLoadWAVFile("wavdata/planetBullet.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_PLANETINVASION], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);
    
    alutLoadWAVFile("wavdata/gotHit.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_HURT], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);
    
    alutLoadWAVFile("wavdata/die.wav", &format, &data, &size, &freq, &loop);
	alBufferData(Buffers[MUSIC_URDEAD], format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);	
/******************************************************************************/
	// Bind the buffer with the source.
	alGenSources(NUM_SOURCES, Sources);
    
	if(alGetError() != AL_NO_ERROR)
		return AL_FALSE;
  
    for(int i=0; i<NUM_SOURCES; i++){
	alSourcei (Sources[i], AL_BUFFER,   Buffers[i]       );
	alSourcef (Sources[i], AL_PITCH,    1.0              );
	alSourcef (Sources[i], AL_GAIN,     1.0              );
	alSourcefv(Sources[i], AL_POSITION, SourcesPos[i]    );
	alSourcefv(Sources[i], AL_VELOCITY, SourcesVel[i]    );
    if(i==1 || i==MUSIC_VICTORY)
	alSourcei (Sources[i], AL_LOOPING,  AL_TRUE          );
	else
	alSourcei (Sources[i], AL_LOOPING,  AL_FALSE         );
    }
	// Do another error check and return.
	if(alGetError() == AL_NO_ERROR)
		return AL_TRUE;

	return AL_FALSE;
}

/* This function tells OpenAL the values of certain data for the listener     */
void SetListenerValues()
{
	alListenerfv(AL_POSITION,    ListenerPos);
	alListenerfv(AL_VELOCITY,    ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);
}

/* This function frees the memory and buffer*/
void KillALData()
{
	alDeleteBuffers(NUM_BUFFERS, Buffers);
	alDeleteSources(NUM_SOURCES, Sources);
	alutExit();
}
/******************************************************************************/
/*********************END OF OPENAL IMPLEMENTATION*****************************/
/******************************************************************************/
/*********************START OF OPENGL IMPLEMENTATION***************************/
/******************************************************************************/
void drawString(char *s){
     int i;
     for (i=0; i<strlen(s); i++){
         glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
     }
}
/* glmReadPPM: read a PPM raw (type P6) file */
GLubyte* 
glmReadPPM(char* filename, int* width, int* height){
    FILE* fp;
    int i, w, h, d;
    unsigned char* image;
    char head[70];          /* max line <= 70 in PPM (per spec). */
    
    fp = fopen(filename, "rb");
    if (!fp) {
        printf("error opening file %s\n",filename);
        return NULL;
    }
    
    /* grab first two chars of the file and make sure that it has the
       correct magic cookie for a raw PPM file. */
    fgets(head, 70, fp);
    if (strncmp(head, "P6", 2)) {
        fprintf(stderr, "%s: Not a raw PPM file\n", filename);
        return NULL;
    }
    
    /* grab the three elements in the header (width, height, maxval). */
    i = 0;
    while(i < 3) {
        fgets(head, 70, fp);
        if (head[0] == '#')     /* skip comments. */
            continue;
        if (i == 0)
            i += sscanf(head, "%d %d %d", &w, &h, &d);
        else if (i == 1)
            i += sscanf(head, "%d %d", &h, &d);
        else if (i == 2)
            i += sscanf(head, "%d", &d);
    }
    
    /* grab all the image data in one fell swoop. */
    image = (unsigned char*)malloc(sizeof(unsigned char)*w*h*3);
    fread(image, sizeof(unsigned char), w*h*3, fp);
    fclose(fp);
    
    *width = w;
    *height = h;
    return image;
}


void sphericalMapping(int index){
  //declare variables for spherical mapping
  double x,y,z,s,t,step=.05;
  
/*These if statements determine unique rotation patterns for different planets*/
  if(index==EARTH && drawEarth == false)
  return;
  
  if(index==VENUS)
  glTranslatef(venusX,venusY,venusZ);
  
  if(index==JUPITER){
     glTranslatef(jupiterX, jupiterY, jupiterZ);
  }
  
  if(index==MARS){
     glTranslatef(marsX, marsY, marsZ);
  }
  
  if(index==MERCURY){
     glRotatef(-30,0,0,1);
     glTranslatef(mercuryX, mercuryY, mercuryZ);
  }
  
  if(index==NEPTUNE){
     glRotatef(-60,0,0,1);
     glTranslatef(neptuneX, neptuneY, neptuneZ);
  }
  
  if(index==PLUTO){
     glRotatef(30,0,0,1);
     glTranslatef(plutoX, plutoY, plutoZ);
  }
  
  if(index==SUN){
     glRotatef(10,0,0,1);
     glTranslatef(sunX, sunY, sunZ);
  }
  
  if(index==SATURN){
     glRotatef(60,0,0,1);
     glTranslatef(saturnX, saturnY, saturnZ);
  }
  
  if(index==URANUS){
     glRotatef(40,0,0,1);
     glTranslatef(uranusX, uranusY, uranusZ);
  }
/*****************************************************************************/
  glPushMatrix();
  glRotatef(xx,0,1,0);
  for(double t=0.0; t<1.0; t+=step)
    for(double s=0.0; s<1.0; s+=step){
      glBegin(GL_POLYGON);
      x = rPlanet*cos(2*PI*s)*sin(PI*t);
      y = rPlanet*sin(2*PI*s)*sin(PI*t);
      z = rPlanet*cos(PI*t);
      glTexCoord2f(s,t); glVertex3f(x,y,z);
      glNormal3f(x,y,z);
      
      x = rPlanet*cos(2*PI*(s))*sin(PI*(t+step));
      y = rPlanet*sin(2*PI*(s))*sin(PI*(t+step));
      z = rPlanet*cos(PI*(t+step));
      glTexCoord2f(s,t+step); glVertex3f(x,y,z);
      glNormal3f(x,y,z); 
      
      x = rPlanet*cos(2*PI*(s+step))*sin(PI*(t+step));
      y = rPlanet*sin(2*PI*(s+step))*sin(PI*(t+step));
      z = rPlanet*cos(PI*(t+step));
      glTexCoord2f(s+step,t+step); glVertex3f(x,y,z);
      glNormal3f(x,y,z); 

      x = rPlanet*cos(2*PI*(s+step))*sin(PI*t);
      y = rPlanet*sin(2*PI*(s+step))*sin(PI*t);
      z = rPlanet*cos(PI*t);
      glTexCoord2f(s+step,t); glVertex3f(x,y,z);
      glNormal3f(x,y,z); 
      glEnd();
    }
  glPopMatrix();
}


/*drawRing is used to map textures on to saturn's and uranus's rings, these 
  rings are mapped onto a torus with ringRadius be the radius from center of
  the hole to the outside of tube, ringTubeRadius is the radius of the tube   */
void drawRing(){
  double x,y,z,s,t,step=.05;
  for(double t=0.0; t<1.0; t+=step)
    for(double s=0.0; s<1.0; s+=step){
      glBegin(GL_POLYGON);
      x = (ringRadius + ringTubeRadius*cos(2*PI*s))*cos(2*PI*t);
      y = (ringRadius + ringTubeRadius*cos(2*PI*s))*sin(2*PI*t);
      z = zCoeff * sin(2*PI*t);
      glTexCoord2f(s,t); glVertex3f(x,y,z);
      glNormal3f(x,y,z);
      
      x = (ringRadius + ringTubeRadius*cos(2*PI*s))*cos(2*PI*(t+step));
      y = (ringRadius + ringTubeRadius*cos(2*PI*s))*sin(2*PI*(t+step));
      z = zCoeff * sin(2*PI*(t+step));
      glTexCoord2f(s,t+step); glVertex3f(x,y,z);
      glNormal3f(x,y,z);
            
      x = (ringRadius + ringTubeRadius*cos(2*PI*(s+step)))*cos(2*PI*(t+step));
      y = (ringRadius + ringTubeRadius*cos(2*PI*(s+step)))*sin(2*PI*(t+step));
      z = zCoeff * sin(2*PI*(t+step));
      glTexCoord2f(s+step,t+step); glVertex3f(x,y,z);
      glNormal3f(x,y,z);

      x = (ringRadius + ringTubeRadius*cos(2*PI*(s+step)))*cos(2*PI*t);
      y = (ringRadius + ringTubeRadius*cos(2*PI*(s+step)))*sin(2*PI*t);
      z = zCoeff * sin(2*PI*t);
      glTexCoord2f(s+step,t); glVertex3f(x,y,z);
      glNormal3f(x,y,z);
      
      glEnd();
    }
}

void init(void){    
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glEnable(GL_DEPTH_TEST);
  /* enable lighting */
  glEnable(GL_LIGHTING);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  GLfloat material_ambient[] = {0.25f, 0.25f, 0.25f};
  GLfloat material_diffuse[] = {0.90f, 0.90f, 0.90f};
  GLfloat material_specular[]= {0.90f, 0.90f, 0.90f};
  GLfloat material_shininess = 25.0f;

  glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
  glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);

//set two lights, one white and one yellow
  GLfloat light0_diffuse[] = {1.0, 1.0, 0.0, 1.0};
  GLfloat light0_position[] = {0.0, 50.0, -50.0, 0.0};
  GLfloat light1_diffuse[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light1_position[] = {0.0, 50.0, 50.0, 0.0};
  
//enable 2 light sources
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

  glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.0);
  glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_diffuse);
  glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

   //load textures
   texture[0] = glmReadPPM("Textures/earthmap.ppm",   &texWidth[0], &texHeight[0]);
   texture[1] = glmReadPPM("Textures/venusmap.ppm",   &texWidth[1], &texHeight[1]);
   texture[2] = glmReadPPM("Textures/jupitermap.ppm", &texWidth[2], &texHeight[2]);
   texture[3] = glmReadPPM("Textures/marsmap.ppm",    &texWidth[3], &texHeight[3]);
   texture[4] = glmReadPPM("Textures/mercurymap.ppm", &texWidth[4], &texHeight[4]);
   texture[5] = glmReadPPM("Textures/neptunemap.ppm", &texWidth[5], &texHeight[5]);
   texture[6] = glmReadPPM("Textures/plutomap.ppm",   &texWidth[6], &texHeight[6]);
   texture[7] = glmReadPPM("Textures/sunmap.ppm",     &texWidth[7], &texHeight[7]);
   texture[8] = glmReadPPM("Textures/saturnmap.ppm",  &texWidth[8], &texHeight[8]);
   texture[9] = glmReadPPM("Textures/uranusmap.ppm",  &texWidth[9], &texHeight[9]);
   texture[10] = glmReadPPM("Textures/CrossHair.ppm", &texWidth[10], &texHeight[10]);
   texture[11] = glmReadPPM("Textures/GrassTexture.ppm", &texWidth[11], &texHeight[11]);
   texture[12] = glmReadPPM("Guns/RocketLauncher.ppm", &texWidth[12], &texHeight[12]);
   texture[13] = glmReadPPM("Guns/AUG.ppm", &texWidth[13], &texHeight[13]);
   texture[14] = glmReadPPM("Guns/Glock.ppm",    &texWidth[14], &texHeight[14]);
   texture[15] = glmReadPPM("Guns/Deagle.ppm", &texWidth[15], &texHeight[15]);
   texture[16] = glmReadPPM("Guns/MP5A4.ppm", &texWidth[16], &texHeight[16]);
   texture[17] = glmReadPPM("Guns/ak47.ppm",   &texWidth[17], &texHeight[17]);
   texture[18] = glmReadPPM("Guns/awp.ppm",     &texWidth[18], &texHeight[18]);
   texture[19] = glmReadPPM("Guns/Shotgun.ppm",  &texWidth[19], &texHeight[19]);
   texture[20] = glmReadPPM("Guns/G3SG1.ppm",  &texWidth[20], &texHeight[20]);
   texture[21] = glmReadPPM("Guns/M4A1.ppm",    &texWidth[21], &texHeight[21]);
   texture[22] = glmReadPPM("Textures/BloodSpilled.ppm",    &texWidth[22], &texHeight[22]);
   texture[23] = glmReadPPM("Textures/dead.ppm",    &texWidth[23], &texHeight[23]);
   texture[24] = glmReadPPM("Textures/Wall.ppm", &texWidth[24], &texHeight[24]);
   
   //generate texture 'names'
   glGenTextures(NUM_TEXTURES,texName);

   //specify texture parameters for texName[0]
   for(int i=0; i<NUM_TEXTURES; i++){
   glBindTexture(GL_TEXTURE_2D, texName[i]);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth[i], texHeight[i], 0, GL_RGB, GL_UNSIGNED_BYTE, texture[i]);
   }
   //establish frustum
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(-1,1,-1,1,1,600);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

/*This part draws credits for my program so people won't confuse with other 
  people's work*/
void drawCredits(){     
    char s[20];
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, WIDTH, 0, HEIGHT, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glColor4f(1.0,1.0,1.0,0.5);
    glRasterPos2i(WIDTH/3, HEIGHT-20);
    drawString("A Gun & Space Museum by Luong Hoang");
    
    sprintf(s,"Health: %d", health);
    glRasterPos2i(20, HEIGHT-20);
    drawString(s);
    
    sprintf(s, "Number of hits: %d", earthHit);
    glRasterPos2i(WIDTH-150, HEIGHT-20);
    drawString(s);
    
    glDisable(GL_BLEND);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}


/*This function basically draws all instructions and all in-game texts overlays*/
void drawInstructions(int a){
    char* s;
    if(a==GUN_ROCKET)  s = "Rocket Launcher";
    if(a==GUN_AUG)     s = "      AUG      ";
    if(a==GUN_GLOCK)   s = "     Glock     ";
    if(a==GUN_DEAGLE)  s = "     Deagle    ";
    if(a==GUN_MP5)     s = "      MP5      ";
    if(a==GUN_AK47)    s = "      AK47     ";
    if(a==GUN_AWP)     s = "      AWP      ";
    if(a==GUN_SHOTGUN) s = "    Shotgun    ";
    if(a==GUN_G3SG1)   s = "     G3SG1     ";
    if(a==GUN_M4A1)    s = "      M4A1     ";
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, WIDTH, 0, HEIGHT, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    if(drawEarth==false){
      glColor3f(1.0,1.0,1.0);
      glRasterPos2i(WIDTH/2-80, HEIGHT/2+40);
      drawString("Objective Completed!");
    }
    else{
    if(instruction == true){
    glColor4f(1.0,1.0,0.0,0.5);
    glRasterPos2i(WIDTH/3, HEIGHT-60);
    drawString("Welcome to the virtual gun & solar space!");
    glRasterPos2i(WIDTH/3, HEIGHT-90);
    drawString("To move around, use 'W', 'S', 'A', 'D'.");
    glRasterPos2i(WIDTH/3, HEIGHT-120);
    drawString("Walk up to a gun to pick it up.");
    glRasterPos2i(WIDTH/3, HEIGHT-150);
    drawString("Left click to shoot, each gun consists of 30 bullets.");
    glRasterPos2i(WIDTH/3, HEIGHT-180);
    drawString("Press 'R' to reload gun. Right click to jump.");
    glRasterPos2i(WIDTH/3, HEIGHT-210);
    drawString("You can hide/unhide this instruction by pressing 'H'.");
    glRasterPos2i(WIDTH/3, HEIGHT-240);
    drawString("Press 'T' to increase the level of difficulty.");
    glRasterPos2i(WIDTH/3, HEIGHT-270);
    drawString("Objective: Identify a vulnerable planet and destroy it.");
    glRasterPos2i(WIDTH/3, HEIGHT-300);
    drawString("Rule: No cheating by looking at source code. Enjoy!");
    }
    if(haveGun==true){
      glColor4f(1.0,1.0,0.0,alphaText);
      glRasterPos2i(WIDTH/2-55, HEIGHT/2+40);
      drawString(s);
    }
    }
    glDisable(GL_BLEND);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}

void drawCrossHair(){
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, WIDTH, 0, HEIGHT, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glTranslatef(WIDTH/2.0, HEIGHT/2.0,0.0);
    glBindTexture(GL_TEXTURE_2D, texName[CROSSHAIR]);
    glColor4f(1.0,1.0,1.0,0.4);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 

    //drawing left part of crosshair
    glBegin(GL_QUADS);
      glTexCoord2f(0,0);glVertex3f(-13,-2,0);
      glTexCoord2f(1,0);glVertex3f(-3,-2,0);
      glTexCoord2f(1,1);glVertex3f(-3,2,0);
      glTexCoord2f(0,1);glVertex3f(-13,2,0);
    glEnd();

    //drawing top part of crosshair    
    glBegin(GL_QUADS);
      glTexCoord2f(0,0);glVertex3f(-2,12,0);
      glTexCoord2f(1,0);glVertex3f(-2,2,0);
      glTexCoord2f(1,1);glVertex3f(2,2,0);
      glTexCoord2f(0,1);glVertex3f(2,12,0);
    glEnd();

    //drawing right part of crosshair
    glBegin(GL_QUADS);
      glTexCoord2f(0,0);glVertex3f(3,-2,0);
      glTexCoord2f(1,0);glVertex3f(13,-2,0);
      glTexCoord2f(1,1);glVertex3f(13,2,0);
      glTexCoord2f(0,1);glVertex3f(3,2,0);
    glEnd();

    //drawing bottom part of crosshair
    glBegin(GL_QUADS);
      glTexCoord2f(0,0);glVertex3f(-2,-2,0);
      glTexCoord2f(1,0);glVertex3f(-2,-12,0);
      glTexCoord2f(1,1);glVertex3f(2,-12,0);
      glTexCoord2f(0,1);glVertex3f(2,-2,0);
    glEnd();

    glDisable(GL_BLEND);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}

void drawSceneBase(){
     glDisable(GL_LIGHTING);
     glBindTexture(GL_TEXTURE_2D, texName[GRASSTEXTURE]);
     glBegin(GL_POLYGON);
       glTexCoord2f(10,0);  glVertex3f(50,-1,50);
       glTexCoord2f(10,10); glVertex3f(50,-1,-50);
       glTexCoord2f(0,10);  glVertex3f(-50,-1,-50);
       glTexCoord2f(0,0);   glVertex3f(-50,-1,50);
     glEnd();
     glEnable(GL_LIGHTING);
}

void drawGun(int index){
     if(picked[index-GUN_ROCKET]== true)
     return;
     glBindTexture(GL_TEXTURE_2D, texName[index]);
     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     glDisable(GL_LIGHTING);
     glPushMatrix();
     glColor4f(1.0, 1.0, 1.0, 0.5);
     glRotatef(180,0,0,1);
     glTranslatef(15.0,0.0,0.0);
     glTranslatef(2-3*(index+1-GUN_ROCKET),0,0);
     if(index==GUN_AUG || index==GUN_G3SG1 || index==GUN_MP5 || index==GUN_SHOTGUN)
     glRotatef(180,0,1,0);
     
     glRotatef(xx, 0, 1 ,0);
     glBegin(GL_POLYGON);
        glTexCoord2f(0,0); glVertex3f(-0.5,-1.0,0.0);
        glTexCoord2f(0,1); glVertex3f(-0.5,1.0,0.0);
        glTexCoord2f(1,1); glVertex3f(0.5,1.0,0.0);
        glTexCoord2f(1,0); glVertex3f(0.5,-1.0,0.0);
     glEnd();
     glPopMatrix();
     glEnable(GL_LIGHTING);
     glDisable(GL_BLEND);
}
void initializePlanetBullets(){
     for(int i=0; i<9; i++){
       pb[i]=transPBulletX[i]=transPBulletY[i]=transPBulletZ[i]=0;
       pHit[i] = false;
       planetBullets[i].setDestination(ex-5+i,ey,ez-5+i);
     }
     planetBullets[VENUS-1].setPosition(earthX+venusX, earthY+30+venusY, earthZ+venusZ-5);
     planetBullets[JUPITER-1].setPosition(earthX+jupiterX, earthY+30+jupiterY, earthZ+jupiterZ-5);
     planetBullets[MARS-1].setPosition(earthX+marsX, earthY+30+marsY, earthZ+marsZ-5);
     planetBullets[MERCURY-1].setPosition(earthX+mercuryX, earthY+30+mercuryY, earthZ+mercuryZ-5);
     planetBullets[NEPTUNE-1].setPosition(earthX+neptuneX, earthY+30+neptuneY, earthZ+neptuneZ-5);
     planetBullets[PLUTO-1].setPosition(earthX+plutoX, earthY+30+plutoY, earthZ+plutoZ-5);
     planetBullets[SUN-1].setPosition(earthX+sunX, earthY+30+sunY, earthZ+sunZ-5);
     planetBullets[SATURN-1].setPosition(earthX+saturnX, earthY+30+saturnY, earthZ+saturnZ-5);
     planetBullets[URANUS-1].setPosition(earthX+uranusX, earthY+30+uranusY, earthZ+uranusZ-5);
}

/*moveCamera takes in two parameters, if a=0 then move up or down, if a=1 then
  move right or left. The algorithm here was developed with the help of vectors*/
void moveCamera(float speed, int a)
{
	double tempREx = rx-ex;
	double tempREy = ry-ey;
	double tempREz = rz-ez;
    if(a==0){
	   ex = ex + tempREx * speed;
	   ez = ez + tempREz * speed;
	   rx = rx + tempREx * speed;
	   rz = rz + tempREz * speed;
    }
    else{
       ex = ex + tempREz * speed;
       rx = rx + tempREz * speed;
	   ez = ez - tempREx * speed;
	   rz = rz - tempREx * speed;
    }
}

//this function rotates the camera view by angle speed, quite mathy :)
void rotateView(float speed)
{
	double tempREx = rx-ex;
	double tempREy = ry-ey;
	double tempREz = rz-ez;

	rz = (float)(ez + sin(speed)*tempREx + cos(speed)*tempREz);
	rx = (float)(ex + cos(speed)*tempREx - sin(speed)*tempREz);
}

void drawPlanetBullets(){
     for(int i=0; i<9; i++){
         double bx, by, bz, dx, dy, dz, distX, distY, distZ;
         bx = planetBullets[i].getxPos();
         by = planetBullets[i].getyPos();
         bz = planetBullets[i].getzPos();
         dx = planetBullets[i].getxDest();
         dy = planetBullets[i].getyDest();
         dz = planetBullets[i].getzDest();
         
         distX = (dx-bx) * pb[i]/50.0;
         distY = (dy-by) * pb[i]/50.0;
         distZ = (dz-bz) * pb[i]/50.0;
         transPBulletX[i] = bx+distX;
         transPBulletY[i] = by+distY;
         transPBulletZ[i] = bz+distZ;

         glPushMatrix();
           glTranslatef(transPBulletX[i], transPBulletY[i], transPBulletZ[i]);
           glutSolidSphere(0.2, 20, 20);
         glPopMatrix();
   } 
}

bool checkBoundary(){
     if(ex<=-49 || ex>=49 || ez<=-49 || ez>=49)
     return false;
     else
     return true;
}

void drawBloodSpilled(){
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, WIDTH, 0, HEIGHT, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, texName[BLOODSPILLED]);
    glColor4f(1.0,1.0,1.0,bloodSpilledAlpha);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 
     glBegin(GL_POLYGON);
        glTexCoord2f(0,0); glVertex3f(-WIDTH,-HEIGHT,0.0);
        glTexCoord2f(0,1); glVertex3f(-WIDTH,HEIGHT,0.0);
        glTexCoord2f(1,1); glVertex3f(WIDTH,HEIGHT,0.0);
        glTexCoord2f(1,0); glVertex3f(WIDTH,-HEIGHT,0.0);
     glEnd();
    glDisable(GL_BLEND);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}
void drawDeath(){
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, WIDTH, 0, HEIGHT, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, texName[DEAD]);
    glColor4f(1.0,1.0,1.0,dieAlpha);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glTranslatef(WIDTH/2,HEIGHT/2,0.0);
    glRotatef(180,0,1,0);
    
     glBegin(GL_POLYGON);
        glTexCoord2f(0,0); glVertex3f(-WIDTH/2,-HEIGHT/2,0.0);
        glTexCoord2f(0,1); glVertex3f(-WIDTH/2,HEIGHT/2,0.0);
        glTexCoord2f(1,1); glVertex3f(WIDTH/2,HEIGHT/2,0.0);
        glTexCoord2f(1,0); glVertex3f(WIDTH/2,-HEIGHT/2,0.0);
     glEnd();
    glDisable(GL_BLEND);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}

void drawWall(){
     glBindTexture(GL_TEXTURE_2D, texName[WALL]);
     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     glDisable(GL_LIGHTING);
     glPushMatrix();
     glColor4f(1.0, 1.0, 1.0, xx2/360.0);
     //front
     glBegin(GL_POLYGON);
        glTexCoord2f(0,0); glVertex3f(-50,-1.0,-50);
        glTexCoord2f(0,1); glVertex3f(-50,100,-50);
        glTexCoord2f(1,1); glVertex3f(50,100,-50);
        glTexCoord2f(1,0); glVertex3f(50,-1.0,-50);
     glEnd();
     
     //left
     glBegin(GL_POLYGON);
        glTexCoord2f(0,0); glVertex3f(-50,-1.0,-50);
        glTexCoord2f(0,1); glVertex3f(-50,100,-50);
        glTexCoord2f(1,1); glVertex3f(-50,100,50);
        glTexCoord2f(1,0); glVertex3f(-50,-1.0,50);
     glEnd();     
     
     //behind
     glBegin(GL_POLYGON);
        glTexCoord2f(0,0); glVertex3f(50,-1.0,50);
        glTexCoord2f(0,1); glVertex3f(50,100,50);
        glTexCoord2f(1,1); glVertex3f(-50,100,50);
        glTexCoord2f(1,0); glVertex3f(-50,-1.0,50);
     glEnd();
     
     //right
     glBegin(GL_POLYGON);
        glTexCoord2f(0,0); glVertex3f(50,-1.0,-50);
        glTexCoord2f(0,1); glVertex3f(50,100,-50);
        glTexCoord2f(1,1); glVertex3f(50,100,50);
        glTexCoord2f(1,0); glVertex3f(50,-1.0,50);
     glEnd();
     
     glPopMatrix();
     glEnable(GL_LIGHTING);
     glDisable(GL_BLEND);
     
}
void display(void){
   //clear buffers
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();
   glEnable(GL_TEXTURE_2D);
   glEnable(GL_DEPTH_TEST);
   
   glScalef(scaleX, scaleY, scaleZ);
   glPushMatrix();
     glViewport(windowX, windowY, width, height);
     gluLookAt(ex,ey,ez,rx,ry,rz,ux,uy,uz);
     drawSceneBase();
     /*bind textures and map them onto spheres*/
     glPushMatrix();
     for(int i=0; i<URANUS+1; i++){
       glBindTexture(GL_TEXTURE_2D, texName[i]);
       if(i==EARTH)
       glTranslatef(earthX, earthY, earthZ);
       glPushMatrix();
         glTranslatef(0.0,30.0,-5.0);
         if(i==SATURN || i==URANUS)
         glRotatef(60,0,1,0);
         //rotate planets accordingly
         sphericalMapping(i);
         if(i==SATURN || i==URANUS){
            glPushMatrix();
               glTranslatef(0,0,0);
               drawRing();
            glPopMatrix();
         }
       glPopMatrix();
     }
     glPopMatrix();
     
     /*drawing corners of universe*/
     glPushMatrix();
     glRotatef(-90.0,1,0,0);
     glColor3f(1.0,1.0,0.0);
     glTranslatef(-47.0,-47.0,-1.0);
     glutSolidCone(3.0,100.0,15,15);
     glTranslatef(94.0,0.0,0.0);
     glutSolidCone(3.0,100.0,15,15);
     glTranslatef(0,94.0,0.0);
     glutSolidCone(3.0,100.0,15,15);
     glTranslatef(-94.0,0.0,0.0);
     glutSolidCone(3.0,100.0,15,15);
     glColor3f(1.0,1.0,1.0);
     glPopMatrix();
   
   //draw walls
   if(attack==true)
   drawWall();
   
/*Drawing guns on the base*/
   for(int i= GUN_ROCKET; i<=GUN_M4A1; i++)
   drawGun(i);


//Draw bullets
   for(int i=0; i<bulletIndex; i++){
       if(bullets[i].getDraw() == true){
         double bx, by, bz, dx, dy, dz, distX, distY, distZ;
         bx = bullets[i].getxPos();
         by = bullets[i].getyPos();
         bz = bullets[i].getzPos();
         dx = bullets[i].getxDest();
         dy = bullets[i].getyDest();
         dz = bullets[i].getzDest();
         
         distX = (dx-bx) * bb[i]/5.0;
         distY = (dy-by) * bb[i]/5.0;
         distZ = (dz-bz) * bb[i]/5.0;
         transBulletX[i] = bx+distX;
         transBulletY[i] = by+distY;
         transBulletZ[i] = bz+distZ;
         
         if(bb[i]>=200){
            bullets[i].setDraw(false);
            bb[i] = 0;
         }
         glPushMatrix();
           glTranslatef(transBulletX[i], transBulletY[i], transBulletZ[i]);
           glutSolidSphere(0.2, 20, 20);
         glPopMatrix();
       }
   }
   
   if(attack == true)
   drawPlanetBullets();
   
   glPopMatrix();
   
   
   //draw credits
   drawCredits();
   
   //draw instructions
   drawInstructions(pickedGun+12);
   
   //draw blood spilled
   drawBloodSpilled();

   //draw death scene
   if(health==1 && attack == false)
   drawDeath();
   else   
   //Drawing Crosshair
   drawCrossHair();


   
   glFlush();
   glutSwapBuffers();
   glDisable(GL_TEXTURE_2D);
}

void playMusic(){
     if(timer%14 == 0)
        alSourcePlay(Sources[MUSIC]);     
     timer++;
}

void keyboard(unsigned char key, int x, int y){
   if(drawEarth==false && victory == false){
     victory = true;
     alSourceStop(Sources[MUSIC_2]);
     alSourcePlay(Sources[MUSIC_VICTORY]);
   }
   
   keyDown[key] = 1;
   //escape key kills program
   if(key == 27){
     glutLeaveGameMode();
     exit(0);
   }
   
   //'t' pressing increases level of difficulty by increasing diffX
   if(key == 't'){
      if(drawEarth==true)
      diffX+=2;
   }
   
   if(keyDown['a'] == 1){
      moveCamera(eyeSpeed,1);
      if(checkBoundary() == false)
      moveCamera(-eyeSpeed,1);
   }

   if(keyDown['d'] == 1){
      moveCamera(-eyeSpeed,1);
      if(checkBoundary() == false)
      moveCamera(eyeSpeed,1);
   }

   if(keyDown['w'] == 1){
      moveCamera(eyeSpeed,0);
      if(checkBoundary() == false)
      moveCamera(-eyeSpeed,0);
   }

   if(keyDown['s'] == 1){
      moveCamera(-eyeSpeed,0);
      if(checkBoundary() == false)
      moveCamera(eyeSpeed,0);
   }

   if(keyDown['f'] == 1){
      if(haveGun == true)
        if(pickedGun == GUN_AWP-12 || pickedGun == GUN_G3SG1-12)
        {
          zoom = !zoom;
          if(zoom == true){
            windowX = WIDTH/2  - 100;
            windowY = HEIGHT/2 - 100;
            width = height = 200;
            scaleX = scaleY = scaleZ = 5.0;
          }
//            glViewport(WIDTH/2-100, HEIGHT/2-100,200,200);
          else{
            scaleX = scaleY = scaleZ = 1.0;
            windowX = windowY = 0;
            width = WIDTH;
            height= HEIGHT;
          }
        }   
   }
   //'r' is for reloading guns, this resets all bullets and plays sound effect
   if(key == 'r'){
      if(haveGun == true){
        for(int i=0; i<num_bullets; i++){
          bb[i] = 0;
          bullets[i].reset();
          hit[i] = false;
        }
        bulletIndex = 0;
        alSourcePlay(Sources[MUSIC_RELOAD]);
      }
   }
   
   if(key == 'h'){
      instruction = !instruction;
   }
   
   if(key == 'v'){
      if(earthHit>=NUM_HITS/2)
      attack=!attack;
   }
   
   if(keyDown['a'] == 1 || keyDown['d'] == 1 || keyDown['w'] == 1 || keyDown['s'] == 1){
     playMusic();
     for(gunIndex=0; gunIndex<NUM_GUNS; gunIndex++)
     /*Checking if the eye is at any of the gun position*/
     if (ez>=-1 && ez<=1 && ex<=-13.5+3*gunIndex && 
         ex>=-14.5+3*gunIndex && picked[gunIndex] == false){
        picked[gunIndex] = true;
        haveGun = true;
        pickedGun = gunIndex;//store the gun picked
        alphaText = 0.9;
        //set every other gun to be default unpicked
        for(int i=0; i<NUM_GUNS; i++){
           if(i!=gunIndex)
           picked[i] = false;
        }
        gunIndex++;
        alSourcePlay(Sources[MUSIC_3]);
        break;
       }
   }
   glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y){
    keyDown[key] = 0;
}

/*This is where I have most problems writing and testing, finally got it to work
  The function changes view with respect to mouse movement*/
void changeView(int x, int y){
   
	float angle_y  = 0.0f;				
	float angle_z  = 0.0f;							

	angle_z = (float)( (prevMouseY - y) ) / 100;
    angle_y = (float)( (prevMouseX - x) ) / 300;

	ry += angle_z * 2;
    if((ry-ey)> 8) ry = ey + 8;
    if((ry-ey)<-8) ry = ey - 8;
	
    rotateView(-angle_y); 
	
	prevMouseX = x;
	prevMouseY = y;
	
	if(x == 0 || x == WIDTH-1 || y == 0 || y == HEIGHT-1){
       SetCursorPos(WIDTH/2, HEIGHT/2);
       prevMouseX = WIDTH/2;
       prevMouseY = HEIGHT/2;
    }
}
void mouse_move(int x, int y){ 
   if(drawEarth==false && victory == false){
     victory = true;
     alSourceStop(Sources[MUSIC_2]);
     alSourcePlay(Sources[MUSIC_VICTORY]);
   }
    //prevent initial viewing bug where mouse location affects default viewing
	ii++;
     if(ii==1){
        prevMouseX = x;
        prevMouseY = y;
     }
     if(ii>=20)
     ii=2;

    changeView(x,y);
    glutPostRedisplay();
}

void mouse_dragged(int x, int y) {
   if(drawEarth==false && victory == false){
     victory = true;
     alSourceStop(Sources[MUSIC_2]);
     alSourcePlay(Sources[MUSIC_VICTORY]);
   }

    if(left_button_down == true){
       ii++;
       if(ii%10==0)
       for(int i=0; i<NUM_GUNS; i++){
        if(picked[i] == true){
          pickedGun = i;
          if(bulletIndex < num_bullets){
          //start to draw bullets by first setting their positions and destinations
          bullets[bulletIndex].setPosition(ex,ey,ez);
          bullets[bulletIndex].setDestination(rx,ry,rz);
          bullets[bulletIndex].setDraw(true);
          bulletIndex++;
          //play sound effect of gun shooting accordingly
          alSourcePlay(Sources[i+3]);
          }
          else
          //if gun load is empty of bullets then play sound effect
          alSourcePlay(Sources[MUSIC_EMPTYLOAD]);
          break;
          }
      }
    }
	changeView(x,y);
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
  //establish point of reference for dragging mouse in window
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
      left_button_down = TRUE;
      for(int i=0; i<NUM_GUNS; i++){
        if(picked[i] == true){
          if(bulletIndex < num_bullets){
          bullets[bulletIndex].setPosition(ex,ey,ez);
          bullets[bulletIndex].setDestination(rx,ry,rz);
          bullets[bulletIndex].setDraw(true);        
          bulletIndex++;
          alSourcePlay(Sources[i+3]);
          }
          else
          alSourcePlay(Sources[MUSIC_EMPTYLOAD]);
          break;
          }
      }
	  prevMouseX = x;
      prevMouseY = y;
    }

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
      right_button_down = TRUE;
      jumpFlag = true;
      tempRy = ry;
      prevMouseX = x;
      prevMouseY = y;
    }
    else if (state == GLUT_UP){
      left_button_down = FALSE;
	}
	glutPostRedisplay();
}

void my_idle(){
     
     if(attack==true){
     for(int i=0; i<9; i++)
     pb[i]++;
     }
     //xx is very often used in this program, to translate and rotate objects
     xx++;
     if(xx==120||xx==240||xx==360){
       if(attack == true){
       initializePlanetBullets();
       gotHit=false;
       alSourcePlay(Sources[MUSIC_PLANETINVASION]);
       }
     }
     if(xx>=360){
       xx = 0;
     }
     
     //here alphaText is reduced gradually to get the fading effect for overlay text
     alphaText-=.002;
     if(alphaText<=0)
     alphaText=0;
     
     //animate earth rotation in our virtual universe
     earthX = diffX*sin((xx/180.0) * PI);
     earthZ = diffZ*cos((xx/180.0) * PI);
     for(int i=0; i<bulletIndex; i++){
       if(bullets[i].getDraw() == true)
          bb[i]++;
       //checking if the bullet hits the destination
       if(transBulletX[i]>=earthX-1 && transBulletX[i]<=earthX+1 &&
          transBulletY[i]>=30+earthY-1 && transBulletY[i]<=30+earthY+1 &&
          transBulletZ[i]>=earthZ-5-1 && transBulletZ[i]<=earthZ-5+1 &&
          hit[i]==false && drawEarth == true){
          hit[i]==true; //if bullet hits once, don't check it again
          earthHit++; // increase number of bullets hits
          alSourcePlay(Sources[MUSIC_EARTHHIT]);
       }
     }
     
     if(bloodSpilledAlpha>=0)
     bloodSpilledAlpha-=0.05;
     
     if(attack==true){
     xx2+=dir;
     if(xx2==360)
     dir=-1;
     if(xx2==0)
     dir=1;
     
     int prevHealth = health;
     for(int i=0; i<9; i++)
     if(transPBulletX[i]>=ex-2 &&transPBulletX[i]<=ex+2 &&
        transPBulletY[i]>=ey-2 &&transPBulletY[i]<=ey+2 &&
        transPBulletZ[i]>=ez-2 &&transPBulletZ[i]<=ez+2 && pHit[i]==false){
        health-=4;
        pHit[i]=true;
        }
     if(health!=prevHealth){
     alSourcePlay(Sources[MUSIC_HURT]);
     bloodSpilledAlpha = 0.7;
     }
     }
     

     if(earthHit>=NUM_HITS/2 && firstTime == false){
        attack = true;
        firstTime = true;
        initializePlanetBullets();
        alSourcePlay(Sources[MUSIC_PLANETINVASION]);
     }
     
     if(health<=0){
     alSourcePlay(Sources[MUSIC_URDEAD]);
     health = 1;
     attack = false;
     dieAlpha = 0;
     }
     
     if(dieAlpha<1)
     dieAlpha+=0.01;
     
     if(earthHit>=NUM_HITS){
     drawEarth = false; // if hits reach the desired hits, don't draw earth
     attack = false;
     }
     
     //animate venus rotation around earth
     venusX = 4*cos((xx/180.0) * PI);
     venusZ = 4*sin((xx/180.0) * PI);
     
     //animate jupiter rotation around earth
     jupiterY = 6*sin((xx/180.0) * PI);
     jupiterZ = 8*cos((xx/180.0) * PI);
     
     //animate mars rotation around earth
     marsX = 8*sin((xx/180.0) * PI);
     marsY = 8*cos((xx/180.0) * PI);

     //animate mars rotation around earth
     mercuryX = 9*sin((xx/180.0) * PI);
     mercuryY = 10*cos((xx/180.0) * PI); 

     //animate neptune rotation around earth
     neptuneY = 12*sin((xx/180.0) * PI);
     neptuneZ = 5*cos((xx/180.0) * PI); 

     //animate pluto rotation around earth
     plutoX = 11*sin((xx/180.0) * PI);
     plutoY = 11*cos((xx/180.0) * PI); 

     //animate sun rotation around earth
     sunX = 16*sin((xx/180.0) * PI);
     sunY = 13*cos((xx/180.0) * PI); 
     
     //animate saturn rotation around earth
     saturnY = 11*cos((xx/180.0) * PI);
     saturnZ = 14*sin((xx/180.0) * PI); 

     //animate uranus rotation around earth
     uranusX = 18*sin((xx/180.0) * PI);
     uranusZ = 18*cos((xx/180.0) * PI); 
     
     //this code here does the jumping
     if(right_button_down==TRUE){
         ey = 6.25- jumpI*jumpI;
         ry = tempRy+6.25- jumpI*jumpI;
         jumpI+=0.1;
         if(jumpI>=2.5){
            jumpI=-2.5;
            right_button_down=FALSE;
            tempRy = -1.0;
         }
     }
     
     glutPostRedisplay();
     }

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
   glutGameModeString( "1024x768:32@60" );
   glutEnterGameMode();
   ShowCursor(false);
   init();
   glutDisplayFunc(display);
   // Initialize OpenAL and clear the error bit.
   
   alutInit(NULL, 0);
   alGetError();

   // Load the wav data.
   if(LoadALData() == AL_FALSE)
   {
     printf("Error loading data.");
     return 0;
   }
   SetListenerValues();
   alSourcePlay(Sources[MUSIC_2]);
   // Setup an exit procedure.
   atexit(KillALData);
  
   //special keyboard inputs used to check if 2 keys are pressed at once
   memset( keyDown, 0, sizeof( keyDown ) );
   glutKeyboardFunc(keyboard);
   glutKeyboardUpFunc(keyboardUp);
   
   glutMouseFunc(mouse);
   glutMotionFunc(mouse_dragged);
   glutPassiveMotionFunc(mouse_move);
   glutIdleFunc(my_idle);
   glutMainLoop();
   return 0; 
}
