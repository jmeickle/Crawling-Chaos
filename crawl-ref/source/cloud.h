/**
 * @file
 * @brief Functions related to clouds.
**/


#ifndef CLOUD_H
#define CLOUD_H

#include "externs.h"

enum fog_machine_type
{
    FM_GEYSER,
    FM_SPREAD,
    FM_BROWNIAN,
    NUM_FOG_MACHINE_TYPES,
};

struct fog_machine_data
{
    fog_machine_type fm_type;
    cloud_type       cl_type;
    int              size;
    int              power;
};

cloud_type random_smoke_type();

cloud_type cloud_type_at(const coord_def &pos);

void delete_cloud(int cloud);
void delete_cloud_at(coord_def p);
void move_cloud(int cloud, const coord_def& newpos);
void move_cloud_to(coord_def src, coord_def dest);
void swap_clouds(coord_def p1, coord_def p2);

void check_place_cloud(cloud_type cl_type, const coord_def& p, int lifetime,
                       const actor *agent, int spread_rate = -1,
                       int colour = -1,
                       std::string name = "", std::string tile = "");
void place_cloud(cloud_type cl_type, const coord_def& ctarget,
                 int cl_range, const actor *agent,
                 int spread_rate = -1, int colour = -1, std::string name = "",
                 std::string tile = "");

void manage_clouds(void);

bool is_opaque_cloud(int cloud_idx);
int steam_cloud_damage(const cloud_struct &cloud);
int steam_cloud_damage(int decay);

cloud_type beam2cloud(beam_type flavour);
beam_type cloud2beam(cloud_type flavour);

int resist_fraction(int resist, int bonus_res = 0);
int max_cloud_damage(cloud_type cl_type, int power = -1);
int actor_apply_cloud(actor *act);

std::string cloud_name_at_index(int cloudno);
std::string cloud_type_name(cloud_type type, bool terse = true);
int get_cloud_colour(int cloudno);
coord_def get_cloud_originator(const coord_def& pos);

bool is_damaging_cloud(cloud_type type, bool temp = false);
bool is_harmless_cloud(cloud_type type);
bool in_what_cloud (cloud_type type);
cloud_type in_what_cloud();

// Swarms!
bool cloud_is_swarm(cloud_type type);
bool mons_type_is_swarm(monster_type type);
bool mons_is_swarm(monster* swarm);
bool mons_is_swarm(const monster* swarm);
monster_type swarm_which_monster(cloud_type type);
std::string swarm_description(cloud_type type);
monster* swarm_as_monster(cloud_struct& c);
monster* swarm_as_monster(const cloud_struct& c);
void oh_god_angry_bees_everywhere(cloud_struct& c);
void swarm_handle_attack(const cloud_struct& c, actor* act);
bool swarm_handle_movement(cloud_struct& c);
bool swarm_cloud_valid_move(const coord_def p);
bool swarm_mons_valid_move(monster* swarm, const coord_def p);
bool swarm_mons_valid_move(monster_type type, const coord_def p);
int swarm_generate_monster(monster_type mtype, coord_def p);

// fog generator
void place_fog_machine(fog_machine_type fm_type, cloud_type cl_type,
                       int x, int y, int size, int power);

void place_fog_machine(fog_machine_data data, int x, int y);

bool valid_fog_machine_data(fog_machine_data data);

int              num_fogs_for_place(int level_number = -1,
                                const level_id &place = level_id::current());
fog_machine_data random_fog_for_place(int level_number = -1,
                                const level_id &place = level_id::current());

int              fogs_pan_number(int level_number = -1);
fog_machine_data fogs_pan_type(int level_number = -1);

int              fogs_abyss_number(int level_number = -1);
fog_machine_data fogs_abyss_type(int level_number = -1);

int              fogs_lab_number(int level_number = -1);
fog_machine_data fogs_lab_type(int level_number = -1);

#endif
