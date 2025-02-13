#include "screen.h"
#include <stdlib.h>
#include "keyboard.h"
#include <string.h>

int start_screen_option = 1;

void draw_title()
{
    int title_y = MINY + 2;
    screenSetColor(YELLOW, BLACK);
    screenGotoxy((MAXX + MINX) / 2 - 7, title_y);
    printf("================");
    screenGotoxy((MAXX + MINX) / 2 - 4, title_y + 1);
    printf("Void Runner");
    screenGotoxy((MAXX + MINX) / 2 - 7, title_y + 2);
    printf("================");
}

void draw_menu_options()
{
    int first_option_y = MINY + 6;
    int options_margin_x = (MAXX + MINX) / 2 - 8;

    screenGotoxy(options_margin_x, first_option_y);
    printf("%s [ JOGAR ]", start_screen_option == 1 ? "->" : "  ");
    screenGotoxy(options_margin_x, first_option_y + 2);
    printf("%s [ INSTRUÇÕES ]", start_screen_option == 2 ? "->" : "  ");
    screenGotoxy(options_margin_x, first_option_y + 4);
    printf("%s [ SCORES ]", start_screen_option == 3 ? "->" : "  ");
    screenGotoxy(options_margin_x, first_option_y + 6);
    printf("%s [ CRÉDITOS ]", start_screen_option == 4 ? "->" : "  ");
    screenGotoxy(options_margin_x, first_option_y + 8);
    printf("%s [ SAIR ]", start_screen_option == 5 ? "->" : "  ");
}

void draw_border()
{
    char hbc = BOX_HLINE;
    char vbc = BOX_VLINE;

    screenClear();
    screenBoxEnable();

    screenGotoxy(MINX, MINY);
    printf("%c", BOX_UPLEFT);

    for (int i = MINX + 1; i < MAXX; i += 1)
    {
        screenGotoxy(i, MINY);
        printf("%c", hbc);
    }
    screenGotoxy(MAXX, MINY);
    printf("%c", BOX_UPRIGHT);

    for (int i = MINY + 1; i < MAXY; i += 1)
    {
        screenGotoxy(MINX, i);
        printf("%c", vbc);
        screenGotoxy(MAXX, i);
        printf("%c", vbc);
    }

    screenGotoxy(MINX, MAXY);
    printf("%c", BOX_DWNLEFT);
    for (int i = MINX + 1; i < MAXX; i += 1)
    {
        screenGotoxy(i, MAXY);
        printf("%c", hbc);
    }
    screenGotoxy(MAXX, MAXY);
    printf("%c", BOX_DWNRIGHT);

    screenBoxDisable();
}

void show_instructions()
{
    draw_border();
    screenSetColor(YELLOW, BLACK);

    screenGotoxy((MAXX + MINX) / 2 - 10, MINY + 2);
    printf("===== INSTRUÇÕES =====");

    int start_y = MINY + 5;
    int margin_x = (MAXX + MINX) / 2 - 10;

    screenGotoxy(margin_x, start_y);
    printf("| A | -> Mover Esq.");
    screenGotoxy(margin_x, start_y + 2);
    printf("| D | -> Mover Dir.");
    screenGotoxy(margin_x, start_y + 4);
    printf("| Espaço | -> Atirar");
    screenGotoxy(margin_x, start_y + 7);
    printf("Dica: Evite colisões!");
    screenGotoxy(margin_x, start_y + 11);
    printf("Pressione qualquer");
    screenGotoxy(margin_x, start_y + 12);
    printf("tecla para voltar...");
    screenUpdate();
    readch();
    screenClear();
}

void show_credits()
{
    draw_border();
    screenSetColor(YELLOW, BLACK);

    int start_y = MINY + 5;
    int margin_x = (MAXX + MINX) / 2 - 10;

    screenGotoxy((MAXX + MINX) / 2 - 10, MINY + 2);
    printf("====== CRÉDITOS ======");

    screenGotoxy(margin_x, start_y);
    printf("Desenvolvido por:");
    screenGotoxy(margin_x, start_y + 2);
    printf("- Felipe Andrade");
    screenGotoxy(margin_x, start_y + 3);
    printf("- Fernando Cavalcanti");
    screenGotoxy(margin_x, start_y + 4);
    printf("- Pedro Gusmão");
    screenGotoxy(margin_x, start_y + 8);
    printf("Obrigado por jogar!");
    screenGotoxy(margin_x, start_y + 12);
    printf("Pressione qualquer");
    screenGotoxy(margin_x, start_y + 13);
    printf("tecla para voltar...");
    screenUpdate();
    readch();
    screenClear();
}

void show_scores()
{
    draw_border();
    screenSetColor(YELLOW, BLACK);

    screenGotoxy((MAXX + MINX) / 2 - 10, MINY + 2);
    printf("======= SCORES =======");
    screenGotoxy((MAXX + MINX) / 2 - 2, MINY + 4);
    printf("Top 05");

    int start_y = MINY + 7;
    int margin_x = (MAXX + MINX) / 2 - 10;

    FILE *file = fopen("hall.txt", "r");
    if (file == NULL)
    {
        screenGotoxy(margin_x, start_y - 1);
        printf("Erro de armazenamento");
    }
    else
    {
        char name[4];
        int score;
        int count = 0;

        while (fscanf(file, "%3s %d", name, &score) == 2 && count < 5)
        {
            screenGotoxy(margin_x, start_y + count * 2);
            printf("%d. %s - %d", count + 1, name, score);
            count += 1;
        }

        fclose(file);

        if (count == 0)
        {
            screenGotoxy(margin_x + 2, start_y - 1);
            printf("Nenhum score salvo");
        }
    }

    screenGotoxy(margin_x, start_y + 12);
    printf("Pressione qualquer");
    screenGotoxy(margin_x, start_y + 13);
    printf("tecla para voltar...");
    screenUpdate();
    readch();
    screenClear();
}

void start_screen()
{
    int ch = 0;

    draw_border();
    draw_title();
    draw_menu_options();
    screenUpdate();

    while (1)
    {
        if (keyhit())
        {
            ch = readch();

            switch (ch)
            {
            case 'w':
                start_screen_option -= 1;
                if (start_screen_option < 1)
                    start_screen_option = 5;
                break;
            case 's':
                start_screen_option += 1;
                if (start_screen_option > 5)
                    start_screen_option = 1;
                break;
            case '\n':
                switch (start_screen_option)
                {
                case 1:
                    return;
                case 2:
                    show_instructions();
                    break;
                case 3:
                    show_scores();
                    break;
                case 4:
                    show_credits();
                    break;
                case 5:
                    screenDestroy();
                    exit(0);
                }
                break;
            default:
                break;
            }

            draw_border();
            draw_title();
            draw_menu_options();
            screenUpdate();
        }
    }
}

void draw_game_over_static(int score, int title_y, int margin_x)
{
    draw_border();
    screenSetColor(YELLOW, BLACK);

    screenGotoxy((MAXX + MINX) / 2 - 10, title_y);
    printf("====== GAME END ======");
    screenGotoxy(margin_x, title_y + 3);
    printf("SCORE FINAL: %d", score);

    screenGotoxy(margin_x, title_y + 6);
    printf("Selecione seu nome:");
    screenGotoxy(margin_x, title_y + 13);
    printf("Aperte ENTER em 'END'");
    screenGotoxy(margin_x, title_y + 14);
    printf("para continuar...");
}

void game_over_screen(int score, char *name)
{
    int ch;
    int game_over_option = 0;
    int title_y = MINY + 2;
    int margin_x = (MAXX + MINX) / 2 - 10;

    char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    int letter_index[3] = {0, 0, 0};

    draw_game_over_static(score, title_y, margin_x);
    screenUpdate();

    while (1)
    {
        screenGotoxy(margin_x, title_y + 8);
        for (int i = 0; i < 4; i += 1)
        {
            if (i < 3)
            {
                if (i == game_over_option)
                {
                    printf("[ %c ]", alphabet[letter_index[i]]);
                }
                else
                {
                    printf("  %c  ", alphabet[letter_index[i]]);
                }
            }
            else
            {
                if (i == game_over_option)
                {
                    printf("[ END ]");
                }
                else
                {
                    printf("  END  ");
                }
            }
        }

        fflush(stdout);

        if (keyhit())
        {
            ch = readch();

            switch (ch)
            {
            case 'w':
                if (game_over_option < 3)
                {
                    letter_index[game_over_option] = (letter_index[game_over_option] + 1) % 27;
                }
                break;
            case 's':
                if (game_over_option < 3)
                {
                    letter_index[game_over_option] = (letter_index[game_over_option] - 1 + 27) % 27;
                }
                break;

            case 'a':
                game_over_option = (game_over_option - 1 + 4) % 4;
                break;
            case 'd':
                game_over_option = (game_over_option + 1) % 4;
                break;
            case '\n':
                if (game_over_option == 3)
                {
                    for (int i = 0; i < 3; i += 1)
                    {
                        name[i] = alphabet[letter_index[i]];
                    }
                    name[3] = '\0';
                    return;
                }
                break;
            default:
                break;
            }
        }

        screenUpdate();
    }
}

void draw_game_information_borders()
{
    int starting_x = MAXX + 3;
    int starting_y = MINY;
    int width = 20;
    int height = 12;

    screenGotoxy(starting_x, starting_y);

    printf("+");
    for (int i = 0; i < width; i += 1)
    {
        printf("-");
    }
    printf("+");

    screenGotoxy(starting_x, starting_y + 1);
    for (int i = 0; i < height; i += 1)
    {
        printf("|");
        screenGotoxy(starting_x, starting_y + 1 + i);
    }

    screenGotoxy(starting_x, starting_y + height);
    printf("+");
    for (int i = 0; i < width; i += 1)
    {
        printf("-");
    }
    printf("+");

    screenGotoxy(starting_x + width + 1, starting_y + 1);
    for (int i = 0; i < height; i += 1)
    {
        printf("|");
        screenGotoxy(starting_x + width + 1, starting_y + 1 + i);
    }
}