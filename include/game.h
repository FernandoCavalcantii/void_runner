#ifndef GAME_H
#define GAME_H

#include <time.h>

// Definições de constantes
#define PLAYER_VEL 1
#define BULLET_VEL 1
#define BASE_OBJECT_VEL 2.5
#define MEDIUM_OBJECT_VEL 2.75
#define COIN_VEL 2
#define PLAYER_HEIGHT 2
#define PLAYER_WIDTH 3
#define TRUE 1
#define EASY_SCORE 1000
#define MEDIUM_SCORE 2000
#define HARD_SCORE 3000
#define PRO_DIFFICULTY 3500

// Definições de estruturas
typedef struct position {
    int x;
    int y;
} position;

typedef struct player {
    int x;
    int y;
    int direction;
    char img;
    struct player *next;
} player;

typedef struct object {
    position pos;
    int life;
    char *sprite;
    int is_destructible;
    int direction;
    int hit_area_init;
    int hit_area_end;
    struct object *next;
} object;

typedef struct particle {
    position pos;
    char img;
    struct particle *next;
} particle;

typedef struct collectable {
    position pos;
    char *sprite;
    struct collectable *next;
} collectable;

typedef struct player_score
{
    char name[4];
    int score;
} player_score;

// Declarações de funções
int delay_to_action(double delay_time, clock_t *last_t);

int check_collision(player ship, object **objects);

int is_obj_destroyed(particle *bullet, object *obj);

void handle_collision_object_bullet(object **objects, particle **bullets);

void drawPlayer(char (*ps)[PLAYER_WIDTH], player ship);

void draw_bullets(particle *head);

void add_bullet(particle **head, int x, int y, char img);

void move_bullets(particle *head);

void remove_bullets(particle **head);

int len_bullets(particle *head);

char *choose_enemy_sprite(int difficulty);

int define_enemy_type(object *enemy);

void add_object(object **head, int x, int y, int life, char *sprite);

void draw_object(object *head);

int create_random_Xposition(int minx, int maxx);

void spawn_enemy(object **head);

void spawn_collectables(collectable **coin);

void move_object(object **head, int player_y);

void save_score(const char *name, int score);

void draw_game_information(int score, particle *bullets, int out_of_bullets);

void move(player *ship);

void add_collectables(collectable **coin_head, int x, int y, char *sprite);

void draw_collectables(collectable *coin_head);

void move_collectables(collectable *coin_head, int vel);

void collision_collectables(collectable **coin_head, player ship);

#endif // GAME_H
