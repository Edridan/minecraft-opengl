#ifndef _CAMERA_CONTROLLER_H_
#define _CAMERA_CONTROLLER_H_

// Camera controller for position
#include "engine\render\renderer.h"

class FreeFlyCam
{
public:
	FreeFlyCam(NYCamera * camera, bool enabled = true);

	void inputDown(char key);
	void inputUp(char key);

	void update(float elapsed_time);
	void disable(bool disabled);	// Disable/enable the camera controller

	bool isEnable();	// Return if the controller is enable
	bool isController();	// Return if the controller control the player
	float speed;	// Camera speed

	~FreeFlyCam();

private:
	bool forward_step, backward_step;	// Step forward/backward
	bool left_step, right_step;	// Step right/left
	bool up, down;				// Down or Up
	bool enable, controller;	// Enable

	NYCamera * camera;	// Controlled camera
};

#endif _CAMERA_CONTROLLER_H_