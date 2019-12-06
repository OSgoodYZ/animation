
#include <iostream>
#include <string>
#include <vector>
#include </usr/include/GL/gl.h>
#include </usr/include/GL/glu.h>
#include </usr/include/GL/glut.h>
#include "BVHObject.h"
#include "camera.h"

using namespace std;
static Camera camera;

namespace {
    clock_t   timeRecorder;
    BVHObject bvhObject;
    int       frameNumber = 0;
    int       width       = 1024;
    int       height      = 768;
    
    //const Vector3d eyePoint(- 3.0, 3, 8.0);
    //const Vector3d focusPoint(0.0, 1.5, 0.0);
    //const Vector3d upDirection(0.0, 1.0, 0.0);
    const GLfloat  lightPosition[4] = { - 5.0f, 10.0f, 10.0f, 1.0f };
    const float    wallSize         = 40.0f;
    const float    modelScale       = 0.030f;
   
}

static bool mouseRotatePressed = false;
static bool mouseMovePressed   = false;
static bool mouseZoomPressed   = false;
static int lastX = 90, lastY = 500, lastZoom = -5;


extern void glut_display(void);
extern void glut_reshape(int w, int h);
extern void glut_idle(void);
extern void glut_mouse(int button, int state, int x, int y);
extern void glut_motion(int x, int y);
extern void glut_keyboard (unsigned char key, int x, int y);


int option = -1;
string   fileName;

void keyboardCB(unsigned char keyPressed, int x, int y)
{
	switch (keyPressed) {
		case 'q':
			exit(0);
			break;
	}
	glutPostRedisplay();
}

void mouseCB(int button, int state, int x, int y)
{
	if (state == GLUT_UP)
	 {
		mouseMovePressed   = false;
		mouseRotatePressed = false;
		mouseZoomPressed   = true;
	}

		
	else
	{
		if	 (button==GLUT_MIDDLE_BUTTON)
		{
			lastX = x;
			lastY = y;
			mouseMovePressed   = true;
			mouseRotatePressed = false;
			mouseZoomPressed   = false;
		}
		else if (button==GLUT_LEFT_BUTTON && GLUT_ACTIVE_CTRL==glutGetModifiers())
		{
			lastZoom = y;
			mouseMovePressed   = false;
			mouseRotatePressed = false;
			mouseZoomPressed   = true;
		}

		else if (button==GLUT_LEFT_BUTTON)
		{
			camera.beginRotate(x,y);
			mouseMovePressed   = false;
			mouseRotatePressed = true;
			mouseZoomPressed   = false;
		}


	}
	glutPostRedisplay();
}


void motionCB(int x, int y)
{
	if (mouseRotatePressed == true)
	{
		camera.rotate(x, y);
	}
	else if (mouseMovePressed == true)
	{
		camera.move((x - lastX) / static_cast<float>(width),
				(lastY - y) / static_cast<float>(height), 0.0);
		lastX = x;
		lastY = y;
	}
	else if (mouseZoomPressed == true)
	{
		camera.zoom(float(y - lastZoom) / height);
		lastZoom = y;
	}

	glutPostRedisplay();
}



void init() {
    
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDisable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);
	glShadeModel(GL_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);

	const GLfloat lightColor[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat lightAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
	
    const GLfloat ambient[]   = { 0.2f, 0.2f, 0.2f, 1.0f };
	const GLfloat specular[]  = { 0.4f, 1.0f, 0.4f, 1.0f };
    const GLfloat shininess[] = { 40.0f };
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

void scene(void) {
    // wall
    const float size = wallSize;
    const float d    = size / 10.0f;
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    for (double dx0 = - size; dx0 < size; dx0 += d) {
        for (double dy0 = - size; dy0 < size; dy0 += d) {
            double dx1 = dx0 + d, dy1 = dy0 + d;
            glVertex3f(dx0, 0.0, dy0);
            glVertex3f(dx0, 0.0, dy1);
            glVertex3f(dx1, 0.0, dy1);
            glVertex3f(dx1, 0.0, dy0);
        }
    }
    glEnd();
    
    // character
    if (bvhObject.ready) {
        const float scale = modelScale;
        glColor3f(1.0f, 0.0f, 0.0f);
        glScalef(scale, scale, scale);
        bvhObject.render(frameNumber);
        
        frameNumber = frameNumber+2;
        frameNumber %= bvhObject.nFrames;
    }
}

void reshape(int w, int h)
{
	camera.zoom(0.01);
	camera.resize(w, h);
	
}
// void reshape(int w, int h) {
//     width  = w;
//     height = h;
// }
void display()
{

    glLoadIdentity();

	glOrtho(-15, 15, -15, 15, -15, 15);
	glMatrixMode(GL_MODELVIEW);
    
	//glLoadIdentity();
	//glTranslatef( 0.0, 0.0, - 5 );
	//glRotatef( - 20, 1.0, 0.0, 0.0 );
	//glRotatef( 0, 0.0, 1.0, 0.0 );


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	camera.apply();
	glColor3f(0.4, 0.4, 0.4);
    
    scene();

	glutSwapBuffers();
}

// void display(void) {
//     // draw scene
//     const Vector3d& e = eyePoint;
//     const Vector3d& f = focusPoint;
//     const Vector3d& u = upDirection;
    
//     // Set OpenGL viewport and camera
//     int w = width;
//     int h = height;
//     glViewport(0, 0, w, h);
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
//     // camera setting
//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();
//     gluPerspective(40.0, (double)w/h, 0.3, 20.0);
//     glMatrixMode(GL_MODELVIEW);
//     glLoadIdentity();
//     gluLookAt(e.x_, e.y_, e.z_, f.x_, f.y_, f.z_, u.x_, u.y_, u.z_);
//     glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    
//     scene();
    
// 	glutSwapBuffers();
// }


void glut_idle(void) {
    clock_t targetInterval = static_cast<clock_t>(1000.0 * 1000.0 * bvhObject.interval);
    if (clock() - timeRecorder > targetInterval) {
        glutPostRedisplay();
        timeRecorder = clock();
    }
}


void menu(int id)
{
	option = id;
    switch (option)
	{
        case 0:
            fileName = string("BVH_Data/1001.bvh");
            bvhObject.init(fileName);
        break;
        case 1:
            fileName = string("BVH_Data/1002.bvh");
            bvhObject.init(fileName);
        break;
        case 2:
            fileName = string("BVH_Data/1003.bvh");
            bvhObject.init(fileName);
        break;
        case 3:
            fileName = string("BVH_Data/1005.bvh");
            bvhObject.init(fileName);
        break;
        case 4:
            fileName = string("BVH_Data/1006.bvh");
            bvhObject.init(fileName);
        break;
        case 5:
            fileName = string("BVH_Data/1101.bvh");
            bvhObject.init(fileName);
        break;

    }

}

void manual()
{
	std::cout << "==================manual=================" << std::endl;
	std::cout << std::endl;
    std::cout << "   START   :  right click and select bvh" << std::endl;
    std::cout << "   EXIT    :  press 'q' key "<< std::endl;
	std::cout << "  Rotate   :  left click & drag" << std::endl;
	std::cout << "   Zoom    :  ctrl + left click & drag" << std::endl;
	std::cout << " Translate :  middle click & drag" << std::endl;

	std::cout << std::endl;
	std::cout << "=========================================" << std::endl;
}

void glut_mouse(int button, int state, int x, int y) {  }
void glut_motion(int x, int y) {  }
void glut_keyboard (unsigned char key, int x, int y) {  }
void glut_special (int key, int x, int y) {  }

int main(int argc, const char **argv) {
    glutInit(&argc, const_cast<char **>(argv));
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Project");
    manual();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardCB);
    glutReshapeFunc(reshape);
    glutIdleFunc(glut_idle);
    glutMouseFunc(mouseCB);
    glutMotionFunc(motionCB);
    
    init();
    
    glutCreateMenu(menu);
    glutAddMenuEntry(" 1 ", 0);
	glutAddMenuEntry(" 2 ", 1);
    glutAddMenuEntry(" 3 ", 2);
    glutAddMenuEntry(" 4 ", 3);
	glutAddMenuEntry(" 5 ", 4);
    glutAddMenuEntry(" 6 ", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);


    timeRecorder = clock();
    
    glutMainLoop();
    
    return 0;
}

