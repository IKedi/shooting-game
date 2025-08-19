#pragma once

#define PLAYER_HEALTH 100
#define PLAYER_DAMAGE 30

#define ENEMY_HEALTH 100
#define ENEMY_DAMAGE 20
#define ENEMY_SCORE 10

#define ROW_AMOUNT 3 //is dynamic so you can change up to 10 without any problems
#define MAP_WIDTH 80
#define PLAYER_AREA 10
#define MAP_SIZE (PLAYER_AREA + MAP_WIDTH)

struct Point { int x, y; };
struct Enemy {
	struct Point pos;
	int health, damage, speed, score;
	char designation;
};

struct Player { int health, score, kills; };

extern struct Player* player;
extern struct Enemy** enemies;
extern int enemy_count;
extern int next_enemy_type; //probably one of the worst ways to implement this, auto resets to 0.

struct Player* create_player();
void destroy_player();
void hurt_player();

struct Enemy* create_enemy(int row);
void hurt_enemy(struct Enemy* enemy);
void obliterate_enemy(struct Enemy* enemy);
void clear_enemies();

void game_on_tick();
struct Enemy* get_first_enemy(int row);