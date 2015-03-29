/*
 *	Draw cubes for opengl
 *
 *	16/02/2015
 */
#ifndef _GL_CUBE_H_
#define _GL_CUBE_H_

class GLCube
{
public:
	GLCube();
	GLCube(float l_centerX, float l_centerY, float l_centerZ);

	void draw();
	static void applyMat(float r, float v, float b, float ambient_factor = 0.1);

	~GLCube();

	float x, y, z;
	float size;
};

#endif _GL_CUBE_H_