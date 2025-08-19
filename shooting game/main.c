#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "window.h"
#include "game.h"
#include "logs.h" 

#define WINDOW_TITLE "Game"
#define UNCAPPED_LOOP 0 //runs keycheck without any delays. not recommended.

int running = 1;
time_t start_time;
time_t game_start_time;
time_t game_end_time;

static char mapbufferedge[MAP_SIZE +1];
static char mapbuffermid[MAP_SIZE +1];

static char row_buffers[ROW_AMOUNT][MAP_SIZE + 1];

int dead = 0;
int opentab = 0;

#define SELTAB(i) (opentab == i ? ">" : "")

void update_rows() {
	for (int i = 0; i < ROW_AMOUNT; i++) {
		memset(row_buffers[i], ' ', MAP_SIZE);
		row_buffers[i][MAP_SIZE] = '\0';
		row_buffers[i][PLAYER_AREA - 2] = '0'+(i + 1);
	}

	for (int i = 0; i < enemy_count; i++) {
		struct Enemy* enemy = enemies[i];
		if (!enemy) continue;
		if (!row_buffers[enemy->pos.y]) continue;


		if (enemy->pos.x >= 0 && enemy->pos.x < MAP_WIDTH) {
			row_buffers[enemy->pos.y][enemy->pos.x + PLAYER_AREA] = enemy->designation;
		}
	}
}

void render() {
	clear_screen();

	if (dead) {
		printf("You died!\nScore: %d  Kills: %d  Time: %d\n\n\nPress 'n' to start new game",
			player->score, player->kills, (int)(game_end_time- game_start_time));
		return;
	}

	printf("Health: %d Score: %d Kills: %d\n", player->health, player->score, player->kills);
	
	update_rows();
	for (int i = 0; i < ROW_AMOUNT; i++) {
		printf("%s\n%s\n", i == 0 ? mapbufferedge : mapbuffermid, row_buffers[i]);
	}
	printf("%s\n\n", mapbufferedge);

	printf("%s[l]ogs %s[u]pgrades\n%s\n", SELTAB(0), SELTAB(1), mapbufferedge);
	if (opentab == 0) {
		char* logs = get_log_buffer();

		printf("%s\n", logs == NULL ? "": logs);
		free(logs);
	}
	else if (opentab == 1) {
		printf("No upgrades yet.\n");
	}
}

void shoot(int row) {
	struct Enemy* enemy = get_first_enemy(row);
	if (!enemy) return;

	hurt_enemy(enemy);
	render(); //render early
}

void key_press() {
	if (kbhit_c()) {
		char input;
		input = getch_c();
		input = tolower((unsigned char)input); //to lowercase

		switch (input) {
			case 'c': running = 0; break; //exit gracefully
			case 'n': { //start new game
				clear_enemies();
				clear_logs();
				create_player();
				game_start_time = time(NULL);
				dead = 0;
				break;
			}
			case 'b': player->health = 0; break; //death
			case 'l': opentab = 0; render(); break;
			case 'u': opentab = 1; render(); break;
			case '0': shoot(9); break; //because 1 is 0 and so on
			default: {
				int n = input - '1';
				if (n > ROW_AMOUNT-1) break;

				shoot(n);
				break;
			}
				
		}
	}
}

int main() {
	init_keyboard(); //does nothing on windows, used for compability on linux
	create_player();
	if (!player) {
		printf("Could not create player object!");
		return 1;
	}

	printf("\033]0;%s\007", WINDOW_TITLE);

	for (int i = 0; i < MAP_SIZE; i++) {
		mapbufferedge[i] = '-';
		mapbuffermid[i] = i < PLAYER_AREA ? ' ' : '-';
	}

	mapbufferedge[MAP_SIZE] = '\0';
	mapbuffermid[MAP_SIZE] = '\0';

	time_t current_tick = time(NULL);
	start_time = current_tick;
	game_start_time = current_tick;

	while (running) {
		if (player->health <= 0 && !dead) {
			dead = 1;
			game_end_time = time(NULL);
		}

		key_press();

		time_t now = time(NULL);
		if (now != current_tick) {
			current_tick = now;
			game_on_tick();
			render();
		}

		if (!UNCAPPED_LOOP) {
			//actually significant, game goes from 5%
			//(10-15% total if you count console)
			//to around 5% total, mostly under 1% cpu usage. 
			sleep(16); //helps lower cpu usage without much notice
		}
	}

	clear_enemies(); //free memory
	return 0;
}