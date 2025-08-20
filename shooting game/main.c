#include <stdio.h>
#include <time.h>

#include "scenemanager.h"
#include "window.h"

#define WINDOW_TITLE "Game"
#define UNCAPPED_LOOP 0 //runs keycheck without any delays. not recommended.

int running = 1;
time_t start_time; //not sure if this ever will be used.

int main() {
	init_keyboard(); //does nothing on windows, used for compability on linux
	set_window_title(WINDOW_TITLE);

	if (!change_scene(1)) { //good luck figuring the issue
		printf("Failed to load scene\n");
		return 1;
	}

	time_t current_tick = time(NULL);
	start_time = current_tick;

	while (running) {
		time_t now = time(NULL);

		running = UPDATE();
		if (now != current_tick) {
			current_tick = now;
			RENDER();
		}

		if (!UNCAPPED_LOOP) {
			//actually significant, game goes from 5%
			//(10-15% total if you count console)
			//to around 5% total, mostly under 1% cpu usage. 
			sleep_ms(16); //helps lower cpu usage without much notice
		}
	}

	CLEANUP();
	reset_keyboard(); //also does nothing on windows
	return 0;
}