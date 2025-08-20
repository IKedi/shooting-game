#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "gamescene.h"
#include "window.h"
#include "game.h"
#include "logs.h" 
#include "scenemanager.h"

time_t game_start_time;
time_t game_end_time;

time_t current_tick;

//static borders
static char mapbufferedge[MAP_SIZE + 1];
static char mapbuffermid[MAP_SIZE + 1];

static char row_buffers[ROW_AMOUNT][MAP_SIZE + 1];

int dead = 0;
int opentab = 0;

#define SELTAB(i) (opentab == i ? ">" : "")

void update_rows() {
	for (int i = 0; i < ROW_AMOUNT; i++) {
		memset(row_buffers[i], ' ', MAP_SIZE);
		row_buffers[i][MAP_SIZE] = '\0';
		row_buffers[i][PLAYER_AREA - 2] = '0' + (i + 1);
	}

	for (int i = 0; i < enemy_count; i++) {
		struct Enemy* enemy = enemies[i];
		if (!enemy) continue;
		if (!row_buffers[enemy->pos.y]) continue;


		if (enemy->pos.x >= 0 && enemy->pos.x < MAP_WIDTH) {
			row_buffers[enemy->pos.y][enemy->pos.x + PLAYER_AREA - 1] = enemy->designation;
		}
	}
}

void shoot(int row) {
	struct Enemy* enemy = get_first_enemy(row);
	if (!enemy) return;

	hurt_enemy(enemy);
	RENDER(); //render early
}

void game_render() {
	if (dead) {
		clear_screen(); //read below for explanation
		printf("You died!\nScore: %d  Kills: %d  Time: %d\n\n\nPress 'n' to start new game",
			player->score, player->kills, (int)(game_end_time - game_start_time));
		return;
	}
	time_t now = time(NULL);

	if (now != current_tick) { //debounce for when render gets called early(eg when shooting)
		current_tick = now;
		game_on_tick(); //could cause delayed frames on old pc
	}
	clear_screen(); //if screen cleared before calculating stuff

	printf("Health: %d Score: %d Kills: %d\n", player->health, player->score, player->kills);

	update_rows();
	for (int i = 0; i < ROW_AMOUNT; i++) {
		printf("%s\n%s\n", i == 0 ? mapbufferedge : mapbuffermid, row_buffers[i]);
	}
	printf("%s\n\n", mapbufferedge);

	printf("%s[l]ogs %s[u]pgrades [n]ew game [c]lose game\n%s\n", SELTAB(0), SELTAB(1), mapbufferedge);
	if (opentab == 0) {
		char* logs = get_log_buffer();

		printf("%s\n", logs == NULL ? "" : logs);
		free(logs);
	}
	else if (opentab == 1) {
		printf("No upgrades yet.\n");
	}
}
int game_update() {
	if (player->health <= 0 && !dead) {
		dead = 1; //death :pensive:
		game_end_time = time(NULL);
	}

	if (kbhit_c()) {
		char input;
		input = getch_c();
		input = tolower((unsigned char)input); //to lowercase

		switch (input) {
		case 'c': return 0; break; //exit gracefully
		case 'n': { //start new game
			CLEANUP();
			INITIALIZE();
			break;
		}
		case 'b': player->health = 0; break; //death
		case 'l': opentab = 0; RENDER(); break; //logs tab
		case 'u': opentab = 1; RENDER(); break; //upgrades tab

		case '0': shoot(9); break; //because 1 is 0 and so on
		default: {
			int n = input - '1';
			if (n > ROW_AMOUNT - 1) break;

			shoot(n);
			break;
		}
		}
	}

	return 1;
}

int game_initialize() {
	create_player();
	if (!player) {
		printf("Could not create player object!");
		return 0;
	}

	for (int i = 0; i < MAP_SIZE; i++) {
		mapbufferedge[i] = '-';
		mapbuffermid[i] = i < PLAYER_AREA ? ' ' : '-';
	}

	mapbufferedge[MAP_SIZE] = '\0';
	mapbuffermid[MAP_SIZE] = '\0';
	game_start_time = time(NULL);
	current_tick = game_start_time; //avoid calculating multiple times

	dead = 0;

	return 1;
}
void game_cleanup() {
	clear_enemies();
	clear_logs();
	destroy_player();
}