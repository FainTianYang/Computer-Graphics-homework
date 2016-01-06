/*-----------------------------------------------------------
 * An example program to draw a car
 *   Author: 曹郡欣
 *   Date:  10/2015
 */
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>
#include <GL/glut.h>

#define  PI   3.141592653

#define Step  1.0 /*每次移動之距離*/

using namespace std;

int style=4;
bool Con=false; /* Determine whether the continuous advance */ 
bool GF=false; /* Control gridelines */ 
bool LI=false; /* Control light on/off */ 
bool Movf=true; /* car front move */
bool Movb=true; /* car back move */
/*-----Define a unit box , size:1X1X1 center:(0,0,0)--------*/
/* Vertices of the box */
float  points[8][3] = {{-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}, /*0後左下角 , 1後右下角*/
                      {0.5 , 0.5 , -0.5}, {-0.5, 0.5, -0.5}, /*2後右上角 , 3後右上角*/
                      {-0.5, -0.5,  0.5}, {0.5, -0.5,  0.5}, /*4前左下角 , 5前右下角*/
                      { 0.5 , 0.5,  0.5}, {-0.5, 0.5,  0.5}};/*6前右上角 , 7前左上角*/
/* face of box, each face composing of 4 vertices */
int    face[6][4] = {{0, 3, 2, 1}, {0, 1, 5, 4}, {1, 2, 6, 5}, /*後,下,右*/
                    {4, 5, 6, 7}, {2, 3, 7, 6}, {0, 4, 7, 3}}; /*前,上,左*/

/*colors of car body */
float  car_body_colors[6][3]={{0.53,0.133,0.133}, {0.53,0.133,0.133}, {0.53,0.133,0.133},  
                     {0.53,0.133,0.133}, {0.53,0.133,0.133}, {0.53,0.133,0.133}};

/*colors of sky */
float  sky_colors[6][3]={{0.7,0.81,0.92}, {0.7,0.81,0.92}, {0.7,0.81,0.92},  
                     {0.7,0.81,0.92}, {0.7,0.81,0.92}, {0.7,0.81,0.92}};

/*colors of light */
float  light_colors[3]={0.302,0.223,0.0};/*on*/

/*-Declare GLU quadric objects, sphere, cylinder, and disk --*/
GLUquadricObj  *sphere=NULL, *cylind=NULL, *disk ,*cylindline=NULL,*cylindline2=NULL;

/*-Declare car position, orientation--*/
float  car_body_yawang=0.0,car_body_rollang=0.0,car_body_pitchang=0.0;/*y:yaw ,x:roll ,z:pitch*/
float  car_position[3]={-35.0, 4.5, -35.0};
float  car_positionfr[3]={-48.5, 4.5, -13.0};
float  car_positionfl[3]={-21.5, 4.5, -13.0};
float  car_positionbr[3]={-48.5, 4.5, -57.0};
float  car_positionbl[3]={-21.5, 4.5, -57.0};

/*-----Declare wheel angle--*/
float  wheel_yawang=0.0,wheel_rollang=0.0,wheel_pitchang=0.0; 
float  wheel_changeang=0.0;

/*-----Declare world angle--*/
float  world_yawang=0.0,world_rollang=0.0,world_pitchang=0.0; 
float  scale=1.0;
float  windmillangl=0.0;

/*-----Define window size----*/
int width=1012, height=612;

/*-----eye axes----------------------*/
float  eyeT[3]={0.0, 0.0, 0.0};
float  eye[3]={0,4.5,-80};//The origin of the eye coordinate position
float  u[3][3]={{1.0,0.0,0.0}, /*x,y,z方向*/
                {0.0,1.0,0.0}, 
                {0.0,0.0,1.0}}; 

/*-----Perspective parameter-----------*/
double Aspect=(double) width/ (double) height;
double Fovy=90.0;
double ZNear=2.0;
double ZFar=300.0;
double zoom=5.0;

float  normals[6][3]={{0.0,0.0,-1.0}, {0.0,-1.0,0.0}, {1.0,0.0,0.0},
                     {0.0,0.0,1.0}, {0.0,1.0,0.0}, {-1.0,0.0,0.0}};
/*----Define light source properties -------------*/
/*--light0--sun--parallel--*/
float  lit0_direction[] = {0.0, -1.0, 0.0, -1.0}; //為甚麼我的要用-1才會有打光的效果
float  lit0_diffuse[] = {0.95, 0.95, 0.95, 1.0};
float  lit0_specular[] = {0.95, 0.95, 0.95, 1.0};
/*--light1--street lamp--spot--*/
float  lit1_position[] = {11.0 ,49.0, 0.0, 1.0};
float  lit1_direction[] = {0.0, -1.0, 0.0, 0.0};
float  lit1_diffuse[] = {0.8,0.5,0.0, 1.0};
float  lit1_specular[] = {0.7, 0.7, 0.7, 1.0};
float  lit1_cutoff=10.0;
/*--ambient--*/
float  global_ambient[]={0.3, 0.3, 0.4, 1.0};

/*----Define material properties for cube -----*/
float  mat_ambient[] = {0.3, 0.3, 0.3, 1.0};
float  mat_diffuse[] = {0.9, 0.9, 0.9, 1.0};
float  mat_specular[] = {0.9, 0.9, 0.9, 1.0};
float  mat_shininess = 32.0;
float  flr_diffuse[] = {0.0,0.51, 0.0, 1.0};
float  flr_shininess = 4.0;

/*
 * Procedure to initialize the working environment.
 */
void  myinit()
{
  glClearColor(0.0, 0.0, 0.0, 1.0);      /*set the background color BLACK */
                                          
  glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);/*Clear the Depth & Color Buffers */

  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);  /*Enable depth buffer for shading computing */
  glEnable(GL_NORMALIZE);   /*Enable mornalization  */

  glEnable(GL_LIGHTING);    /*Enable lighting effects */

  /*-----Define light0 --sun---------*/
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, lit0_direction);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lit0_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, lit0_specular);
  /*-----Define light1 --streetlamp1---------*/
  glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, lit1_cutoff);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, lit1_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, lit1_specular);
  /*-----Define light2 --streetlamp2---------*/
  glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 15.0);
  glLightf(GL_LIGHT2, GL_DIFFUSE,(1.0,1.0,1.0, 1.0));
  glLightfv(GL_LIGHT2, GL_SPECULAR, lit1_specular);
  /*-----Define some global lighting status -----*/
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); /* local viewer */
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient); /*global ambient*/

  /*---Create quadratic objects---*/
  if(sphere==NULL){
    sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricNormals(sphere, GLU_SMOOTH);
  }
  if(cylind==NULL){
    cylind = gluNewQuadric();
    gluQuadricDrawStyle(cylind, GLU_FILL);
    gluQuadricNormals(cylind, GLU_SMOOTH);
  }
  if(cylindline==NULL){
    cylindline = gluNewQuadric();
    gluQuadricDrawStyle(cylindline, GLU_LINE);
    gluQuadricNormals(cylindline, GLU_SMOOTH);
  }
  if(cylindline2==NULL){
    cylindline2 = gluNewQuadric();
    gluQuadricDrawStyle(cylindline2, GLU_FILL);
    gluQuadricNormals(cylindline2, GLU_SMOOTH);
  }
  if(disk==NULL){
    disk = gluNewQuadric();
    gluQuadricDrawStyle(disk, GLU_FILL);
    gluQuadricNormals(disk, GLU_SMOOTH);
  }

}
/*--------------------------------------------------------
 * Procedure to draw a 1x1x1 cube. The cube is within
 * (-0.5,-0.5,-0.5) ~ (0.5,0.5,0.5)
 */
void draw_cube(int num)
{
  int    i;

  for(i=0;i<6;i++){
	  switch (num) /* Select and Set color */
	  {
	  case 1 :
		  glColor3fv(car_body_colors[i]);  
		  break;
	  case 2 :
		  glColor3fv(sky_colors[i]);  
		  break;
	  }
	glNormal3fv(normals[i]);
    glBegin(GL_POLYGON);  /* Draw the face */
      glVertex3fv(points[face[i][0]]);
      glVertex3fv(points[face[i][1]]);
      glVertex3fv(points[face[i][2]]);
      glVertex3fv(points[face[i][3]]);
    glEnd();
  }
}
/*---------------------------------------------------------
 * Procedure to draw the floor.
 * center:(0,-2.5,0) size:100X100
 */
void draw_floor()
{
  int  i, j;

  for(i=0;i<30;i++)
    for(j=0;j<30;j++){
      glColor3f(0.5451,0.271,0.745);
	  glLineWidth (2.0);
      glBegin(GL_LINE_LOOP);
        glVertex3f((i-15.0)*10.0, 0.1, (j-15.0)*10.0);
        glVertex3f((i-15.0)*10.0, 0.1, (j-14.0)*10.0);
        glVertex3f((i-14.0)*10.0, 0.1, (j-14.0)*10.0);
        glVertex3f((i-14.0)*10.0, 0.1, (j-15.0)*10.0);
      glEnd();
    }
}
/*-------------------------------------------------------
 * Procedure to draw three axes and the orign(0,0,0)
 */
void draw_axes()
{
  /*----Draw a white sphere to represent the original-----*/
  glColor3f(1.0, 0.9, 0.9);

  gluSphere(sphere, 0.7,   /* radius*/
	        24,            /* slices*/
	        24);           /* stacks */

  /*----Draw three axes in colors, yellow, meginta, and cyan--*/

  /* Draw Z axis  */
  glPushMatrix(); /* Save Main coord. sys */
  /* Cylinder */
     glColor3f(0.54, 0.0, 0.0);
     gluCylinder(cylind, 0.3, 0.3, /* radius of top and bottom circle */
	             7.0,              /* height of the cylinder */
                 12,               /* use 12-side polygon approximating circle*/
	             3);               /* Divide it into 3 sections */
     /* Cone */
     glPushMatrix(); /* Save Z_axis coord. sys */
        glTranslatef(0,0,7);
        gluCylinder(cylind, 0.8, 0.0, 
	                4.0,              
                    12,               
	                3); 
     glPopMatrix(); /* Back to Z_axis coord. sys */
  glPopMatrix();    /* Back to Main coord. sys */

  /* Draw Y axis */
  glPushMatrix();  /* Save Main coord. sys */
  /* Cylinder */
     glRotatef(-90.0, 1.0, 0.0, 0.0);  /*Rotate about x by -90', z becomes y */
     glColor3f(0.0, 0.392, 0.0);
     gluCylinder(cylind, 0.3, 0.3, /* radius of top and bottom circle */
	             7.0,              /* height of the cylinder */
                 12,               /* use 12-side polygon approximating circle*/
	             3);               /* Divide it into 3 sections */
  /* Cone */
     glPushMatrix();   /* Save Y_axis coord. sys */
        glTranslatef(0,0,7);
        gluCylinder(cylind, 0.8, 0.0, /* radius of top and bottom circle */
	                4.0,             /* height of the cylinder */
                    12,               /* use 12-side polygon approximating circle*/
	                3);               /* Divide it into 3 sections */
     glPopMatrix();    /* Back to Y_axis coord. sys*/
  glPopMatrix();    /* Back to Main_axis coord. sys*/

  /* Draw X axis */
  glPushMatrix();  /* Save Main coord. sys */
  /* Cylinder */
     glRotatef(90.0, 0.0, 1.0, 0.0);  /*Rotate about y by 90', z becomes x */
     glColor3f(0.0,0.0, 0.54);
     gluCylinder(cylind, 0.3, 0.3, /* radius of top and bottom circle */
	             7.0,              /* height of the cylinder */
                 12,               /* use 12-side polygon approximating circle*/
	             3);               /* Divide it into 3 sections */
  /* Cone */
     glPushMatrix();   /* Save X_axis coord. sys */
        glTranslatef(0,0,7);
        gluCylinder(cylind, 0.8, 0.0, /* radius of top and bottom circle */
	                4.0,             /* height of the cylinder */
                    12,               /* use 12-side polygon approximating circle*/
	                3);               /* Divide it into 3 sections */
     glPopMatrix();    /* Back to X_axis coord. sys*/
  glPopMatrix();    /* Back to Main_axis coord. sys*/

  glPopMatrix();    /* Back to Original_axis coord. sys*/
}
/*---------------------------------------------------------
 * Procedure to draw view volume, eye position, focus ,...
 * for perspective projection
 */
void draw_eye_axes()
{
  glMatrixMode(GL_MODELVIEW);
  /*----Draw Eye position-----*/
  glPushMatrix();

      glTranslatef((eye[0]+eyeT[0]), (eye[1]+eyeT[1]), (eye[2]+eyeT[2]));
      glRotatef(world_yawang,0.0,1.0,0.0);
      glRotatef(world_rollang,1.0,0.0,0.0);
	  glRotatef(world_pitchang,0.0,0.0,1.0); 
      glTranslatef(-(eye[0]+eyeT[0]), -(eye[1]+eyeT[1]), -(eye[2]+eyeT[2]));

      glPushMatrix();

          glTranslatef(eye[0]+eyeT[0], eye[1]+eyeT[1], eye[2]+eyeT[2]);
          glColor3f(0.0, 0.0, 0.0);
          gluSphere(sphere, 1.2, 24, 24);   

      glPopMatrix();
  /*----Draw eye coord. axes -----*/
      glColor3f(1.0, 1.0, 0.0);           /* Draw Xe */
      glLineWidth (1.5);
      glBegin(GL_LINES);
        glVertex3f(eye[0]+eyeT[0], eye[1]+eyeT[1], eye[2]+eyeT[2]);
        glVertex3f(eye[0]+eyeT[0]+10.0*u[0][0], eye[1]+eyeT[1]+10.0*u[0][1], eye[2]+eyeT[2]+10.0*u[0][2]);
      glEnd();

      glColor3f(1.0, 0.0, 1.0);          /* Draw Ye */
      glBegin(GL_LINES);
        glVertex3f(eye[0]+eyeT[0], eye[1]+eyeT[1], eye[2]+eyeT[2]);
        glVertex3f(eye[0]+eyeT[0]+10.0*u[1][0], eye[1]+eyeT[1]+10.0*u[1][1], eye[2]+eyeT[2]+10.0*u[1][2]);
      glEnd();

      glColor3f(0.0, 1.0, 1.0);          /* Draw Ze */
      glBegin(GL_LINES);
        glVertex3f(eye[0]+eyeT[0], eye[1]+eyeT[1], eye[2]+eyeT[2]);
        glVertex3f(eye[0]+eyeT[0]+10.0*u[2][0], eye[1]+eyeT[1]+10.0*u[2][1], eye[2]+eyeT[2]+10.0*u[2][2]);
      glEnd();

	/*---------Draw view volum (Perspective)-----------------------------*/\
	  glColor3f(0.0314, 0.1451, 0.404);           
      glLineWidth (1.2);
	  float vnw=2*ZNear*tan(PI*Fovy/360.0)*Aspect;
	  float vnh=2*ZNear*tan(PI*Fovy/360.0);
	  float vfw=2*ZFar*tan(PI*Fovy/360.0)*Aspect;
	  float vfh=2*ZFar*tan(PI*Fovy/360.0);
      glBegin(GL_LINE_LOOP);
        glVertex3f(eye[0]+eyeT[0]+vnw/2, eye[1]+eyeT[1]+vnh/2,eye[2]+eyeT[2]-ZNear);
        glVertex3f(eye[0]+eyeT[0]-vnw/2, eye[1]+eyeT[1]+vnh/2,eye[2]+eyeT[2]-ZNear);
		glVertex3f(eye[0]+eyeT[0]-vnw/2, eye[1]+eyeT[1]-vnh/2,eye[2]+eyeT[2]-ZNear);
		glVertex3f(eye[0]+eyeT[0]+vnw/2, eye[1]+eyeT[1]-vnh/2,eye[2]+eyeT[2]-ZNear);
      glEnd();
	  glBegin(GL_LINE_LOOP);
        glVertex3f(eye[0]+eyeT[0]+vfw/2, eye[1]+eyeT[1]+vfh/2,eye[2]+eyeT[2]-ZFar);
        glVertex3f(eye[0]+eyeT[0]-vfw/2, eye[1]+eyeT[1]+vfh/2,eye[2]+eyeT[2]-ZFar);
		glVertex3f(eye[0]+eyeT[0]-vfw/2, eye[1]+eyeT[1]-vfh/2,eye[2]+eyeT[2]-ZFar);
		glVertex3f(eye[0]+eyeT[0]+vfw/2, eye[1]+eyeT[1]-vfh/2,eye[2]+eyeT[2]-ZFar);
      glEnd();
	  glColor3f(0.455, 0.0, 0.6314);  
	  glBegin(GL_LINES);
	    glVertex3f(eye[0]+eyeT[0]+vnw/2, eye[1]+eyeT[1]+vnh/2,eye[2]+eyeT[2]-ZNear);
		glVertex3f(eye[0]+eyeT[0]+vfw/2, eye[1]+eyeT[1]+vfh/2,eye[2]+eyeT[2]-ZFar);

		glVertex3f(eye[0]+eyeT[0]-vnw/2, eye[1]+eyeT[1]+vnh/2,eye[2]+eyeT[2]-ZNear);
		glVertex3f(eye[0]+eyeT[0]-vfw/2, eye[1]+eyeT[1]+vfh/2,eye[2]+eyeT[2]-ZFar);

		glVertex3f(eye[0]+eyeT[0]-vnw/2, eye[1]+eyeT[1]-vnh/2,eye[2]+eyeT[2]-ZNear);
		glVertex3f(eye[0]+eyeT[0]-vfw/2, eye[1]+eyeT[1]-vfh/2,eye[2]+eyeT[2]-ZFar);

		glVertex3f(eye[0]+eyeT[0]+vnw/2, eye[1]+eyeT[1]-vnh/2,eye[2]+eyeT[2]-ZNear);
		glVertex3f(eye[0]+eyeT[0]+vfw/2, eye[1]+eyeT[1]-vfh/2,eye[2]+eyeT[2]-ZFar);
	  glEnd();
	  /*-------------draw faces of volum(transparent)-------------*/
	  glColor4f(0.3,0.3,0.3,0.5); 
      glBegin(GL_POLYGON);  
	      glVertex3f(eye[0]+eyeT[0]+vnw/2, eye[1]+eyeT[1]+vnh/2,eye[2]+eyeT[2]-ZNear);
		  glVertex3f(eye[0]+eyeT[0]+vfw/2, eye[1]+eyeT[1]+vfh/2,eye[2]+eyeT[2]-ZFar); 
		  glVertex3f(eye[0]+eyeT[0]-vfw/2, eye[1]+eyeT[1]+vfh/2,eye[2]+eyeT[2]-ZFar);
          glVertex3f(eye[0]+eyeT[0]-vnw/2, eye[1]+eyeT[1]+vnh/2,eye[2]+eyeT[2]-ZNear);
      glEnd();
	  glColor4f(0.1,0.3,0.3,0.5); 
      glBegin(GL_POLYGON);  
	      glVertex3f(eye[0]+eyeT[0]+vnw/2, eye[1]+eyeT[1]+vnh/2,eye[2]+eyeT[2]-ZNear);
		  glVertex3f(eye[0]+eyeT[0]+vfw/2, eye[1]+eyeT[1]+vfh/2,eye[2]+eyeT[2]-ZFar);
		  glVertex3f(eye[0]+eyeT[0]+vfw/2, eye[1]+eyeT[1]-vfh/2,eye[2]+eyeT[2]-ZFar);
		  glVertex3f(eye[0]+eyeT[0]+vnw/2, eye[1]+eyeT[1]-vnh/2,eye[2]+eyeT[2]-ZNear);
      glEnd();
	  glColor4f(0.3,0.1,0.3,0.5); 
      glBegin(GL_POLYGON);  
	      glVertex3f(eye[0]+eyeT[0]+vnw/2, eye[1]+eyeT[1]-vnh/2,eye[2]+eyeT[2]-ZNear);
		  glVertex3f(eye[0]+eyeT[0]+vfw/2, eye[1]+eyeT[1]-vfh/2,eye[2]+eyeT[2]-ZFar);
		  glVertex3f(eye[0]+eyeT[0]-vfw/2, eye[1]+eyeT[1]-vfh/2,eye[2]+eyeT[2]-ZFar);
		  glVertex3f(eye[0]+eyeT[0]-vnw/2, eye[1]+eyeT[1]-vnh/2,eye[2]+eyeT[2]-ZNear);
      glEnd();
	  glColor4f(0.3,0.3,0.1,0.5); 
      glBegin(GL_POLYGON);  
		  glVertex3f(eye[0]+eyeT[0]-vfw/2, eye[1]+eyeT[1]-vfh/2,eye[2]+eyeT[2]-ZFar);
		  glVertex3f(eye[0]+eyeT[0]-vnw/2, eye[1]+eyeT[1]-vnh/2,eye[2]+eyeT[2]-ZNear);
		  glVertex3f(eye[0]+eyeT[0]-vfw/2, eye[1]+eyeT[1]+vfh/2,eye[2]+eyeT[2]-ZFar);
		  glVertex3f(eye[0]+eyeT[0]-vnw/2, eye[1]+eyeT[1]+vnh/2,eye[2]+eyeT[2]-ZNear);
      glEnd();
  glPopMatrix();
}
/*-------------------------------------------------------
 * Procedure to draw windmill
 */
void draw_windmill()
{
	glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glPushMatrix();  /*主要圓柱*/
	   glRotatef(-90,1.0,0.0,0.0);
	   glMaterialf(GL_FRONT, GL_SHININESS, 8.0);
       glMaterialf(GL_FRONT, GL_SPECULAR, (0.3,0.3,0.3,1.0));
	   glColor3f(1.0,1.0,0.6);
	   gluCylinder(cylind,10.0,10.0,25.0, 31,31);
	glPopMatrix();
	glPushMatrix();  /*圓錐屋頂*/
       glTranslatef(0.0,25.0,0.0);
	   glRotatef(-90,1.0,0.0,0.0);
	   glMaterialf(GL_FRONT, GL_SHININESS, 64.0);
       glMaterialf(GL_FRONT, GL_SPECULAR, (0.9,0.9,0.9,1.0));
	   glColor3f(0.0,0.0,0.2);
	   gluCylinder(cylind,15.0,0.0,18.0, 31, 31);
	glPopMatrix();
	glPushMatrix();  /*連接屋頂和葉片的圓柱*/
       glTranslatef(0.0,35.0,0.0);
	   glRotatef(-90,0.0,1.0,0.0);
	   glMaterialf(GL_FRONT, GL_SHININESS, 64.0);
       glMaterialf(GL_FRONT, GL_SPECULAR, (0.3,0.3,0.3,1.0));
	   glColor3f(0.111,0.111,0.3);
	   gluCylinder(cylind,3.0,3.0,20.0, 31, 31);
	   glPushMatrix();  
	      glTranslatef(0.0,0.0,20.0);
	      glRotatef(windmillangl,0.0,0.0,1.0);
		  glPushMatrix();  /*葉片*/
		     glColor3f(0.1,0.1,0.15); 
             glBegin(GL_POLYGON);  /*和圓柱連接的桿子1*/
			    for(int i=0 ; i<2 ; i++)
				{
					for(int j=0 ; j<44 ; j++)
					{
						glVertex3f(i-1,j-22,0.0);
						glVertex3f(i+1-1,j-22,0.0);
						glVertex3f(i+1-1,j+1-22,0.0);
                        glVertex3f(i-1,j+1-22,0.0);                          
					}
				}
             glEnd();
			 glColor3f(0.1,0.1,0.15); 
             glBegin(GL_POLYGON);  /*和圓柱連接的桿子2*/
			 for(int i=0 ; i<44 ; i++)
				{
					for(int j=0 ; j<2 ; j++)
					{
						glVertex3f(i-22,j-1,0.0);
						glVertex3f(i+1-22,j-1,0.0);
						glVertex3f(i+1-22,j+1-1,0.0);
                        glVertex3f(i-22,j+1-1,0.0);                          
					}
				}
             glEnd();
			 glColor3f(0.44,0.259,0.078); 
             glBegin(GL_POLYGON);  /*葉片*/
			    for(int i=0 ; i<4 ; i++)
				{
					for(int j=0 ; j<19 ; j++)
					{
						glVertex3f(i+1,j+3,0.0);
						glVertex3f(i+1+1,j+3,0.0);
						glVertex3f(i+1+1,j+1+3,0.0);
                        glVertex3f(i+1,j+1+3,0.0);                          
					}
				}
             glEnd();
			 glColor3f(0.44,0.259,0.078); 
             glBegin(GL_POLYGON); 
			 for(int i=0 ; i<4 ; i++)
				{
					for(int j=0 ; j<19 ; j++)
					{
						glVertex3f(i-5,j-22,0.0);
						glVertex3f(i+1-5,j-22,0.0);
						glVertex3f(i+1-5,j+1-22,0.0);
                        glVertex3f(i-5,j+1-22,0.0);                          
					}
				}
             glEnd();
			 glColor3f(0.44,0.259,0.078); 
             glBegin(GL_POLYGON);  
			 for(int i=0 ; i<19 ; i++)
				{
					for(int j=0 ; j<4 ; j++)
					{
						glVertex3f(i-22,j+1,0.0);
						glVertex3f(i+1-22,j+1,0.0);
						glVertex3f(i+1-22,j+1+1,0.0);
                        glVertex3f(i-22,j+1+1,0.0);                          
					}
				}
             glEnd();
			 glColor3f(0.44,0.259,0.078); 
             glBegin(GL_POLYGON);  
			 for(int i=0 ; i<19 ; i++)
				{
					for(int j=0 ; j<4 ; j++)
					{
						glVertex3f(i+3,j-5,0.0);
						glVertex3f(i+1+3,j-5,0.0);
						glVertex3f(i+1+3,j+1-5,0.0);
                        glVertex3f(i+3,j+1-5,0.0);                          
					}
				}
             glEnd();
		  glPopMatrix();
	   glPopMatrix();
	glPopMatrix();
    glDisable(GL_COLOR_MATERIAL);
}
/*-------------------------------------------------------
 * This procedure draw the car and background
 */
void draw_scene()
{
  glPushMatrix();
     glEnable(GL_LIGHTING);
	 glEnable(GL_LIGHT1);
     glLightfv(GL_LIGHT1, GL_POSITION, lit1_position);
     glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lit1_direction);
	 glEnable(GL_LIGHT2);
     glLightf(GL_LIGHT2, GL_POSITION, (10.0 ,46.0, 0.0, 1.0));
     glLightf(GL_LIGHT2, GL_SPOT_DIRECTION,(0.0,1.0,0.0,0.0));
	 
  /* Draw sky ---------------------------------------------
  glDisable(GL_DEPTH_TEST);
  glColor3f(0.7,0.81,0.92);
  float len=150.0;
  glBegin(GL_POLYGON);  
      glVertex3f(len,len,-len);
      glVertex3f(len,len,len);
      glVertex3f(len,-len,len);
      glVertex3f(len,-len,-len);
  glEnd();
  glColor3f(0.6,0.71,0.92); 
  glBegin(GL_POLYGON);  
      glVertex3f(len,len,-len);
      glVertex3f(len,len,len);
      glVertex3f(-len,len,len);
      glVertex3f(-len,len,-len);
  glEnd();
  glColor3f(0.5,0.61,0.92); 
  glBegin(GL_POLYGON);  
      glVertex3f(-len,-len,len);
	  glVertex3f(len,-len,len);
	  glVertex3f(len,len,len);
      glVertex3f(-len,len,len);     
  glEnd();
  glColor3f(0.7,0.81,0.92); 
  glBegin(GL_POLYGON);  
      glVertex3f(-len,len,-len);
      glVertex3f(-len,len,len);
      glVertex3f(-len,-len,len);
      glVertex3f(-len,-len,-len);
  glEnd();
  glColor3f(0.6,0.71,0.92); 
  glBegin(GL_POLYGON);  
      glVertex3f(len,-len,-len);
      glVertex3f(len,-len,len);
      glVertex3f(-len,-len,len);
      glVertex3f(-len,-len,-len);
  glEnd();
  glColor3f(0.5,0.61,0.92); 
  glBegin(GL_POLYGON);  
      glVertex3f(-len,len,-len);
      glVertex3f(len,len,-len);
      glVertex3f(len,-len,-len);
      glVertex3f(-len,-len,-len);
  glEnd();
  glEnable(GL_DEPTH_TEST);*/
  /*Draw road---------------------------------------------------------*/
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
  glMaterialf(GL_FRONT, GL_SHININESS, 64.0);
  glMaterialf(GL_FRONT, GL_DIFFUSE, (0.6,0.6,0.6,1.0));
  glMaterialf(GL_FRONT, GL_SPECULAR, (0.3,0.3,0.3,1.0));

  glColor3f(0.6,0.6,0.6);
  glNormal3f(0.0, 1.0, 0.0);
  for(int i = 0; i < 100; ++i)
  {
	  for(int j = 0; j < 150; ++j)
	  {
		  glBegin(GL_POLYGON);
			glVertex3f(i - 120, 0.005, j - 150);
			glVertex3f(i - 120, 0.005, j + 1 - 150);
			glVertex3f(i + 1 - 120, 0.005, j + 1 - 150);
			glVertex3f(i + 1 - 120, 0.005, j - 150);
		  glEnd();
	  }
  }
  glPushMatrix();
  float rota =(atan((29.0/80.0)))/PI*180;  //斜坡的傾斜角度
  glRotatef(rota,-1.0,0.0,0.0);
  glNormal3f(0.0, 1.0, 0.0);
  for(int i = 0; i < 100; ++i)
  {
	  for(int j = 0; j < sqrt(29.0*29.0+80.0*80.0); ++j)
	  {
		  glBegin(GL_POLYGON);
			glVertex3f(i -120.0, 0.0, j );
			glVertex3f(i -120.0, 0.0, j + 1 );
			glVertex3f(i + 1 -120.0, 0.0, j + 1 );
			glVertex3f(i + 1 -120.0, 0.0, j );
		  glEnd();
	  }
  }
  glPopMatrix();
  glNormal3f(0.0, 1.0, 0.0);
  for(int i = 0; i < 100; ++i)
  {
	  for(int j = 0; j < 70; ++j)
	  {
		  glBegin(GL_POLYGON);
			glVertex3f(i - 120.0, 29.0, j +80.0);
			glVertex3f(i - 120.0, 29.0, j + 1 + 80.0);
			glVertex3f(i + 1 - 120.0, 29.0, j + 1 + 80.0);
			glVertex3f(i + 1 - 120.0, 29.0, j + 80.0);
		  glEnd();
	  }
  }
  glDisable(GL_COLOR_MATERIAL);
  /* Draw floor ---------------------------------------------*/
  if(GF) draw_floor();
  glEnable(GL_COLOR_MATERIAL);
  glMaterialf(GL_FRONT, GL_SHININESS, 4.0);
  glMaterialf(GL_FRONT, GL_DIFFUSE, (0.0,0.51,0.0,1.0));
  glMaterialf(GL_FRONT, GL_SPECULAR, (0.3,0.3,0.3,1.0));
  glColor3f(0.0,0.51, 0.0); /*Green*/
  glNormal3f(0.0, 1.0, 0.0);
  for(int i = 0; i < 300; ++i)
  {
	  for(int j = 0; j < 300; ++j)
	  {
		  glBegin(GL_POLYGON);
			glVertex3f(i - 150, 0, j - 150);
			glVertex3f(i - 150, 0, j + 1 - 150);
			glVertex3f(i + 1 - 150, 0, j + 1 - 150);
			glVertex3f(i + 1 - 150, 0, j - 150);
		  glEnd();
	  }
  }
  glDisable(GL_COLOR_MATERIAL);

  /*Draw Slope---------------------------------------------------*/
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
  glMaterialf(GL_FRONT, GL_SHININESS, 6.0);
  glMaterialf(GL_FRONT, GL_DIFFUSE, (0.411,0.545, 0.133,1.0));
  glColor3f(0.411,0.545, 0.133);
  glPushMatrix();
  glRotatef(rota,-1.0,0.0,0.0);
  glNormal3f(0.0, 1.0, 0.0);
  for(int i = 0; i < 300; ++i)
  {
	  for(int j = 0; j < sqrt(29.0*29.0+80.0*80.0); ++j)
	  {
		  glBegin(GL_POLYGON);
			glVertex3f(i -150.0, 0.0, j );
			glVertex3f(i -150.0, 0.0, j + 1 );
			glVertex3f(i + 1 -150.0, 0.0, j + 1 );
			glVertex3f(i + 1 -150.0, 0.0, j );
		  glEnd();
	  }
  }
  glPopMatrix();
  glColor3f(0.411,0.459, 0.0); 
  glNormal3f(0.0, 1.0, 0.0);
  for(int i = 0; i < 300; ++i)
  {
	  for(int j = 0; j < 70; ++j)
	  {
		  glBegin(GL_POLYGON);
			glVertex3f(i - 150.0, 29.0, j +80.0);
			glVertex3f(i - 150.0, 29.0, j + 1 + 80.0);
			glVertex3f(i + 1 - 150.0, 29.0, j + 1 + 80.0);
			glVertex3f(i + 1 - 150.0, 29.0, j + 80.0);
		  glEnd();
	  }
  }
  glColor3f(0.411,0.467, 0.137);
  glBegin(GL_POLYGON);  
      glVertex3f(-150.0,0.0,0.0);
      glVertex3f(-150.0,29.1,80.0);
      glVertex3f(-150.0,29.1,150.0);
      glVertex3f(-150.0,0,150.0);
  glEnd();
  glColor3f(0.411,0.467, 0.137);
  glBegin(GL_POLYGON);  
      glVertex3f(150.0,0.0,0.0);
      glVertex3f(150.0,29.1,80.0);
      glVertex3f(150.0,29.1,150.0);
      glVertex3f(150.0,0,150.0);
  glEnd();
  glColor3f(0.0,0.392, 0.0);
  glBegin(GL_POLYGON);  
      glVertex3f(-150.0,29.1,150);
      glVertex3f(150.0,29.1,150);
      glVertex3f(150.0,0.0,150.0);
      glVertex3f(-150.0,0.0,150.0);
  glEnd();
  glDisable(GL_COLOR_MATERIAL);
  
  /*Draw coodrinate building------------------------------------------*/
  glPushMatrix();  /* Save Main coord. sys */
     glRotatef(-90,1.0,0.0,0.0);
	 glEnable(GL_LIGHTING);
     glShadeModel(GL_SMOOTH);
	 glEnable(GL_COLOR_MATERIAL);
     glMaterialf(GL_FRONT, GL_SHININESS, 64.0);
	 glMaterialf(GL_FRONT, GL_SPECULAR,(0.8,0.8,0.8,1.0));
     glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

     glNormal3f(0.0, 1.0, 0.0);  
     glColor3f(0.408,0.514,0.545);
	 glLineWidth (4.0);
     gluCylinder(cylindline,6.0,1.0,19.0,23,23);
	 glPushMatrix();  /* Save cylinder coord. sys */
	    glRotatef(90,1.0,0.0,0.0);
        glTranslatef(0.0,20.0,0.0);
        draw_axes();
     glPopMatrix();   /*Back to cylinder coord. sys*/
	 glDisable(GL_COLOR_MATERIAL);
  glPopMatrix();   /*Back to Main coord. sys*/
  /* Draw windmill----------------------------------------------------*/
  glPushMatrix();              /* Save Main coord. sys */
     glTranslatef(120.0,0.0,-120.0);
	 draw_windmill();
  glPopMatrix();               /* Back to Main coord. sys */
  /* Draw streetlamp--------------------------------------------------*/
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
  glMaterialf(GL_FRONT, GL_SHININESS, 128.0);
  glMaterialf(GL_FRONT, GL_SPECULAR,(0.9,0.9,0.9,1.0));

  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glPushMatrix();              /* Save Main coord. sys */
     glTranslatef(15.0,0.0,0.0);
	 glPushMatrix();
	    glRotatef(-90,1.0,0.0,0.0);
        glColor3f(0.5,0.5,0.5);
        gluCylinder(cylindline2,1.0,1.0,50.0, 7, 3);
	 glPopMatrix(); 

	 glPushMatrix();
	 glTranslatef(-3.0,52.0,0.0);
	   glPushMatrix();
	      glScalef(8.0,4.0,4.0);
		  glColor3f(0.5,0.5,0.5);
		  for(int a=0 ; a<6 ; a++){
			glNormal3fv(normals[a]);
		    glBegin(GL_POLYGON);  /* Draw the cube */
               glVertex3fv(points[face[a][0]]);
               glVertex3fv(points[face[a][1]]);
               glVertex3fv(points[face[a][2]]);
               glVertex3fv(points[face[a][3]]);
            glEnd();
		  }
	   glPopMatrix(); 
	 glPopMatrix(); 
	 /*light1*/
	 //glDisable(GL_LIGHTING);
	 glMaterialf(GL_FRONT, GL_SHININESS, 128.0);
     glMaterialf(GL_FRONT,GL_DIFFUSE,(0.7,0.5,0.0,1.0));
	 glMaterialf(GL_FRONT, GL_EMISSION,(1.0,1.0,1.0,1.0));
	 glPushMatrix();
	  glTranslatef(-4.0,49.0,0.0);
	   glPushMatrix();
	      glScalef(6.0,2.0,4.0);
		  glColor3f(0.7,0.6,0.0);
		  for(int a=0 ; a<6 ; a++){
			glNormal3fv(normals[a]);
		    glBegin(GL_POLYGON);  /* Draw the cube */
               glVertex3fv(points[face[a][0]]);
               glVertex3fv(points[face[a][1]]);
               glVertex3fv(points[face[a][2]]);
               glVertex3fv(points[face[a][3]]);
            glEnd();
		  }
	   glPopMatrix();
	 glPopMatrix();
	 glEnable(GL_LIGHTING);
  glPopMatrix();               /* Back to Main coord. sys */
  glDisable(GL_COLOR_MATERIAL);
  /*Draw car ----------------------------------------------------*/
  glEnable(GL_COLOR_MATERIAL);
  glMaterialf(GL_FRONT, GL_SHININESS, 6.0);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glPushMatrix();              /* Save Main coord. sys */
     glTranslatef(car_position[0],car_position[1],car_position[2]); /* Car_M1 coord. sys */
     glRotatef(car_body_rollang, 1.0, 0.0, 0.0); /* angle of depression or elevation */
	 glRotatef(car_body_yawang, 0.0, 1.0, 0.0); /* right and left rotation angle */
	 /*car body(front1)*/
	 glPushMatrix();              /* Save Car_M1 coord. sys */
	    glTranslatef(0.0,0.0,22.011); 
         glColor3fv(light_colors);
        glBegin(GL_POLYGON);  
           glVertex3f(5.5,4.0,0.0);
           glVertex3f(9.5,4.0,0.0);
           glVertex3f(9.5,2.5,0.0);
           glVertex3f(5.5,2.5,0.0);
        glEnd();
		glBegin(GL_POLYGON);  
           glVertex3f(-5.5,4.0,0.0);
           glVertex3f(-9.5,4.0,0.0);
           glVertex3f(-9.5,2.5,0.0);
           glVertex3f(-5.5,2.5,0.0);
        glEnd();
     glPopMatrix();               /* Back to Car_M1 coord. sys */
	 /*car light1*/
	 glPushMatrix();              /* Save Car_M1 coord. sys */
	    glTranslatef(0.0,3.7,19.5); 
        glScalef(27.0,7.5,5.0);
        draw_cube(1);
     glPopMatrix();               /* Back to Car_M1 coord. sys */
	 /*car body(front2)*/
	 glPushMatrix();              /* Save Car_M1 coord. sys */
	    glTranslatef(0.0,6.25,13.5); 
        glScalef(27.0,7.5,7.0);
        draw_cube(1);
     glPopMatrix();               /* Back to Car_M1 coord. sys */
      /*car body(base_main)*/
     glPushMatrix();              /* Save Car_M1 coord. sys */
        glScalef(17.0, 1.0, 34.0);
        draw_cube(1);
     glPopMatrix();               /* Back to Car_M1 coord. sys */
     /*car body(base_left)*/
      glPushMatrix();              /* Save Car_M1 coord. sys */
         glTranslatef(11.0,0.0,0.0); 
         glScalef(5.0,1.0,20.0);
         draw_cube(1);
      glPopMatrix();               /* Back to Car_M1 coord. sys */
      /*car body(base_right)*/
      glPushMatrix();              /* Save Car_M1 coord. sys */
         glTranslatef(-11.0,0.0,0.0); 
         glScalef(5.0,1.0,20.0);
         draw_cube(1);
      glPopMatrix();               /* Back to Car_M1 coord. sys */
      /*car body(1-1)*/
      glPushMatrix();              /* Save Car_M1 coord. sys */
         glTranslatef(11.0,0.75,16.9); 
         glScalef(5.0, 2.5,0.2);
         draw_cube(1);
      glPopMatrix();               /* Back to Car_M1 coord. sys */
      /*car body(1-2)*/
      glPushMatrix();              /* Save Car_M1 coord. sys */
         glTranslatef(11.0,2.25,13.5); 
         glScalef(5.0,0.5,7.0);
         draw_cube(1);
      glPopMatrix();               /* Back to Car_M1 coord. sys */
      /*car body(1-3)*/
      glPushMatrix();              /* Save Car_M1 coord. sys */
         glTranslatef(11.0,0.75,10.1); 
         glScalef(5.0, 2.5,0.2);
         draw_cube(1);
      glPopMatrix();               /* Back to Car_M1 coord. sys */
      /*car body(2-1)*/
      glPushMatrix();              /* Save Car_M1 coord. sys */
         glTranslatef(11.0,0.75,-10.1); 
         glScalef(5.0, 2.5,0.2);
         draw_cube(1);
      glPopMatrix();               /* Back to Car_M1 coord. sys */
      /*car body(2-2)*/
      glPushMatrix();              /* Save Car_M1 coord. sys */
         glTranslatef(11.0,2.25,-13.5); 
         glScalef(5.0,0.5,7.0);
         draw_cube(1);
      glPopMatrix();                /* Back to Car_M1 coord. sys */
      /*car body(2-3)*/
      glPushMatrix();              /* Save Car_M1 coord. sys */
         glTranslatef(11.0,0.75,-16.9); 
         glScalef(5.0, 2.5,0.2);
         draw_cube(1);
      glPopMatrix();                /* Back to Car_M1 coord. sys */
      /*car body(3-1)*/
      glPushMatrix();              /* Save Car_M1 coord. sys */
         glTranslatef(-11.0,0.75,16.9); 
         glScalef(5.0, 2.5,0.2);
         draw_cube(1);
      glPopMatrix();                /* Back to Car_M1 coord. sys */
      /*car body(3-2)*/
      glPushMatrix();              /* Save Car_M1 coord. sys */
         glTranslatef(-11.0,2.25,13.5); 
         glScalef(5.0,0.5,7.0);
         draw_cube(1);
      glPopMatrix();                /* Back to Car_M1 coord. sys */
      /*car body(3-3)*/
      glPushMatrix();              /* Save Car_M1 coord. sys */
         glTranslatef(-11.0,0.75,10.1); 
         glScalef(5.0, 2.5,0.2);
         draw_cube(1);
      glPopMatrix();                /* Back to Car_M1 coord. sys */
      /*car body(4-1)*/
      glPushMatrix();              /* Save Car_M1 coord. sys */
         glTranslatef(-11.0,0.75,-10.1); 
         glScalef(5.0, 2.5,0.2);
         draw_cube(1);
      glPopMatrix();                /* Back to Car_M1 coord. sys */
      /*car body(4-2)*/
      glPushMatrix();              /* Save Car_M1 coord. sys */
         glTranslatef(-11.0,2.25,-13.5); 
         glScalef(5.0,0.5,7.0);
         draw_cube(1);
      glPopMatrix();                /* Back to Car_M1 coord. sys */
      /*car body(4-3)*/
      glPushMatrix();              /* Save Car_M1 coord. sys */
         glTranslatef(-11.0,0.75,-16.9); 
         glScalef(5.0, 2.5,0.2);
         draw_cube(1);
      glPopMatrix();                /* Back to Car_M1 coord. sys */  
	  /*car body(back)*/
	 glPushMatrix();              /* Save Car_M1 coord. sys */
	    glTranslatef(0.0,3.7,-19.5); 
        glScalef(27.0,7.5,5.0);
        draw_cube(1);
     glPopMatrix();               /* Back to Car_M1 coord. sys */

      /*-------Draw the front_left wheels -----*/
	  glPushMatrix();               /* Save Car_M1 coord. sys */
	     glTranslatef(8.5,-1.0,13.5);
	     glRotatef(wheel_yawang, 0.0, 1.0, 0.0); /* right and left rotation angle */
         glRotatef(wheel_rollang, 1.0, 0.0, 0.0); /* Scroll */
         glRotatef(wheel_pitchang, 0.0, 0.0, 1.0); /* angle of depression or elevation */
		      glPushMatrix();               /* Save Wheel_1 coord. sys */
			     glRotatef(-90, 0.0, 1.0, 0.0);
			     glColor3f(0.751,0.751, 0.751);
                 gluDisk( disk,0.0,1.8,26, 13); /* Base circle face (dark Gray) */
			  glPopMatrix();                /* Back to Wheel_1 coord. sys */
			  glPushMatrix();               /* Save Wheel_1 coord. sys */
                 glColor3f(0.0,0.0,0.0);
                 glTranslatef(0.01,0.0,0.0); 
				 glRotatef(-90, 0.0, 1.0, 0.0);         
                 gluDisk( disk,0.0,3.0,26,13); /* Base circle face (black) */
			  glPopMatrix();                 /* Back to Wheel_1 coord. sys */
			  glPushMatrix();               /* Save Wheel_1 coord. sys */
			     glTranslatef(0.01,0.0,0.0); 
			     glRotatef(90, 0.0, 1.0, 0.0);
			     glColor3f(0.0,0.0, 0.0);
                 gluCylinder(cylind, 3.0,3.0,5.0,26,13); /*Cylinder*/
			  glPopMatrix();                 /* Back to Wheel_1 coord. sys */
			  glPushMatrix();               /* Save Wheel_1 coord. sys */
                 glColor3f(0.0,0.0,0.0);
                 glTranslatef(5.01,0.0,0.0); 
				 glRotatef(-90, 0.0, 1.0, 0.0);         
                 gluDisk( disk,0.0,3.0,26,13); /* Top circle face (black) */
			  glPopMatrix();                 /* Back to Wheel_1 coord. sys */
			  glPushMatrix();               /* Save Wheel_1 coord. sys */
			     glTranslatef(5.02,0.0,0.0); 
			     glRotatef(-90, 0.0, 1.0, 0.0);
			     glColor3f(0.751,0.751, 0.751);
                 gluDisk( disk,0.0,1.8,26, 13); /* Top circle face (dark Gray) */
				 glLineWidth (5.0);
				 glColor3f(0.412,0.412, 0.412);
                 glBegin(GL_LINES);             /* Draw lines */
                    glVertex3f(1.7,0.0,0.01);
                    glVertex3f(-1.7,0.0,0.01);
                 glEnd();
                 glBegin(GL_LINES);             /* Draw lines */
                    glVertex3f(0.0,1.7,0.01);
                    glVertex3f(0.0,-1.7,0.01);
                 glEnd();
			  glPopMatrix();                /* Back to Wheel_1 coord. sys */
	  glPopMatrix(); /* Back to Car_M1 coord. sys */  
   
	  /*-------Draw the front_right wheels -----*/
	  glPushMatrix();               /* Save Car_M1 coord. sys */
	     glTranslatef(-8.5,-1.0, 13.5); 
	     glRotatef(wheel_yawang, 0.0, 1.0, 0.0); /* right and left rotation angle */
         glRotatef(wheel_rollang, 1.0, 0.0, 0.0); /* Scroll */
         glRotatef(wheel_pitchang, 0.0, 0.0, 1.0); /* angle of depression or elevation */
		      glPushMatrix();               /* Save Wheel_1 coord. sys */
			     glRotatef(-90, 0.0, 1.0, 0.0);
			     glColor3f(0.751,0.751, 0.751);
                 gluDisk( disk,0.0,1.8,26, 13); /* Base circle face (dark Gray) */
			  glPopMatrix();                /* Back to Wheel_1 coord. sys */
			  glPushMatrix();               /* Save Wheel_1 coord. sys */
                 glColor3f(0.0,0.0,0.0);
                 glTranslatef(-0.01,0.0,0.0); 
				 glRotatef(-90, 0.0, 1.0, 0.0);         
                 gluDisk( disk,0.0,3.0,26,13); /* Base circle face (black) */
			  glPopMatrix();                 /* Back to Wheel_1 coord. sys */
			  glPushMatrix();               /* Save Wheel_1 coord. sys */
			     glTranslatef(-0.01,0.0,0.0); 
			     glRotatef(-90, 0.0, 1.0, 0.0);
			     glColor3f(0.0,0.0, 0.0);
                 gluCylinder(cylind, 3.0,3.0,5.0,26,13); /*Cylinder*/
			  glPopMatrix();                 /* Back to Wheel_1 coord. sys */
			  glPushMatrix();               /* Save Wheel_1 coord. sys */
                 glColor3f(0.0,0.0,0.0);
                 glTranslatef(-5.01,0.0,0.0); 
				 glRotatef(-90, 0.0, 1.0, 0.0);         
                 gluDisk( disk,0.0,3.0,26,13); /* Top circle face (black) */
			  glPopMatrix();                 /* Back to Wheel_1 coord. sys */
			  glPushMatrix();               /* Save Wheel_1 coord. sys */
			     glTranslatef(-5.02,0.0,0.0); 
			     glRotatef(-90, 0.0, 1.0, 0.0);
			     glColor3f(0.751,0.751, 0.751);
                 gluDisk( disk,0.0,1.8,26, 13); /* Top circle face (dark Gray) */
				 glLineWidth (5.0);
				 glColor3f(0.412,0.412, 0.412);
                 glBegin(GL_LINES);             /* Draw lines */
                    glVertex3f(1.7,0.0,0.01);
                    glVertex3f(-1.7,0.0,0.01);
                 glEnd();
                 glBegin(GL_LINES);             /* Draw lines */
                    glVertex3f(0.0,1.7,0.01);
                    glVertex3f(0.0,-1.7,0.01);
                 glEnd();
			  glPopMatrix();                /* Back to Wheel_1 coord. sys */
	  glPopMatrix(); /* Back to Car_M1 coord. sys */  

	  /*-------Draw the back_left wheels -----*/
	  glPushMatrix();               /* Save Car_M1 coord. sys */
	     glTranslatef(8.5,-1.0,-13.5);
	     glRotatef(wheel_changeang, 0.0, 1.0, 0.0); /* right and left rotation angle , the same as car_body*/
         glRotatef(wheel_rollang, 1.0, 0.0, 0.0); /* Scroll */
         glRotatef(wheel_pitchang, 0.0, 0.0, 1.0); /* angle of depression or elevation */
		      glPushMatrix();               /* Save Wheel_1 coord. sys */
			     glRotatef(-90, 0.0, 1.0, 0.0);
			     glColor3f(0.751,0.751, 0.751);
                 gluDisk( disk,0.0,1.8,26, 13); /* Base circle face (dark Gray) */
			  glPopMatrix();                /* Back to Wheel_1 coord. sys */
			  glPushMatrix();               /* Save Wheel_1 coord. sys */
                 glColor3f(0.0,0.0,0.0);
                 glTranslatef(0.01,0.0,0.0); 
				 glRotatef(-90, 0.0, 1.0, 0.0);         
                 gluDisk( disk,0.0,3.0,26,13); /* Base circle face (black) */
			  glPopMatrix();                 /* Back to Wheel_1 coord. sys */
			  glPushMatrix();               /* Save Wheel_1 coord. sys */
			     glTranslatef(0.01,0.0,0.0); 
			     glRotatef(90, 0.0, 1.0, 0.0);
			     glColor3f(0.0,0.0, 0.0);
                 gluCylinder(cylind, 3.0,3.0,5.0,26,13); /*Cylinder*/
			  glPopMatrix();                 /* Back to Wheel_1 coord. sys */
			  glPushMatrix();               /* Save Wheel_1 coord. sys */
                 glColor3f(0.0,0.0,0.0);
                 glTranslatef(5.01,0.0,0.0); 
				 glRotatef(-90, 0.0, 1.0, 0.0);         
                 gluDisk( disk,0.0,3.0,26,13); /* Top circle face (black) */
			  glPopMatrix();                 /* Back to Wheel_1 coord. sys */
			  glPushMatrix();               /* Save Wheel_1 coord. sys */
			     glTranslatef(5.02,0.0,0.0); 
			     glRotatef(-90, 0.0, 1.0, 0.0);
			     glColor3f(0.751,0.751, 0.751);
                 gluDisk( disk,0.0,1.8,26, 13); /* Top circle face (dark Gray) */
				 glLineWidth (5.0);
				 glColor3f(0.412,0.412, 0.412);
                 glBegin(GL_LINES);             /* Draw lines */
                    glVertex3f(1.7,0.0,0.01);
                    glVertex3f(-1.7,0.0,0.01);
                 glEnd();
                 glBegin(GL_LINES);             /* Draw lines */
                    glVertex3f(0.0,1.7,0.01);
                    glVertex3f(0.0,-1.7,0.01);
                 glEnd();
			  glPopMatrix();                /* Back to Wheel_1 coord. sys */
	  glPopMatrix(); /* Back to Car_M1 coord. sys */  

	  /*-------Draw the back_right wheels -----*/
	  glPushMatrix();               /* Save Car_M1 coord. sys */
	     glTranslatef(-8.5,-1.0,-13.5); 
	     glRotatef(wheel_changeang, 0.0, 1.0, 0.0); /* right and left rotation angle ,the same as car_body*/
         glRotatef(wheel_rollang, 1.0, 0.0, 0.0); /* Scroll */
         glRotatef(wheel_pitchang, 0.0, 0.0, 1.0); /* angle of depression or elevation */
		      glPushMatrix();               /* Save Wheel_1 coord. sys */
			     glRotatef(-90, 0.0, 1.0, 0.0);
			     glColor3f(0.751,0.751, 0.751);
                 gluDisk( disk,0.0,1.8,26, 13); /* Base circle face (dark Gray) */
			  glPopMatrix();                /* Back to Wheel_1 coord. sys */
			  glPushMatrix();               /* Save Wheel_1 coord. sys */
                 glColor3f(0.0,0.0,0.0);
                 glTranslatef(-0.01,0.0,0.0); 
				 glRotatef(-90, 0.0, 1.0, 0.0);         
                 gluDisk( disk,0.0,3.0,26,13); /* Base circle face (black) */
			  glPopMatrix();                 /* Back to Wheel_1 coord. sys */
			  glPushMatrix();               /* Save Wheel_1 coord. sys */
			     glTranslatef(-0.01,0.0,0.0); 
			     glRotatef(-90, 0.0, 1.0, 0.0);
			     glColor3f(0.0,0.0, 0.0);
                 gluCylinder(cylind, 3.0,3.0,5.0,26,13); /*Cylinder*/
			  glPopMatrix();                 /* Back to Wheel_1 coord. sys */
			  glPushMatrix();               /* Save Wheel_1 coord. sys */
                 glColor3f(0.0,0.0,0.0);
                 glTranslatef(-5.01,0.0,0.0); 
				 glRotatef(-90, 0.0, 1.0, 0.0);         
                 gluDisk( disk,0.0,3.0,26,13); /* Top circle face (black) */
			  glPopMatrix();                 /* Back to Wheel_1 coord. sys */
			  glPushMatrix();               /* Save Wheel_1 coord. sys */
			     glTranslatef(-5.02,0.0,0.0); 
			     glRotatef(-90, 0.0, 1.0, 0.0);
			     glColor3f(0.751,0.751, 0.751);
                 gluDisk( disk,0.0,1.8,26, 13); /* Top circle face (dark Gray) */
				 glLineWidth (5.0);
				 glColor3f(0.412,0.412, 0.412);
                 glBegin(GL_LINES);             /* Draw lines */
                    glVertex3f(1.7,0.0,0.01);
                    glVertex3f(-1.7,0.0,0.01);
                 glEnd();
                 glBegin(GL_LINES);             /* Draw lines */
                    glVertex3f(0.0,1.7,0.01);
                    glVertex3f(0.0,-1.7,0.01);
                 glEnd();
			  glPopMatrix();                /* Back to Wheel_1 coord. sys */
	  glPopMatrix(); /* Back to Car_M1 coord. sys */  
  glPopMatrix();/* Back to Main coord. sys */
  glPopMatrix();/* Back to Main coord. sys */
  glDisable(GL_COLOR_MATERIAL);
}
/*------------------------------------------------------
 * Procedure to make projection matrix
 */
void make_projection(int x)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if(x==4)
	  gluPerspective(Fovy, Aspect, ZNear, ZFar);//gluPerspective(GLdouble fovy,GLdouble aspect,GLdouble zNear,GLdouble zFar)(沿Z負軸的距離)
  else
	  glOrtho(-150.0, 150.0, -150.0*(float)height/(float)width, //void glOrtho(GLdouble left,GLdouble right,GLdouble bottom,GLdouble top,GLdouble near,GLdouble far)
		      150.0*(float)height/(float)width, 1.0, 300.0);

  glMatrixMode(GL_MODELVIEW);
}
/*-------------------------------------------------------
 * Make viewing matrix
 */
void make_view(int x)
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  switch(x){
  case 4:       /* Perspective */

    glRotatef(-world_yawang,0.0,1.0,0.0);
    glRotatef(world_rollang,1.0,0.0,0.0); 
	glRotatef(-world_pitchang,0.0,0.0,1.0); 
	glTranslatef(-eyeT[0],-eyeT[1], -eyeT[2]); 
    gluLookAt(eye[0], eye[1], eye[2], 
	      eye[0]-u[2][0], eye[1]-u[2][1], eye[2]-u[2][2],
	      u[1][0], u[1][1], u[1][2]);
    break;

  case 1:       /* X direction parallel viewing */
    gluLookAt(150.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    break;
  case 2:       /* Y direction parallel viewing */
    gluLookAt(0.0, 150.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0);
    break;
  case 3:       /* Z direction parallel viewing */
    gluLookAt(0.0, 0.0, 150.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    break;
  }
}
/*-------------------------------------------------------
 * Display callback func. This func. draws three
 * cubes at proper places to simulate a solar system.
 */
void display()
{
  /*Clear previous frame and the depth buffer */
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
  switch(style){
  case 0:

    make_view(4);
    make_projection(4);
    glViewport(width/2, 0, width/2, height/2);
    draw_scene();
	 
	make_view(1);
    make_projection(1);
	glViewport(0, height/2, width/2, height/2);
    draw_scene();
	make_view(1);
    draw_eye_axes();

	make_view(2);
    glViewport(width/2, height/2, width/2, height/2);
    draw_scene();
    make_view(2);
    draw_eye_axes();

    make_view(3);
    glViewport(0, 0, width/2, height/2);
    draw_scene();
    make_view(3);
    draw_eye_axes();
    break;

	case 4:
    make_view(4);
    make_projection(4);
	glViewport(0, 0, width, height);
    draw_scene();
	break;
  }
  /*-------Swap the back buffer to the front --------*/
  glutSwapBuffers();
  return;
}
/*--------------------------------------------------
 * Reshape callback function which defines the size
 * of the main window when a reshape event occurrs.
 */
void my_reshape(int w, int h)
{

  width = w;
  height = h;

  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if(w>h)
    glOrtho(-40.0, 40.0, -40.0*(float)h/(float)w, 40.0*(float)h/(float)w, 
             -100.0, 100.0);
  else
    glOrtho(-40.0*(float)w/(float)h, 40.0*(float)w/(float)h, -40.0, 40.0, 
            -100.0, 100.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}
void car_up_down()
{
	if((car_positionfr[0]-car_positionbr[0])/(car_positionfr[2]-car_positionbr[2]) >= 0)
		car_body_rollang=-20;
	if((car_positionfr[0]-car_positionbr[0])/(car_positionfr[2]-car_positionbr[2]) < 0)
		car_body_rollang=20;
}
/*-------------------------------------------------
*算出車體的前後左右實際位置
*判斷是否在爬坡
*旋轉car_body_rollang和car_body_pitchang
*/
void climb()
{
	if( 0.0 < car_position[2] && car_position[2]+10<80.0)
	{
		car_up_down();
		car_position[1]=(car_position[2]+17)*tan(20.0*PI/180.0);
	}
	else if(car_position[2]+10>=80.0)
	{
		car_body_rollang=0.0;
		car_position[1]=29.1+4.5;
	}
	else
	{
		car_body_rollang=0.0;
		car_position[1]=4.5;
	}
}
/*-----------------------------------------------------
*判斷車子是否撞到障礙物
*
*/
void car_mov()
{
	float  car_positionfm[3]={(car_positionfr[0]+car_positionfl[0])/2, 4.5, (car_positionfr[2]+car_positionfl[2])/2};
    float  car_positionbm[3]={(car_positionbr[0]+car_positionbl[0])/2, 4.5, (car_positionbr[2]+car_positionbl[2])/2};

	if( (pow((car_positionfr[0]+0.0),2.0)+pow((car_positionfr[2]+0.0),2.0))<36.0 || (pow((car_positionfl[0]+0.0),2.0)+pow((car_positionfl[2]+0.0),2.0))<36.0 || (pow((car_positionfm[0]+0.0),2.0)+pow((car_positionfm[2]+0.0),2.0))<36.0)
		Movf=false;
	else
		Movf=true;

	if( (pow((car_positionbr[0]+0.0),2.0)+pow((car_positionbr[2]+0.0),2.0))<36.0 || (pow((car_positionbl[0]+0.0),2.0)+pow((car_positionbl[2]+0.0),2.0))<36.0 || (pow((car_positionbm[0]+0.0),2.0)+pow((car_positionbm[2]+0.0),2.0))<36.0)
		Movb=false;
	else
		Movb=true;

	if( Movf )
	{
		if( (pow((car_positionfr[0]-120.0),2.0)+pow((car_positionfr[2]+120.0),120.0))<110.0 || (pow((car_positionfl[0]-120.0),2.0)+pow((car_positionfl[2]+120.0),2.0))<110.0 || (pow((car_positionfm[0]-120.0),2.0)+pow((car_positionfm[2]+120.0),2.0))<110.0)
		    Movf=false;
	    else
		    Movf=true;    
	}
	else{}
	if( Movb )
	{
		if( (pow((car_positionbr[0]-120.0),2.0)+pow((car_positionbr[2]+120.0),120.0))<110.0 || (pow((car_positionbl[0]-120.0),2.0)+pow((car_positionbl[2]+120.0),2.0))<110.0 || (pow((car_positionbm[0]-120.0),2.0)+pow((car_positionbm[2]+120.0),2.0))<110.0)
		    Movb=false;
	    else
		    Movb=true;
	}
	else{}
		  	  
}
void car_four_position_f()
{
	car_positionfr[2] += Step*cos(car_body_yawang*PI/180.0);
    car_positionfr[0] += Step*sin(car_body_yawang*PI/180.0);
	car_positionfl[2] += Step*cos(car_body_yawang*PI/180.0);
    car_positionfl[0] += Step*sin(car_body_yawang*PI/180.0);
	car_positionbr[2] += Step*cos(car_body_yawang*PI/180.0);
    car_positionbr[0] += Step*sin(car_body_yawang*PI/180.0);
	car_positionbl[2] += Step*cos(car_body_yawang*PI/180.0);
    car_positionbl[0] += Step*sin(car_body_yawang*PI/180.0);
}
void car_four_position_b()
{
	car_positionfr[2] -= Step*cos(car_body_yawang*PI/180.0);
    car_positionfr[0] -= Step*sin(car_body_yawang*PI/180.0);
	car_positionfl[2] -= Step*cos(car_body_yawang*PI/180.0);
    car_positionfl[0] -= Step*sin(car_body_yawang*PI/180.0);
	car_positionbr[2] -= Step*cos(car_body_yawang*PI/180.0);
    car_positionbr[0] -= Step*sin(car_body_yawang*PI/180.0);
	car_positionbl[2] -= Step*cos(car_body_yawang*PI/180.0);
    car_positionbl[0] -= Step*sin(car_body_yawang*PI/180.0);
}
void idle()
{
	windmillangl+=0.5;
	display();
}
/*--------------------------------------------------
 * Keyboard callback func. When a 'q' key is pressed,
 * the program is aborted.
 */
void my_keyboard(unsigned char key, int x, int y)
{
  if(key=='Q'||key=='q') exit(0);
  if(key=='s') /*Show gridlines of floor*/
  {
	  if(GF)
		  GF=false;
	  else
		  GF=true;
  }
  if(key=='o') /*Light on/off*/
  {
	  if(LI)
	  {
		  LI=false;
		  light_colors[0]=0.302;
		  light_colors[1]=0.223;
		  light_colors[2]=0.0;
	  }
	  else
	  {
		  LI=true;
		  light_colors[0]=1.0;
		  light_colors[1]=0.843;
		  light_colors[2]=0.0;
	  }
  }
  if(key=='i')  /*Forward */
  {
	  if(Con)
	  {}
	  else
	  {
	      car_body_yawang += wheel_yawang;
	      wheel_changeang = wheel_yawang;
		  wheel_yawang=0.0;
	  }
	  car_mov();
	  if(Movf)
	  {
      car_position[2] += Step*cos(car_body_yawang*PI/180.0);
      car_position[0] += Step*sin(car_body_yawang*PI/180.0);
	  car_four_position_f();
	  wheel_rollang+=5.0;
	  climb();
	  Con=true;
	  }
	  else
	  {}
  }else if(key=='k') /* Back */
  { 
	  if(Con)
	  {}
	  else
	  {
	      car_body_yawang += wheel_yawang;
	      wheel_changeang = wheel_yawang;
		  wheel_yawang=0.0;
	  }
	  car_mov();
	  if(Movb)
	  {
      car_position[2] -= Step*cos(car_body_yawang*PI/180.0);
      car_position[0] -= Step*sin(car_body_yawang*PI/180.0);
	  car_four_position_b();
	  wheel_rollang-=5.0;
	  climb();
	  Con=true;
	  }
	  else
	  {}
  }else if(key=='l') /*Turn right*/
  {
	  if(Con)
		   Con=false;
	  else
	  
		  if(wheel_yawang>=(-5.0))
               wheel_yawang -=0.5;
	       else{}
		  
  }else if(key=='j') /*Turn left*/
  {
	  if(Con)
		   Con=false;
	  else
	  
		  if(wheel_yawang<= 5.0)
               wheel_yawang +=0.5;
	       else{}
  }
  if(key=='a')
  {
	  world_yawang=0.0;
	  world_rollang=0.0;
	  world_pitchang=0.0; 
	  eyeT[0]=0.0;
	  eyeT[1]=0.0;
	  eyeT[2]=0.0;
	  eye[0]=0.0;
	  eye[1]=4.5;
	  eye[2]=-80.0;
	  Fovy=90.0;
  }
  if(key=='z') world_yawang-=5.0;  /*Horizontal clockwise*/
  if(key=='x') world_yawang+=5.0;  /*Horizontal counterclockwise*/
  if(key=='c') world_rollang-=5.0; /*Turn up*/
  if(key=='v') world_rollang+=5.0; /*Turn down*/
  if(key=='b') world_pitchang-=5.0; 
  if(key=='n') world_pitchang+=5.0; 
  if(key=='m') eyeT[0]+=3.0;
  if(key==',') eyeT[0]-=3.0;
  if(key=='.') eyeT[1]+=3.0;
  if(key=='/') eyeT[1]-=3.0;
  if(key=='[') eyeT[2]+=3.0;   
  if(key==']') eyeT[2]-=3.0;   
  if(key=='-') style=0;
  if(key=='=') style=4;
  if(key=='1') Fovy+=zoom;/* zoom out */
  if(key=='2') Fovy-=zoom;/* zoom in */
  display();
}
/*---------------------------------------------------
 * Main procedure which defines the graphics environment,
 * the operation style, and the callback func's.
 */
void main(int argc, char **argv)
{
  /*-----Initialize the GLUT environment-------*/
  glutInit(&argc, argv);

  /*-----Depth buffer is used, be careful !!!----*/
  glutInitDisplayMode(GLUT_DOUBLE| GLUT_RGB| GLUT_DEPTH);

  glutInitWindowSize(width, height);
  glutCreateWindow("HW_2_Car");

  myinit();      /*---Initialize other state varibales----*/
  
  /*----Associate callback func's whith events------*/
  glutDisplayFunc(display);
  /*  glutIdleFunc(display); */
  glutReshapeFunc(my_reshape);
  glutKeyboardFunc(my_keyboard);
  glutIdleFunc(idle);

  glutMainLoop();
}

