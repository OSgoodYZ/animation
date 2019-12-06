#include "camera.h"
#include <GL/glu.h>
#include <cmath>
#include <iostream>

#define TRACKBALLSIZE  (0.6f)

Camera::Camera ()
{
	fovAngle = 45.0;
	aspectRatio = 1.0;
	nearPlane = 0.1;
	farPlane = 10000.0;

	moving = 0;
	beginu = 0;
	beginv = 0;

	trackball (curquat, 0.0, 0.0, 0.0, 0.0, 0.0);
	x = y = z = 0.0;
	dolly = 3.0;

	rot_scale   = 2.0;
	trans_scale = 3.0;
	zoom_scale  = 10.0;
}

void Camera::resize (int _W, int _H)
{
	H = _H;
	W = _W;
	glViewport (0, 0, (GLint)W, (GLint)H);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	aspectRatio = static_cast<float>(W)/static_cast<float>(H);
	gluPerspective (fovAngle, aspectRatio, nearPlane, farPlane);
	glMatrixMode (GL_MODELVIEW);
}

void Camera::move (float dx, float dy, float dz)
{
	x += trans_scale * dx;
	y += trans_scale * dy;
	z += trans_scale * dz;
}

void Camera::beginRotate (int u, int v)
{
	beginu = u;
	beginv = v;
	moving = 1;
}

void Camera::rotate (int u, int v)
{
	if (moving)
	{
		trackball(lastquat,
				(2.0 * beginu - W) / W,
				(H - 2.0 * beginv) / H,
				(2.0 * u - W) / W,
				(H - 2.0 * v) / H, rot_scale);
		beginu = u;
		beginv = v;
		multiply (lastquat, curquat, curquat);
	}
}

void Camera::endRotate ()
{
	moving = false;
}

void Camera::zoom (float z)
{
	dolly += zoom_scale * z;
}

void Camera::apply ()
{
	glLoadIdentity();
	glTranslatef (x, y, z);
	GLfloat m[4][4];
	quat_to_matrix(m, curquat);
	glTranslatef (0.0, 0.0, -(dolly+10));
	glMultMatrixf(&m[0][0]);
}

//
//  Simulating virtual Trackball
//
float project_to_sphere(float r, float x, float y)
{
	float d, t, z;

	d = sqrt(x*x + y*y);
	if (d < r * 0.70710678118654752440) {    /* Inside sphere */
		z = sqrt(r*r - d*d);
	} else {           /* On hyperbola */
		t = r / 1.41421356237309504880;
		z = t*t / d;
	}

	return z;
}

void Camera::trackball(float q[4], float p1x, float p1y, float p2x, float p2y, float scale)
{
	float a[3]; /* Axis of rotation */
	float phi;  /* how much to rotate about axis */
	float p1[3], p2[3], d[3];
	float t;

	if (p1x == p2x && p1y == p2y) {
		q[0] = 0.0;
		q[1] = 0.0;
		q[2] = 0.0;
		q[3] = 1.0;
		return;
	}

	p1[0] = p1x;
	p1[1] = p1y;
	p1[2] = project_to_sphere(TRACKBALLSIZE,p1x,p1y);
	p2[0] = p2x;
	p2[1] = p2y;
	p2[2] = project_to_sphere(TRACKBALLSIZE,p2x,p2y);

	a[0] = p2[1]*p1[2]-p2[2]*p1[1];
	a[1] = p2[2]*p1[0]-p2[0]*p1[2];
	a[2] = p2[0]*p1[1]-p2[0]*p1[1];

	d[0] = p1[0]-p2[0];
	d[1] = p1[1]-p2[1];
	d[2] = p1[2]-p2[2];
	t  = sqrt(d[0]*d[0]+d[1]*d[1]+d[2]*d[2]) /
		(2.0*TRACKBALLSIZE);

	if (t > 1.0) t = 1.0;
	if (t < -1.0) t = -1.0;
	phi = scale * asin(t);

	axis_to_quat(a,phi,q);
}

void Camera::axis_to_quat(float a[3], float phi, float q[4])
{
	float l = sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
	float s = sin(phi/2.0);
	float c = cos(phi/2.0);

	q[0] = a[0]*s/l;
	q[1] = a[1]*s/l;
	q[2] = a[2]*s/l;
	q[3] = c;
}

void Camera::multiply(float q1[4], float q2[4], float d[4])
{
	d[0] = q2[1]*q1[2] - q2[2]*q1[1] + q1[0]*q2[3] + q2[0]*q1[3];
	d[1] = q2[2]*q1[0] - q2[0]*q1[2] + q1[1]*q2[3] + q2[1]*q1[3];
	d[2] = q2[0]*q1[1] - q2[1]*q1[0] + q1[2]*q2[3] + q2[2]*q1[3];
	d[3] = q1[3]*q2[3] - q1[0]*q2[0] - q1[1]*q2[1] - q1[2]*q2[2];

	float l = sqrt(d[0]*d[0]+d[1]*d[1]+d[2]*d[2]+d[3]*d[3]);
	d[0] /= l;
	d[1] /= l;
	d[2] /= l;
	d[3] /= l;
}

void Camera::quat_to_matrix(float m[4][4], float q[4])
{
	m[0][0] = 1.0 - 2.0 * (q[1] * q[1] + q[2] * q[2]);
	m[0][1] = 2.0 * (q[0] * q[1] - q[2] * q[3]);
	m[0][2] = 2.0 * (q[2] * q[0] + q[1] * q[3]);
	m[0][3] = 0.0;

	m[1][0] = 2.0 * (q[0] * q[1] + q[2] * q[3]);
	m[1][1] = 1.0 - 2.0 * (q[2] * q[2] + q[0] * q[0]);
	m[1][2] = 2.0 * (q[1] * q[2] - q[0] * q[3]);
	m[1][3] = 0.0;

	m[2][0] = 2.0 * (q[2] * q[0] - q[1] * q[3]);
	m[2][1] = 2.0 * (q[1] * q[2] + q[0] * q[3]);
	m[2][2] = 1.0 - 2.0 * (q[1] * q[1] + q[0] * q[0]);
	m[2][3] = 0.0;

	m[3][0] = 0.0;
	m[3][1] = 0.0;
	m[3][2] = 0.0;
	m[3][3] = 1.0;
}
