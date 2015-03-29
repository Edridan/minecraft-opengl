#pragma once

#include "engine/render/renderer.h"
#include "cube.h"

#include "GLCube.h"

/**
  * On utilise des chunks pour que si on modifie juste un cube, on ait pas
  * besoin de recharger toute la carte dans le buffer, mais juste le chunk en question
  */
class NYChunk
{
	public :

		static const int CHUNK_SIZE = 16; ///< Taille d'un chunk en nombre de cubes (n*n*n)
		NYCube _Cubes[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE]; ///< Cubes contenus dans le chunk

		GLuint _BufWorld; ///< Identifiant du VBO pour le monde
		
		static float _WorldVert[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE*3*4*6]; ///< Buffer pour les sommets
		static float _WorldCols[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE*3*4*6]; ///< Buffer pour les couleurs
		static float _WorldNorm[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE*3*4*6]; ///< Buffer pour les normales

		static const int SIZE_VERTICE = 3 * sizeof(float); ///< Taille en octets d'un vertex dans le VBO
		static const int SIZE_COLOR = 3 * sizeof(float);  ///< Taille d'une couleur dans le VBO
		static const int SIZE_NORMAL = 3 * sizeof(float);  ///< Taille d'une normale dans le VBO
		
		int _NbVertices; ///< Nombre de vertices dans le VBO (on ne met que les faces visibles)

		NYChunk * Voisins[6];
		
		NYChunk()
		{
			_NbVertices = 0;
			_BufWorld = 0;
			memset(Voisins,0x00,sizeof(void*) * 6);
		}

		void setVoisins(NYChunk * xprev, NYChunk * xnext,NYChunk * yprev,NYChunk * ynext,NYChunk * zprev,NYChunk * znext)
		{
			Voisins[0] = xprev;
			Voisins[1] = xnext;
			Voisins[2] = yprev;
			Voisins[3] = ynext;
			Voisins[4] = zprev;
			Voisins[5] = znext;
		}

		/**
		  * Raz de l'état des cubes (a draw = false)
		  */
		void reset(void)
		{
			for(int x=0;x<CHUNK_SIZE;x++)
				for(int y=0;y<CHUNK_SIZE;y++)
					for(int z=0;z<CHUNK_SIZE;z++)
					{
						_Cubes[x][y][z]._Draw = true;
						_Cubes[x][y][z]._Type = CUBE_AIR;
					}
		}

		/**
		  *	Ajout un vertrex dans le buffer _WorldVert a l'indexe vert_index (met a jour l'index) /!\ SCALED sur la taille des cubes
		  */
		void addVertexToBuffer(float x, float y, float z, int & vert_index)
		{
			// Add the vertex to the buffer
			_WorldVert[vert_index++] = x;
			_WorldVert[vert_index++] = y;
			_WorldVert[vert_index++] = z;
		}

		/**
		  *	Ajout d'une couleur dans le buffer _WorldCols a l'indexe col_index
		  */
		void addColorToBuffer(float r, float g, float b, int & col_index)
		{
			// Add the color to the buffer
			_WorldCols[col_index++] = r;
			_WorldCols[col_index++] = g;
			_WorldCols[col_index++] = b;
		}

		/**
		  *	Ajout d'une normale dans le buffer _WorldNorm a l'indexe norm_index
		  */
		void addNormalToBuffer(float x, float y, float z, int & norm_index)
		{
			// Add the vertex to the buffer
			_WorldNorm[norm_index++] = x;
			_WorldNorm[norm_index++] = y;
			_WorldNorm[norm_index++] = z;
		}

		/**
		  *	Ajout d'une couleur dans le buffer en fonction du type de cube passé en paramètre
		  */
		void addColorToBuffer(NYCubeType type, int & col_index, float col_change = 0)
		{
			switch (type)
			{
			case CUBE_EAU:
				addColorToBuffer(0.f + col_change / 2000.0f, (162.f / 255.f) + col_change / 2000.0f, (232.f / 255.f), col_index);
				break;
			case CUBE_HERBE:
				addColorToBuffer((34.f / 255.f) + col_change / 2000.0f, (177.f / 255.f) + col_change / 2000.0f, (76.f / 255.f) + col_change / 2000.0f, col_index);
				break;
			case CUBE_TERRE:
				addColorToBuffer(0.5 + col_change / 2000.0f, 0.5 + col_change / 2000.0f, 0.25 + col_change / 2000.0f, col_index);
				break;
			case CUBE_NEIGE:
				addColorToBuffer(0.95 + col_change / 2000.0f, 0.95 + col_change / 2000.0f, 0.95 + col_change / 2000.0f, col_index);
				break;
			case CUBE_PIERRE:
				addColorToBuffer(0.6 + col_change / 2000.0f, 0.6 + col_change / 2000.0f, 0.6 + col_change / 2000.0f, col_index);
				break;
			case CUBE_BOIS:
				addColorToBuffer((72.f / 255.f) + col_change / 2000.0f, (45.f / 255.f) + col_change / 2000.0f, (30.f / 255.f) + col_change / 2000.0f, col_index);
				break;
			case CUBE_FEUILLE:
				addColorToBuffer((198.f / 255.f) + col_change / 2000.0f, (236.f / 255.f) + col_change / 2000.0f, (81.f / 255.f) + col_change / 2000.0f, col_index);
				break;
			}
		}

		/**
		  * Retourne le type du cube voulut (Retourne CUBE_AIR si en dehors)
		  */
		NYCubeType getCubeType(int x, int y, int z)
		{
			if (x < 0)
			{
				if (Voisins[0] != NULL)
					return Voisins[0]->getCubeType(CHUNK_SIZE + x, y ,z);
				else
					return CUBE_AIR;
			}

			if (x >= CHUNK_SIZE)
			{
				if (Voisins[1] != NULL)
					return Voisins[1]->getCubeType(CHUNK_SIZE - x, y , z);
				else
					return CUBE_AIR;
			}

			if (y < 0)
			{
				if (Voisins[2] != NULL)
					return Voisins[2]->getCubeType(x, CHUNK_SIZE + y, z);
				else
					return CUBE_AIR;
			}

			if (y >= CHUNK_SIZE)
			{
				if (Voisins[3] != NULL)
					return Voisins[3]->getCubeType(x, CHUNK_SIZE - y, z);
				else
					return CUBE_AIR;
			}

			if (z < 0)
			{
				if (Voisins[4] != NULL)
					return Voisins[4]->getCubeType(x, y, CHUNK_SIZE + z);
				else
					return CUBE_AIR;
			}

			if (z >= CHUNK_SIZE)
			{
				if (Voisins[5] != NULL)
					return Voisins[5]->getCubeType(x, y, CHUNK_SIZE - z);
				else
					return CUBE_AIR;
			}

			return _Cubes[x][y][z]._Type;
		}

		/**
		  *	Ajout d'un cube en fonction de son type dans les 3 buffers
		  */
		void addCubeToBuffer(int x, int y, int z, NYCubeType type, int & vert_index, int & col_index, int & norm_index)
		{
			//Position du cube (coin bas gauche face avant)
			float xPos = x*(float)NYCube::CUBE_SIZE;
			float yPos = y*(float)NYCube::CUBE_SIZE;
			float zPos = z*(float)NYCube::CUBE_SIZE;

			float random = (rand() % 100);

			// FIRST QUAD (DOWN)
			if (getCubeType(x, y, z - 1) == CUBE_AIR)
			{
				// Vertex 1
				addVertexToBuffer(xPos, yPos, zPos, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(0, 0, -1, norm_index);
				_NbVertices++;

				// Vertex 2
				addVertexToBuffer(xPos, yPos + NYCube::CUBE_SIZE, zPos, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(0, 0, -1, norm_index);
				_NbVertices++;

				// Vertex 3
				addVertexToBuffer(xPos + NYCube::CUBE_SIZE, yPos + NYCube::CUBE_SIZE, zPos, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(0, 0, -1, norm_index);
				_NbVertices++;

				// Vertex 4
				addVertexToBuffer(xPos + NYCube::CUBE_SIZE, yPos, zPos, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(0, 0, -1, norm_index);
				_NbVertices++;
			}

			// SECOND QUAD (RIGHT)
			if (getCubeType(x + 1, y, z) == CUBE_AIR || (type != CUBE_EAU && getCubeType(x + 1, y, z) == CUBE_EAU))
			{
				// Vertex 1
				addVertexToBuffer(xPos + NYCube::CUBE_SIZE, yPos, zPos, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(1, 0, 0, norm_index);
				_NbVertices++;

				// Vertex 2
				addVertexToBuffer(xPos + NYCube::CUBE_SIZE, yPos + NYCube::CUBE_SIZE, zPos, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(1, 0, 0, norm_index);
				_NbVertices++;

				// Vertex 3
				addVertexToBuffer(xPos + NYCube::CUBE_SIZE, yPos + NYCube::CUBE_SIZE, zPos + NYCube::CUBE_SIZE, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(1, 0, 0, norm_index);
				_NbVertices++;

				// Vertex 4
				addVertexToBuffer(xPos + NYCube::CUBE_SIZE, yPos, zPos + NYCube::CUBE_SIZE, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(1, 0, 0, norm_index);
				_NbVertices++;
			}

			// THIRD QUAD (LEFT)

			if (getCubeType(x - 1, y, z) == CUBE_AIR || (type != CUBE_EAU && getCubeType(x - 1, y, z) == CUBE_EAU))
			{
				// Vertex 1
				addVertexToBuffer(xPos, yPos, zPos, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(-1, 0, 0, norm_index);
				_NbVertices++;

				// Vertex 2
				addVertexToBuffer(xPos, yPos, zPos + NYCube::CUBE_SIZE, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(-1, 0, 0, norm_index);
				_NbVertices++;

				// Vertex 3
				addVertexToBuffer(xPos, yPos + NYCube::CUBE_SIZE, zPos + NYCube::CUBE_SIZE, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(-1, 0, 0, norm_index);
				_NbVertices++;

				// Vertex 4
				addVertexToBuffer(xPos, yPos + NYCube::CUBE_SIZE, zPos, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(-1, 0, 0, norm_index);
				_NbVertices++;
			}

			// FOURTH QUAD (TOP)
			if (getCubeType(x, y + 1, z) == CUBE_AIR || (type != CUBE_EAU && getCubeType(x, y + 1, z) == CUBE_EAU))
			{
				// Vertex 1
				addVertexToBuffer(xPos, yPos + NYCube::CUBE_SIZE, zPos, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(0, 1, 0, norm_index);
				_NbVertices++;

				// Vertex 2
				addVertexToBuffer(xPos, yPos + NYCube::CUBE_SIZE, zPos + NYCube::CUBE_SIZE, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(0, 1, 0, norm_index);
				_NbVertices++;

				// Vertex 3
				addVertexToBuffer(xPos + NYCube::CUBE_SIZE, yPos + NYCube::CUBE_SIZE, zPos + NYCube::CUBE_SIZE, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(0, 1, 0, norm_index);
				_NbVertices++;

				// Vertex 4
				addVertexToBuffer(xPos + NYCube::CUBE_SIZE, yPos + NYCube::CUBE_SIZE, zPos, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(0, 1, 0, norm_index);
				_NbVertices++;
			}

			// FIFTH QUAD (BOTTOM)
			if (getCubeType(x, y - 1, z) == CUBE_AIR || (type != CUBE_EAU && getCubeType(x, y - 1, z) == CUBE_EAU))
			{
				// Vertex 1
				addVertexToBuffer(xPos, yPos, zPos, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(0, -1, 0, norm_index);
				_NbVertices++;

				// Vertex 2
				addVertexToBuffer(xPos + NYCube::CUBE_SIZE, yPos, zPos, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(0, -1, 0, norm_index);
				_NbVertices++;

				// Vertex 3
				addVertexToBuffer(xPos + NYCube::CUBE_SIZE, yPos, zPos + NYCube::CUBE_SIZE, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(0, -1, 0, norm_index);
				_NbVertices++;

				// Vertex 4
				addVertexToBuffer(xPos, yPos, zPos + NYCube::CUBE_SIZE, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(0, -1, 0, norm_index);
				_NbVertices++;
			}

			// SIXTH QUAD (BACKWARD)
			if (getCubeType(x, y, z + 1) == CUBE_AIR)
			{
				// Vertex 1
				addVertexToBuffer(xPos, yPos, zPos + NYCube::CUBE_SIZE, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(0, 0, 1, norm_index);
				_NbVertices++;

				// Vertex 2
				addVertexToBuffer(xPos + NYCube::CUBE_SIZE, yPos, zPos + NYCube::CUBE_SIZE, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(0, 0, 1, norm_index);
				_NbVertices++;

				// Vertex 3
				addVertexToBuffer(xPos + NYCube::CUBE_SIZE, yPos + NYCube::CUBE_SIZE, zPos + NYCube::CUBE_SIZE, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(0, 0, 1, norm_index);
				_NbVertices++;

				// Vertex 4
				addVertexToBuffer(xPos, yPos + NYCube::CUBE_SIZE, zPos + NYCube::CUBE_SIZE, vert_index);
				addColorToBuffer(type, col_index, random);
				addNormalToBuffer(0, 0, 1, norm_index);
				_NbVertices++;
			}
		}

		/**
		  *	Met a jour les buffer _WorldVert, _WorldCols, _WorldNorm
		  */
		void updateBuffer(void)
		{
			_NbVertices = 0;

			int vert_index = 0;	// Index pour le tableau des vertex
			int col_index = 0;
			int norm_index = 0;

			for (int x = 0; x<CHUNK_SIZE; x++)
				for (int y = 0; y<CHUNK_SIZE; y++)
					for (int z = 0; z<CHUNK_SIZE; z++)
					{
						// Le cube n'est pas un cube d'air et doit être affiché
						if (_Cubes[x][y][z]._Type != CUBE_AIR && _Cubes[x][y][z]._Draw)
							addCubeToBuffer(x, y, z, _Cubes[x][y][z]._Type, vert_index, col_index, norm_index);

					}

			_NbVertices = vert_index / 3;
		}

		//On met le chunk ddans son VBO
		void toVbo(void)
		{
			// Mise a jour des buffers avant de les mettre sur la carte vidéo
			updateBuffer();

			GLenum error;	// Error for opengl
			GLuint g_sizeVertice	=	sizeof(float) * 3;	// Size for vertices
			GLuint g_sizeColor		=	sizeof(float) * 3;	// Size for color

			if (_BufWorld != 0)
				glDeleteBuffers(1, &_BufWorld);

			glGenBuffers(1, &_BufWorld);

			glBindBuffer(GL_ARRAY_BUFFER, _BufWorld);

			glBufferData(GL_ARRAY_BUFFER,
				_NbVertices * SIZE_VERTICE +
				_NbVertices * SIZE_COLOR +
				_NbVertices * SIZE_NORMAL,
				NULL,
				GL_STREAM_DRAW);

			error = glGetError();

			glBufferSubData(GL_ARRAY_BUFFER,
				0,
				_NbVertices * SIZE_VERTICE,
				_WorldVert);

			error = glGetError();

			glBufferSubData(GL_ARRAY_BUFFER,
				_NbVertices * SIZE_VERTICE,
				_NbVertices * SIZE_COLOR,
				_WorldCols);

			error = glGetError();

			glBufferSubData(GL_ARRAY_BUFFER,
				_NbVertices * SIZE_VERTICE +
				_NbVertices * SIZE_COLOR,
				_NbVertices * SIZE_NORMAL,
				_WorldNorm);

			error = glGetError();

			glBindBuffer(GL_ARRAY_BUFFER, 0);

		}

		void render(void)
		{
			// Enable light and color material
			glEnable(GL_COLOR_MATERIAL);
			glEnable(GL_LIGHTING);

			//On bind le buuffer
			glBindBuffer(GL_ARRAY_BUFFER, _BufWorld /* World buffer vertrex */);
			NYRenderer::checkGlError("glBindBuffer");

			//On active les datas que contiennent le VBO
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);

			//On place les pointeurs sur les datas, aux bons offsets
			glVertexPointer(3, GL_FLOAT, 0, (void*)(0));
			glColorPointer(3, GL_FLOAT, 0, (void*)(_NbVertices*SIZE_VERTICE));
			glNormalPointer(GL_FLOAT, 0, (void*)(_NbVertices*SIZE_VERTICE + _NbVertices*SIZE_COLOR));

			//On demande le dessin
			glDrawArrays(GL_QUADS, 0, _NbVertices);

			//On cleane
			glDisableClientState(GL_COLOR_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
		}

		/**
		  * On verifie si le cube peut être vu
		  */
		bool test_hidden(int x, int y, int z)
		{
			NYCube * cubeXPrev = NULL; 
			NYCube * cubeXNext = NULL; 
			NYCube * cubeYPrev = NULL; 
			NYCube * cubeYNext = NULL; 
			NYCube * cubeZPrev = NULL; 
			NYCube * cubeZNext = NULL; 

			if(x == 0 && Voisins[0] != NULL)
				cubeXPrev = &(Voisins[0]->_Cubes[CHUNK_SIZE-1][y][z]);
			else if(x > 0)
				cubeXPrev = &(_Cubes[x-1][y][z]);

			if(x == CHUNK_SIZE-1 && Voisins[1] != NULL)
				cubeXNext = &(Voisins[1]->_Cubes[0][y][z]);
			else if(x < CHUNK_SIZE-1)
				cubeXNext = &(_Cubes[x+1][y][z]);

			if(y == 0 && Voisins[2] != NULL)
				cubeYPrev = &(Voisins[2]->_Cubes[x][CHUNK_SIZE-1][z]);
			else if(y > 0)
				cubeYPrev = &(_Cubes[x][y-1][z]);

			if(y == CHUNK_SIZE-1 && Voisins[3] != NULL)
				cubeYNext = &(Voisins[3]->_Cubes[x][0][z]);
			else if(y < CHUNK_SIZE-1)
				cubeYNext = &(_Cubes[x][y+1][z]);

			if(z == 0 && Voisins[4] != NULL)
				cubeZPrev = &(Voisins[4]->_Cubes[x][y][CHUNK_SIZE-1]);
			else if(z > 0)
				cubeZPrev = &(_Cubes[x][y][z-1]);

			if(z == CHUNK_SIZE-1 && Voisins[5] != NULL)
				cubeZNext = &(Voisins[5]->_Cubes[x][y][0]);
			else if(z < CHUNK_SIZE-1)
				cubeZNext = &(_Cubes[x][y][z+1]);

			if( cubeXPrev == NULL || cubeXNext == NULL ||
				cubeYPrev == NULL || cubeYNext == NULL ||
				cubeZPrev == NULL || cubeZNext == NULL )
				return false;

			if( cubeXPrev->isSolid() == true && //droite
				cubeXNext->isSolid() == true && //gauche
				cubeYPrev->isSolid() == true && //haut
				cubeYNext->isSolid() == true && //bas
				cubeZPrev->isSolid() == true && //devant
				cubeZNext->isSolid() == true )  //derriere
				return true;
			return false;
		}

		void disableHiddenCubes(void)
		{
			for(int x=0;x<CHUNK_SIZE;x++)
				for(int y=0;y<CHUNK_SIZE;y++)
					for(int z=0;z<CHUNK_SIZE;z++)
					{
						_Cubes[x][y][z]._Draw = true;
						if(test_hidden(x,y,z))
							_Cubes[x][y][z]._Draw = false;
					}
		}


};