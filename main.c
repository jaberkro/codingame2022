#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
typedef struct s_entity{
    // Unique identifier
    int id;
    // 0=monster, 1=your hero, 2=opponent hero
    int type;
    // Position of this entity
    int x;
    int y;
    // Ignore for this league; Count down until shield spell fades
    int shield_life;
    // Ignore for this league; Equals 1 when this entity is under a control spell
    int is_controlled;
    // Remaining health of this monster
    int health;
    // Trajectory of this monster
    int vx;
    int vy;
    // 0=monster with no target yet, 1=monster targeting a base
    int near_base;
    // Given this monster's trajectory, is it a threat to 1=your base, 2=your opponent's base, 0=neither
    int threat_for;
}   t_entity;

typedef struct s_player{
    int posx;
    int posy;
    int destx;
    int desty;
    int action;
    int spell_id;
    int shielded;
} t_player;

int find_closest_to_player(int x, int y, int amount, t_entity entities[100], int base_x)
{
    int closest_id;
    int closest_x;
    int closest_y;
    int i;

    i = 0;
    closest_x = 2200;
    closest_y = 2200;
    closest_id = 0;
    while (i < amount)
    {
        if (entities[i].type != 1 && abs(x - entities[i].x) <= closest_x && abs(y - entities[i].y) <= closest_y && \
            ((base_x == 0 && (entities[i].vx < 0 && entities[i].vy < 0)) || (base_x != 0 && (entities[i].vx > 0 && entities[i].vy > 0))))
        {
            closest_x = entities[i].x;
            closest_y = entities[i].y;
            closest_id = entities[i].id;
            return (closest_id);
            fprintf(stderr, "found a spider with id %d, vx %d, vy%d\n", closest_id, entities[i].vx, entities[i].vy);
        }
        else if (entities[i].type != 1 && abs(x - entities[i].x) <= closest_x && abs(y - entities[i].y) <= closest_y && \
            ((base_x == 0 && (entities[i].vx < 0 || entities[i].vy < 0)) || (base_x != 0 && (entities[i].vx > 0 || entities[i].vy > 0))))
        {
            closest_x = entities[i].x;
            closest_y = entities[i].y;
            closest_id = entities[i].id;
            fprintf(stderr, "found a spider with id %d, vx %d, vy%d\n", closest_id, entities[i].vx, entities[i].vy);
        }
        i++;
    }
    return (closest_id);
}

void find_closest_to_base(int base_x, int base_y, int *destx, int *desty, int amount, t_entity entities[100], int range)
{
    int closest_x;
    int closest_y;
    int closest_vx;
    int closest_vy;
    int closest_dist;
    int i;

    i = 0;
    if (range < 5000)
        range = 5000;
    closest_x = range;
    closest_y = range;
    closest_dist = range * 2;
    closest_vx = 0;
    closest_vy = 0;
    while (i < amount)
    {
        if (entities[i].type == 0 && sqrt(abs(base_x - entities[i].x) * abs(base_x - entities[i].x) + \
            abs(base_y - entities[i].y) * abs(base_y - entities[i].y)) <= closest_dist)
        {
            closest_x = entities[i].x;
            closest_y = entities[i].y;
            closest_vx = entities[i].vx;
            closest_vy = entities[i].vy;
            closest_dist = sqrt(abs(base_x - entities[i].x) * abs(base_x - entities[i].x) + \
                abs(base_y - entities[i].y) * abs(base_y - entities[i].y));
        }
        i++;
    }
    if (closest_x != range && closest_y != range)
    {
        *destx = closest_x + closest_vx;
        *desty = closest_y + closest_vy;
    }
}

int monster_in_wind_range(int x, int y, t_entity entities[100], int amount)
{
    int i;

    i = 0;
    while (i < amount)
    {
        if (entities[i].type != 1 && sqrt(abs(x - entities[i].x) * abs(x - entities[i].x) + \
            abs(y - entities[i].y) * abs(y - entities[i].y)) < 1280)
        {
            if (entities[i].shield_life > 0)
                return (0);
            return (1);
        }
        i++;
    }
    return (0);
}

int monster_in_control_range(int x, int y, t_entity entities[100], int amount, t_player *player, int base_x, int base_y)
{
    int i;

    i = 0;
    while (i < amount)
    {
        if (entities[i].type == 0 && sqrt(abs(x - entities[i].x) * abs(x - entities[i].x) + \
            abs(y - entities[i].y) * abs(y - entities[i].y)) < 2200 && \
            entities[i].shield_life == 0 && sqrt(abs(base_x - entities[i].x) * abs(base_x - entities[i].x) + \
            abs(base_y - entities[i].y) * abs(base_y - entities[i].y)) > 5000)
        {
            player->spell_id = entities[i].id;
            if (base_x == 0 && entities[i].vx > 0 && entities[i].vy > 0 && sqrt(abs(base_x - entities[i].x) * abs(base_x - entities[i].x) + \
                abs(base_y - entities[i].y) * abs(base_y - entities[i].y)) < 7000)
                return (2);
            else if (base_x != 0 && entities[i].vx < 0 && entities[i].vy < 0 && sqrt(abs(base_x - entities[i].x) * abs(base_x - entities[i].x) + \
                abs(base_y - entities[i].y) * abs(base_y - entities[i].y)) < 7000)
                return (2);
            else if (base_x == 0 && (entities[i].vx < 0 || entities[i].vy < 0))
                return (1);
            else if (base_x != 0 && (entities[i].vx > 0 || entities[i].vy > 0))
                return (1);
        }
        i++;
    }
    return (0);
}

int own_id(int player, int amount, t_entity entities[100])
{
    int player_count;
    int i;

    i = 0;
    player_count = 0;
    while (i < amount)
    {
        if (entities[i].type == 1)
            player_count++;
        if (player_count == player)
            return (entities[i].id);
        i++;
    }
    return (0);
}

int enemy_id(int x, int y, int amount, t_entity entities[100])
{
    int i;

    i = 0;
    while (i < amount)
    {
        if (entities[i].type == 2 && sqrt(abs(x - entities[i].x) * abs(x - entities[i].x) + \
            abs(y - entities[i].y) * abs(y - entities[i].y)) < 2200)
            return (entities[i].id);
        i++;
    }
    return (0);
}

int opponent_in_wind_range(int x, int y, int amount, t_entity entities[100])
{
    int i;

    i = 0;
    while (i < amount)
    {
        if (entities[i].type == 2 && sqrt(abs(x - entities[i].x) * abs(x - entities[i].x) + \
            abs(y - entities[i].y) * abs(y - entities[i].y)) < 1280)
            return (1);
        i++;
    }
    return (0);
}

void find_closest_to_me(int x, int y, int *destx, int *desty, int amount, t_entity entities[100])
{
    int closest_x;
    int closest_y;
    int closest_vx;
    int closest_vy;
    int closest_dist;
    int i;

    i = 0;
    closest_x = 7000;
    closest_y = 7000;
    closest_dist = 10000;
    closest_vx = 0;
    closest_vy = 0;
    while (i < amount)
    {
        if (entities[i].type == 0 && sqrt(abs(x - entities[i].x) * abs(x - entities[i].x) + \
            abs(y - entities[i].y) * abs(y - entities[i].y)) < closest_dist)
        {
            closest_x = entities[i].x;
            closest_y = entities[i].y;
            closest_vx = entities[i].vx;
            closest_vy = entities[i].vy;
            closest_dist = sqrt(abs(x - entities[i].x) * abs(x - entities[i].x) + abs(y - entities[i].y) * abs(y - entities[i].y));
        }
        i++;
    }
    if (closest_x != 7000 && closest_y != 7000)
    {
        *destx = closest_x + closest_vx;
        *desty = closest_y + closest_vy;
    }
}

int main()
{
    // The corner of the map representing your base
    int base_x;
    int base_y;
    scanf("%d%d", &base_x, &base_y);
    // Always 3
    int heroes_per_player;
    scanf("%d", &heroes_per_player);

    t_entity    entities[100];
    t_player    players[3];

    int phase = 0;
    int threat_count;
    int enemy_count;
    int player_found;
    int attack_strategie;
    bool last_control_location = 0;
    // game loop
    while (1) {
        bzero(players, sizeof(players));
        threat_count = 0;
        enemy_count = 0;
        player_found = 0;
        int mana;
        int oppmana;
        int opphealth;
        for (int i = 0; i < 2; i++) {
            // Your base health
            int health;
            // Ignore in the first league; Spend ten mana to cast a spell
            if (i == 0)
                scanf("%d%d", &health, &mana);
            else
                scanf("%d%d", &opphealth, &oppmana);
        }
        // Amount of heros and monsters you can see
        int entity_count;
        scanf("%d", &entity_count);
        //fprintf(stderr, "entity count: %d\n", entity_count);
        for (int i = 0; i < entity_count; i++) {
            scanf("%d%d%d%d%d%d%d%d%d%d%d", &entities[i].id, &entities[i].type, &entities[i].x, &entities[i].y, &entities[i].shield_life, &entities[i].is_controlled, &entities[i].health, &entities[i].vx, &entities[i].vy, &entities[i].near_base, &entities[i].threat_for);
            //SAVE PLAYERS CURRENT POSITION
            if (entities[i].type == 1)
            {
                players[player_found].posx = entities[i].x;
                players[player_found].posy = entities[i].y;
                players[player_found].shielded = entities[i].shield_life;
                player_found++;
            }
            //COUNT THE THREAT
            if (entities[i].threat_for == 1 && entities[i].near_base == 1)
                threat_count++;
            //COUNT THE ENEMY
            if (entities[i].type == 2)
                enemy_count++;
        }
        if (phase == 0)
        {
            //PLAYERS SET DESTINATION DEFAULT
            if (base_x == 0)
            {
                players[0].destx = 1000;
                players[0].desty = 1000;
                players[1].destx = 4000;
                players[1].desty = 6000;
                players[2].destx = 6000;
                players[2].desty = 4000;
            }
            else
            {
                players[0].destx = 16630;
                players[0].desty = 8000;
                players[1].destx = 13630;
                players[1].desty = 3000;
                players[2].destx = 11630;
                players[2].desty = 5000;
            }

            //SET DESTINATION TO CLOSEST THREAT TO BASE
            find_closest_to_base(base_x, base_y, &players[0].destx, &players[0].desty, entity_count, entities, 8000 - threat_count * 1000);
            find_closest_to_base(base_x, base_y, &players[1].destx, &players[1].desty, entity_count, entities, 8000 - threat_count * 500);
            find_closest_to_base(base_x, base_y, &players[2].destx, &players[2].desty, entity_count, entities, 9000);

            //IF THERES NO THREAT TO BASE, SET DEST OF PLAYER 2 TO CLOSEST SPIDER
            if (threat_count == 0)
                find_closest_to_me(players[2].posx, players[2].posy, &players[2].destx, &players[2].posy, entity_count, entities);

            //WALK TO THE DESTINATIONS
            players[0].action = 1;
            players[1].action = 1;
            players[2].action = 1;

            //IF P1 OR P0 IS OUT OF RANGE AND THERE IS A THREAT_COUNT, SHIELD YOURSELF
            if (threat_count > 0 && sqrt(abs(base_x - players[0].posx) * abs(base_x - players[0].posx) + \
            abs(base_y - players[0].posy) * abs(base_y - players[0].posy)) > 5000 && \
                enemy_count > 0 && players[0].shielded != 0)
            {
                fprintf(stderr, "help! from p0\n");
                players[0].spell_id = enemy_id(players[0].posx, players[0].posy, entity_count, entities);
                if (players[0].spell_id != 0)
                    players[0].action = 4;
            }
            if (threat_count > 0 && sqrt(abs(base_x - players[1].posx) * abs(base_x - players[1].posx) + \
            abs(base_y - players[1].posy) * abs(base_y - players[1].posy)) > 5000 && \
                enemy_count > 0 && players[1].shielded != 0)
            {
                fprintf(stderr, "help! from p1\n");
                players[1].spell_id = enemy_id(players[1].posx, players[1].posy, entity_count, entities);
                if (players[1].spell_id != 0)
                    players[1].action = 4;
            }

            //IF THREATCOUNT > 2 && SPIDER IN WIND RANGE, USE WIND
            if (opponent_in_wind_range(players[0].posx, players[0].posy, entity_count, entities) || threat_count > 1 && monster_in_wind_range(players[0].posx, players[0].posy, entities, entity_count) == 1 && \
                mana > 10 && sqrt(abs(base_x - players[0].posx) * abs(base_x - players[0].posx) + \
                abs(base_y - players[0].posy) * abs(base_y - players[0].posy)) < 5000)
                players[0].action = 2;

            //IF WE REACH 400 MANA, START NEXT PHASE
            if (mana > 300)
                phase = 1;
        }

        if (phase == 1)
        {
            //LET PLAYERS WALK TO LOCATION
            if (base_x == 0)
            {
                players[0].destx = 1000;
                players[0].desty = 1000;
                players[1].destx = 3000;
                players[1].desty = 3000;
                players[2].destx = 11630;
                players[2].desty = 3000;
            }
            else
            {
                players[0].destx = 16630;
                players[0].desty = 8000;
                players[1].destx = 14630;
                players[1].desty = 6000;
                players[2].destx = 6000;
                players[2].desty = 6000;
            }
            
            //LET PLAYER 1 AND 0 GO AFTER CLOSEST THREAT TO BASE
            find_closest_to_base(base_x, base_y, &players[1].destx, &players[1].desty, entity_count, entities, 8000 - threat_count * 1500);
            find_closest_to_base(base_x, base_y, &players[0].destx, &players[0].desty, entity_count, entities, 8000 - threat_count * 1500);
            
            find_closest_to_me(players[2].posx, players[2].posy, &players[2].destx, &players[2].posy, entity_count, entities);

            //WALK TO THE DESTINATIONS
            players[0].action = 1;
            players[1].action = 1;
            players[2].action = 1;

            //IF THREATCOUNT > 2 && SPIDER IN WIND RANGE, USE WIND
            if (threat_count > 1 && monster_in_wind_range(players[0].posx, players[0].posy, entities, entity_count) == 1 && \
                mana > 10 && sqrt(abs(base_x - players[0].posx) * abs(base_x - players[0].posx) + \
                abs(base_y - players[0].posy) * abs(base_y - players[0].posy)) < 5000)
                players[0].action = 2;
            if (threat_count > 6 && monster_in_wind_range(players[1].posx, players[1].posy, entities, entity_count) == 1 && \
                mana > 20 && sqrt(abs(base_x - players[1].posx) * abs(base_x - players[1].posx) + \
                abs(base_y - players[1].posy) * abs(base_y - players[1].posy)) < 5000)
                players[1].action = 2;

            for (int i = 0; i < 2; i++)
            {
                if (opponent_in_wind_range(players[i].posx, players[i].posy, entity_count, entities))
                    players[i].action = 2;
            }
            
            //IF P1 OR P0 IS OUT OF RANGE AND THERE IS A THREAT_COUNT, CONTROL ENEMY
            if (threat_count > 0 && sqrt(abs(base_x - players[0].posx) * abs(base_x - players[0].posx) + \
                abs(base_y - players[0].posy) * abs(base_y - players[0].posy)) > 6000 && \
                enemy_count > 0 && players[0].shielded != 0)
            {
                players[0].spell_id = enemy_id(players[0].posx, players[0].posy, entity_count, entities);
                if (players[0].spell_id != 0)
                    players[0].action = 4;
            }
            if (threat_count > 0 && sqrt(abs(base_x - players[1].posx) * abs(base_x - players[1].posx) + \
                abs(base_y - players[1].posy) * abs(base_y - players[1].posy)) > 6000 && \
                enemy_count > 0 && players[1].shielded != 0)
            {
                players[1].spell_id = enemy_id(players[1].posx, players[1].posy, entity_count, entities);
                if (players[1].spell_id != 0)
                    players[1].action = 4;
            }

            //LET PLAYER 2 CONTROL SPIDERS AND PUSH WIND IF AT LOCATION
            attack_strategie = monster_in_control_range(players[2].posx, players[2].posy, entities, entity_count, &players[2], base_x, base_y);
            if (attack_strategie == 1 && sqrt(abs(base_x - players[2].posx) * abs(base_x - players[2].posx) + \
                abs(base_y - players[2].posy) * abs(base_y - players[2].posy)) > 8000 && mana > 100)
                players[2].action = 4;
            else if (attack_strategie == 2 && sqrt(abs(base_x - players[2].posx) * abs(base_x - players[2].posx) + \
                abs(base_y - players[2].posy) * abs(base_y - players[2].posy)) > 8000 && mana > 100)
                players[2].action = 3;
            else if (sqrt(abs(base_x - players[2].posx) * abs(base_x - players[2].posx) + \
                abs(base_y - players[2].posy) * abs(base_y - players[2].posy)) > 8000 && mana > 100)
                players[2].action = 2;
        }
        //PRINT 3 MOVES
        for (int i = 0; i < heroes_per_player; i++) {
            fprintf(stderr, "I am p%d. My action is %d\n", i, players[i].action);
            if (players[i].action == 0)
                printf("WAIT p%d\n", i);
            else if (players[i].action == 1)
                printf("MOVE %d %d p%d\n", players[i].destx, players[i].desty, i);
            else if  (players[i].action == 2)
            {
                if (base_x == 0)
                    printf("SPELL WIND %d %d p%d\n", 17630, 9000, i);
                else
                    printf("SPELL WIND %d %d p%d\n", 0, 0, i);
            }
            else if  (players[i].action == 3)
            {
                if (base_x == 0)
                    printf("SPELL SHIELD %d p%d\n", players[i].spell_id, i);
                else
                    printf("SPELL SHIELD %d p%d\n", players[i].spell_id, i);
            }
            else if  (players[i].action == 4)
            {
                if (base_x == 0 && last_control_location == 0)
                    printf("SPELL CONTROL %d %d %d p%d\n", players[i].spell_id, 17630, 5000, i);
                else if (base_x == 0)
                    printf("SPELL CONTROL %d %d %d p%d\n", players[i].spell_id, 13630, 9000, i);
                else if (last_control_location == 0)
                    printf("SPELL CONTROL %d %d %d p%d\n", players[i].spell_id, 0, 4000, i);
                else
                    printf("SPELL CONTROL %d %d %d p%d\n", players[i].spell_id, 4000, 0, i);
                last_control_location = !last_control_location;
            }
        }
    }
    return 0;
}
