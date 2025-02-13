#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <screen.h>
#include <keyboard.h>
#include <timer.h>
#include <ui_utils.h>
#include <game.h>
#include <power_ups.h>

int score = 0;
int level = 1;
int lives;
int invulnerability_time = 0;
int direction_obj = 1;

char player_sprite[PLAYER_HEIGHT][PLAYER_WIDTH] = {
    {' ', '^', ' '},
    {'/', '=', '\\'}};

char object_sprite1[] = "===============";
char object_sprite2[] = "===//===";
char object_sprite3[] = "====---====";
char object_sprite4[] = "====//====";
char object_sprite5[] = "=========";
char object_sprite6[] = "=====--=====";
char object_sprite7[] = "===-----===";

int delay_to_action(double delay_time, clock_t *last_t)
{
    clock_t current_t = clock();
    if ((double)(current_t - *last_t) / CLOCKS_PER_SEC >= delay_time)
    {
        *last_t = current_t;
        return 1;
    }
    return 0;
}

int check_collision(player ship, object **objects)
{
    object *iterate_object = *objects, *temp = *objects;

    if (*objects != NULL)
    {
        for (int i = 0; iterate_object->sprite[i] != '\0'; i++)
        {
            if (((ship.x <= (iterate_object->pos.x + i) && (ship.x + PLAYER_WIDTH - 1) >= iterate_object->pos.x + i)) && (ship.y <= (iterate_object->pos.y) && (ship.y + PLAYER_HEIGHT - 1) >= (iterate_object->pos.y)))
            {
                *objects = iterate_object->next;
                free(temp);
                return 0;
            }
        }

        while (iterate_object->next != NULL)
        {
            for (int i = 0; iterate_object->next->sprite[i] != '\0'; i++)
            {
                if (((ship.x <= (iterate_object->next->pos.x + i) && (ship.x + PLAYER_WIDTH - 1) >= iterate_object->next->pos.x + i)) && (ship.y <= (iterate_object->next->pos.y) && (ship.y + PLAYER_HEIGHT - 1) >= (iterate_object->next->pos.y)))
                {
                    temp = iterate_object->next;
                    iterate_object = iterate_object->next->next;
                    free(temp);
                    return 0;
                }
            }
            iterate_object = iterate_object->next;
        }
    }
    return 1;
}

int is_obj_destroyed(particle *bullet, object *obj)
{
    if (obj->is_destructible)
    {
        if ((bullet->pos.x >= obj->pos.x + obj->hit_area_init) && (bullet->pos.x <= obj->pos.x + obj->hit_area_end))
        {
            return 1;
        }
    }
    return 0;
}

void handle_collision_object_bullet(object **objects, particle **bullets)
{
    object *curr_object = *objects, *prev_object = NULL;
    particle *curr_bullet = *bullets, *prev_bullet = NULL;

    while (curr_bullet != NULL)
    {
        int bullet_destroyed = 0;
        curr_object = *objects;
        prev_object = NULL;

        while (curr_object != NULL)
        {
            if (abs(curr_bullet->pos.y - curr_object->pos.y) <= 1)
            {
                int relative_x = curr_bullet->pos.x - curr_object->pos.x;
                if (relative_x >= 0 && relative_x < strlen(curr_object->sprite))
                {
                    char hit_char = curr_object->sprite[relative_x];

                    if ((strcmp(curr_object->sprite, object_sprite2) == 0 && hit_char == '/') ||
                        (strcmp(curr_object->sprite, object_sprite3) == 0 && hit_char == '-') || (strcmp(curr_object->sprite, object_sprite4) == 0 && hit_char == '/')
                        || (strcmp(curr_object->sprite, object_sprite6) == 0 && hit_char == '-') || (strcmp(curr_object->sprite, object_sprite7) == 0 && hit_char == '-'))
                    {

                        if (prev_object == NULL)
                        {
                            *objects = curr_object->next;
                        }
                        else
                        {
                            prev_object->next = curr_object->next;
                        }

                        score += 25;
                        free(curr_object->sprite);
                        free(curr_object);

                        bullet_destroyed = 1;
                        break;
                    }

                    if (hit_char == '=')
                    {
                        bullet_destroyed = 1;
                        break;
                    }
                }
            }

            prev_object = curr_object;
            curr_object = curr_object->next;
        }

        if (bullet_destroyed)
        {
            if (prev_bullet == NULL)
            {
                *bullets = curr_bullet->next;
            }
            else
            {
                prev_bullet->next = curr_bullet->next;
            }
            particle *temp_bullet = curr_bullet;
            curr_bullet = curr_bullet->next;
            free(temp_bullet);
        }
        else
        {
            prev_bullet = curr_bullet;
            curr_bullet = curr_bullet->next;
        }
    }
}

void drawPlayer(char (*ps)[PLAYER_WIDTH], player ship)
{
    for (int i = 0; i < PLAYER_HEIGHT; i++)
    {
        for (int j = 0; j < PLAYER_WIDTH; j++)
        {
            screenGotoxy(ship.x + j, ship.y + i);

            if (invulnerability_time > 0)
            {
                screenSetColor(BLUE, BLACK);
            }
            else
            {
                screenSetColor(YELLOW, BLACK);
            }
            printf("%c", ps[i][j]);
        }
    }
    screenSetColor(YELLOW, BLACK);
    screenUpdate();
}


void draw_bullets(particle *head)
{
    particle *iterate_bullets = head;
    screenSetColor(LIGHTCYAN, BLACK);
    while (iterate_bullets != NULL)
    {
        screenGotoxy((iterate_bullets->pos).x, (iterate_bullets->pos).y);
        printf("%c", iterate_bullets->img);
        iterate_bullets = iterate_bullets->next;
    }

    screenSetColor(YELLOW, BLACK);
}

void add_bullet(particle **head, int x, int y, char img)
{
    particle *iterate_bullets = *head, *new_bullet = (particle *)malloc(sizeof(particle));
    (new_bullet->pos).x = x;
    (new_bullet->pos).y = y;
    new_bullet->img = img;
    new_bullet->next = NULL;

    if (*head == NULL)
    {
        *head = new_bullet;
    }
    else
    {
        while (iterate_bullets->next != NULL)
        {
            iterate_bullets = iterate_bullets->next;
        }
        iterate_bullets->next = new_bullet;
    }
}

void move_bullets(particle *head)
{
    particle *iterate_bullets = head;

    while (iterate_bullets != NULL)
    {
        (iterate_bullets->pos).y -= BULLET_VEL;
        iterate_bullets = iterate_bullets->next;
    }
}

void remove_bullets(particle **head)
{
    particle *iterate_bullets = *head, *temp = *head;
    if (*head != NULL)
    {
        if ((*head)->pos.y <= MINY)
        {
            *head = (*head)->next;
            free(temp);
        }

        while (iterate_bullets != NULL && iterate_bullets->next != NULL)
        {
            if (iterate_bullets->next->pos.y <= MINY)
            {
                temp = iterate_bullets->next;
                iterate_bullets->next = iterate_bullets->next->next;
                free(temp);
            }
            else
            {
                iterate_bullets = iterate_bullets->next;
            }
        }
    }
}

int len_bullets(particle *head)
{
    particle *iterate_bullets = head;
    int count = 0;
    while (iterate_bullets != NULL)
    {
        count += 1;
        iterate_bullets = iterate_bullets->next;
    }
    return count;
}

char *enemy_sprite_easy_option(int sprite_choice)
{
    switch (sprite_choice)
    {
    case 1:
    case 2:
    case 3:
    case 4:
        return object_sprite5;
    case 5:
    case 6:
    case 7:
        return object_sprite2;
    case 8:
    case 9:
        return object_sprite3;
    case 10:
        return object_sprite1;
    default:
        return object_sprite5;
    }
}

char *enemy_sprite_medium_option(int sprite_choice)
{
    switch (sprite_choice)
    {
    case 1:
    case 2:
    case 3:
        return object_sprite5;
    case 4:
    case 5:
    case 6:
        return object_sprite4;
    case 7:
    case 8:
    case 9:
        return object_sprite3;
    case 10:
    case 11:
        return object_sprite1;
    case 12:
        return object_sprite7;
    default:
        return object_sprite5;
    }
}

char *enemy_sprite_hard_option(int sprite_choice)
{
    switch (sprite_choice)
    {
    case 1:
    case 2:
    case 3:
        return object_sprite5;
    case 4:
    case 5:
        return object_sprite4;
    case 6:
    case 7:
    case 8:
        return object_sprite6;
    case 9:
    case 10:
        return object_sprite1;
    case 11:
    case 12:
        return object_sprite7;
    default:
        return object_sprite5;
    }
}

char *choose_enemy_sprite(int difficulty)
{
    unsigned int seed = time(0);
    int sprite_choice = rand_r(&seed) % 12 + 1;

    if (difficulty == 1)
    {
        enemy_sprite_easy_option(sprite_choice);
    }
    else if (difficulty == 2)
    {
        enemy_sprite_medium_option(sprite_choice);
    }
    else if (difficulty == 3)
    {
        enemy_sprite_hard_option(sprite_choice);
    }
}

int define_enemy_type(object *enemy)
{
    if (strcmp(enemy->sprite, object_sprite2) == 0)
    {
        enemy->hit_area_init = 3;
        enemy->hit_area_end = 4;
        return 1;
    }
    if (strcmp(enemy->sprite, object_sprite3) == 0)
    {
        enemy->hit_area_init = 3;
        enemy->hit_area_end = 7;
        return 1;
    }
    if (strcmp(enemy->sprite, object_sprite4) == 0)
    {
        enemy->hit_area_init = 4;
        enemy->hit_area_end = 5;
        return 1;
    }
    if (strcmp(enemy->sprite, object_sprite6) == 0)
    {
        enemy->hit_area_init = 4;
        enemy->hit_area_end = 7;
        return 1;
    }

    enemy->hit_area_init = 0;
    enemy->hit_area_end = 0;
    return 0;
}

void add_object(object **head, int x, int y, int life, char *sprite)
{
    object *iterate_object = *head, *new_object = (object *)malloc(sizeof(object));
    new_object->sprite = (char *)malloc(sizeof(char) * (strlen(sprite) + 1));
    strcpy(new_object->sprite, sprite);
    new_object->is_destructible = define_enemy_type(new_object);
    new_object->direction = 1;
    (new_object->pos).x = x;
    (new_object->pos).y = y;
    new_object->next = NULL;

    if (*head == NULL)
    {
        *head = new_object;
    }
    else
    {
        while (iterate_object->next != NULL)
        {
            iterate_object = iterate_object->next;
        }
        iterate_object->next = new_object;
    }
}

void draw_object(object *head)
{
    object *iterate_object = head;
    while (iterate_object != NULL)
    {
        for (int i = 0; iterate_object->sprite[i] != '\0'; i++)
        {
            if (iterate_object->sprite[i] == '/' || iterate_object->sprite[i] == '-')
            {
                screenSetColor(RED, BLACK);
                screenGotoxy(iterate_object->pos.x + i, iterate_object->pos.y);
                printf("%c", iterate_object->sprite[i]);
            }
            else if (strcmp(iterate_object->sprite, object_sprite7) == 0)
            {
                screenSetColor(BROWN, BLACK);
                screenGotoxy(iterate_object->pos.x + i, iterate_object->pos.y);
                printf("%c", iterate_object->sprite[i]);
            }
            else {
                screenSetColor(MAGENTA, BLACK);
                screenGotoxy(iterate_object->pos.x + i, iterate_object->pos.y);
                printf("%c", iterate_object->sprite[i]);
            }
        }
        iterate_object = iterate_object->next;
    }
    screenSetColor(YELLOW, BLACK);
}

int create_random_Xposition(int minx, int maxx)
{
    return rand() % (maxx - minx + 1) + minx;
}

void spawn_enemy(object **head)
{
    char *enemy_sprite;
    const int EASY = 1, MEDIUM = 2, HARD = 3;

    if (score < EASY_SCORE)
    {
        enemy_sprite = choose_enemy_sprite(EASY);
    }
    else if (score >= EASY_SCORE && score < MEDIUM_SCORE)
    {
        enemy_sprite = choose_enemy_sprite(MEDIUM);
    }
    else
    {
        enemy_sprite = choose_enemy_sprite(HARD);
    }

    int sprite_length = strlen(enemy_sprite);
    int adjusted_minx = MINX + 1;
    int adjusted_maxx = MAXX - sprite_length;

    int enemy_x = create_random_Xposition(adjusted_minx, adjusted_maxx);
    add_object(head, enemy_x, -2, 2, enemy_sprite);
}

void spawn_collectables(collectable **coin)
{
    int adjusted_minx = MINX + 1;
    int adjusted_maxx = MAXX - 1;

    int coin_x = create_random_Xposition(adjusted_minx, adjusted_maxx);
    add_collectables(coin, coin_x, 0, "℗");
}

void move_object(object **head, int player_y)
{
    object *iterate_object = *head, *prev_object = NULL;
    while (iterate_object != NULL)
    {

        if (iterate_object->pos.y < player_y + 5)
        {
            iterate_object->pos.y += BASE_OBJECT_VEL;

            if (strcmp(iterate_object->sprite, object_sprite7) == 0) {
                if (iterate_object->pos.x + 1 + strlen(iterate_object->sprite) > MAXX - 1)
                {
                    iterate_object->direction = -1;
                }

                if (iterate_object->pos.x - 1 < MINX + 2)
                {
                    iterate_object->direction = 1;
                }

                iterate_object->pos.x += BASE_OBJECT_VEL * iterate_object->direction;
            }

        }
        if (iterate_object->pos.y >= player_y + 5)
        {
            if (prev_object == NULL)
            {
                *head = iterate_object->next;
            }
            else
            {
                prev_object->next = iterate_object->next;
            }

            free(iterate_object);
            iterate_object = (prev_object == NULL) ? *head : prev_object->next;
        }
        else
        {
            prev_object = iterate_object;
            iterate_object = iterate_object->next;
        }
    }
}

void save_score(const char *name, int score)
{
    FILE *file;
    player_score players[100];
    int n = 0;

    file = fopen("hall.txt", "r");
    while (fscanf(file, "%20s %d", players[n].name, &players[n].score) == 2)
    {
        n++;
    }
    fclose(file);

    player_score new_player;
    strncpy(new_player.name, name, 3);
    new_player.name[3] = '\0';
    new_player.score = score;

    players[n++] = new_player;

    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (players[i].score < players[j].score)
            {
                player_score temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }

    // Salva os jogadores no arquivo
    file = fopen("hall.txt", "w");
    for (int i = 0; i < n; i++)
    {
        fprintf(file, "%s %d\n", players[i].name, players[i].score);
    }
    fclose(file);
}

void draw_game_information(int score, particle *bullets, int out_of_bullets)
{
    int starting_x = MAXX + 5;
    int starting_y = MINY + 1;

    // Exibir pontuação
    screenGotoxy(starting_x, starting_y);
    printf("SCORE: %d", score);

    // Exibir status das balas
    int bullets_to_shoot = len_bullets(bullets);
    screenGotoxy(starting_x, starting_y + 2);
    printf("BALAS: ");

    if (bullets_to_shoot == 2 || out_of_bullets)
        printf(" ");
    else if (bullets_to_shoot == 1)
        printf("| ");
    else if (bullets_to_shoot == 0)
        printf("| |");

    // Exibir tempo de invulnerabilidade
    screenGotoxy(starting_x, starting_y + 4);
    printf("SHIELD: (%d)s", invulnerability_time);    

    // Exibir nível
    screenGotoxy(starting_x, starting_y + 6);
    if (level == 1)
    {
        printf("NÍVEL: Easy");
    }
    else if (level == 2)
    {
        printf("NÍVEL: Medium");
    }
    else if (level == 3)
    {
        printf("NÍVEL: Hard");
    }
    else if (level == 4)
    {
        printf("NÍVEL: Very Hard");
    }
    else if (level == 5)
    {
        printf("NÍVEL: Pro");
        screenGotoxy(starting_x + 5, starting_y + 8);
        printf("Boa sorte!");
    }

    // Exibir vidas
    screenGotoxy(starting_x, starting_y + 10);
    for (int i = 0; i < lives; i++)
    {
        printf("❤️");
    }
}


void move(player *ship)
{
    ship->x += PLAYER_VEL * ship->direction;
}

void add_collectables(collectable **coin_head, int x, int y, char *sprite)
{
    collectable *iterate_coin = *coin_head, *new_coin = (collectable *)malloc(sizeof(collectable));
    new_coin->pos.x = x;
    new_coin->pos.y = y;
    new_coin->sprite = (char *)malloc(sizeof(char) * strlen(sprite));
    strcpy(new_coin->sprite, sprite);
    new_coin->next = NULL;
    if (*coin_head == NULL)
    {
        *coin_head = new_coin;
    }
    else
    {
        while (iterate_coin->next != NULL)
        {
            iterate_coin = iterate_coin->next;
        }
        iterate_coin->next = new_coin;
    }
}

void draw_collectables(collectable *coin_head)
{
    collectable *iterate_coin = coin_head;

    while (iterate_coin != NULL)
    {
        screenGotoxy(iterate_coin->pos.x, iterate_coin->pos.y);
        screenSetColor(GREEN, BLACK);
        for (int i = 0; iterate_coin->sprite[i] != '\0'; i++)
        {
            printf("%c", iterate_coin->sprite[i]);
        }

        iterate_coin = iterate_coin->next;
    }
    screenSetColor(YELLOW, BLACK);
}

void move_collectables(collectable *coin_head, int vel)
{
    collectable *iterate_coin = coin_head;

    while (iterate_coin != NULL)
    {
        iterate_coin->pos.y += vel;
        iterate_coin = iterate_coin->next;
    }
}

void collision_collectables(collectable **coin_head, player ship)
{
    collectable *iterate_coin = *coin_head, *coin_temp = *coin_head;

    if (*coin_head != NULL)
    {
        for (int i = 0; iterate_coin->sprite[i] != '\0'; i++)
        {
            if (((ship.x <= iterate_coin->pos.x + i && ship.x + PLAYER_WIDTH - 1 >= iterate_coin->pos.x + i) &&
                 (ship.y <= iterate_coin->pos.y && ship.y + PLAYER_HEIGHT - 1 >= iterate_coin->pos.y)) ||
                iterate_coin->pos.y >= MAXY)
            {

                *coin_head = (*coin_head)->next;
                free(coin_temp);

                score += 50;
                break;
            }
        }

        int flag = 1;

        while (iterate_coin->next != NULL)
        {
            for (int i = 0; iterate_coin->next->sprite[i] != '\0'; i++)
            {
                if (((ship.x <= iterate_coin->next->pos.x + i && ship.x + PLAYER_WIDTH - 1 >= iterate_coin->next->pos.x + i) &&
                     (ship.y <= iterate_coin->next->pos.y && ship.y + PLAYER_HEIGHT - 1 >= iterate_coin->next->pos.y)) ||
                    iterate_coin->next->pos.y >= MAXY)
                {

                    coin_temp = iterate_coin->next;
                    iterate_coin->next = iterate_coin->next->next;
                    free(coin_temp);

                    flag = 0;
                    score += 50;
                    break;
                }
            }
            iterate_coin = flag ? iterate_coin->next : iterate_coin;
            flag = 1;
        }
    }
}

void handle_shield_collisions(shield **shields_head, player *ship)
{
    shield *cur_shield = *shields_head, *prev_shield = NULL;

    while (cur_shield != NULL)
    {
        int collided = 0;

        for (int i = 0; cur_shield->sprite[i] != '\0'; i++)
        {
            if (ship->x <= cur_shield->pos.x + i && ship->x + PLAYER_WIDTH - 1 >= cur_shield->pos.x + i &&
                ship->y <= cur_shield->pos.y && ship->y + PLAYER_HEIGHT - 1 >= cur_shield->pos.y)
            {
                collided = 1;
                break;
            }
        }

        if (collided)
        {
            score += 50;
            invulnerability_time += 6;

            if (cur_shield == *shields_head)
            {
                *shields_head = cur_shield->next;
            }
            else
            {
                prev_shield->next = cur_shield->next;
            }

            shield *temp = cur_shield;
            cur_shield = cur_shield->next;

            free(temp->sprite);
            free(temp);
        }
        else if (cur_shield->pos.y >= MAXY)
        {
            if (cur_shield == *shields_head)
            {
                *shields_head = cur_shield->next;
            }
            else
            {
                prev_shield->next = cur_shield->next;
            }

            shield *temp = cur_shield;
            cur_shield = cur_shield->next;

            free(temp->sprite);
            free(temp);
        }
        else
        {
            prev_shield = cur_shield;
            cur_shield = cur_shield->next;
        }
    }
}


int main()
{
    screenInit(1);
    keyboardInit();
    char name[4] = {0};

    while (TRUE)
    {
        start_screen();
        srand(time(0));
        lives = 3;
        player ship = {47, 18, 5, '>', NULL};
        particle *ship_bullets = NULL;
        object *enemy = NULL;
        collectable *coins = NULL;
        shield *shield_power_ups = NULL;
        clock_t spawn_clock = clock(), move_clock = clock(), score_clock = clock(), bullet_clock = clock(), spawn_coin_clock = clock(), coin_clock = clock(), cooldown_clock = clock();
        clock_t shield_clock = clock(), spawn_shield_clock = clock(), invulnerability_clock = clock();
        int out_of_bullets = 0;

        while (TRUE)
        {
            if (score >= EASY_SCORE && score < MEDIUM_SCORE)
            {
                level = 2;
            }
            else if (score >= MEDIUM_SCORE && score < HARD_SCORE)
            {
                level = 3;
            }
            else if (score >= HARD_SCORE && score < PRO_DIFFICULTY)
            {
                level = 4;
            }
            else if (score >= PRO_DIFFICULTY)
            {
                level = 5;
            }

            out_of_bullets = out_of_bullets ? !delay_to_action(0.01, &cooldown_clock) : out_of_bullets;

            if (keyhit())
            {
                switch (readch())
                {
                case 'a':
                    if (ship.x > MINX + 1)
                    {
                        ship.direction = -1;
                        move(&ship);
                    }
                    break;
                case 'd':
                    if (ship.x < (MAXX - PLAYER_WIDTH))
                    {
                        ship.direction = 1;
                        move(&ship);
                    }
                    break;
                case ' ':
                    if (!out_of_bullets)
                    {
                        if (len_bullets(ship_bullets) < 2 && delay_to_action(0.001, &bullet_clock))
                        {
                            add_bullet(&ship_bullets, ship.x + 1, ship.y - 1, '|');
                        }
                        out_of_bullets = len_bullets(ship_bullets) == 2 ? 1 : 0;
                    }
                    break;
                default:
                    break;
                }
            }

            system("clear");
            draw_border();

            if (delay_to_action(0.01, &score_clock))
            {
                score += 5;
            }

            if (delay_to_action(0.003, &move_clock))
            {
                move_object(&enemy, ship.y);

                if (level == 5)
                {
                    if (delay_to_action(0.0088, &spawn_clock))
                    {
                        spawn_enemy(&enemy);
                    }
                }
                if (level == 4)
                {
                    if (delay_to_action(0.010, &spawn_clock))
                    {
                        spawn_enemy(&enemy);
                    }
                }
                else
                {
                    if (delay_to_action(0.015, &spawn_clock))
                    {
                        spawn_enemy(&enemy);
                    }
                }
            }

            if (delay_to_action(0.003, &coin_clock))
            {
                move_collectables(coins, COIN_VEL);
                if (delay_to_action(0.03, &spawn_coin_clock))
                {
                    spawn_collectables(&coins);
                }
            }

            if (delay_to_action(0.003, &shield_clock)) {
                move_shield_power_ups(shield_power_ups, SHIELD_VEL);
                if (delay_to_action(0.15, &spawn_shield_clock)) {
                    spawn_shields(&shield_power_ups);
                }
            }

            
            if (invulnerability_time > 0 && delay_to_action(0.01, &invulnerability_clock))
            {
                invulnerability_time -= 1;
            }

            

            draw_shield_power_ups(shield_power_ups);
            handle_shield_collisions(&shield_power_ups, &ship);

            move_bullets(ship_bullets);
            remove_bullets(&ship_bullets);
            draw_object(enemy);
            draw_bullets(ship_bullets);
            draw_collectables(coins);
            draw_game_information(score, ship_bullets, out_of_bullets);
            draw_game_information_borders();

            handle_collision_object_bullet(&enemy, &ship_bullets);
            collision_collectables(&coins, ship);
            if (check_collision(ship, &enemy) == 0)
            {
                if (invulnerability_time > 0)
                {
                    invulnerability_time = 0; // Invulnerabilidade desativada
                    continue; // Evita perder vida
                }

                lives--;
                draw_game_information(score, ship_bullets, out_of_bullets);

                if (lives > 0)
                {
                    continue;
                }
                else
                {
                    sleep(1);
                    game_over_screen(score, name);
                    save_score(name, score);
                    score = 0;
                    level = 1;
                    break;
                }
            }

            drawPlayer(player_sprite, ship);

            screenUpdate();
            usleep(33333);
        }
    }

    screenHomeCursor();
    usleep(100000);
    keyboardDestroy();
    screenDestroy();

    return 0;
}
