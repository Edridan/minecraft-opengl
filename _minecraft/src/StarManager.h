#ifndef _STAR_MANAGER_H_
#define _STAR_MANAGER_H_

#include "DayManager.h"

struct star
{
	NYVert3Df position;
	float size;
};

class StarManager
{
public:
	StarManager(DayManager * _day, int star_num, NYRenderer * _renderer = NULL);

	void createSky(float r_min, float r_max, float c_x, float c_y, float c_z);

	void draw();

	~StarManager();

private:
	DayManager * day;
	NYRenderer * renderer;

	NYColor starColor;

	float poppin_star;

	int starNumber;
	float starSize;
	star * stars;
};

#endif _STAR_MANAGER_H_