#ifndef __AVATAR__
#define __AVATAR__

#include "engine/utils/types_3d.h"
#include "engine/render/camera.h"
#include "world.h"
#include "my_physics.h"

class NYAvatar
{
public:
	NYVert3Df Position;
	NYVert3Df Speed;
	
	NYVert3Df Velocity;

	NYVert3Df MoveDir;

	NYVert3Df Look;

	bool Move;
	bool Jump;
	float Height;
	float Width;
	bool avance;
	bool recule;
	bool gauche;
	bool droite;
	bool pick;
	bool alreadyPick;
	bool Standing;

	bool PickRender;
	
	NYCamera * Cam;
	NYWorld * World;

	NYAvatar(NYCamera * cam, NYWorld * world)
	{
		Speed = NYVert3Df(50, 50, 0);
	
		Position = NYVert3Df(((MAT_SIZE * NYChunk::CHUNK_SIZE * NYCube::CUBE_SIZE) / 2), ((MAT_SIZE * NYChunk::CHUNK_SIZE * NYCube::CUBE_SIZE) / 2), (MAT_HEIGHT * NYChunk::CHUNK_SIZE * NYCube::CUBE_SIZE));
		
		Height = 10;
		Width = 3;

		Cam = cam;

		Look = Cam->_Direction;

		avance = false;
		recule = false;
		gauche = false;
		droite = false;

		alreadyPick = false;
		pick = false;

		Standing = false;
		Jump = false;

		PickRender = false;

		World = world;
	}

	/**
	  *
	  */
	void drawPicking()
	{
		glDisable(GL_LIGHTING);

		glBegin(GL_LINES);
		// X Axes
		glColor3d(1, 0, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(Look.X * 100, Look.Y * 100, Look.Z * 100);
		glEnd();
		glEnable(GL_LIGHTING);
	}

	/**
	  *	Affichage du personnage
	  */
	void render(void)
	{
		glTranslatef(Position.X, Position.Y, Position.Z);

		if (PickRender)
			drawPicking();

		glutSolidCube(Width / 2);
	}

	/**
	  *	Management des input du personnage (Touche qui s'enfonce)
	  */
	void inputDown(char key)
	{
		switch (key)
		{
		case 'z': avance = true; break;
		case 's': recule = true; break;
		case 'q': gauche = true; break;
		case 'd': droite = true; break;
		case ' ': Jump = true; break;
		}
	}

	/**
	*	Management des input du personnage (Touche relachée)
	*/
	void inputUp(char key)
	{
		switch (key)
		{
		case 'z': avance = false; break;
		case 's': recule = false; break;
		case 'q': gauche = false; break;
		case 'd': droite = false; break;
		case ' ': Jump = false; break;
		}
	}

	void updateLook()
	{
		Look = Cam->_Direction;
	}

	/**
	* MAJ du vecteur de velocité en fonction des collisions
	*/
	void collisionManager(float elapsed)
	{
		NYCollision collide;

		NYCollision collision = World->collide_with_world(Position + (Velocity * elapsed), Width, Height, collide);

		if (collision & NY_COLLIDE_BOTTOM && Velocity.Z < 0)
			Velocity.Z = 0;
		if (collision & NY_COLLIDE_UP && Velocity.Z > 0)
			Velocity.Z = 0;

		if (collision & NY_COLLIDE_FRONT && Velocity.X > 0)
			Velocity.X = 0;
		if (collision & NY_COLLIDE_BACK && Velocity.X < 0)
			Velocity.X = 0;

		if (collision & NY_COLLIDE_LEFT && Velocity.Y > 0)
			Velocity.Y = 0;
		if (collision & NY_COLLIDE_RIGHT && Velocity.Y < 0)
			Velocity.Y = 0;
	}

	/**
	*	Physique de l'avatar (il tombe constamment)
	*/
	void physic(float elapsed)
	{
		Velocity.Z -= 100.f * elapsed;
	}

	/**
	  *	Creuse un cube dans le monde a l'endroit visé
	  */

	void pickCube()
	{
		NYVert3Df intersection;
		
		int xCube, yCube, zCube;

		/*if (World->getRayCollision(Position, (Position + Look) * 10, intersection, xCube, yCube, zCube))
		{
			printf("Delete cube at : %i, %i, %i\n", xCube, yCube, zCube);

			World->deleteCube(xCube, yCube, zCube);
		}*/
	}

	/**
	*	Update de l'avatar : velocité et MAJ position
	*/
	void update(float elapsed)
	{
		updateLook();

		MoveDir = NYVert3Df(0, 0, 0);

		if (pick && !alreadyPick)
		{
			pickCube();
			alreadyPick = true;
		}

		if (!pick)
			alreadyPick = false;

		// Update du personnage
		if (avance)
			MoveDir = Cam->_Direction;
		else if (recule)
			MoveDir = NYVert3Df(-Cam->_Direction.X, -Cam->_Direction.Y, -Cam->_Direction.Z);


		if (droite)
			MoveDir += Cam->_NormVec;
		else if (gauche)
			MoveDir += NYVert3Df(-Cam->_NormVec.X, -Cam->_NormVec.Y, -Cam->_NormVec.Z);

		if (Jump)
		{
			if (Velocity.Z == 0)
				Velocity.Z = +50.f;
		}

		Velocity = NYVert3Df(MoveDir.X * Speed.X, MoveDir.Y * Speed.Y, Velocity.Z);
		
		physic(elapsed);
		collisionManager(elapsed);

		Position += Velocity * elapsed;
	}

};

#endif