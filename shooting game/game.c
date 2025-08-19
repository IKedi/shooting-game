#include "game.h"
#include "logs.h"

#include <stdio.h>
#include <stdlib.h>

struct Player* player = NULL;
struct Enemy** enemies = NULL;
int enemy_count = 0;
int enemy_capacity = 0; //helps with dynamic sizing of enemies array
int next_enemy_type = 0;

const float HEALTH_SCORE_WEIGHT = 0.1f;
const float DAMAGE_SCORE_WEIGHT = 0.5f;
const float SPEED_SCORE_WEIGHT = 0.2f;

void realloc_enemy_array() {
	struct Enemy** tmp = realloc(enemies, enemy_capacity * sizeof(struct Enemy*));
	if (!tmp) return;
	enemies = tmp;
}

int clamp_row(int row) {
	return (row >= ROW_AMOUNT) ? ROW_AMOUNT - 1 : ((row < 0) ? 0 : row);
}

int get_random_enemy_type() {
	int r = rand() % 100;

	if (r < 70) return 0;
	if (r < 90) return 1;
	if (r < 98) return 2;
	return 3;
}

void set_enemy_score(struct Enemy* enemy) {
	float score = (HEALTH_SCORE_WEIGHT * enemy->health) +
		(DAMAGE_SCORE_WEIGHT * enemy->damage) +
		(SPEED_SCORE_WEIGHT * enemy->speed);

	if (score < 2) score = 2;
	enemy->score = (int)score;
}

void assign_enemy_type(struct Enemy* enemy) {
	if (next_enemy_type == 0 || !enemy) return;

	switch (next_enemy_type) {
	case 1: //speedy, weak
		enemy->speed = 4;
		enemy->damage = ENEMY_DAMAGE / 1.5;
		enemy->health = ENEMY_HEALTH / 2;
		enemy->designation = '.';
		break;
	case 2: //stronger
		enemy->damage = ENEMY_DAMAGE * 1.2;
		enemy->designation = '+';
		break;
	case 3: //boss
		enemy->speed = 1;
		enemy->health = ENEMY_HEALTH * 5;
		enemy->damage = ENEMY_DAMAGE * 2;
		enemy->designation = '#';
		break;
	}
	
	next_enemy_type = 0;
	set_enemy_score(enemy);
}

struct Player* create_player() {
	destroy_player();

	player = malloc(sizeof(struct Player));
	if (!player) return NULL;

	player->health = PLAYER_HEALTH;
	player->score = 0;
	player->kills = 0;
	return player;
}

void destroy_player() {
	if (player != NULL) {
		free(player);
		player = NULL;
	}
}

void hurt_player(int damage) {
	if (!player) return;
	player->health -= damage;
	
	if (player->health < 0) player->health = 0;
	else create_log("An enemy hits you for %d damage!", damage);
}

struct Enemy* create_enemy(int row) {
	row = clamp_row(row);

	struct Enemy* enemy = malloc(sizeof(struct Enemy));
	if (!enemy) return NULL;
	enemy->health = ENEMY_HEALTH;
	enemy->damage = ENEMY_DAMAGE;
	enemy->speed = 2;
	enemy->designation = '@';
	enemy->score = ENEMY_SCORE;

	assign_enemy_type(enemy);

	enemy->pos.x = MAP_WIDTH;
	enemy->pos.y = row;

	enemy_count++;

	if (enemy_count >= enemy_capacity) {
		enemy_capacity = (enemy_capacity == 0) ? 4 : enemy_capacity * 2;
		realloc_enemy_array();

		if (!enemies) {
			free(enemy);
			return NULL;
		}
	}

	enemies[enemy_count-1] = enemy;
	return enemy;
}

void hurt_enemy(struct Enemy* enemy) {
	if (enemy == NULL) return;
	enemy->health -= PLAYER_DAMAGE;

	if (enemy->health <= 0) {
		obliterate_enemy(enemy);
		return;
	}
	create_log("You hit an enemy for %d damage", PLAYER_DAMAGE);
}

void obliterate_enemy(struct Enemy* enemy) {
	if (enemy == NULL) return;

	if (player) {
		player->kills += 1;
		player->score += enemy->score;

		create_log("You finish off an enemy for %d points!", enemy->score);
	}

	for (int i = 0; i < enemy_count; i++) {
		if (enemies[i] == enemy) {
			enemies[i] = enemies[enemy_count - 1]; //fill the gap with last enemy
			enemy_count--;

			break;
		}
	}

	if (enemy_count < enemy_capacity / 4) {
		enemy_capacity /= 2;
		realloc_enemy_array();
	}

	free(enemy); //free my boy he dun nuthin'
}

void clear_enemies() {
	for (int i = 0; i < enemy_count; i++) {
		free(enemies[i]);
	}
	free(enemies);
	enemies = NULL;

	enemy_count = 0;
	enemy_capacity = 0;
}

void game_on_tick() {
	if (!player || player->health <= 0) return;

	if (enemy_count > 0) {
		for (int i = 0; i < enemy_count; i++) {
			struct Enemy* enemy = enemies[i];

			if (enemy) {
				if (enemy->pos.x > 0) {
					enemy->pos.x -= enemy->speed;

					if (enemy->pos.x < 0) enemy->pos.x = 0;
				}
				else {
					hurt_player(enemy->damage);
				}
			}
		}
	}
	

	//create new enemy randomly
	int random = rand() % 100;
	if (random < 60) return;

	int row = rand() % ROW_AMOUNT;

	next_enemy_type = get_random_enemy_type();
	create_enemy(row);
}

struct Enemy* get_first_enemy(int row) {
	row = clamp_row(row);

	struct Enemy* closest = NULL;

	for (int i = 0; i < enemy_count; i++) {
		if (enemies[i] && enemies[i]->pos.y == row) {
			if (closest == NULL || enemies[i]->pos.x < closest->pos.x) {
				closest = enemies[i];
			}
		}
	}
	
	return closest;
}