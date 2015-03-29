/*
 *	Sun manager, draw and update light for the sun and the background
 *	
 *
 *	16/02/2015
 */

#ifndef _DAY_MANAGER_H_
#define _DAY_MANAGER_H_

#include "engine\render\renderer.h"

#define MORNING 1
#define AFTERNOON 2
#define NIGHT 3
#define END_NIGHT 4

class DayManager
{
public:
	DayManager(NYRenderer * render_engine);

	void draw();	// Draw the sun and update the lights (call drawSun() and setLights())

	// Draw functions
	void drawSun(bool setLight = false);
	void setLights();	// Set the light (already call in the draw function)

	void update(float elapsed_time);

	void setSunCenteredOnPlayer(bool centered);	// Set if the sun is centered on the player or on the map (0,0,0)
	void setCurrentDayTime(float dayTime);	// Set the day_time (0 -> 1)
	void setSunDist(float _dist);
	void setTweakTime(float _tweakTime);

	float getTweakTime();

	int getDayState();	// Return the state of the day (0 -> morning,  1 -> afternoon, 2 -> night, 3->end_night)

	float getDayTime();

	void input(char key);	// Manage the input

	~DayManager();

private:
	float day_time;	// Day time
	float current_day_time;	// Current dayTime

	float sun_dist, sun_scale, moon_scale;	// Distance of the sun from the player

	float tweak_time;	// Multiply the elapsed time by the tweak time

	bool is_centered;

	NYColor sun_color;	// Day of the sun
	NYColor sky_color;	// Current color for the sky
	NYColor morning_color, mid_day_color, afternoon_color, midnight_color;

	NYRenderer * renderer_engine;	// Get the position of the player and the background state
};

#endif _DAY_MANAGER_H_