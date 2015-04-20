//Includes application
#include <conio.h>
#include <vector>
#include <string>
#include <windows.h>
#include <time.h>       /* time_t, struct tm, difftime, time, mktime */

#include "external/gl/glew.h"
#include "external/gl/freeglut.h"

// Include
#include "FreeFlyCam.h"
#include "DayManager.h"
#include "StarManager.h"
#include "WeatherSystem.h"
#include "world.h"
#include "avatar.h"

//Moteur
#include "engine/utils/types_3d.h"
#include "engine/timer.h"
#include "engine\sound\sound.h"
#include "engine\sound\granular_sound.h"
#include "engine/log/log_console.h"
#include "engine/render/renderer.h"
#include "engine/gui/screen.h"
#include "engine/gui/screen_manager.h"


NYRenderer * g_renderer = NULL;
NYTimer * g_timer = NULL;
int g_nb_frames = 0;
float g_elapsed_fps = 0;
int g_main_window_id;
int g_mouse_btn_gui_state = 0;
bool g_fullscreen = false;

// Application variable
GLuint g_program;
FreeFlyCam * camera_controller;
DayManager * day_manager;
StarManager * star_manager;
NYWorld * g_world;
NYAvatar * avatar;
WeatherSystem * weather;
int mouse_last_position_x, mouse_last_position_y;
bool mouse_enable = false;
bool apply_shader = true;
bool draw_axes = false;

//GUI 
GUIScreenManager * g_screen_manager = NULL;
GUIBouton * BtnParams = NULL;
GUIBouton * BtnClose = NULL;
GUILabel * LabelFps = NULL;
GUILabel * LabelShader = NULL;
GUILabel * LabelCam = NULL;
GUIScreen * g_screen_params = NULL;
GUIScreen * g_screen_jeu = NULL;
GUISlider * g_slider;

//Sons
SoundEngine * g_sound_engine = NULL;
SoundBasic * g_sound_ambiance;
SoundBasic * g_sound_btn;
SoundGrain * g_sound_grain;


//////////////////////////////////////////////////////////////////////////
// GESTION APPLICATION
//////////////////////////////////////////////////////////////////////////
void update(void)
{
	float elapsed = g_timer->getElapsedSeconds(true);
	
	static float g_eval_elapsed = 0;

	//Calcul des fps
	g_elapsed_fps += elapsed;

	if (elapsed > 0.05f)
		elapsed = 0;

	g_nb_frames++;
	if(g_elapsed_fps > 1.0)
	{
		LabelFps->Text = std::string("FPS : ") + toString(g_nb_frames);
		g_elapsed_fps -= 1.0f;
		g_nb_frames = 0;
	}

	if (apply_shader)
		LabelShader->Text = std::string("Shader : ON");
	else
		LabelShader->Text = std::string("Shader : OFF");

	// Avatar
	if (!camera_controller->isEnable())
	{
		g_renderer->_Camera->moveTo(avatar->Position);
		avatar->update(elapsed);
	}
	else if (camera_controller->isController())
		avatar->update(elapsed);

	// Camera controller
	camera_controller->update(elapsed);

	// Weather
	weather->update(elapsed);

	// Background MAJ
	day_manager->update(elapsed);

	//Rendu
	g_renderer->render(elapsed);
}


void render2d(void)
{
	g_screen_manager->render();
}

//////////////////////////////////////////
// SHADER MANAGEMENT
//////////////////////////////////////////
/**
* Affiche et applique les shaders
*/
void drawShaders()
{
	glUseProgram(g_program);

	// Passage de paramètres aux shader
	GLuint elap = glGetUniformLocation(g_program, "elapsed");
	glUniform1f(elap, NYRenderer::_DeltaTimeCumul);

	GLuint amb = glGetUniformLocation(g_program, "ambientLevel");
	glUniform1f(amb, 0.4);

	GLuint invView = glGetUniformLocation(g_program, "invertView");
	glUniformMatrix4fv(invView, 1, true, g_renderer->_Camera->_InvertViewMatrix.Mat.t);

	// Camera position
	float camPosition[3] = { g_renderer->_Camera->_Position.X, g_renderer->_Camera->_Position.Y, g_renderer->_Camera->_Position.Z};

	GLuint camPos = glGetUniformLocation(g_program, "cameraPos");
	glUniformMatrix3fv(camPos, 1, true, camPosition);

	// Cam direction
	float camDirection[3] = { g_renderer->_Camera->_Direction.X, g_renderer->_Camera->_Direction.Y, g_renderer->_Camera->_Direction.Z};

	GLuint camDir = glGetUniformLocation(g_program, "cameraDir");
	glUniformMatrix3fv(camDir, 1, true, camDirection);
}

/////////////////////////////////////////
//	RENDERING
/////////////////////////////////////////
void renderObjects(void)
{
	//Rendu des axes
	glDisable(GL_LIGHTING);

	if (draw_axes)
	{
		glBegin(GL_LINES);
		// X Axes
		glColor3d(1, 0, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(10000, 0, 0);

		// Y Axes
		glColor3d(0, 1, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 10000, 0);

		// Z Axes
		glColor3d(0, 0, 1);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, 10000);

		glEnd();
	}
	
	// Draw the shaders
	if (apply_shader)
		drawShaders();

	star_manager->draw();
	day_manager->draw();
	weather->draw();

	//Render world
	glPushMatrix();
	g_world->render_world_vbo();
	glPopMatrix();

	/*if (camera_controller->isEnable())
		avatar->render();*/
}

void setLights(void)
{
	//On active la light 0
	glEnable(GL_LIGHT0);

	//On définit une lumière directionelle (un soleil)
	float direction[4] = {1,0,0,0}; ///w = 0 donc elle est a l'infini
	glLightfv(GL_LIGHT0, GL_POSITION, direction );
	float color[4] = {0.5f,0.5f,0.5f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, color );
	float color2[4] = {0.3f,0.3f,0.3f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, color2 );
	float color3[4] = {0.3f,0.3f,0.3f};
	glLightfv(GL_LIGHT0, GL_SPECULAR, color3 );
}

void resizeFunction(int width, int height)
{
	glViewport(0, 0, width, height);
	g_renderer->resize(width,height);
}

//////////////////////////////////////////////////////////////////////////
// GESTION CLAVIER SOURIS
//////////////////////////////////////////////////////////////////////////
void specialDownFunction(int key, int p1, int p2)
{
	//On change de mode de camera
	if (key == GLUT_KEY_F1)
	{
		if (glIsShader(g_program))
			glDeleteShader(g_program);	// Deleting the current used shader

		g_program = g_renderer->createProgram("shaders/psbase.glsl", "shaders/vsbase.glsl");

		Log::log(Log::ENGINE_INFO, "Shader shaders/psbase.glsl, shaders/vsbase.glsl loaded");
	}
	if (key == GLUT_KEY_F2)
	{
		apply_shader = !apply_shader;
	}
	if (key == GLUT_KEY_F3)
	{
		draw_axes = !draw_axes;
	}

	if (key == GLUT_KEY_F4)
	{
		srand(time(NULL));
		g_world->_FacteurGeneration = (float)(rand() % 100) / 50.f;
		g_world->init_world();

		g_world->add_world_to_vbo();
	}
}

void specialUpFunction(int key, int p1, int p2)
{

}

void keyboardDownFunction(unsigned char key, int p1, int p2)
{
	if(key == VK_ESCAPE)
	{
		glutDestroyWindow(g_main_window_id);	
		exit(0);
	}

	if(key == 'f')
	{
		if(!g_fullscreen){
			glutFullScreen();
			g_fullscreen = true;
		} else if(g_fullscreen){
			glutLeaveGameMode();
			glutLeaveFullScreen();
			glutReshapeWindow(g_renderer->_ScreenWidth, g_renderer->_ScreenWidth);
			glutPositionWindow(0,0);
			g_fullscreen = false;
		}
	}

	// Input manager

	if (!camera_controller->isEnable() || camera_controller->isController())
		avatar->inputDown(key);


	camera_controller->inputDown(key);
	day_manager->input(key);
}

void keyboardUpFunction(unsigned char key, int p1, int p2)
{
	avatar->inputUp(key);
	camera_controller->inputUp(key);
}

void mouseWheelFunction(int wheel, int dir, int x, int y)
{
	
}

void mouseFunction(int button, int state, int x, int y)
{
	//Gestion de la roulette de la souris
	if((button & 0x07) == 3 && state)
		mouseWheelFunction(button,1,x,y);
	if((button & 0x07) == 4 && state)
		mouseWheelFunction(button,-1,x,y);

	//GUI
	g_mouse_btn_gui_state = 0;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		g_mouse_btn_gui_state |= GUI_MLBUTTON;
		mouse_enable = true;
	}
	else
		mouse_enable = false;

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		avatar->pick = true;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		avatar->pick = false;
	}

	
	bool mouseTraite = false;
	mouseTraite = g_screen_manager->mouseCallback(x,y,g_mouse_btn_gui_state,0,0);
}

void mouseMoveFunction(int x, int y, bool pressed)
{
	if (mouse_last_position_x == 0 && mouse_last_position_y == 0)
	{
		mouse_last_position_x = x;
		mouse_last_position_y = y;
	}


	bool mouseTraite = false;

	mouseTraite = g_screen_manager->mouseCallback(x,y,g_mouse_btn_gui_state,0,0);
	if(pressed && mouseTraite)
	{
		//Mise a jour des variables liées aux sliders
	}

	if (mouse_enable)
	{
		g_renderer->_Camera->rotate((mouse_last_position_x - x) * 0.01f);
		g_renderer->_Camera->rotateUp((mouse_last_position_y - y) *0.01f);
	}

	mouse_last_position_x = x;
	mouse_last_position_y = y;
}

void mouseMoveActiveFunction(int x, int y)
{
	mouseMoveFunction(x,y,true);
}
void mouseMovePassiveFunction(int x, int y)
{
	mouseMoveFunction(x,y,false);
}


void clickBtnParams (GUIBouton * bouton)
{
	g_screen_manager->setActiveScreen(g_screen_params);
}

void clickBtnCloseParam (GUIBouton * bouton)
{
	g_screen_manager->setActiveScreen(g_screen_jeu);
}

/**
* POINT D'ENTREE PRINCIPAL
**/
int main(int argc, char* argv[])
{
	LogConsole::createInstance();

	srand(time(NULL));

	int screen_width = 800;
	int screen_height = 600;

	glutInit(&argc, argv);
	glutInitContextVersion(3, 0);
	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
		);

	glutInitWindowSize(screen_width, screen_height);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);

	glEnable(GL_MULTISAMPLE);

	Log::log(Log::ENGINE_INFO, (toString(argc) + " arguments en ligne de commande.").c_str());
	bool gameMode = true;
	for (int i = 0; i<argc; i++)
	{
		if (argv[i][0] == 'w')
		{
			Log::log(Log::ENGINE_INFO, "Arg w mode fenetre.\n");
			gameMode = false;
		}
	}

	if (gameMode)
	{
		int width = glutGet(GLUT_SCREEN_WIDTH);
		int height = glutGet(GLUT_SCREEN_HEIGHT);

		char gameModeStr[200];
		sprintf(gameModeStr, "%dx%d:32@60", width, height);
		glutGameModeString(gameModeStr);
		g_main_window_id = glutEnterGameMode();
	}
	else
	{
		g_main_window_id = glutCreateWindow("MyNecraft");
		glutReshapeWindow(screen_width, screen_height);
	}

	if (g_main_window_id < 1)
	{
		Log::log(Log::ENGINE_ERROR, "Erreur creation de la fenetre.");
		exit(EXIT_FAILURE);
	}

	GLenum glewInitResult = glewInit();

	if (glewInitResult != GLEW_OK)
	{
		Log::log(Log::ENGINE_ERROR, ("Erreur init glew " + std::string((char*)glewGetErrorString(glewInitResult))).c_str());
		_cprintf("ERROR : %s", glewGetErrorString(glewInitResult));
		exit(EXIT_FAILURE);
	}

	//Affichage des capacités du système
	Log::log(Log::ENGINE_INFO, ("OpenGL Version : " + std::string((char*)glGetString(GL_VERSION))).c_str());

	glutDisplayFunc(update);
	glutReshapeFunc(resizeFunction);
	glutKeyboardFunc(keyboardDownFunction);
	glutKeyboardUpFunc(keyboardUpFunction);
	glutSpecialFunc(specialDownFunction);
	glutSpecialUpFunc(specialUpFunction);
	glutMouseFunc(mouseFunction);
	glutMotionFunc(mouseMoveActiveFunction);
	glutPassiveMotionFunc(mouseMovePassiveFunction);
	glutIgnoreKeyRepeat(1);

	//Initialisation du renderer
	g_renderer = NYRenderer::getInstance();
	g_renderer->setRenderObjectFun(renderObjects);
	g_renderer->setRender2DFun(render2d);
	//g_renderer->setLightsFun(setLights);
	g_renderer->setBackgroundColor(NYColor());
	g_renderer->initialise();

	//On applique la config du renderer
	glViewport(0, 0, g_renderer->_ScreenWidth, g_renderer->_ScreenHeight);
	g_renderer->resize(g_renderer->_ScreenWidth, g_renderer->_ScreenHeight);

	//Ecran de jeu
	uint16 x = 10;
	uint16 y = 10;
	g_screen_jeu = new GUIScreen();

	g_screen_manager = new GUIScreenManager();

	//Bouton pour afficher les params
	BtnParams = new GUIBouton();
	BtnParams->Titre = std::string("Params");
	BtnParams->X = x;
	BtnParams->setOnClick(clickBtnParams);
	g_screen_jeu->addElement(BtnParams);

	y += BtnParams->Height + 1;

	LabelFps = new GUILabel();
	LabelFps->Text = "FPS";
	LabelFps->X = x;
	LabelFps->Y = y;
	LabelFps->Visible = true;
	g_screen_jeu->addElement(LabelFps);

	y += LabelFps->Height + 1;

	LabelShader = new GUILabel();
	LabelShader->Text = "Shader";
	LabelShader->X = x;
	LabelShader->Y = y;
	LabelShader->Visible = true;
	g_screen_jeu->addElement(LabelShader);

	//Ecran de parametrage
	x = 10;
	y = 10;
	g_screen_params = new GUIScreen();

	GUIBouton * btnClose = new GUIBouton();
	btnClose->Titre = std::string("Close");
	btnClose->X = x;
	btnClose->setOnClick(clickBtnCloseParam);
	g_screen_params->addElement(btnClose);

	y += btnClose->Height + 1;
	y += 10;
	x += 10;

	GUILabel * label = new GUILabel();
	label->X = x;
	label->Y = y;
	label->Text = "Param :";
	g_screen_params->addElement(label);

	y += label->Height + 1;

	g_slider = new GUISlider();
	g_slider->setPos(x, y);
	g_slider->setMaxMin(1, 0);
	g_slider->Visible = true;
	g_screen_params->addElement(g_slider);

	y += g_slider->Height + 1;
	y += 10;

	//Ecran a rendre
	g_screen_manager->setActiveScreen(g_screen_jeu);

	//Init Camera
	g_renderer->_Camera->setPosition(NYVert3Df(100, 100, 100));
	g_renderer->_Camera->setLookAt(NYVert3Df(0, 0, 0));

	//Fin init moteur

	//Init application
	//Init Timer
	g_timer = new NYTimer();

	//On start
	g_timer->start();

	// Mouse position
	mouse_last_position_x = 0;
	mouse_last_position_y = 0;

	camera_controller = new FreeFlyCam(g_renderer->_Camera, false);
	day_manager = new DayManager(g_renderer);
	day_manager->setTweakTime(0);

	//World generation
	g_world = new NYWorld();
	g_world->_FacteurGeneration = (float)(rand() % 100) / 50.f;
	g_world->init_world();

	g_world->add_world_to_vbo();

	// Avatar creation
	avatar = new NYAvatar(g_renderer->_Camera, g_world);
	avatar->PickRender = true;

	// Star Manager
	star_manager = new StarManager(day_manager, 400, g_renderer);
	star_manager->createSky(300 * NYCube::CUBE_SIZE, 350 * NYCube::CUBE_SIZE, 0, 0, 0);

	weather = new WeatherSystem(g_world, day_manager, g_renderer->_Camera, 20, 0, 50);

	//Active le post process (chargera le shader de post process "postprocess/pshader.glsl")
	g_renderer->initialise(true);

	//Creation d'un programme de shader, avec vertex et fragment shaders
	g_program = g_renderer->createProgram("shaders/psbase.glsl", "shaders/vsbase.glsl");

	glutMainLoop();

	return 0;
}