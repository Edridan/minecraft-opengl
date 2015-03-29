#include "DayManager.h"


DayManager::DayManager(NYRenderer * _renderer)
	:tweak_time(1), sun_dist(5000), sun_scale(200), moon_scale(150), day_time(240), is_centered(true), renderer_engine(_renderer)
{
	current_day_time = 0;

	morning_color = NYColor(153.f / 255.f, 217.f / 255.f, 234.f / 255.f, 1);
	mid_day_color = NYColor(0, 162.f / 255.f, 232.f / 255.f, 1);
	afternoon_color = NYColor(1, 127.f / 255.f, 39.f / 255.f, 1);
	midnight_color = NYColor(21.f / 255.f, 26.f / 255.f, 81.f / 255.f, 1);

	sun_color = NYColor(1.f, 0.7f, 0, 1.f);

	sky_color = morning_color;
}

void DayManager::update(float elapsed_time)
{
	// Update the time
	current_day_time += elapsed_time * tweak_time;

	if (current_day_time >= day_time)
		current_day_time = 0;

	// Update the color
	switch (getDayState())
	{
	case MORNING:	sky_color = morning_color.interpolate(mid_day_color, (current_day_time / (day_time / 4.f)));
					break;
	case AFTERNOON: sky_color = mid_day_color.interpolate(afternoon_color, ((current_day_time - (day_time / 4.f)) / (day_time / 4.f)));
					break;
	case NIGHT:		sky_color = afternoon_color.interpolate(midnight_color, ((current_day_time - (day_time / 2.f)) / (day_time / 4.f))); 
					break;
	case END_NIGHT: sky_color = midnight_color.interpolate(morning_color, ((current_day_time - (3.f * day_time / 4.f)) / (day_time / 4.f))); 
					break;
	}

	renderer_engine->setBackgroundColor(sky_color);
}

void DayManager::draw()
{
	drawSun(true);	// Draw the sun plus set the lights
}

void DayManager::drawSun(bool setLight)
{
	glPushMatrix();	// Have a new matrix

	if (is_centered)
		glTranslatef(renderer_engine->_Camera->_Position.X, renderer_engine->_Camera->_Position.Y, renderer_engine->_Camera->_Position.Z);

	glRotated(-(current_day_time * (360.f/day_time)), 0 , 1, 0);

	glPushMatrix();

	glTranslatef(sun_dist, 0, 0);
	glColor3f(1.f, 0.7f, 0);

	// SUN AND MOON DRAW
	glutSolidCube(sun_scale);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-sun_dist, 0, 0);

	glColor3f(1.f, 1.f, 1.f);
	glutSolidCube(moon_scale);
	glPopMatrix();

	if (setLight)
	{
		glEnable(GL_LIGHTING);
		setLights();
	}
	glPopMatrix();	// Delete the matrice used
}

void DayManager::setLights()
{
	//On active la light 0
	glEnable(GL_LIGHT0);

	// Calcul de la direction

	//On définit une lumière directionelle (un soleil)
	float direction[4] = { 1, 0, 0, 0 }; ///w = 0 donc elle est a l'infini
	
	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	float color[4] = { 0.5f, 0.5f, 0.5f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, color);
	float color2[4] = { 0.3f, 0.3f, 0.3f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, color2);
	float color3[4] = { 0.3f, 0.3f, 0.3f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, color3);
}

void DayManager::setCurrentDayTime(float _time)
{
	if (_time == 0.f)
		_time = 0.1f;

	current_day_time = (_time * day_time);
}

void DayManager::input(char key)
{
	switch (key)
	{
	case '+': setTweakTime(tweak_time + 1); break;
	case '-': setTweakTime(tweak_time - 1); break;
	}
}

int DayManager::getDayState()
{
	if (current_day_time < day_time / 4)
		return MORNING;
	else if (current_day_time < day_time / 2)
		return AFTERNOON;
	else if (current_day_time < (3 * day_time) / 4)
		return NIGHT;

	return END_NIGHT;
}

void DayManager::setSunCenteredOnPlayer(bool centered)
{
	is_centered = centered;
}

float DayManager::getDayTime()
{
	return current_day_time / day_time;
}

void DayManager::setSunDist(float _dist)
{
	sun_dist = _dist;
}

void DayManager::setTweakTime(float _tweak)
{
	if (_tweak < 0.f)
		_tweak = 0.f;


	tweak_time = _tweak;
}

float DayManager::getTweakTime()
{
	return tweak_time;
}


DayManager::~DayManager()
{
}
