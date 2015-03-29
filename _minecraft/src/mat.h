#ifndef _MAT_H_
#define _MAT_H_

#include "engine\render\renderer.h"

void applyMaterial(float r, float v, float b, float ambient_factor)
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
	GLfloat materialAmbient[] = { r * ambient_factor, v * ambient_factor, b * ambient_factor, 0.1 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
}


#endif _MAT_H_