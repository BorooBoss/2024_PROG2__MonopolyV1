//#include <ncurses/curses.h> //pre mingw
#include <curses.h>           //pre cygwin a linux
#include <string.h>
#include <stdlib.h>
#include <time.h>             //pre nahodny generator
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define WIDTH_CARD 18         //sirka jednej karty
#define HEIGHT_CARD 7        //vyska jednej karty
#define HEIGHT_LABEL_CARD 2   //vyska (z hora) predelovacej ciary na farbu
#define NUM_CARD_IN_ROW (NUM_SPACES/4) //pocet kariet v jednom riadku

// definovanie nazvov pre farebne objekty - musi zacinat od 1
#define COL_LABEL_BROWN 1
#define COL_LABEL_SKYBLUE 2
#define COL_LABEL_PURPLE 3
#define COL_LABEL_ORANGE 4
#define COL_LABEL_RED 5
#define COL_LABEL_YELLOW 6
#define COL_LABEL_GREEN 7
#define COL_LABEL_BLUE 8
#define COL_LABEL_NOCOLOR 9
#define COL_NOPLAYER 10
#define COL_PLAYER1 11
#define COL_PLAYER2 12
#define COL_PLAYER3 13
#define COL_PLAYER4 14

// definovanie nazvov pre farby
#define COL_BROWN 17
#define COL_SKYBLUE 18
#define COL_PURPLE 19
#define COL_ORANGE 20
#define COL_RED 21
#define COL_YELLOW 22
#define COL_GREEN 23
#define COL_BLUE 24
// Makra
#define MAX_NAME 100        // max. dlzka nazvu (bez znaku '\0')
#define NUM_PROPERTIES 16   // pocet nehnutelnosti na hracom plane
#define NUM_SPACES 24       // pocet poli na hracom plane
#define MAX_PLAYERS 4       // max. pocet hracov
#define NUM_SPACE_TYPES 6   // pocet typov poli na hracom plane
#define NUM_COLORS 8        // pocet farieb nehnutelnosti

// Definicie vlastnych datovych typov

// Typy poli na hracom plane
typedef enum _space_type {
    Property,       // nehnutelnost
    Start,          // startovacie pole
    Free_parking,   // specialne pole bez akcie
    In_jail,        // specialne pole, predstavuje vazenie
    Go_to_jail,     // specialne pole, ktore presunie hraca do vazenia
    Jail_pass       // specialne pole s priepustkou
} SPACE_TYPE;

// Farba nehnutelnosti
typedef enum _color {
    Brown,
    Skyblue,
    Purple,
    Orange,
    Red,
    Yellow,
    Green,
    Blue
} COLOR;

// Struktura pre nehnutelnost
typedef struct _property {
    char name[MAX_NAME + 1];    // nazov
    int price;                  // cena nehnutelnosti/vyska najomneho
    COLOR color;                // farba
} PROPERTY;

// Struktura pre pole na hracom plane
typedef struct _space {
    SPACE_TYPE type;        // typ pola na hracom plane
    PROPERTY *property;     // pointer na nehnutelnost ak je pole
    // typu Property, inak NULL
} SPACE;

// Struktura pre hraca
typedef struct _player {
    int number;             // cislo hraca (1, 2, 3 alebo 4)
    int space_number;       // cislo pola, na ktorom stoji (pole Start ma cislo 1)
    int cash;               // hotovost hraca
    int num_jail_pass;      // pocet nazbieranych priepustiek z vazenia
    int is_in_jail;         // info, ci je hrac vo vazeni: 0 - nie je; 1 - je
    PROPERTY *owned_properties[NUM_PROPERTIES]; // zoznam kupenych nehnutelnosti
    int num_properties;                         // pocet kupenych nehnutelnosti
} PLAYER;

// Deklaracie premennych
extern const char *space_types[NUM_SPACE_TYPES];    // nazvy typov poli na hracom plane
extern const char *property_colors[NUM_COLORS];     // nazvy farieb nehnutelnosti
extern PROPERTY properties[NUM_PROPERTIES];         // zoznam vsetkych nehnutelnosti
extern SPACE game_board[NUM_SPACES];                // hraci plan s 24 poliami

//#endif //Z4_MONOPOLY_H
//#include "monopoly.h"

// nazvy typov poli na hracom plane
const char *space_types[NUM_SPACE_TYPES] = {
        "PROPERTY",
        "START",
        "FREE PARKING",
        "IN JAIL",
        "GO TO JAIL",
        "JAIL PASS"
};

// nazvy farieb nehnutelnosti
const char *property_colors[] = {
        "Brown",
        "Skyblue",
        "Purple",
        "Orange",
        "Red",
        "Yellow",
        "Green",
        "Blue"
};

// zoznam nehnutelnosti na hracom plane
PROPERTY properties[NUM_PROPERTIES] = {
        {.name = "FOOD TRUCK", .price = 1, .color = Brown},
        {.name = "PIZZA", .price = 1, .color = Brown},
        {.name = "SCHOOL", .price = 1, .color = Skyblue},
        {.name = "ICE CREAM", .price = 1, .color = Skyblue},
        {.name = "MUSEUM", .price = 2, .color = Purple},
        {.name = "LIBRARY", .price = 2, .color = Purple},
        {.name = "THE PARK", .price = 2, .color = Orange},
        {.name = "THE BEACH", .price = 2, .color = Orange},
        {.name = "POST OFFICE", .price = 3, .color = Red},
        {.name = "BUS STATION", .price = 3, .color = Red},
        {.name = "GARDEN", .price = 3, .color = Yellow},
        {.name = "PET RESCUE", .price = 3, .color = Yellow},
        {.name = "AQUARIUM", .price = 4, .color = Green},
        {.name = "THE ZOO", .price = 4, .color = Green},
        {.name = "WATER PARK", .price = 5, .color = Blue},
        {.name = "HOSPITAL", .price = 5, .color = Blue}
};

// inicializacia hracieho pola
SPACE game_board[NUM_SPACES] = {
        {.type = Start, .property = NULL},
        {.type = Property, .property = &properties[0]},
        {.type = Property, .property = &properties[1]},
        {.type = Jail_pass, .property = NULL},
        {.type = Property, .property =  &properties[2]},
        {.type = Property, .property =  &properties[3]},
        {.type = In_jail, .property =  NULL},
        {.type = Property, .property =  &properties[4]},
        {.type = Property, .property =  &properties[5]},
        {.type = Jail_pass, .property =  NULL},
        {.type = Property, .property =  &properties[6]},
        {.type = Property, .property =  &properties[7]},
        {.type = Free_parking, .property =  NULL},
        {.type = Property, .property =  &properties[8]},
        {.type = Property, .property =  &properties[9]},
        {.type = Jail_pass, .property =  NULL},
        {.type = Property, .property =  &properties[10]},
        {.type = Property, .property =  &properties[11]},
        {.type = Go_to_jail, .property =  NULL},
        {.type = Property, .property =  &properties[12]},
        {.type = Property, .property =  &properties[13]},
        {.type = Jail_pass, .property =  NULL},
        {.type = Property, .property =  &properties[14]},
        {.type = Property, .property =  &properties[15]}
};

struct _win_st *create_property_card(int starty, int startx, SPACE game_space);
void clearCardArea(struct _win_st **win, int act_location, int player, int dice);
void drawGameBoard(struct _win_st **win, SPACE *gameboard, int maxy, int maxx);
void drawPlayer0(struct _win_st **win, int act_location, int new_location);
void drawPlayer1(struct _win_st **win, int act_location, int new_location);
void drawPlayer2(struct _win_st **win, int act_location, int new_location);
void drawPlayer3(struct _win_st **win, int act_location, int new_location);
void init_custom_color_pair();
int getCubeNumber(int lower, int upper);
int genNumDigits(int number);
int get_new_location(int actual_location, int dice);
bool owned_monopoly(const PLAYER *player, COLOR color);
bool is_in_jail(const PLAYER *player);
bool next_round(const PLAYER *player, int dice);
bool pass(const PLAYER *player);
bool move_to_jail(const PLAYER *player);
bool is_property(const PLAYER *player);
bool owned_property(PLAYER *all_players, PLAYER *current_player, int num_players, bool *game_ended);
void after_turn(PLAYER *all_players, PLAYER *current_player, int dice, bool *game_stat, int num_players);
void before_turn(PLAYER *all_players, PLAYER *current_player, int dice, bool *game_stat, int num_players);
PLAYER* init(int num_players);
int sum_property_prices(PLAYER *player);
void print_s(PLAYER *players, int num_players);
void print_p(PLAYER *players, int num_players);
void print_g(PLAYER *players, int num_players);

int roll_dice() {
    return (rand() % 6) + 1;
}

PLAYER* loser = NULL; //Global pointer na hraca, ktory prehra

int main(void) {
    system("gnome-terminal --geometry=180x60 -- z6.c");
    struct _win_st *win[NUM_SPACES];
    int maxy, maxx;
    int c = 0;
    int location = 1, new_location = 1;

    //srand(time(0));   //inicializacia nahodneho generatora cisel
    srand(0);   //inicializacia nahodneho generatora cisel = vzdy rovnake nahodne c.
    initscr();            // Start curses mode
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    resize_term(60, 200);
    getmaxyx(stdscr, maxy, maxx);   //vrati maximalny pocet riadkov a stlpcov konzoly
    if (has_colors() == FALSE)     //test, ci konzola podporuje farby
    {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }

    printw("\n");
    printw("                                   555       555 5555555555 555      .d5555b.   .d55555b.  555b     d555 5555555555      55555555555 .d55555b.\n");
    printw("                                   555   o   555 555        555     d55P  Y55b d55P\" \"Y55b 5555b   d5555 555                 555    d55P\" \"Y55b\n");
    printw("                                   555  d5b  555 555        555     555    555 555     555 55555b.d55555 555                 555    555     555\n");
    printw("                                   555 d555b 555 5555555    555     555        555     555 555Y55555P555 5555555             555    555     555\n");
    printw("                                   555d55555b555 555        555     555        555     555 555 Y555P 555 555                 555    555     555\n");
    printw("                                   55555P Y55555 555        555     555    555 555     555 555  Y5P  555 555                 555    555     555\n");
    printw("                                   5555P   Y5555 555        555     Y55b  d55P Y55b. .d55P 555   \"   555 555                 555    Y55b. .d55P\n");
    printw("                                   555P     Y555 5555555555 55555555 \"Y5555P\"   \"Y55555P\"  555       555 5555555555          555     \"Y55555P\"\n");
    printw("\n");
    printw("\n");
    printw("\n");
    printw("\n");
    printw("                                              555b     d555  .d55555b.  555b    555  .d55555b.  5555555b.   .d55555b.  555    Y55b   d55P\n");
    printw("                                              5555b   d5555 d55P\" \"Y55b 5555b   555 d55P\" \"Y55b 555   Y55b d55P\" \"Y55b 555     Y55b d55P\n");
    printw("                                              55555b.d55555 555     555 55555b  555 555     555 555    555 555     555 555      Y55o55P\n");
    printw("                                              555Y55555P555 555     555 555Y55b 555 555     555 555   d55P 555     555 555       Y555P\n");
    printw("                                              555 Y555P 555 555     555 555 Y55b555 555     555 5555555P\"  555     555 555        555\n");
    printw("                                              555  Y5P  555 555     555 555  Y55555 555     555 555        555     555 555        555\n");
    printw("                                              555   \"   555 Y55b. .d55P 555   Y5555 Y55b. .d55P 555        Y55b. .d55P 555        555\n");
    printw("                                              555       555  \"Y55555P\"  555    Y555  \"Y55555P\"  555         \"Y55555P\"  55555555   555\n");
    printw("\n\n\n\n");
    mvprintw(23,0,"Note: If the game resolution does not match yours, unzoom console and restart the program (Ctrl + C) in Linux\n");
    mvprintw(24,0,"GAME MODES:\n");
    mvprintw(25,0,"Press 1 for GAME MODE *TILL BANKRUPT* \n");
    mvprintw(26,0,"Press 2 for GAME MODE *TURNS NUMBER*\n");
    int menu_gameMode = 0;
    while (menu_gameMode != 1 || menu_gameMode != 2){
    menu_gameMode = getch() - '0'; // Convert the pressed key to integer
    if (menu_gameMode == 1) { //BANKRUPT MOD
        break;
    } else if (menu_gameMode == 2) { //TURNS MOD
        int numTurns;
        mvprintw(27, 0, "Enter the number of turns you want to play: ");
        refresh();
        scanw("%d", &numTurns);
        break;
    } else {
        mvprintw(27, 0, "Invalid input! Please press either 1 or 2.");
        refresh();
    }
     }

    mvprintw(23,0,"                                                                                                                 \n");
    mvprintw(24,0,"                                                                                                                 \n");
    mvprintw(25,0,"                                                                                                                 \n");
    mvprintw(26,0,"                                                                                                                 \n");
    mvprintw(24,0,"PLAYERS:");
    mvprintw(25,0,"Press 1 for PLAYER VS PLAYER");
    mvprintw(26,0,"Press 2 for PLAYER VS PLAYER VS PLAYER");
    mvprintw(27,0,"Press 3 for PLAYER VS PLAYER VS PLAYER VS PLAYER");
    mvprintw(28,0,"Press 4 for PLAYER VS COMPUTER");
    mvprintw(29,0,"Press 5 for PLAYER VS COMPUTER VS COMPUTER");
    mvprintw(30,0,"Press 6 for PLAYER VS COMPUTER VS COMPUTER VS COMPUTER");
    mvprintw(31,0,"Press 6 for PLAYER VS PLAYER VS COMPUTER\n");
    int menu_players = 0;
    int num_players = 0;
    int bots = 0;
    while (menu_gameMode != 1 || menu_gameMode != 2 || menu_gameMode != 3 || menu_gameMode != 4 || menu_gameMode != 5 || menu_gameMode != 6 ){
        menu_players = getch() - '0'; // Convert the pressed key to integer
        if (menu_players == 1) { //PLAYER VS PLAYER
            num_players = 2;
            bots = 0;
            break;
        } else if (menu_players == 2) { //PLAYER VS PLAYER VS PLAYER
            num_players = 3;
            bots = 0;
            break;
        } else if (menu_players == 3) { //PLAYER VS PLAYER VS PLAYER VS PLAYER
            num_players = 4;
            bots = 0;
            break;
        } else if (menu_players == 4) { //PLAYER VS COMPUTER
            num_players = 2;
            bots = 1;
            break;
        } else if (menu_players == 5) { //PLAYER VS COMPUTER VS COMPUTER
            num_players = 3;
            bots = 2;
            break;
        } else if (menu_players == 6) { //PLAYER VS PLAYER VS COMPUTER
            num_players = 3;
            bots = 1;
            break;
        } else {
            mvprintw(32, 0, "Invalid input! Please press either 1,2,3,4,5 or 6.");
            refresh();
        }
    }


    clear();
    refresh();
    PLAYER *players = init(num_players);
    int turns = 1;

    print_g(players, num_players); //prvy vypis


    //definovanie vlastnych farieb a farebnych parov (text, pozadie)
    init_custom_color_pair();
    //mvprintw(0, 0, "Number of supported colors: %d", COLORS);
    mvprintw(0, 0, "hod kockou: 'c'  Quit: 'Esc'");
    refresh();

    //nakreslenie hracieho planu monopolov - pre 24 policok
    drawGameBoard(win, game_board, maxy, maxx);
    //umiestnenie hraca na START
    switch (num_players) {
        case 2:
            drawPlayer0(win, location, new_location);
            drawPlayer1(win, location, new_location);
            break;
        case 3:
            drawPlayer0(win, location, new_location);
            drawPlayer1(win, location, new_location);
            drawPlayer2(win, location, new_location);
            break;
        case 4:
            drawPlayer0(win, location, new_location);
            drawPlayer1(win, location, new_location);
            drawPlayer2(win, location, new_location);
            drawPlayer3(win, location, new_location);
            break;
        default:
            // Handle unexpected case
            break;
    }

    int dice = 0;

    int older_location = 0;
    //hra moze zacat...
    //int debug = 0;
    bool game_ended = false;
    int player_locations[4] = {1,1,1,1};
    while (game_ended != true) {

        for (int i = 0; i < num_players; i++) { //kazdy hrac hodi kockou
            
            c = getch();
            print_g(players, num_players);
            if (c == 'c') {
                //dice = getCubeNumber(1, 6);
                dice = roll_dice();
                if (dice == 61){
                    dice = 5;
                }
                else if (dice == 51){
                    dice = 6;
                }
                //dice = 1;
                //dice = 3;// nahodne cislo od 1 - 6
                older_location = player_locations[i];
                //new_location = get_new_location(player_locations[i], dice);
                new_location = player_locations[i] + dice;
                //vypocet novej polohy hraca (osetrenie prejdenia cez START)
                player_locations[i] = new_location; // Update the player's location
                before_turn(players, &players[i], dice, &game_ended, num_players);
                if (new_location >= 24){
                    new_location = new_location-24 + 1;
                }
                switch (i) {
                    case 0:
                        //clearCardArea(win, players[0].space_number, i, dice);
                        clearCardArea(win, player_locations[0], 0, dice+1);
                        drawPlayer0(win, player_locations[0], new_location);
                        //mvprintw(1,1,"%d", player_locations[i]);

//prekreslenie polohy hraca
                        break;
                    case 1:
                        clearCardArea(win, player_locations[1], 1, dice+1);
                        drawPlayer1(win, player_locations[1], new_location);
                        //mvprintw(1,1,"%d", player_locations[i]);
                        break;
                    case 2:
                        clearCardArea(win, player_locations[2], 2, dice+1);
                        drawPlayer2(win, player_locations[2], new_location);
                        //mvprintw(1,1,"%d", player_locations[i]);
                        break;
                    case 3:
                        clearCardArea(win, player_locations[3], 3, dice+1);
                        drawPlayer3(win, player_locations[3], new_location);
                        //mvprintw(1,1,"%d %d %d %d", older_location, new_location, player_locations[i], dice);
                        break;
                    default:
                        // Handle unexpected case
                        break;
                }
                refresh();

                    if (i == 0){
                        if (new_location == 19) { //ak skocime na policko go to jail
                            if (players[0].num_jail_pass > 0) { //ak mame priespustky odratame jednu
                                players[0].num_jail_pass -= 1;
                            } else { //ak nie tak sa presunme do vazenia
                                clearCardArea(win, 19, 0, 1);
                                //clearCardArea(win, player_locations[0], i, dice);
                                players[0].space_number -= 12;
                                players[0].is_in_jail = 1; //nastavim mu ze je vo vazeni
                                drawPlayer0(win, player_locations[0], player_locations[0]-12);
                                player_locations[0] -= 12;
                                new_location -= 12;
                                refresh();


                            }
                        }
                    }
                    else if (i == 1){
                        if (new_location == 19) { //ak skocime na policko go to jail
                            if (players[1].num_jail_pass > 0) { //ak mame priespustky odratame jednu
                                players[1].num_jail_pass -= 1;
                            } else { //ak nie tak sa presunme do vazenia
                                clearCardArea(win, 19, 1, 1);
                                //clearCardArea(win, player_locations[1], i, dice);
                                players[1].space_number -= 12;
                                players[1].is_in_jail = 1; //nastavim mu ze je vo vazeni
                                drawPlayer1(win, player_locations[1], player_locations[1]-12);
                                player_locations[1] -= 12;
                                new_location -= 12;
                                refresh();
                            }
                        }
                    }
                    else if (i == 2){
                        if (new_location == 19) { //ak skocime na policko go to jail
                            if (players[2].num_jail_pass > 0) { //ak mame priespustky odratame jednu
                                players[2].num_jail_pass -= 1;
                            } else { //ak nie tak sa presunme do vazenia
                                clearCardArea(win, 19, 2, 1);
                                //clearCardArea(win, player_locations[2], i, dice);
                                players[2].space_number -= 12;
                                players[2].is_in_jail = 1; //nastavim mu ze je vo vazeni
                                drawPlayer2(win, player_locations[2], player_locations[2]-12);
                                player_locations[2] -= 12;
                                new_location -= 12;
                                refresh();
                            }
                        }
                    }
                    else if (i == 3){
                        if (new_location == 19) { //ak skocime na policko go to jail
                            if (players[3].num_jail_pass > 0) { //ak mame priespustky odratame jednu
                                players[3].num_jail_pass -= 1;
                            } else { //ak nie tak sa presunme do vazenia
                                clearCardArea(win, 19, 3, 1);
                                //clearCardArea(win, player_locations[3], i, dice);
                                players[3].space_number -= 12;
                                players[3].is_in_jail = 1; //nastavim mu ze je vo vazeni
                                drawPlayer3(win, player_locations[3], player_locations[3]-12);
                                player_locations[3] -= 12;
                                new_location -= 12;
                                refresh();
                            }
                        }
                    }



            }
                // koniec hry
            else if (c == 27) {
                mvprintw(maxy, 0, "exiting...");
                refresh();
                //napms(500);
                break;
            }
            //before_turn(players, &players[i], dice, &game_ended,
                        //num_players, player_locations[0],win); //po hode kockou zavolaj funkcia co skontroluje nejkae veci, pohni sa a zavolaj funkciu co skontroluje dalsie veci
            //sum_property_prices(&players[i]);
            if (game_ended) { // ked hra skoncila
                if ((num_players == 2) && loser != NULL) { //ak su dvaja hraci
                    mvprintw(28,150,"R: %d\nTurn: %d\nPlayer on turn: P%d\n", dice, turns, i + 1);
                    mvprintw(29,150,"\n");
                    print_g(players, num_players); //posledny print
                    refresh();
                    if (loser->number == players[0].number) { //ak je prvy loser, druhy je winner
                        printf("WINNER: P2\n");
                        printf("\n");
                    } else if (loser->number == players[1].number) { //ak je druhy loser, prvy je winner
                        printf("WINNER: P1\n");
                        printf("\n");
                    }
                } else { //ak je hracov viac, treba porvnavat peniaze
                    PLAYER winner = players[0]; //vyherca (zatial prvy hrac)
                    int tie = 1; //kontroluje remizu
                    int winner_properties_sum = sum_property_prices(&players[0]); //sucet pozemkov vyhercu
                    for (int j = 1; j < num_players; j++) { //kazdeho hraca (okrem 1. hraca porovnaj s winnerom (1.hrac)
                        if (players[j].number == loser->number) { //ak to je loser ta preskoc
                            continue;
                        }
                        if (winner.cash <
                            players[j].cash) { //ak ma porovnavany hrac viac penazi ako winner, nastav ho na winnera noveho
                            winner = players[j];
                            winner_properties_sum = sum_property_prices(
                                    &players[j]); //vyrataj jaku cenu maju pozemky jeho
                            tie = 1;
                        } else if (winner.cash == players[j].cash) { //ak sa peniaze rovnaju
                            if (winner_properties_sum < sum_property_prices(
                                    &players[j])) { //ak ma porovnavany hrac viac penazi v pozemkoch ako winner
                                winner = players[j]; //nastav noveho winnera
                                winner_properties_sum = sum_property_prices(&players[j]);
                                tie = 1;
                            }
                            if (winner.number !=
                                players[j].number) { //ak sa pozemky rovnaju a neporovnavame toho isteho hraca so samym sebou, ta remiza
                                if (winner_properties_sum == sum_property_prices(&players[j])) {
                                    tie++;
                                }
                            }
                        }
                    }
                    print_g(players, num_players); //posledny print
                    if (tie > 1) {
                        printf("WINNER: ?\n");
                        printf("\n");
                    } else {
                        printf("WINNER: P%d\n", winner.number);
                        printf("\n");
                    }
                }
                free(players);
                return 0;
            }

            //WINDOW *win2 = newwin(11, 21, 18, 40);
            WINDOW *win2 = newwin(11, 25, 30, 67);
            wbkgd(win2, COLOR_PAIR(6));
            wrefresh(win2);
            wattron(win2, COLOR_PAIR(0)); // Use color pair 0 (black) for the border
            box(win2, 0, 0); // Draw the box
            wattroff(win2, COLOR_PAIR(0));

            wrefresh(win2);
            int y_center = getmaxy(win2) / 2;
            int x_center = getmaxx(win2) / 2;
            wattron(win2, COLOR_PAIR(6)); // Use color pair 1 for white text on black background
            mvwprintw(win2, y_center, x_center, "%d", dice);     // Print the number 1 at the center
            wattroff(win2, COLOR_PAIR(6)); // Unset the color pair
            wrefresh(win2);


            mvprintw(50,150,"R: %d\n", dice);
            mvprintw(51,150,"Turn: %d\n", turns);
            mvprintw(52,150,"Player on turn: P%d\n", i + 1);
            //mvprintw(26,100,"\n");
            refresh();
            turns++;
            dice = 0;
            if (player_locations[i] >= 24){ //skurvena picovina fakt ked som ti pri dacom vytrhal vsetky vlasy tak pri tomto
                 player_locations[i] = new_location;
            }

        }
    }
    endwin();
}

void clearCardArea(struct _win_st **win, int act_location, int player, int dice) { //premazeme staru poziciu hraca
    int old_location = (act_location - dice);
    if (player == 0){
        wattron(win[old_location], COLOR_PAIR(COL_NOPLAYER));
        mvwprintw(win[old_location], HEIGHT_CARD - 2, 2, "  ");
        wattroff(win[old_location], COLOR_PAIR(COL_NOPLAYER));

        wrefresh(win[old_location]);

    }
    else if (player == 1){
        wattron(win[old_location], COLOR_PAIR(COL_NOPLAYER));
        mvwprintw(win[old_location], HEIGHT_CARD - 2, 6, "  ");
        wattroff(win[old_location], COLOR_PAIR(COL_NOPLAYER));

        wrefresh(win[old_location]);

    }
    else if (player == 2){
        wattron(win[old_location], COLOR_PAIR(COL_NOPLAYER));
        mvwprintw(win[old_location], HEIGHT_CARD - 2, 10, "  ");
        wattroff(win[old_location], COLOR_PAIR(COL_NOPLAYER));

        wrefresh(win[old_location]);

    }
    else if (player == 3){
        wattron(win[old_location], COLOR_PAIR(COL_NOPLAYER));
        mvwprintw(win[old_location], HEIGHT_CARD - 2, 14, "  ");
        wattroff(win[old_location], COLOR_PAIR(COL_NOPLAYER));

        wrefresh(win[old_location]);

    }
}

    void drawPlayer0(struct _win_st **win, int act_location, int new_location) { //vykreslenie hraca

/*
        wattron(win[act_location], COLOR_PAIR(COL_NOPLAYER));
        mvwprintw(win[act_location], HEIGHT_CARD - 2, 2, "  ");
        wattroff(win[act_location], COLOR_PAIR(COL_NOPLAYER));
*/
        wattron(win[new_location-1], COLOR_PAIR(COL_PLAYER1));
        mvwprintw(win[new_location-1], HEIGHT_CARD - 2, 2, ":)");
        wattroff(win[new_location-1], COLOR_PAIR(COL_PLAYER1));

        wrefresh(win[new_location-1]);
    }
void drawPlayer1(struct _win_st **win, int act_location, int new_location) {


    //wattron(win[act_location-1], COLOR_PAIR(COL_NOPLAYER));
    //mvwprintw(win[act_location-1], HEIGHT_CARD - 2, 6, "  ");
    //wattroff(win[act_location-1], COLOR_PAIR(COL_NOPLAYER));

    wattron(win[new_location-1], COLOR_PAIR(COL_PLAYER2));
    mvwprintw(win[new_location-1], HEIGHT_CARD - 2, 6, ":)");
    wattroff(win[new_location-1], COLOR_PAIR(COL_PLAYER2));

    //wrefresh(win[act_location]);
    wrefresh(win[new_location-1]);
}
void drawPlayer2(struct _win_st **win, int act_location, int new_location) {

    //wattron(win[act_location], COLOR_PAIR(COL_NOPLAYER));
    //mvwprintw(win[act_location], HEIGHT_CARD - 2, 10, "  ");
    //wattroff(win[act_location], COLOR_PAIR(COL_NOPLAYER));

    wattron(win[new_location-1], COLOR_PAIR(COL_PLAYER3));
    mvwprintw(win[new_location-1], HEIGHT_CARD - 2, 10, ":)");
    wattroff(win[new_location-1], COLOR_PAIR(COL_PLAYER3));

    //wrefresh(win[act_location]);
    wrefresh(win[new_location-1]);
}
void drawPlayer3(struct _win_st **win, int act_location, int new_location) {

    //wattron(win[act_location], COLOR_PAIR(COL_NOPLAYER));
    //mvwprintw(win[act_location], HEIGHT_CARD - 2, 14, "  ");
    //wattroff(win[act_location], COLOR_PAIR(COL_NOPLAYER));

    wattron(win[new_location-1], COLOR_PAIR(COL_PLAYER4));
    mvwprintw(win[new_location-1], HEIGHT_CARD - 2, 14, ":)");
    wattroff(win[new_location-1], COLOR_PAIR(COL_PLAYER4));

    //wrefresh(win[act_location]);
    wrefresh(win[new_location-1]);
}
// vykreslenie policka MONOPOLU cez strukturu WINDOW
    struct _win_st *create_property_card(int starty, int startx, SPACE game_space) {

        struct _win_st *local_win;

        local_win = newwin(HEIGHT_CARD, WIDTH_CARD, starty, startx - 57); //OKNO SOM POSUNUL NA KRAJ
    WINDOW *win2 = newwin(25, 25, 20, 65);
        //vonkajsie ohranicenie
        box(local_win, ACS_VLINE, ACS_HLINE);

        //label oddelovac
        mvwhline(local_win, HEIGHT_LABEL_CARD, 1, ACS_HLINE, WIDTH_CARD - 1);
        mvwaddch(local_win, HEIGHT_LABEL_CARD, 0, ACS_LTEE);
        mvwaddch(local_win, HEIGHT_LABEL_CARD, WIDTH_CARD - 1, ACS_RTEE);

        //zafarbenie label-u
        if (game_space.type == Property) {
            wattron(local_win, COLOR_PAIR(game_space.property->color + 1));
            for (int y = 1; y < HEIGHT_LABEL_CARD; y++) {
                for (int x = 1; x < WIDTH_CARD - 1; x++) {
                    mvwprintw(local_win, y, x, " ");
                }
            }
        } else {
            wattron(local_win, COLOR_PAIR(COL_LABEL_NOCOLOR));
            for (int y = 1; y < HEIGHT_LABEL_CARD; y++) {
                for (int x = 1; x < WIDTH_CARD - 1; x++) {
                    mvwprintw(local_win, y, x, " ");
                }
            }
        }

        //nastavenie textu: farba
        if (game_space.type == Property) {
            mvwprintw(local_win, (HEIGHT_LABEL_CARD / 2),
                      (WIDTH_CARD - strlen(property_colors[game_space.property->color])) / 2, "%s",
                      property_colors[game_space.property->color]);

            //nastavenie textu: nazov, cena
            wattroff(local_win, COLOR_PAIR(game_space.property->color + 1));
            mvwprintw(local_win, (HEIGHT_LABEL_CARD + 1), (WIDTH_CARD - strlen(game_space.property->name)) / 2, "%s",
                      game_space.property->name);
            mvwprintw(local_win, (HEIGHT_LABEL_CARD + 2), (WIDTH_CARD - genNumDigits(game_space.property->price)) / 2,
                      "%d",
                      game_space.property->price);
        } else {
            mvwprintw(local_win, (HEIGHT_LABEL_CARD / 2),
                      (WIDTH_CARD - strlen(space_types[game_space.type])) / 2, "%s",
                      space_types[game_space.type]);
            wattroff(local_win, COLOR_PAIR(COL_LABEL_NOCOLOR));
        }
        wrefresh(local_win);        /* Show that box 		*/

        return local_win;
    }
    void drawGameBoard(struct _win_st **win, SPACE *gameboard, int maxy, int maxx) {
        char cost[3] = {'\0'};
        int n = 0;
        for (int i = 0; i < NUM_SPACES; i++) {
            sprintf(cost, "%d", i);
            // juh 0 - 5
            if (i >= 0 && i < NUM_CARD_IN_ROW) {
                win[i] = create_property_card(maxy - HEIGHT_CARD, maxx - WIDTH_CARD * (n + 1), gameboard[i]);
                n++;
                if (n >= NUM_CARD_IN_ROW) {
                    n = 0;
                }
            }
            // zapad 6 - 11
            if (i >= 1 * NUM_CARD_IN_ROW && i < 2 * NUM_CARD_IN_ROW) {
                //win[i] = create_property_card(maxy-HEIGHT_CARD*((i-NUM_CARD_IN_ROW+1)), maxx-WIDTH_CARD*(NUM_CARD_IN_ROW+1),"Cirkus", "red", cost );
                win[i] = create_property_card(maxy - HEIGHT_CARD * (n + 1), maxx - WIDTH_CARD * (NUM_CARD_IN_ROW + 1),
                                              gameboard[i]);
                n++;
                if (n >= NUM_CARD_IN_ROW) {
                    n = NUM_CARD_IN_ROW;
                }
            }

            // sever 12 - 17
            if (i >= 2 * NUM_CARD_IN_ROW && i < 3 * NUM_CARD_IN_ROW) {
                //win[i] = create_property_card(maxy-HEIGHT_CARD*(NUM_CARD_IN_ROW+1), maxx-WIDTH_CARD*( NUM_CARD_IN_ROW*(NUM_CARD_IN_ROW-3)-i+1),"Cirkus", "red", cost );
                win[i] = create_property_card(maxy - HEIGHT_CARD * (NUM_CARD_IN_ROW + 1), maxx - WIDTH_CARD * (n + 1),
                                              gameboard[i]);
                n--;
                if (n <= 0) {
                    n = NUM_CARD_IN_ROW;
                }
            }
            // vychod 18 - 23
            if (i >= 3 * NUM_CARD_IN_ROW && i < 4 * NUM_CARD_IN_ROW) {
                //win[i] = create_property_card(maxy-HEIGHT_CARD*(NUM_CARD_IN_ROW*(NUM_CARD_IN_ROW-2)-i+1), maxx-WIDTH_CARD,"Cirkus", "red", cost );
                win[i] = create_property_card(maxy - HEIGHT_CARD * (n + 1), maxx - WIDTH_CARD, gameboard[i]);
                n--;
                if (n <= 0) {
                    n = NUM_CARD_IN_ROW;
                }
            }
        }
    }
    void init_custom_color_pair() {
        int r = 4; //po spravnosti by malo byt r=1000/255 => 3.92
        start_color();
        //inicializacia vlastnych farebnych parov (text,pozadie)
        init_color(COL_BROWN, 139 * r, 69 * r, 19 * r);
        init_color(COL_SKYBLUE, 65 * r, 105 * r, 225 * r);
        init_color(COL_PURPLE, 128 * r, 128 * r, 0 * r);
        init_color(COL_ORANGE, 250 * r, 128 * r, 0 * r);
        init_color(COL_RED, 249 * r, 0 * r, 0 * r);
        init_color(COL_YELLOW, 249 * r, 249 * r, 0 * r);
        init_color(COL_GREEN, 0 * r, 100 * r, 0 * r);
        init_color(COL_BLUE, 0 * r, 0 * r, 249 * r);
        init_pair(COL_LABEL_BROWN, COLOR_WHITE, COL_BROWN);
        init_pair(COL_LABEL_SKYBLUE, COLOR_WHITE, COL_SKYBLUE);
        init_pair(COL_LABEL_PURPLE, COLOR_WHITE, COL_PURPLE);
        init_pair(COL_LABEL_ORANGE, COLOR_WHITE, COL_ORANGE);
        init_pair(COL_LABEL_RED, COLOR_WHITE, COL_RED);
        init_pair(COL_LABEL_YELLOW, COLOR_BLACK, COL_YELLOW);
        init_pair(COL_LABEL_GREEN, COLOR_WHITE, COL_GREEN);
        init_pair(COL_LABEL_BLUE, COLOR_WHITE, COL_BLUE);
        // iniicalizacia zakladneho farebneho paru: B&W
        init_pair(COL_LABEL_NOCOLOR, COLOR_WHITE, COLOR_BLACK);
        init_pair(COL_PLAYER1, COLOR_WHITE, COLOR_MAGENTA);
        init_pair(COL_PLAYER2, COLOR_WHITE, COLOR_BLUE);
        init_pair(COL_PLAYER3, COLOR_WHITE, COLOR_RED);
        init_pair(COL_PLAYER4, COLOR_WHITE, COLOR_GREEN);
        init_pair(COL_NOPLAYER, COLOR_BLACK, COLOR_BLACK);
    }
    int get_new_location(int actual_location, int dice) {
        if (actual_location + dice >= NUM_SPACES) {
            return actual_location + dice - NUM_SPACES;
        }
        return actual_location + dice;
    }
    int genNumDigits(int number) {
        int count = 0;

        while (number != 0) {
            number = number / 10;
            count++;
        }
        return count;
    }
    int getCubeNumber(int lower, int upper) {
        int num = (rand() % (upper - lower + 1)) + lower;
        return num;
    }

    bool owned_monopoly(const PLAYER *player, COLOR color) { //funkcia, ktora zistuje, ci policko na ktorom stoji hrac je monopoly
        int properties_of_color_owned = 0;
        for (int i = 0; i < player->num_properties; i++) { //prechadzame vsetky pozemky hraca a porovnavame farbu
            if (player->owned_properties[i]->color == color) {
                properties_of_color_owned++;
            }
        }

        if (properties_of_color_owned == 2) { //ak ma hrac dva pozemky s danou farbou, je to monopoly
            return true;
        } else {
            return false;
        }
    }
    bool is_in_jail(const PLAYER *player) { //funkcia, zisti ci stojime na policku in jail
        if (game_board[player->space_number - 1].type == In_jail) {
            return true;
        } else {
            return false;
        }
    }

    bool next_round(const PLAYER *player, int dice) { //funkcia, zisti, ci sa posuvame na dalsie kolo
        if (player->space_number - 1 + dice > NUM_SPACES - 1) {
            return true;
        } else {
            return false;
        }
    }
    bool pass(const PLAYER *player) { //funkcia, zisti ci stojime na policku in jail pass
        if (game_board[player->space_number - 1].type == Jail_pass) {
            return true;
        } else {
            return false;
        }

    }
    bool move_to_jail(const PLAYER *player) { //funkcia, zisti ci stojime na policku in go to jail
        if (game_board[player->space_number-1].type == Go_to_jail) {
            return true;
        } else {
            return false;
        }
    }
    bool is_property(const PLAYER *player) { //funkcia, zisti ci stojime na policku property
        if (game_board[player->space_number - 1].type == Property) {
            return true;
        } else {
            return false;
        }
    }
    bool owned_property(PLAYER *all_players, PLAYER *current_player, int num_players,
                        bool *game_ended) { //funkcia, zisti ci stojime na property, ktoru niekto vlastni
        SPACE *current_space = &game_board[current_player->space_number - 1]; //skratka
        PROPERTY *property = current_space->property; //skratka (nemusim robit dvojity pointer)

        for (int i = 0; i < num_players; i++) { //cyklus tolkokrat, kolko mame hracov
            for (int j = 0;
                 j < all_players[i].num_properties; j++) { //cyklus, prechadzam konkretneho hraca pocet pozemkov
                // Check if the current player owns the property
                if (all_players[i].owned_properties[j] == current_space->property) { //ak je majitel pozemku
                    if (&all_players[i] == current_player) { //vlastni to dany hrac
                        //printf("vlastnis to\n");
                    } else {
                        if (owned_monopoly(&all_players[i], property->color) == true) { //je to monopol
                            int price = property->price; //cena pozemku, sucet cien vyuzivam na zistenie ceny monopolu
                            if (current_player->cash >= property->price + price) {
                                current_player->cash -= property->price + price; //zaplat tomu co ma pozemok
                                all_players[i].cash += property->price + price; //prirataj majitelovi peniaze
                                //printf("%d -> platil som\n", i);
                            } else {
                                if ((current_player->cash - (property->price + price)) <
                                    0) { // ak ma hrac po odcitani menej ako 0 penazi
                                    loser = current_player;
                                    *game_ended = true;
                                }
                            }
                        } else { //nie je to monopol
                            if (current_player->cash >= property->price) { //ak ma hrac dost penazi na zaplatenie
                                current_player->cash -= property->price; //zaplat tomu co ma pozemok
                                all_players[i].cash += property->price; //prirataj peniaze tomu co ma pozemok
                                //printf("%d -> platil som\n", i);
                            } else {
                                if (current_player->cash - property->price < 0) { //ak nema dost
                                    loser = current_player;
                                    *game_ended = true;
                                }
                            }

                        }
                    }

                    return true;
                }

            }
        }
        return false;
    }
    void after_turn(PLAYER *all_players, PLAYER *current_player, int dice, bool *game_stat,
                    int num_players) { //funckia, co kontroluje veci po posunuti sa na policko
        if (pass(current_player) == true) { //ak skocime na policko jail pass, priratame pocet priepustiek
            current_player->num_jail_pass += 1;
        }
        if (is_property(current_player) == true) { //ak sa postavime na property
            SPACE *current_space = &game_board[current_player->space_number - 1];
            PROPERTY *property = current_space->property;
            if (owned_property(all_players, current_player, num_players, game_stat) ==
                false) { //ak nikto nevlastni danu property
                if (current_player->cash >= property->price) { //ak mame peniaze, kupime, zapiseme
                    current_player->cash -= property->price;
                    current_player->owned_properties[current_player->num_properties] = property;
                    current_player->num_properties++;
                } else {
                    if (current_player->cash - property->price < 0) { //ak nema dost penazi ta bankrot
                        loser = current_player;
                        *game_stat = true;
                    }
                }
            }
        }
        //printf("\n%d mince\n", current_player->cash);
    }

    void before_turn(PLAYER *all_players, PLAYER *current_player, int dice, bool *game_stat,
                     int num_players) { //funckia, co kontroluje pred pohnutim sa
        if (is_in_jail(current_player) == true) { //ak stojime na policku vazenie
            if (current_player->is_in_jail == 1) { //ak sme prisli do vazenia z go to jail policka
                if (current_player->cash >= 1) { //ak mas peniaze, zaplat, vymaz informaiu, o tom ze je hrac vo vazeni
                    current_player->cash -= 1;
                    current_player->is_in_jail = 0;
                } else {
                    if (current_player->cash - 1 < 0) { //ak nema peniaze bankrot
                        loser = current_player;
                        *game_stat = true;
                    }
                }

            }
            //printf("vo vazeni, treba dorobic ");

        }
        if (next_round(current_player, dice) == true) { //ak sme presli na nove kolo
            //printf("nove kolo");
            current_player->cash += 2; //prirataj peniaze
            current_player->space_number = (current_player->space_number + dice) - NUM_SPACES; //posun ho
        } else { //ak sme v starom kole, pohni sa normalne
            current_player->space_number += dice;
        }
        //printf("%d", current_player->space_number);
        after_turn(all_players, current_player, dice, game_stat,
                   num_players); // zavolam funkciu ktora kontroluje co sa deje po posunuti sa hraca
    }
    PLAYER *init(int num_players) { //uvodne nastavovacky
        int player_money; //peniaze
        // Allocate memory for an array of n players
        PLAYER *players = (PLAYER *) malloc(num_players * sizeof(PLAYER)); //pocet hracov urcim tak jak mi treba
        //PLAYER players[4];
        // Check if memory allocation was successful
        if (players == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            free(players);
            exit(EXIT_FAILURE);
        }
        if (num_players == 2) { //podmienky na priradenie penazi
            player_money = 20;
        } else if (num_players == 3)
            player_money = 18;
        else if (num_players == 4)
            player_money = 16;

        // Initialize each player
        for (int i = 0; i < num_players; i++) { //kazdemu hracovi nastavim nejake udaje
            players[i].number = i + 1;
            players[i].space_number = 1;
            players[i].cash = player_money;
            players[i].num_jail_pass = 0;
            players[i].is_in_jail = 0;
            players[i].num_properties = 0;
            for (int j = 0; j < NUM_PROPERTIES; j++) {
                players[i].owned_properties[j] = NULL;
            }
        }
        //for (int i = 0; i < num_players; i++) {
        //printf("Player %d:\n", players[i].number);
        //printf("  Number: %d\n", players[i].number);
        //printf("  Space Number: %d\n", players[i].space_number);
        //printf("  Cash: %d\n", players[i].cash);
        //printf("  Jail Passes: %d\n", players[i].num_jail_pass);
        //printf("  Is in Jail: %d\n", players[i].is_in_jail);
        //printf("  Number of Properties: %d\n", players[i].num_properties);
        // Print owned properties if needed
        //printf("\n");
        //}
        return players;

    }
    int sum_property_prices(PLAYER *player) { //pocet penazi vo vlastnenych properties
        int sum = 0;
        for (int i = 0; i < player->num_properties; i++) {
            sum += player->owned_properties[i]->price;
        }
        return sum;
    }

    void print_s(PLAYER *players, int num_players) {
        mvprintw(24,150,"Game board:\n");
        for (int i = 0; i < NUM_SPACES; i++) {
            // Get the space at the current index
            SPACE space = game_board[i];
            if (space.type == Start || space.type == Free_parking || space.type == In_jail ||
                space.type == Go_to_jail || space.type == Jail_pass) {
                if (i < 9) {
                    mvprintw(25 + i, 150, "%d.  %s\n", i + 1, space_types[space.type]);
                } else {
                    mvprintw(25 + i, 150, "%d. %s\n", i + 1, space_types[space.type]);
                }
            }
            if (space.type == Property && space.property != NULL) {
                bool owned = false;
                for (int j = 0; j < num_players; j++) {
                    PLAYER *current_player = &players[j];
                    for (int k = 0; k < current_player->num_properties; k++) {
                        if (current_player->owned_properties[k] == space.property) {
                            if (owned_monopoly(current_player, space.property->color)) {
                                if (i < 9) {
                                    mvprintw(25 + i, 150, "%d.  %s %d %s P%d yes\n", i + 1, space.property->name,
                                             space.property->price, property_colors[space.property->color],
                                             current_player->number);
                                } else {
                                    mvprintw(25 + i, 150, "%d. %s %d %s P%d yes\n", i + 1, space.property->name,
                                             space.property->price, property_colors[space.property->color],
                                             current_player->number);
                                }
                                owned = true;
                                break;
                            } else {
                                if (i < 9) {
                                    mvprintw(25 + i, 150, "%d.  %s %d %s P%d no\n", i + 1, space.property->name,
                                             space.property->price, property_colors[space.property->color],
                                             current_player->number);
                                } else {
                                    mvprintw(25 + i, 150, "%d. %s %d %s P%d no\n", i + 1, space.property->name,
                                             space.property->price, property_colors[space.property->color],
                                             current_player->number);
                                }
                                owned = true;
                                break;
                            }
                        }
                    }
                    if (owned) {
                        break;
                    }
                }
                if (!owned) {
                    if (i < 9) {
                        mvprintw(25 + i, 150, "%d.  %s %d %s\n", i + 1, space.property->name, space.property->price,
                                 property_colors[space.property->color]);
                    } else {
                        mvprintw(25 + i, 150, "%d. %s %d %s\n", i + 1, space.property->name, space.property->price,
                                 property_colors[space.property->color]);
                    }
                }
            }
        }
        refresh();
    }

    void print_p(PLAYER *players, int num_players) { //rezim p
        mvprintw(16,150,"Players:\n");
        int shift1 = 0;
        if (num_players == 2){
            int shift2 = 10;
        }
        for (int j = 0; j < num_players; j++) {
            PLAYER *current_player = &players[j];
            if (current_player->is_in_jail == 1) {
                mvprintw(17 + j, 150, "%i. S: %i, C: %i, JP: %i, IJ: yes\n", current_player->number,
                         current_player->space_number, current_player->cash, current_player->num_jail_pass);
            } else {
                mvprintw(17 + j, 150, "%i. S: %i, C: %i, JP: %i, IJ: no\n", current_player->number,
                         current_player->space_number, current_player->cash, current_player->num_jail_pass);
            }
            /*
            for (int k = 0; k < current_player->num_properties; k++) {
                for (int space_index = 0; space_index < NUM_SPACES; space_index++) {
                    SPACE *current_space = &game_board[space_index];
                    if (current_space->type == Property &&
                        current_space->property == current_player->owned_properties[k]) {
                        mvprintw(7 + j + k+1, 150, "      %s %d %s S%d\n", current_player->owned_properties[k]->name,
                                 current_player->owned_properties[k]->price,
                                 property_colors[current_player->owned_properties[k]->color], space_index + 1);
                        break;
                    }
                }
            }
            shift1 = 7 + j+ + current_player->num_properties;
             */
        }
        refresh();
    }
    void print_g(PLAYER *players, int num_players) { //rezim g
        print_p(players, num_players);
        refresh();
        print_s(players, num_players);
        refresh();
    }