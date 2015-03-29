#include "StarManager.h"

#include <time.h>
#include <math.h>

#define PI 3.14159265359

StarManager::StarManager(DayManager * _day, int star_num, NYRenderer * _renderer)
	:day(_day), starNumber(star_num), renderer(_renderer), poppin_star(1.f)
{
	int i;

	srand(time(NULL));

	starColor = NYColor(1, 1, 1, 1);

	stars = new star[star_num];
}

void StarManager::createSky(float r_min, float r_max, float c_x, float c_y, float c_z)
{
	// Boucle de création des étoiles
	for (int i = 0; i < starNumber; i++)
	{
		/* Calcul de la distance de l'étoile par rapport au centre */
		float rad = rand() % (int)(r_max - r_min) + r_min;
		float alpha = rand() % (int)180;
		float beta = rand() % (int)360;

		float x = rad * cos(alpha * (PI / 180)) * sin(beta * (PI / 360));
		float y = rad * cos(beta * (PI / 360));
		float z = rad * sin(alpha * (PI / 180)) * sin(beta * (PI / 360));// rad * sin(alpha * (PI / 180));

		stars[i].position = NYVert3Df(x + c_x, y + c_y, z + c_z);
		stars[i].size = (rand() % 5) + 2;
	}
}

void StarManager::draw()
{
	glPushMatrix();

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	if (renderer != NULL)
		glTranslatef(renderer->_Camera->_Position.X, renderer->_Camera->_Position.Y, renderer->_Camera->_Position.Z);

	glRotated(-(day->getDayTime() * 360.f) - 90, 0, 1, 0);

	if (day->getDayState() == NIGHT || day->getDayState() == END_NIGHT)
	{
		glColor3f(starColor.R, starColor.V, starColor.B);

		for (int i = 0; i < starNumber; i++)
		{
			glPushMatrix();

			glTranslatef(stars[i].position.X, stars[i].position.Y, stars[i].position.Z);
			glutSolidCube(stars[i].size);

			glPopMatrix();
		}
	}

	glPopMatrix();
}


StarManager::~StarManager()
{
}
