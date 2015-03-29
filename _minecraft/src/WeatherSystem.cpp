#include "WeatherSystem.h"


WeatherSystem::WeatherSystem(NYWorld * _world, DayManager * _day, NYCamera * cam, int _max_cloud, int _max_drop, int _drop_per_cloud)
	:world(_world), day(_day), max_cloud(_max_cloud), max_drop(_max_drop), drop_per_cloud(_drop_per_cloud), camera(cam)
{
	currentWeather = EWeather::Clear;
	wind_dir = NYVert3Df(10, 0, 0);

	if (max_drop == 0 && drop_per_cloud != 0)
		max_drop = drop_per_cloud * max_cloud;

	if (_drop_per_cloud == 0)
		_drop_per_cloud = max_drop / max_cloud;

	current_drop = 0;

	cloud = new Cloud * [max_cloud];

	isChanged = true;

	for (int i = 0; i < max_cloud; i++)
	{
		NYVert3Df pos;

		// Un chunck au dessus
		pos.Z = (MAT_HEIGHT + 1) * NYChunk::CHUNK_SIZE * NYCube::CUBE_SIZE ;

		pos.X = rand() % (MAT_SIZE * NYChunk::CHUNK_SIZE * NYCube::CUBE_SIZE);
		pos.Y = rand() % (MAT_SIZE * NYChunk::CHUNK_SIZE * NYCube::CUBE_SIZE);

		cloud[i] = new Cloud(pos, wind_dir, ECloudType::White, (max_drop / max_cloud), world);
	}
}

void WeatherSystem::draw()
{
	glDisable(GL_LIGHTING);

	glPushMatrix();
	for (int i = 0; i < max_cloud; i++)
	{
		if (cloud[i] != NULL)
		{
			cloud[i]->draw(camera->_Position);
		}
	}
	glPopMatrix();
}

void WeatherSystem::update(float elapsed)
{
	elapsed *= day->getTweakTime();

	for (int i = 0; i < max_cloud; i++)
	{
		if (cloud[i] != NULL)
		{
			// Manage the cloud
			if (cloud[i]->getPosition().X > (MAT_SIZE * NYChunk::CHUNK_SIZE * NYCube::CUBE_SIZE))
			{
				NYVert3Df pos = cloud[i]->getPosition();
				pos.X = 0;
				cloud[i]->setPosition(pos);

				switch (currentWeather)
				{
				case EWeather::Clear:
					cloud[i]->setType(ECloudType::White);
					break;
				case EWeather::Cloudy:
					cloud[i]->setType(ECloudType::Grey);
					break;
				case EWeather::Storm:
					if (current_drop + drop_per_cloud >= max_drop)
						cloud[i]->setType(ECloudType::Grey);
					else
					{
						cloud[i]->setType(ECloudType::Stormy);
						current_drop += drop_per_cloud;
					}
					break;
				default:
					break;
				}
			}
			else
				cloud[i]->update(elapsed);
		}
	}

	if (day->getDayState() == MORNING && !isChanged)
	{
		isChanged = true;

		switch (rand() % 3)
		{
		case 0: currentWeather = EWeather::Clear;
			break;
		case 1: currentWeather = EWeather::Cloudy;
			break;
		case 2: currentWeather = EWeather::Storm;
			break;
		}
	}
	if (day->getDayState() != MORNING)
		isChanged = false;
}


WeatherSystem::~WeatherSystem()
{
}
