#include "GLCube.h"

#include "external/gl/glew.h"
#include "external/gl/freeglut.h"

GLCube::GLCube()
	:x(0), y(0), z(0)
{
}

GLCube::GLCube(float l_centerX, float l_centerY, float l_centerZ)
	:x(l_centerX), y(l_centerY), z(l_centerZ)
{
	size = 1;
}


void GLCube::draw()
{
	glPushMatrix();

	glTranslatef(x, y, z);

	// Draw the cube
	glBegin(GL_TRIANGLES);

	// Z back
	applyMat(0, 0, 1);

	// Normal
	glNormal3f(0, 0, -size);

	glVertex3d(size, -size, -size);
	glVertex3d(-size, -size, -size);
	glVertex3d(-size, size, -size);

	glVertex3d(size, size, -size);
	glVertex3d(size, -size, -size);
	glVertex3d(-size, size, -size);

	// Z front 

	// Normal
	glNormal3f(0, 0, size);

	glVertex3d(-size, -size, size);
	glVertex3d(size, -size, size);
	glVertex3d(-size, size, size);

	glVertex3d(size, -size, size);
	glVertex3d(size, size, size);
	glVertex3d(-size, size, size);

	// Y back
	//Emissive
	applyMat(0, 1, 0);

	// Normal
	glNormal3f(0, -size, 0);

	glVertex3d(-size, -size, size);
	glVertex3d(-size, -size, -size);
	glVertex3d(size, -size, -size);

	glVertex3d(size, -size, size);
	glVertex3d(-size, -size, size);
	glVertex3d(size, -size, -size);

	// Y front

	// Normal
	glNormal3f(0, size, 0);

	glVertex3d(-size, size, -size);
	glVertex3d(-size, size, size);
	glVertex3d(size, size, -size);

	glVertex3d(-size, size, size);
	glVertex3d(size, size, size);
	glVertex3d(size, size, -size);

	// X back
	//Emissive
	applyMat(1, 0, 0);

	// Normal
	glNormal3f(-size, 0, 0);

	glVertex3d(-size, size, -size);
	glVertex3d(-size, -size, -size);
	glVertex3d(-size, -size, size);

	glVertex3d(-size, size, size);
	glVertex3d(-size, size, -size);
	glVertex3d(-size, -size, size);

	// X front

	// Normal
	glNormal3f(size, 0, 0);

	glVertex3d(size, -size, -size);
	glVertex3d(size, size, -size);
	glVertex3d(size, -size, size);

	glVertex3d(size, size, -size);
	glVertex3d(size, size, size);
	glVertex3d(size, -size, size);

	glEnd();

	glPopMatrix();
}

void GLCube::applyMat(float r, float v, float b, float ambient_factor)
{
	//Diffuse
	GLfloat materialDiffuse[] = { r, v, b, 1.0 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	//Speculaire
	GLfloat whiteSpecularMaterial[] = { 0.3, 0.3, 0.3, 0.1 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecularMaterial);

	GLfloat mShininess = 100;
	glMaterialf(GL_FRONT, GL_SHININESS, mShininess);

	//Ambient
	GLfloat materialAmbient[] = { r * ambient_factor, v * ambient_factor, b * ambient_factor, 0.1};
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

}

GLCube::~GLCube()
{
}
