#ifndef _WEATHER_SYSTEM_H_
#define _WEATHER_SYSTEM_H_

// Include
#include "Cloud.h"
#include "DayManager.h"

enum EWeather
{
	Clear,
	Cloudy,
	Storm,
};

class WeatherSystem
{
public:
	WeatherSystem(NYWorld * _world, DayManager * _day, NYCamera * camera, int max_cloud = 10, int max_drop = 200, int drop_per_cloud = 0);

	void update(float elapsed);
	void draw();

	~WeatherSystem();

private:
	NYWorld * world;
	DayManager * day;
	NYVert3Df wind_dir;

	bool isChanged;

	Cloud ** cloud;

	EWeather currentWeather;

	NYCamera * camera;

	int max_drop, max_cloud;
	int drop_per_cloud;
	int current_drop;	// Current number of drop
};

#endif _WEATHER_SYSTEM_H_