#include "Cloud.h"


float Cloud::cloud_size = 50.f;
float Cloud::drop_height = 2.f;
float Cloud::drop_width = 0.5f;

float Cloud::max_draw_dist = 1400.f;

Cloud::Cloud(NYVert3Df pos, NYVert3Df _wind_dir, ECloudType _type, int max_drop, NYWorld * _world)
	:wind(_wind_dir), position(pos), maxDrop(max_drop), world(_world)
{
	drop = new NYVert3Df[maxDrop];

	if (position.Z == 0)
		position.Z = 1.0;

	setType(_type);

	randShape();
}

void Cloud::setType(ECloudType _type)
{

	type = _type;
	for (int i = 0; i < maxDrop; i++)
	{
		drop[i] = newDropPos();

		drop[i].Z = rand() % (int)position.Z;
	}
}

void Cloud::setPosition(NYVert3Df pos)
{
	position = pos;
}

NYVert3Df Cloud::getPosition()
{
	return position;
}

void Cloud::drawCube(float height, int i, int j)
{
	float _width = cloud_size / 2;
	float _height = height / 2;

	// TO DO (not get the faces)

	glBegin(GL_QUADS);

	if (!isFull(i, j - 1))
	{
		// Front side
		glVertex3f(-_width	, -_width	, -_height);
		glVertex3f(_width	, -_width	, -_height);
		glVertex3f(_width	, -_width	, _height);
		glVertex3f(-_width	, -_width	, _height);
	}

	if (!isFull(i, j + 1))
	{
		// Back Side
		glVertex3f(-_width	, _width	, -_height);
		glVertex3f(-_width	, _width	, _height);
		glVertex3f(_width	, _width	, _height);
		glVertex3f(_width	, _width	, -_height);
	}


	// Bottom Side
	glVertex3f(-_width	, -_width	, -_height);
	glVertex3f(-_width	, _width	, -_height);
	glVertex3f(_width	, _width	, -_height);
	glVertex3f(_width	, -_width	, -_height);

	// Top Side
	glVertex3f(-_width	, -_width	, _height);
	glVertex3f(_width	, -_width	, _height);
	glVertex3f(_width	, _width	, _height);
	glVertex3f(-_width	, _width	, _height);

	if (!isFull(i - 1, j))
	{
		// Left side
		glVertex3f(-_width	, -_width	, -_height);
		glVertex3f(-_width	, -_width	, _height);
		glVertex3f(-_width	, _width	, _height);
		glVertex3f(-_width	, _width	, -_height);
	}

	if (!isFull(i + 1, j))
	{
		// Right side
		glVertex3f(_width	, -_width	, -_height);
		glVertex3f(_width	, _width	, -_height);
		glVertex3f(_width	, _width	, _height);
		glVertex3f(_width	, -_width	, _height);
	}

	glEnd();
}

void Cloud::drawDrop(NYVert3Df pos)
{
	glPushMatrix();

	glTranslatef(pos.X, pos.Y, pos.Z);
	glColor3f(0, 0, 0.9);

	glBegin(GL_QUADS);

	glVertex3f(-drop_width, 0, -drop_height);
	glVertex3f(drop_width, 0, -drop_height);
	glVertex3f(drop_width, 0, drop_height);
	glVertex3f(-drop_width, 0, drop_height);

	glVertex3f(-drop_width, 0, -drop_height);
	glVertex3f(-drop_width, 0, drop_height);
	glVertex3f(drop_width, 0, drop_height);
	glVertex3f(drop_width, 0, -drop_height);

	glEnd();

	glPopMatrix();
}

bool Cloud::isFull(int x, int y)
{
	if (x >= 3 || y >= 3 || x < 0 || y < 0)
		return false;

	return shape[x][y];
}

void Cloud::draw(NYVert3Df pos)
{
	glPushMatrix();
	glTranslatef(position.X, position.Y, position.Z);

	NYColor color = getColor();

	glColor3f(color.R, color.V, color.B);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			
			if (shape[i][j])
			{
				glPushMatrix();
				glTranslatef(i * cloud_size, j * cloud_size, 0);
				drawCube(10, i, j);
				glPopMatrix();
			}
		}
	}
	glPopMatrix();


	glPushMatrix();
	glTranslatef(position.X, position.Y, 0);
	float dist = sqrt(((position.X - pos.X) * (position.X - pos.X)) + ((position.Y - pos.Y) * (position.Y - pos.Y)));

	if (type == ECloudType::Stormy && dist <= max_draw_dist)
	{
		for (int i = 0; i < maxDrop; i++)
		{
			drawDrop(drop[i]);
		}
	}
	glPopMatrix();

}

void Cloud::update(float elapsed_time)
{
	position += wind * elapsed_time;

	if (type == ECloudType::Stormy)
	{
		for (int i = 0; i < maxDrop; i++)
		{
			drop[i].Z -= GRAVITY_DROP * elapsed_time;

			if (world->getCube(drop[i].X * NYCube::CUBE_SIZE, drop[i].Y * NYCube::CUBE_SIZE, drop[i].Z * NYCube::CUBE_SIZE)->_Type != CUBE_AIR)
			{
				// Destroy de la goutte
				drop[i] = newDropPos();
			}
		}
		
	}
}

NYVert3Df Cloud::newDropPos()
{
	float x = rand() % (int)cloud_size * 3;
	float y = rand() % (int)cloud_size * 3;

	return NYVert3Df(x, y, position.Z);
}

void Cloud::randShape()
{

	int i = 0;
	// Version 1
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			if (rand()%100 > 50)
			{
				shape[i][j] = true;
				i++;
			}
			else
				shape[i][j] = false;
		}

	/*if (i == 0)
		shape[0][0] = true;*/

}


NYColor Cloud::getColor()
{
	switch (type)
	{
	case ECloudType::White: //return NYColor(0.9, 0.9, 0.9, 1); break;
		return NYColor(0.9, 0.9, 0.9, 1); break;
	case ECloudType::Grey:
	case ECloudType::Stormy:  return NYColor(0.7, 0.7, 0.7, 1); break;
	default: return NYColor(0.7, 0.7, 0.7, 1); break;
	}
}

Cloud::~Cloud()
{
}
