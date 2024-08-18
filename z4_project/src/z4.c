#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "monopoly.h" // NEMENIT !!!
PLAYER* loser = NULL;

// TODO vlastne funkcie


bool owned_monopoly(const PLAYER *player, COLOR color){
    int properties_of_color_owned = 0;
    for (int i = 0; i < player->num_properties; i++) {
        // Check if the color of the property matches the specified color
        if (player->owned_properties[i]->color == color) {
            properties_of_color_owned++;
        }
    }

    // Check if the player owns all properties of the specified color
    if (properties_of_color_owned == 2){
        return true;
    }
    else{
        return false;
    }
}
bool is_in_jail(const PLAYER *player) {
    if (game_board[player->space_number - 1].type == In_jail){
        return true;
    }
    else{
        return false;
    }
}

bool next_round(const PLAYER *player, int dice) {
    if (player->space_number - 1 + dice > NUM_SPACES - 1){
        return true;
    }
    else{
        return false;
    }
}
bool pass(const PLAYER *player){
    if (game_board[player->space_number - 1].type == Jail_pass){
        return true;
    }
    else{
        return false;
    }

}
bool move_to_jail(const PLAYER *player) {
    if (game_board[player->space_number - 1].type == Go_to_jail) {
        return true;
    } else {
        return false;
    }
}
bool is_property(const PLAYER *player) {
    if (game_board[player->space_number - 1].type == Property) {
        return true;
    } else {
        return false;
    }
}
bool owned_property(PLAYER *all_players, PLAYER *current_player, int num_players, bool *game_ended){
    SPACE *current_space = &game_board[current_player->space_number - 1];
    PROPERTY *property = current_space->property;

    for (int i = 0; i < num_players; i++) {
        for (int j = 0; j < all_players[i].num_properties; j++) {
            // Check if the current player owns the property
            if (all_players[i].owned_properties[j] == current_space->property) { //ak je majitel pozemku
                if (&all_players[i] == current_player){
                    //printf("vlastnis to\n");
                }
                else {
                    if (owned_monopoly(&all_players[i],property->color) == true){ //je to monopol
                        int price = property->price;
                        if (current_player->cash >= property->price+price) {
                            current_player->cash -= property->price+price; //zaplat tomu co ma pozemok
                            all_players[i].cash += property->price+price;
                            //printf("%d -> platil som\n", i);
                        }
                        else {
                            if ((current_player->cash - (property->price+price)) < 0){
                                loser = current_player;
                                *game_ended = true;
                            }
                        }
                    }
                    else{
                        if (current_player->cash >= property->price) {
                            current_player->cash -= property->price; //zaplat tomu co ma pozemok
                            all_players[i].cash += property->price;
                            //printf("%d -> platil som\n", i);
                        }
                        else {
                            if (current_player->cash - property->price < 0){
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
void after_turn(PLAYER *all_players, PLAYER *current_player, int dice, bool *game_stat, int num_players) {
    if (pass(current_player) == true) {
        current_player->num_jail_pass += 1;
    }
    if (move_to_jail(current_player) == true) {
        if (current_player->num_jail_pass > 0) {
            current_player->num_jail_pass -= 1;
        } else {
            current_player->space_number = 7;
            current_player->is_in_jail = 1;
        }
    }
    if (is_property(current_player) == true) {
        SPACE *current_space = &game_board[current_player->space_number - 1];
        PROPERTY *property = current_space->property;
        if (owned_property(all_players, current_player, num_players, game_stat) == false) {
            if (current_player->cash >= property->price) {
                // Subtract the property price from the player's cash
                current_player->cash -= property->price;
                // Add the property to the player's owned properties
                current_player->owned_properties[current_player->num_properties] = property;
                // Increment the number of properties owned by the player
                current_player->num_properties++;
            }
            else{
                if (current_player->cash - property->price < 0){
                    loser = current_player;
                    *game_stat = true;
                }
            }
        }
    }
    //printf("\n%d mince\n", current_player->cash);
}

void before_turn(PLAYER *all_players, PLAYER *current_player, int dice, bool *game_stat, int num_players){
    if (is_in_jail(current_player) == true) {
        if (current_player->is_in_jail == 1){
            if (current_player->cash >= 1) {
                current_player->cash -= 1;
                current_player->is_in_jail = 0;
            }
            else{
                if (current_player->cash - 1 < 0 ){
                    loser = current_player;
                    *game_stat = true;
                }
            }

        }
        //printf("vo vazeni, treba dorobic ");

    }
    if (next_round(current_player, dice) == true) {
        //printf("nove kolo");
        current_player->cash += 2;
        current_player->space_number = (current_player->space_number + dice) % NUM_SPACES;
    }
    else{
        current_player->space_number += dice;
    }
    //printf("%d", current_player->space_number);
    after_turn(all_players, current_player, dice, game_stat, num_players);
}
PLAYER* init(int num_players) {
    int player_money;
    // Allocate memory for an array of n players
    PLAYER *players = (PLAYER *)malloc(num_players * sizeof(PLAYER));

    // Check if memory allocation was successful
    if (players == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(players);
        exit(EXIT_FAILURE);
    }
    if (num_players == 2){
        player_money = 20;
    }
    else if (num_players == 3)
        player_money = 18;
    else if (num_players == 4)
        player_money = 16;

    // Initialize each player
    for (int i = 0; i < num_players; i++) {
        players[i].number = i + 1; // Player numbers start from 1
        players[i].space_number = 1; // Start at space number 1
        players[i].cash = player_money; // Initial cash
        players[i].num_jail_pass = 0; // No jail passes initially
        players[i].is_in_jail = 0; // Not in jail initially
        players[i].num_properties = 0; // No properties initially
        // Initialize owned_properties to NULL initially
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
int sum_property_prices(PLAYER *player) {
    int sum = 0;
    for (int i = 0; i < player->num_properties; i++) {
        sum += player->owned_properties[i]->price;
    }
    return sum;
}

void print_s(PLAYER *players, int num_players) {
    printf("Game board:\n");
    for (int i = 0; i < NUM_SPACES; i++) {
        // Get the space at the current index
        SPACE space = game_board[i];
        if (space.type == Start || space.type == Free_parking || space.type == In_jail || space.type == Go_to_jail || space.type == Jail_pass){
            printf("%d. %s\n",i+1, space_types[space.type]);
        }
        //
        if (space.type == Property && space.property != NULL) {
            bool owned = false;
            for (int j = 0; j < num_players; j++) {
                PLAYER *current_player = &players[j];
                for (int k = 0; k < current_player->num_properties; k++) {
                    if (current_player->owned_properties[k] == space.property) {
                        if (owned_monopoly(current_player, space.property->color) == true){
                            printf("%d. %s %d %s P%d yes\n", i+1, space.property->name, space.property->price, property_colors[space.property->color], current_player->number);
                            owned = true;
                            break; // No need to check further if owned by any player
                        }
                        else{
                            printf("%d. %s %d %s P%d no\n", i+1, space.property->name, space.property->price, property_colors[space.property->color], current_player->number);
                            owned = true;
                            break; // No need to check further if owned by any player
                        }
                    }
                }
                if (owned) {
                    break; // No need to check further if owned by any player
                }
            }
            if (!owned) {
                printf("%d. %s %d %s\n", i+1, space.property->name, space.property->price, property_colors[space.property->color]);
            }
        }
    }
}

void print_p(PLAYER *players, int num_players){
    printf("Players:\n");
    for (int j = 0; j < num_players; j++) {
        PLAYER *current_player = &players[j];
        if (current_player->is_in_jail == 1){
            printf("%i. S: %i, C: %i, JP: %i, IJ: yes\n", current_player->number, current_player->space_number, current_player->cash, current_player->num_jail_pass);
        }
        else{
            printf("%i. S: %i, C: %i, JP: %i, IJ: no\n", current_player->number, current_player->space_number, current_player->cash, current_player->num_jail_pass);
        }
        for (int k = 0; k < current_player->num_properties; k++) {
            for (int space_index = 0; space_index < NUM_SPACES; space_index++) {
                SPACE *current_space = &game_board[space_index];
                if (current_space->type == Property && current_space->property == current_player->owned_properties[k]) {
                    printf("      %s %d %s S%d\n", current_player->owned_properties[k]->name, current_player->owned_properties[k]->price, property_colors[current_player->owned_properties[k]->color], space_index + 1);
                    break;
                }
            }
        }
    }

}
void print_g(PLAYER *players, int num_players){
    print_p(players, num_players);
    print_s(players, num_players);
}
int main(int argc, char *argv[]) {
    char opt;

    int switch_s = 0;
    int switch_p = 0;
    int switch_g = 0;
    int switch_n = 0;

    int num_players = 0;
    char *optstring = ":sgpn:";
    bool game_ended = false;

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
            case 's':
                // Ak bol zadaný argument -d
                //printf("Option -s was specified\n");
                switch_s = 1;
                break;
            case 'g':
                // Ak bol zadaný argument -p
                //printf("Option -g was specified\n");
                switch_g = 1;
                break;
            case 'p':
                // Ak bol zadaný argument -u
                //printf("Option -p was specified\n");
                switch_p = 1;

                break;
            case 'n':
                // Ak bol zadaný argument -l
                //printf("Option -n was specified\n");
                switch_n = 1;
                if (switch_n == 1){
                    num_players = atoi(optarg);
                }
                break;
            case ':':
                // Ak chýba argument pre predchádzajúcu voľbu
                //fprintf(stderr, "Option '%c' requires an argument.\n", optopt);
                return 0;
            case '?':
                // Neznáma voľba
                //fprintf(stderr, "Unknown option '%c'\n", optopt);
                return 0;
            default:
                //fprintf(stderr, "Unexpected error occurred\n");
                exit(EXIT_FAILURE);
        }
    }
    if (switch_n == 0){
        num_players = 2;
    }
    PLAYER* players = init(num_players);
    //printf("hello\n");
    int turns = 1;

    print_g(players, num_players); //prvy vypis
    printf("WINNER: -\n");
    printf("\n");

    while (game_ended != true){
        for (int i = 0; i < num_players; i++){
            int dice;
            if (scanf("%d",&dice) != 1){
                free(players);
                return 0;
            }
            before_turn(players, &players[i], dice, &game_ended, num_players);
            //sum_property_prices(&players[i]);
            if (game_ended){
                PLAYER winner = players[0];
                int tie = 1;
                int winner_properties_sum = sum_property_prices(&players[0]);
                for (int j = 1; j < num_players; j++) {
                    if (players[j].number == loser->number ){
                        continue;
                    }
                    if (winner.cash < players[j].cash) {
                        winner = players[j];
                        winner_properties_sum = sum_property_prices(&players[j]);
                        tie = 1;
                    } else if (winner.cash == players[j].cash) {
                        if (winner_properties_sum < sum_property_prices(&players[j])) {
                            winner = players[j];
                            winner_properties_sum = sum_property_prices(&players[j]);
                            tie = 1;
                        }
                        if(winner.number != players[j].number){
                            if (winner_properties_sum == sum_property_prices(&players[j]) ){
                                tie ++;
                            }
                        }
                    }
                }
                //if (tie > 1){
                //printf("-");
                //}
                //else{
                //printf("P%d",  winner.number);
                //}
                printf("R: %d\nTurn: %d\nPlayer on turn: P%d\n",dice, turns,i+1);
                printf("\n");
                print_g(players, num_players); //posledny print
                if (tie > 1){
                    printf("WINNER: ?\n");
                    printf("\n");
                }
                else{
                    printf("WINNER: P%d\n",  winner.number);
                    printf("\n");
                }

                free(players);
                return 0;
            }
            printf("R: %d\nTurn: %d\nPlayer on turn: P%d\n",dice, turns,i+1);
            printf("\n");
            if (switch_s == 1){
                print_s(players, num_players);
                printf("\n");
            }
            else if (switch_p == 1){
                print_p(players, num_players);
                printf("\n");
            }
            else if (switch_g == 1) {
                print_g(players, num_players);
                if (game_ended == false) {
                    printf("WINNER: -\n");
                }
                printf("\n");

            }
            turns++;

        }
        //game_ended = true;
        //ak nie je dane n - 2 hraci
    }
}