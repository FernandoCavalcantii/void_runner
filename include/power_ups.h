#ifndef SHIELD_H
#define SHIELD_H
#include <game.h>
#include <unistd.h>

// Declaração de constantes
#define SHIELD_VEL 2

// Declaração de estruturas
typedef struct shield {
    position pos;
    char *sprite;
    struct shield *next;
} shield;

// Declaração de funções
void add_shield_power_up(shield **shields_head, position pos, char *sprite);
void draw_shield_power_ups(shield *shields_head);
void move_shield_power_ups(shield *shields_head, int vel);
void spawn_shields(shield **shields);

#endif
