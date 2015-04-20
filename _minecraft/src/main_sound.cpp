//Includes application
#include <conio.h>
#include <vector>

#include "external/gl/glew.h"
#include "external/gl/freeglut.h"
#include "engine/sound/sound.h"
#include "engine/sound/granular_sound.h"

//Nos includes
#include "engine/utils/types_3d.h"
#include "engine/timer.h"
#include "engine/render/renderer.h"

#include "engine/gui/screen.h"
#include "engine/gui/screen_manager.h"
#include "engine/log/log_console.h"

NYRenderer * g_renderer = NULL;
NYTimer * g_timer = NULL;
int g_nb_frames = 0;
float g_elapsed_fps = 0;
int g_main_window_id;
int g_mouse_btn_gui_state = 0;
bool g_fullscreen = false;


//GUI
GUIScreenManager * g_screen_manager = NULL;
GUIBouton * BtnParams = NULL;
GUIBouton * BtnClose = NULL;
GUILabel * LabelFps = NULL;
GUIScreen * g_screen_params = NULL;
GUIScreen * g_screen_jeu = NULL;
GUISlider * g_slider_random;
GUISlider * g_slider_pos;
GUISlider * g_slider_size;
GUISlider * g_slider_overlap;
GUISlider * g_slider_volume_ambiance;

//Sons
SoundEngine * g_sound_engine = NULL;
SoundBasic * g_sound_ambiance;
SoundBasic * g_sound_btn;
SoundGrain * g_sound_grain;

//////////////////////////////////////////////////////////////////////////
// GESTION APPLICATION
//////////////////////////////////////////////////////////////////////////
void runApplication(float elapsed)
{
	static float g_eval_elapsed = 0;

	//Calcul des fps
	g_elapsed_fps += elapsed;
	g_nb_frames++;
	if (g_elapsed_fps > 1.0)
	{
		LabelFps->Text = std::string("FPS : ") + toString(g_nb_frames);
		g_elapsed_fps -= 1.0f;
		g_nb_frames = 0;
	}

	//Update son
	g_sound_engine->update(elapsed);


}

void render2D(void)
{
	g_screen_manager->render();
}
void renderObjectsDepthOnly(void)
{

}

void renderObjects(void)
{

}

void renderFunction(void)
{
	float elapsed = g_timer->getElapsedSeconds(true);
	runApplication(elapsed);
	g_renderer->render(elapsed);
}

void resizeFunction(int width, int height)
{
	glViewport(0, 0, width, height);
	g_renderer->resize(width, height);
}

//////////////////////////////////////////////////////////////////////////
// GESTION CLAVIER SOURIS
//////////////////////////////////////////////////////////////////////////

void specialDownFunction(int key, int p1, int p2)
{
	if (key == GLUT_KEY_F5)
	{

	}
}

void specialUpFunction(int key, int p1, int p2)
{

}

void keyboardDownFunction(unsigned char key, int p1, int p2)
{
	if (key == VK_ESCAPE)
	{
		glutDestroyWindow(g_main_window_id);
		exit(0);
	}

	if (key == 'f')
	{
		if (!g_fullscreen){
			glutFullScreen();
			g_fullscreen = true;
		}
		else if (g_fullscreen){
			glutLeaveGameMode();
			glutLeaveFullScreen();
			glutReshapeWindow(g_renderer->_ScreenWidth, g_renderer->_ScreenWidth);
			glutPositionWindow(0, 0);
			g_fullscreen = false;
		}
	}
}

void keyboardUpFunction(unsigned char key, int p1, int p2)
{

}

void mouseWheelFunction(int wheel, int dir, int x, int y)
{

}

void mouseFunction(int button, int state, int x, int y)
{
	//GUI
	g_mouse_btn_gui_state = 0;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		g_mouse_btn_gui_state |= GUI_MLBUTTON;

	bool mouseTraite = false;
	mouseTraite = g_screen_manager->mouseCallback(x, y, g_mouse_btn_gui_state, 0, 0);

	if (mouseTraite && state == GLUT_DOWN)
		g_sound_btn->play();
	if (mouseTraite)
	{
		g_sound_grain->setGrainParam((float)g_slider_pos->Value, (float)g_slider_size->Value, (float)g_slider_random->Value, (float)g_slider_overlap->Value);
		g_sound_ambiance->setVolume((float)g_slider_volume_ambiance->Value);
	}
}

void mouseMoveFunction(int x, int y, bool pressed)
{
	bool mouseTraite = false;

	mouseTraite = g_screen_manager->mouseCallback(x, y, g_mouse_btn_gui_state, 0, 0);
	if (pressed && mouseTraite)
	{
		g_sound_grain->setGrainParam((float)g_slider_pos->Value, (float)g_slider_size->Value, (float)g_slider_random->Value, (float)g_slider_overlap->Value);
		g_sound_ambiance->setVolume((float)g_slider_volume_ambiance->Value);
	}
}

void mouseMoveActiveFunction(int x, int y)
{
	mouseMoveFunction(x, y, true);
}
void mouseMovePassiveFunction(int x, int y)
{
	mouseMoveFunction(x, y, false);
}

void joystickFunction(unsigned int buttonmask, int x, int y, int z)
{
	_cprintf("%x %d %d %d\n", buttonmask, x, y, z);
}

void clickBtnParams(GUIBouton * bouton)
{
	g_screen_manager->setActiveScreen(g_screen_params);
}

void clickBtnCloseParam(GUIBouton * bouton)
{
	g_screen_manager->setActiveScreen(g_screen_jeu);
}

void clickBtnPlayGrain(GUIBouton * bouton)
{
	if (g_sound_grain->isPlaying())
	{
		g_sound_grain->stop();
		bouton->Titre = "Play Grains";
	}
	else
	{
		g_sound_grain->play();
		bouton->Titre = "Stop Grains";
	}

}




/**
* POINT D'ENTREE PRINCIPAL
**/
int main(int argc, char* argv[])
{
	int screen_width = 800;
	int screen_height = 600;

	Log::addLog(new LogConsole());

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

	bool gameMode = true;
	for (int i = 0; i<argc; i++)
	{
		if (argv[i][0] == 'w')
		{
			Log::log(Log::ENGINE_INFO, "arg w asking window mode");
			gameMode = false;
		}
	}

	if (gameMode)
	{
		//glutGameModeGet	(	GLenum info );
		int width = glutGet(GLUT_SCREEN_WIDTH);
		int height = glutGet(GLUT_SCREEN_HEIGHT);
		//int color = glutGet();

		char gameModeStr[200];
		sprintf(gameModeStr, "%dx%d:32@60", width, height);
		glutGameModeString(gameModeStr);
		g_main_window_id = glutEnterGameMode();
	}
	else
	{
		g_main_window_id = glutCreateWindow("Synthese Granulaire");
		glutReshapeWindow(screen_width, screen_height);
	}

	if (g_main_window_id < 1)
	{
		Log::log(Log::ENGINE_ERROR, "Unable to create main window");
		exit(EXIT_FAILURE);
	}

	GLenum glewInitResult = glewInit();

	if (glewInitResult != GLEW_OK)
	{
		Log::log(Log::ENGINE_ERROR, ("Unable to init glew " + toString(glewGetErrorString(glewInitResult))).c_str());
		exit(EXIT_FAILURE);
	}

	glutDisplayFunc(renderFunction);
	glutReshapeFunc(resizeFunction);
	glutKeyboardFunc(keyboardDownFunction);
	glutKeyboardUpFunc(keyboardUpFunction);
	glutSpecialFunc(specialDownFunction);
	glutSpecialUpFunc(specialUpFunction);
	glutMouseFunc(mouseFunction);
	glutMotionFunc(mouseMoveActiveFunction);
	glutPassiveMotionFunc(mouseMovePassiveFunction);
	//glutJoystickFunc(joystickFunction,10);
	glutIgnoreKeyRepeat(1);

	//Initialisation du renderer
	g_renderer = NYRenderer::getInstance();
	g_renderer->setRenderObjectFun(renderObjects);
	g_renderer->setRenderObjectDepthOnlyFun(renderObjectsDepthOnly);
	g_renderer->setRender2DFun(render2D);
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
	label->Text = "Ambiance :";
	g_screen_params->addElement(label);

	y += label->Height + 1;

	g_slider_volume_ambiance = new GUISlider();
	g_slider_volume_ambiance->setPos(x, y);
	g_slider_volume_ambiance->setMaxMin(1, 0);
	g_slider_volume_ambiance->Visible = true;
	g_screen_params->addElement(g_slider_volume_ambiance);

	y += g_slider_volume_ambiance->Height + 1;
	y += 10;


	GUIBouton * btnPlayGrain = new GUIBouton();
	btnPlayGrain->Titre = std::string("Play Grains");
	btnPlayGrain->X = x;
	btnPlayGrain->Y = y;
	btnPlayGrain->setOnClick(clickBtnPlayGrain);
	g_screen_params->addElement(btnPlayGrain);

	y += btnPlayGrain->Height + 1;

	g_slider_pos = new GUISlider();
	g_slider_pos->setPos(x, y);
	g_slider_pos->setMaxMin(1, 0);
	g_slider_pos->Visible = true;
	g_screen_params->addElement(g_slider_pos);

	y += g_slider_pos->Height + 1;

	g_slider_size = new GUISlider();
	g_slider_size->setPos(x, y);
	g_slider_size->setMaxMin(1.0, 0.01);
	g_slider_size->Visible = true;
	g_screen_params->addElement(g_slider_size);

	y += g_slider_size->Height + 1;

	g_slider_overlap = new GUISlider();
	g_slider_overlap->setPos(x, y);
	g_slider_overlap->setMaxMin(0.95, -0.95);
	g_slider_overlap->Visible = true;
	g_screen_params->addElement(g_slider_overlap);

	y += g_slider_overlap->Height + 1;

	g_slider_random = new GUISlider();
	g_slider_random->setPos(x, y);
	g_slider_random->setMaxMin(1.0, 0);
	g_slider_random->Visible = true;
	g_screen_params->addElement(g_slider_random);

	y += g_slider_random->Height + 1;

	//Ecran a rendre
	g_screen_manager->setActiveScreen(g_screen_jeu);

	//On charge les sons
	g_sound_engine = SoundEngine::getInstance();
	g_sound_ambiance = new SoundBasic();
	g_sound_ambiance->load("sound/snd_background_1.wav");
	g_sound_ambiance->setVolume(0.2f);
	g_slider_volume_ambiance->setValue(0.2);
	g_sound_ambiance->_Loop = true;
	g_sound_ambiance->play();
	g_sound_engine->addSound(g_sound_ambiance);


	g_sound_btn = new SoundBasic();
	g_sound_btn->load("sound/snd_btn_1.wav");
	g_sound_engine->addSound(g_sound_btn);

	g_sound_grain = new SoundGrain();
	g_sound_grain->loadBaseFile("sound/grain_2.wav");
	g_sound_grain->_Loop = true;
	g_sound_grain->setGrainParam(0.5f, 0.05f, 0.05f, 0.7f);
	g_sound_engine->addSound(g_sound_grain);

	g_slider_pos->setValue(0.5);
	g_slider_size->setValue(0.05);
	g_slider_random->setValue(0.05);
	g_slider_overlap->setValue(0.7);

	//Init Timer
	g_timer = new NYTimer();

	//On start
	g_timer->start();

	glutMainLoop();

	return 0;
}

