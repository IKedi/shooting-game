#include "scenemanager.h"
#include <time.h>

//scenes
#include "mainmenu.h"
#include "gamescene.h"

int current_scene = 0;
int initialized = 0;

int change_scene(int target_scene) {
	if (initialized) {
		CLEANUP(); //cleanup old scene
	}
	current_scene = target_scene;
	return INITIALIZE(); //initialize new scene and return result

	//no need to thank for such comments
	//im pretty sure noone would be able to guess what these do if not for me explaining
}

//im sure theres a way better way than to hardcode every scene
//but this works in small scale.
void RENDER()
{
	switch (current_scene) {
	case 1:game_render(); break;
	default:mainmenu_render(); break;
	}
}
int UPDATE()
{
	switch (current_scene) {
	case 1:return game_update(); break;
	default:return mainmenu_update(); break;
	}
}
int INITIALIZE()
{
	initialized = 1;

	int r = 1; //result
	switch (current_scene) {
	case 1:r=game_initialize(); break;
	default:r=mainmenu_initialize(); break;
	}

	return r;
}
void CLEANUP()
{
	switch (current_scene) {
	case 1:game_cleanup(); break;
	default:mainmenu_cleanup(); break;
	}
}
