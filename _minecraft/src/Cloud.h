#ifndef _CLOUD_H_
#define _CLOUD_H_

// Include 
#include "engine\utils\types_3d.h"
#include "world.h"

#define GRAVITY_DROP 80.f	// Gravity for the drop

// Enum
enum ECloudType	// Type of cloud
{
	White,
	Grey,	
	Stormy,
};

class Cloud
{
public:
	Cloud(NYVert3Df pos, NYVert3Df wind_dir, ECloudType type, int max_drop, NYWorld * _world);

	void setType(ECloudType type);
	void setPosition(NYVert3Df pos);
	void draw(NYVert3Df cam_pos);
	void update(float elapsed_time);

	NYVert3Df getPosition();

	~Cloud();

private:
	NYColor		getColor();	// Return the color 
	void		randShape();	// Randomize de shape of the cloud
	void		drawDrop(NYVert3Df pos = NYVert3Df(0,0,0));	// Draw a drop to the pos
	NYVert3Df	newDropPos();	// Make a new position for the drop
	bool		isFull(int x, int y);	// Return true if the cloud is full in this part
	void		drawCube(float height, int i, int j);

	ECloudType	type;	// Type of the cloud

	NYVert3Df * drop;	// Drop of the cloud (only for the Stormy clouds)
	int maxDrop;		// Size of the tab

	NYWorld * world;

	NYVert3Df position;	// Current positions
	NYVert3Df wind;	// Wind direction

	static float cloud_size;
	static float drop_height;
	static float drop_width;
	
	static float max_draw_dist; // Distance d'affichage pour les gouttes

	// Forme du nuage
	bool shape[3][3];	
};

#endif _CLOUD_H_