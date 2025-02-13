// #include <game.h>>
#include "screen.h"
#include <power_ups.h>
#include <stdlib.h>
#include <string.h>

void add_shield_power_up(shield **shields_head, position pos, char *sprite)
{
    shield *cur_shield = *shields_head, *new_shield = (shield *)malloc(sizeof(shield));
    if (new_shield == NULL) {
        perror("Failed to allocate memory for shield");
        exit(EXIT_FAILURE);
    }

    new_shield->pos.x = pos.x;
    new_shield->pos.y = pos.y;
    
    // Alocar memória para sprite
    new_shield->sprite = malloc(strlen(sprite) + 1);
    if (new_shield->sprite == NULL) {
        perror("Failed to allocate memory for shield sprite");
        free(new_shield);
        exit(EXIT_FAILURE);
    }
    strcpy(new_shield->sprite, sprite);

    new_shield->next = NULL;

    if (*shields_head == NULL) {
        *shields_head = new_shield;
    } else {
        while (cur_shield->next != NULL) {
            cur_shield = cur_shield->next;
        }
        cur_shield->next = new_shield;
    }
}


void draw_shield_power_ups(shield *shields_head)
{
    shield *cur_shield = shields_head;

    while (cur_shield != NULL)
    {
        screenGotoxy(cur_shield->pos.x, cur_shield->pos.y);
        screenSetColor(BLUE, BLACK);
        
        for (int i = 0; cur_shield->sprite[i] != '\0'; i++)
        {
            printf("%c", cur_shield->sprite[i]);
        }

        cur_shield = cur_shield->next;
    }
    screenSetColor(YELLOW, BLACK);
}

void move_shield_power_ups(shield *shields_head, int vel)
{
    shield *cur_shield = shields_head;

    while (cur_shield != NULL)
    {
        cur_shield->pos.y += vel;
        cur_shield = cur_shield->next;
    }
}

void spawn_shields(shield **shields)
{
    int adjusted_minx = MINX + 1;
    int adjusted_maxx = MAXX - 1;
    position shield_position = {create_random_Xposition(adjusted_minx, adjusted_maxx), 0};
    add_shield_power_up(shields, shield_position, "S");
}
