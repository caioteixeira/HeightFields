// assign1.cpp : Defines the entry point for the console application.
//

/*
  CSCI 480 Computer Graphics
  Assignment 1: Height Fields
  C++ starter code
*/

#include "stdafx.h"
#include <pic.h>
#include <windows.h>
#include <stdlib.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>

int g_iMenuId;

int g_vMousePos[2] = {0, 0};
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;

CONTROLSTATE g_ControlState = ROTATE;

/* state of the world */
/* Arbitrary good initial values*/
float g_vLandRotate[3] = {0.0, 0.0, 0.0};
float g_vLandTranslate[3] = { -0.45, -0.45, 0.0 };
float g_vLandScale[3] = {1.0, 1.0, 1.0};

/* Render mode. */
typedef enum { VERTICES, WIREFRAME, TRIANGLES} RENDERMODE;
RENDERMODE g_RenderMode = TRIANGLES;

/* Color Mode*/
typedef enum { HEIGHT, COLOR} COLORMODE;
COLORMODE g_ColorMode = HEIGHT;

/* Window scale*/
float g_width = 640;
float g_height = 480;

/* see <your pic directory>/pic.h for type Pic */
Pic * g_pHeightData;

/* Color data*/
Pic * g_pColorData;

/* Write a screenshot to the specified filename */
void saveScreenshot (char *filename)
{
  int i, j;
  Pic *in = NULL;

  if (filename == NULL)
    return;

  /* Allocate a picture buffer */
  in = pic_alloc(640, 480, 3, NULL);

  printf("File to save to: %s\n", filename);

  for (i=479; i>=0; i--) {
    glReadPixels(0, 479-i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
                 &in->pix[i*in->nx*in->bpp]);
  }

  if (jpeg_write(filename, in))
    printf("File saved Successfully\n");
  else
    printf("Error in Saving\n");

  pic_free(in);
}

void reshape(int w, int h)
{
	GLfloat aspect = (GLfloat)w / (GLfloat)h;
	

	//Set up view
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, aspect, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
}

void myinit()
{
  /* setup gl view here */
  glClearColor(0.0, 0.0, 0.0, 0.0);

  glEnable(GL_DEPTH_TEST);
}

GLfloat getHeight(int j, int i)
{
	unsigned char red = PIC_PIXEL(g_pHeightData, j, i, 0);
	unsigned char green = PIC_PIXEL(g_pHeightData, j, i, 1);
	unsigned char blue = PIC_PIXEL(g_pHeightData, j, i, 2);

	return (GLfloat) (0.3*red + 0.3*green + 0.4*blue)/255;
}

void setColor(int x, int y, GLfloat heightVal)
{
	//If Color Data is Null
	if (g_ColorMode == COLOR && !g_pColorData)
		g_ColorMode = HEIGHT;

	//Set the color for the current pixel
	if (g_ColorMode == COLOR)
	{
		unsigned char red = PIC_PIXEL(g_pColorData, x, y, 0);
		unsigned char green = PIC_PIXEL(g_pColorData, x, y, 1);
		unsigned char blue = PIC_PIXEL(g_pColorData, x, y, 2);
		glColor3f((GLfloat)red / 255, (GLfloat)green / 255, (GLfloat)blue /255);
	}
	else{
		glColor3f(1.0 - heightVal, 1.0 - heightVal, 1.0);
	}
}

void render()
{
	int width = g_pHeightData->ny;
	int height = g_pHeightData->nx;

	for (int i = 0; i < width - 1; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j < height; j++)
		{
			//Top Vertex
			GLfloat x = (GLfloat)j / height;
			GLfloat y = (GLfloat)i / width;
			GLfloat heightVal = getHeight(j, i);
			setColor(j, i, heightVal);
			glVertex3f(x, y, heightVal/2);

			//Bottom Vertex
			x = (GLfloat)j / height;
			y = (GLfloat)(i + 1) / width;
			heightVal = getHeight(j, i+1);
			setColor(j, i+1, heightVal);
			glVertex3f(x, y, heightVal/2);
		}
		glEnd();
	}
}

void display()
{
	//Update the renderMode
	switch (g_RenderMode)
	{
	case VERTICES:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case TRIANGLES:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	/*Transformations of the current world state*/
	glTranslatef(g_vLandTranslate[0], g_vLandTranslate[1], g_vLandTranslate[2]);
	glRotatef(g_vLandRotate[0], 1.0, 0.0, 0.0);
	glRotatef(g_vLandRotate[1], 0.0, 1.0, 0.0);
	glRotatef(g_vLandRotate[2], 0.0, 0.0, 1.0);
	glScalef(g_vLandScale[0], g_vLandScale[1], g_vLandScale[2]);

	/*Rendering*/
	render();
	

	glutSwapBuffers();
}

void menufunc(int value)
{
  switch (value)
  {
    case 0:
      exit(0);
      break;
  }
}

void doIdle()
{
  /* do some stuff... */

  /* make the screen update */
  glutPostRedisplay();
}


/*Keyboard callback*/
void keyboard(unsigned char key, int x, int y)
{
	//Rendermodes
	switch (key)
	{
	case '1':
		g_RenderMode = VERTICES;
		break;
	case '2':
		g_RenderMode = WIREFRAME;
		break;
	case '3':
		g_RenderMode = TRIANGLES;
		break;
	case '4':
		g_ColorMode = HEIGHT;
		break;
	case '5':
		g_ColorMode = COLOR;
		break;
	}
	

}



/* converts mouse drags into information about 
rotation/translation/scaling */
void mousedrag(int x, int y)
{
  int vMouseDelta[2] = {x-g_vMousePos[0], y-g_vMousePos[1]};
  
  switch (g_ControlState)
  {
    case TRANSLATE:  
      if (g_iLeftMouseButton)
      {
        g_vLandTranslate[0] += vMouseDelta[0]*0.01;
        g_vLandTranslate[1] -= vMouseDelta[1]*0.01;
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandTranslate[2] += vMouseDelta[1]*0.01;
      }
      break;
    case ROTATE:
      if (g_iLeftMouseButton)
      {
        g_vLandRotate[0] += vMouseDelta[1];
        g_vLandRotate[1] += vMouseDelta[0];
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandRotate[2] += vMouseDelta[1];
      }
      break;
    case SCALE:
      if (g_iLeftMouseButton)
      {
        g_vLandScale[0] *= 1.0+vMouseDelta[0]*0.01;
        g_vLandScale[1] *= 1.0-vMouseDelta[1]*0.01;
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandScale[2] *= 1.0-vMouseDelta[1]*0.01;
      }
      break;
  }
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void mouseidle(int x, int y)
{
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{

  switch (button)
  {
    case GLUT_LEFT_BUTTON:
      g_iLeftMouseButton = (state==GLUT_DOWN);
      break;
    case GLUT_MIDDLE_BUTTON:
      g_iMiddleMouseButton = (state==GLUT_DOWN);
      break;
    case GLUT_RIGHT_BUTTON:
      g_iRightMouseButton = (state==GLUT_DOWN);
      break;
  }
 
  switch(glutGetModifiers())
  {
    case GLUT_ACTIVE_CTRL:
      g_ControlState = TRANSLATE;
      break;
    case GLUT_ACTIVE_SHIFT:
      g_ControlState = SCALE;
      break;
    default:
      g_ControlState = ROTATE;
      break;
  }

  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

int main(int argc, char* argv[])
{
	// I've set the argv[1] to spiral.jpg.
	// To change it, on the "Solution Explorer",
	// right click "assign1", choose "Properties",
	// go to "Configuration Properties", click "Debugging",
	// then type your texture name for the "Command Arguments"
	if (argc<2)
	{  
		printf ("usage: %s heightfield.jpg\n", argv[0]);
		exit(1);
	}

	g_pHeightData = jpeg_read((char*)argv[1], NULL);
	if (!g_pHeightData)
	{
	    printf ("error reading %s.\n", argv[1]);
	    exit(1);
	}

	if (argc = 3)
	{
		g_pColorData = jpeg_read((char*)argv[2], NULL);
	}
	else
	{
		g_pColorData = NULL;
	}

	glutInit(&argc,(char**)argv);
	
	/*
	create a window here..should be double buffered and use depth testing

	the code past here will segfault if you don't have a window set up....
	replace the exit once you add those calls.
	*/
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	//Window creation
	glutInitWindowSize(g_width, g_height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Height Field");

	/* tells glut to use a particular display function to redraw */
	glutDisplayFunc(display);
  
	/* allow the user to quit using the right mouse button menu */
	g_iMenuId = glutCreateMenu(menufunc);
	glutSetMenu(g_iMenuId);
	glutAddMenuEntry("Quit",0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
  
	/* replace with any animate code */
	glutIdleFunc(doIdle);

	/* callback for mouse drags */
	glutMotionFunc(mousedrag);
	/* callback for idle mouse movement */
	glutPassiveMotionFunc(mouseidle);
	/* callback for mouse button changes */
	glutMouseFunc(mousebutton);
	/* callback for keyboard changes*/
	glutKeyboardFunc(keyboard);


	/* do initialization */
	myinit();

	glutMainLoop();
	return 0;
}