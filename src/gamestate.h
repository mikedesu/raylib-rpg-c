#pragma once

#include "ComponentTable.h"
#include "attack_result.h"
#include "character_creation.h"
#include "controlmode.h"
#include "debugpanel.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "entityid.h"
#include "entitytype.h"
#include "gamestate_flag.h"
#include "get_racial_hd.h"
#include "get_racial_modifiers.h"
#include "inputstate.h"
#include "libgame_defines.h"
#include "libgame_version.h"
#include "magic_values.h"
#include "orc_names.h"
#include "potion.h"
#include "roll.h"
#include "scene.h"
#include "sfx.h"
#include "stat_bonus.h"
#include <ctime>
#include <raylib.h>
#include <raymath.h>

#define DEFAULT_MUSIC_VOLUME 0.0f
#define GAMESTATE_SIZEOFTIMEBUF 64
#define GAMESTATE_SIZEOFDEBUGPANELBUF 1024
#define GAMESTATE_SIZEOFTEXINFOARRAY 2048
#define MAX_MESSAGES 64
#define MAX_MSG_LENGTH 256
#define LIST_INIT_CAPACITY 16
#define DEFAULT_MAX_HISTORY_SIZE 1024
#define MAX_MUSIC_PATHS 1024
#define MAX_MUSIC_PATH_LENGTH 256
#define GAMESTATE_DEBUGPANEL_DEFAULT_X 0
#define GAMESTATE_DEBUGPANEL_DEFAULT_Y 0
#define GAMESTATE_DEBUGPANEL_DEFAULT_WIDTH 200
#define GAMESTATE_DEBUGPANEL_DEFAULT_HEIGHT 200

#define GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE 20
#define GAMESTATE_INIT_ENTITYIDS_MAX 3000000


typedef ComponentTable CT;
typedef function<void(CT& ct, const entityid)> with_fun;


class gamestate {
public:
    controlmode_t controlmode;
    debugpanel_t debugpanel;

    entityid hero_id;
    entityid new_entityid_begin;
    entityid new_entityid_end;
    entityid next_entityid;
    entityid entity_turn;

    time_t timebegan;
    time_t currenttime;

    struct tm* timebegantm;
    struct tm* currenttimetm;

    char timebeganbuf[GAMESTATE_SIZEOFTIMEBUF];
    char currenttimebuf[GAMESTATE_SIZEOFTIMEBUF];

    bool test;
    bool debugpanelon;
    bool gridon;
    bool cam_lockon;
    bool player_input_received;
    bool is_locked;
    bool processing_actions;
    bool gameover;
    bool player_changing_dir;
    bool test_guard;
    bool display_inventory_menu;
    bool display_quit_menu;
    bool do_quit;
    bool dirty_entities;
    bool display_help_menu;
    bool cam_changed;
    bool frame_dirty;
    bool display_gameplay_settings_menu;
    bool display_sort_inventory_menu;
    bool music_volume_changed;
    bool do_restart;
    bool msg_system_is_active;

    int framecount;
    int fadealpha;
    int targetwidth;
    int targetheight;
    int windowwidth;
    int windowheight;
    int lock;
    int turn_count;
    int frame_updates;
    int font_size;
    int pad;
    int inventory_menu_selection;
    int gameplay_settings_menu_selection;
    int title_screen_selection;
    int max_title_screen_selections;
    int msg_history_max_len_msg;
    int msg_history_max_len_msg_measure;
    int current_music_index;
    int restart_count;

    float line_spacing;
    float music_volume;

    double last_frame_time;

    dungeon_t dungeon;

    char frame_time_str[32];

    gamestate_flag_t flag;

    scene_t current_scene;

    vector<string> msg_system;
    vector<string> msg_history;
    vector<Sound> sfx;
    vector<string> music_file_paths;

    character_creation chara_creation;

    string version;

    Color message_history_bgcolor;
    Vector2 last_click_screen_pos;
    Vector2 inventory_cursor = {0, 0};
    Camera2D cam2d;
    Font font;

    ComponentTable ct;




    gamestate() {
        minfo("Initializing gamestate");
        reset();
    }




    ~gamestate() {
        reset();
    }




    inline void init_music_paths() {
        const char* music_path_file = "music.txt";
        FILE* file = fopen(music_path_file, "r");
        massert(file, "Could not open music path file: %s", music_path_file);
        char buffer[128];
        music_file_paths.clear();
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            // Remove newline character if present
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
            }
            // if it begins with a #, skip for now
            if (buffer[0] == '#') {
                mwarning("Comment received, skipping...");
                continue;
            }
            // copy into g->music_file_paths
            // need to pre-pend the folder path
            string fullpath = "audio/music/" + string(buffer);
            music_file_paths.push_back(fullpath);
        }
        fclose(file);
    }




    inline void reset() {
        version = GAME_VERSION;
        debugpanel.x = GAMESTATE_DEBUGPANEL_DEFAULT_X;
        debugpanel.y = GAMESTATE_DEBUGPANEL_DEFAULT_Y;
        debugpanel.w = GAMESTATE_DEBUGPANEL_DEFAULT_WIDTH;
        debugpanel.h = GAMESTATE_DEBUGPANEL_DEFAULT_HEIGHT;
        debugpanel.fg_color = RAYWHITE;
        debugpanel.bg_color = RED;
        debugpanel.font_size = GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE;
        targetwidth = targetheight = windowwidth = windowheight = -1;
        hero_id = entity_turn = new_entityid_begin = new_entityid_end = ENTITYID_INVALID;
        timebegan = currenttime = time(NULL);
        timebegantm = localtime(&timebegan);
        currenttimetm = localtime(&currenttime);
        bzero(timebeganbuf, GAMESTATE_SIZEOFTIMEBUF);
        bzero(currenttimebuf, GAMESTATE_SIZEOFTIMEBUF);
        strftime(timebeganbuf, GAMESTATE_SIZEOFTIMEBUF, "Start Time: %Y-%m-%d %H:%M:%S", timebegantm);
        strftime(currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", currenttimetm);
        cam_lockon = true;
        frame_dirty = true;
        debugpanelon = false;
        player_input_received = false;
        is_locked = false;
        gridon = false;
        display_inventory_menu = false;
        display_quit_menu = false;
        display_help_menu = false;
        do_quit = false;
        processing_actions = false;
        cam_changed = false;
        gameover = false;
        test_guard = false;
        dirty_entities = false;
        display_sort_inventory_menu = false;
        music_volume_changed = false;
        player_changing_dir = false;
        msg_system_is_active = false;
        test = false;
        gameplay_settings_menu_selection = 0;
        cam2d.target = cam2d.offset = (Vector2){0, 0};
        cam2d.zoom = 4.0f;
        cam2d.rotation = 0.0;
        fadealpha = 0.0;
        controlmode = CONTROLMODE_PLAYER;
        // current displayed dungeon floor
        flag = GAMESTATE_FLAG_PLAYER_INPUT;
        font_size = GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE;
        pad = 20;
        line_spacing = 1.0f;
        // weird bug maybe when set to 0?
        next_entityid = 1;
        current_music_index = 0;
        restart_count = 0;
        do_restart = 0;
        title_screen_selection = 0;
        lock = 0;
        frame_updates = 0;
        framecount = 0;
        turn_count = 0;
        inventory_menu_selection = 0;
        msg_history_max_len_msg_measure = 0;
        debugpanel.pad_top = 0;
        debugpanel.pad_left = 0;
        debugpanel.pad_right = 0;
        debugpanel.pad_bottom = 0;
        msg_history_max_len_msg = 0;
        max_title_screen_selections = 2;
        // initialize character creation
        chara_creation.name = "hero";
        chara_creation.strength = chara_creation.dexterity = chara_creation.intelligence = chara_creation.wisdom = chara_creation.constitution =
            chara_creation.charisma = 10;
        chara_creation.race = RACE_HUMAN;
        chara_creation.hitdie = get_racial_hd(RACE_HUMAN);
        // why is the above line crashing?
        // the above line is also crashing
        //msuccess("Gamestate character creation name set to empty string");
        current_scene = SCENE_TITLE;
        music_volume = DEFAULT_MUSIC_VOLUME;
        last_click_screen_pos = (Vector2){-1, -1};
        msg_system.clear();
        msg_history.clear();
        ct.clear();
        for (size_t i = 0; i < dungeon.floors.size(); i++) {
            df_free(dungeon.floors[i]);
        }
        dungeon.floors.clear();
        dungeon.is_initialized = false;
        init_music_paths();
    }




    const inline bool set_hero_id(const entityid id) {
        massert(id != ENTITYID_INVALID, "id is invalid");
        hero_id = id;
        return true;
    }




    const inline entityid add_entity() {
        const entityid id = next_entityid;
        if (!dirty_entities) {
            dirty_entities = true;
            new_entityid_begin = id;
            new_entityid_end = id + 1;
        } else {
            new_entityid_end = id + 1;
        }
        next_entityid++;
        return id;
    }




    inline tile_t& tile_at_cur_floor(const vec3 loc) {
        return df_tile_at(d_get_current_floor(dungeon), loc);
    }




    inline void init_dungeon(const int df_count) {
        massert(df_count > 0, "df_count is <= 0");
        d_create(dungeon);
        if (dungeon.is_initialized) {
            return;
        }
        // max size of 128x128 for now to maintain 60fps
        // dungeon floors, tiles etc will require re-write/re-design for optimization
        const int w = 32;
        const int h = 32;
        const biome_t type = BIOME_STONE;
        //dungeon_floor_type_t type = DUNGEON_FLOOR_TYPE_GRASS;
        for (int i = 0; i < df_count; i++) {
            d_add_floor(dungeon, type, w, h);
        }
        dungeon.is_initialized = true;
    }




    const inline entityid create_door_with() {
        const entityid id = add_entity();
        ct.set<entitytype>(id, ENTITY_DOOR);
        //doorInitFunction(g, id);
        return id;
    }




    inline void recompute_entity_cache(tile_t& t) {
        // Only recompute if cache is dirty
        if (!t.dirty_entities)
            return;
        // Reset counters
        t.cached_live_npcs = 0;
        t.cached_item_count = 0;
        t.cached_player_present = false;
        t.cached_npc = ENTITYID_INVALID;
        t.cached_item = ENTITYID_INVALID;
        // Iterate through all entities on the tile
        for (size_t i = 0; i < t.entities->size(); i++) {
            const entityid id = t.entities->at(i);
            // Skip dead entities
            if (ct.get<dead>(id).value_or(false)) {
                continue;
            }
            // Check entity type
            const entitytype_t type = ct.get<entitytype>(id).value_or(ENTITY_NONE);
            if (type == ENTITY_NPC) {
                t.cached_live_npcs++;
                t.cached_npc = id;
            } else if (type == ENTITY_PLAYER) {
                t.cached_player_present = true;
                t.cached_npc = id;
            } else if (type == ENTITY_ITEM) {
                t.cached_item_count++;
                t.cached_item = id;
            }
        }
        // Cache is now clean
        t.dirty_entities = false;
    }




    inline void recompute_entity_cache_at(const int x, const int y, const int z) {
        massert(x >= 0 && y >= 0 && z >= 0, "x, y, or z is out of bounds: %d, %d, %d", x, y, z);
        massert((size_t)z < dungeon.floors.size(), "z is out of bounds");
        auto df = dungeon.floors[z];
        auto t = df_tile_at(df, (vec3){x, y, z});
        recompute_entity_cache(t);
    }




    const inline bool tile_has_live_npcs(tile_t& t) {
        recompute_entity_cache(t);
        return t.cached_live_npcs > 0;
    }




    const inline entityid create_door_at_with(const vec3 loc) {
        dungeon_floor_t& df = d_get_floor(dungeon, loc.z);
        tile_t& tile = df_tile_at(df, loc);
        if (!tile_is_walkable(tile.type))
            return ENTITYID_INVALID;
        if (tile_has_live_npcs(tile))
            return ENTITYID_INVALID;
        const auto id = create_door_with();
        if (id == ENTITYID_INVALID)
            return ENTITYID_INVALID;
        if (!df_add_at(df, id, loc.x, loc.y))
            return ENTITYID_INVALID;
        ct.set<location>(id, loc);
        ct.set<door_open>(id, false);
        ct.set<update>(id, true);
        return id;
    }




    const inline size_t place_doors() {
        size_t placed_doors = 0;
        for (int z = 0; z < (int)dungeon.floors.size(); z++) {
            dungeon_floor_t& df = d_get_floor(dungeon, z);
            for (int x = 0; x < df.width; x++) {
                for (int y = 0; y < df.height; y++) {
                    const vec3 loc = {x, y, z};
                    tile_t& tile = df_tile_at(df, loc);
                    if (!tile.can_have_door) {
                        continue;
                    }
                    const entityid door_id = create_door_at_with(loc);
                    if (door_id == ENTITYID_INVALID) {
                        continue;
                    }
                    placed_doors++;
                }
            }
        }
        return placed_doors;
    }




    const inline entityid create_prop_with(const proptype_t type) {
        const auto id = add_entity();
        ct.set<entitytype>(id, ENTITY_PROP);
        ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
        ct.set<update>(id, true);
        ct.set<proptype>(id, type);
        //propInitFunction(g, id);
        return id;
    }




    const inline entityid create_prop_at_with(const proptype_t type, const vec3 loc) {
        auto df = d_get_floor(dungeon, loc.z);
        auto tile = df_tile_at(df, loc);
        const entityid id = create_prop_with(type);
        if (id == ENTITYID_INVALID) {
            return ENTITYID_INVALID;
        }
        const entityid result = df_add_at(df, id, loc.x, loc.y);
        if (result == ENTITYID_INVALID) {
            return ENTITYID_INVALID;
        }
        ct.set<location>(id, loc);
        return id;
    }




    const inline int place_props() {
        //auto mydefault = [](gamestate& g, entityid id) {};
        //auto set_solid = [](gamestate& g, entityid id) { g.ct.set<solid>(id, true); };
        //auto set_solid_and_pushable = [](gamestate& g, entityid id) {
        //    g.ct.set<solid>(id, true);
        //    g.ct.set<pushable>(id, true);
        //};
        int placed_props = 0;
        for (int z = 0; z < (int)dungeon.floors.size(); z++) {
            auto df = d_get_floor(dungeon, z);
            for (int x = 0; x < df.width; x++) {
                for (int y = 0; y < df.height; y++) {
                    const vec3 loc = {x, y, z};
                    auto tile = df_tile_at(df, loc);
                    if (tile.type == TILE_UPSTAIRS || tile.type == TILE_DOWNSTAIRS)
                        continue;
                    if (tile.can_have_door)
                        continue;
                    if (tile_is_wall(tile)) {
                        const int flip = GetRandomValue(0, 20);
                        if (flip == 0) {
                            const int r = GetRandomValue(1, 3);
                            switch (r) {
                            case 1: {
                                const entityid id = create_prop_at_with(PROP_DUNGEON_BANNER_00, loc);
                                if (id != ENTITYID_INVALID)
                                    placed_props++;
                            } break;
                            case 2: {
                                const entityid id = create_prop_at_with(PROP_DUNGEON_BANNER_01, loc);
                                if (id != ENTITYID_INVALID)
                                    placed_props++;
                            } break;
                            case 3: {
                                const entityid id = create_prop_at_with(PROP_DUNGEON_BANNER_02, loc);
                                if (id != ENTITYID_INVALID)
                                    placed_props++;
                            } break;
                            default: break;
                            }
                        }
                    } else {
                        const int flip = GetRandomValue(0, 20);
                        if (flip == 0) {
                            const entityid id = create_prop_at_with(PROP_DUNGEON_WOODEN_TABLE_00, loc);
                            ct.set<solid>(id, true);
                            if (id != ENTITYID_INVALID)
                                placed_props++;

                        } else if (flip == 1) {
                            const entityid id = create_prop_at_with(PROP_DUNGEON_WOODEN_TABLE_01, loc);
                            if (id != ENTITYID_INVALID) {
                                placed_props++;
                                ct.set<solid>(id, true);
                            }
                        } else if (flip == 2) {
                            const entityid id = create_prop_at_with(PROP_DUNGEON_WOODEN_CHAIR_00, loc);
                            if (id != ENTITYID_INVALID)
                                placed_props++;

                        } else if (flip == 3) {
                            const entityid id = create_prop_at_with(PROP_DUNGEON_STATUE_00, loc);
                            if (id != ENTITYID_INVALID) {
                                placed_props++;
                                ct.set<solid>(id, true);
                                ct.set<pushable>(id, true);
                            }
                        }
                    }
                }
            }
        }
        return placed_props;
    }




    const inline entityid create_weapon_with(with_fun weaponInitFunction) {
        //minfo("create weapon with");
        const entityid id = add_entity();
        ct.set<entitytype>(id, ENTITY_ITEM);
        ct.set<itemtype>(id, ITEM_WEAPON);
        ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
        ct.set<update>(id, true);
        weaponInitFunction(ct, id);
        return id;
    }




    inline with_fun dagger_init() {
        return [](CT& ct, const entityid id) {
            ct.set<name>(id, "dagger");
            ct.set<description>(id, "Stabby stabby.");
            ct.set<weapontype>(id, WEAPON_DAGGER);
            ct.set<damage>(id, (vec3){1, 4, 0});
            ct.set<durability>(id, 100);
            ct.set<max_durability>(id, 100);
            ct.set<rarity>(id, RARITY_COMMON);
        };
    }


    inline with_fun shield_init() {
        return [](CT& ct, const entityid id) {
            ct.set<name>(id, "kite shield");
            ct.set<description>(id, "Standard knight's shield");
            ct.set<shieldtype>(id, SHIELD_KITE);
            ct.set<block_chance>(id, 90);
        };
    }


    inline with_fun potion_init(potiontype_t pt) {
        return [pt](CT& ct, const entityid id) {
            ct.set<potiontype>(id, pt);
            if (pt == POTION_HP_SMALL) {
                ct.set<name>(id, "small healing potion");
                ct.set<description>(id, "a small healing potion");
                ct.set<healing>(id, (vec3){1, 6, 0});
            }
        };
    }




    const inline entityid create_weapon_at_with(ComponentTable& ct, const vec3 loc, with_fun weaponInitFunction) {
        //minfo("create weapon at with: %d %d %d", loc.x, loc.y, loc.z);
        if (dungeon.floors.size() == 0) {
            merror("dungeon floors size is 0");
            return ENTITYID_INVALID;
        }
        if (!dungeon.is_initialized) {
            merror("dungeon is_initialized flag not set");
            return ENTITYID_INVALID;
        }
        auto df = d_get_floor(dungeon, loc.z);
        auto tile = df_tile_at(df, loc);
        //minfo("checking if tile is walkable...");
        if (!tile_is_walkable(tile.type)) {
            merror("cannot create entity on non-walkable tile");
            return ENTITYID_INVALID;
        }
        //minfo("checking if tile has live NPCs");
        if (tile_has_live_npcs(tile)) {
            merror("cannot create entity on tile with live NPCs");
            return ENTITYID_INVALID;
        }
        //minfo("creating weapon...");
        const auto id = create_weapon_with(weaponInitFunction);
        if (id == ENTITYID_INVALID) {
            minfo("failed to create weapon");
            return ENTITYID_INVALID;
        }
        //minfo("attempting df_add_at: %d, %d ", id, loc.x, loc.y);
        if (df_add_at(df, id, loc.x, loc.y) == ENTITYID_INVALID) {
            minfo("failed to add weapon to df");
            return ENTITYID_INVALID;
        }
        ct.set<location>(id, loc);
        return id;
    }




    const inline entityid create_shield_with(with_fun shieldInitFunction) {
        const auto id = add_entity();
        ct.set<entitytype>(id, ENTITY_ITEM);
        ct.set<itemtype>(id, ITEM_SHIELD);
        ct.set<durability>(id, 100);
        ct.set<max_durability>(id, 100);
        ct.set<rarity>(id, RARITY_COMMON);
        shieldInitFunction(ct, id);
        return id;
    }




    const inline entityid create_shield_at_with(const vec3 loc, with_fun shieldInitFunction) {
        if (dungeon.floors.size() == 0)
            return ENTITYID_INVALID;
        const auto id = create_shield_with(shieldInitFunction);
        //minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);
        auto df = d_get_floor(dungeon, loc.z);
        if (!df_add_at(df, id, loc.x, loc.y))
            return ENTITYID_INVALID;
        ct.set<location>(id, loc);
        return id;
    }




    const inline entityid create_potion_with(with_fun potionInitFunction) {
        const auto id = add_entity();
        ct.set<entitytype>(id, ENTITY_ITEM);
        ct.set<itemtype>(id, ITEM_POTION);
        potionInitFunction(ct, id);
        return id;
    }




    const inline entityid create_potion_at_with(const vec3 loc, with_fun potionInitFunction) {
        auto df = d_get_floor(dungeon, loc.z);
        auto tile = df_tile_at(df, loc);
        if (!tile_is_walkable(tile.type))
            return ENTITYID_INVALID;
        if (tile_has_live_npcs(tile))
            return ENTITYID_INVALID;
        const auto id = create_potion_with(potionInitFunction);
        if (id == ENTITYID_INVALID)
            return ENTITYID_INVALID;
        //minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);
        if (!df_add_at(df, id, loc.x, loc.y))
            return ENTITYID_INVALID;
        ct.set<location>(id, loc);
        ct.set<update>(id, true);
        return id;
    }




    const inline race_t random_monster_type() {
        const vector<race_t> monster_races = {RACE_GOBLIN, RACE_ORC, RACE_BAT, RACE_WOLF, RACE_WARG, RACE_ZOMBIE, RACE_SKELETON, RACE_RAT, RACE_GREEN_SLIME};
        const int random_index = GetRandomValue(0, monster_races.size() - 1);
        return monster_races[random_index];
    }




    inline void set_npc_starting_stats(const entityid id) {
        const race_t rt = ct.get<race>(id).value_or(RACE_NONE);
        if (rt == RACE_NONE)
            return;
        // stats racial modifiers for stats
        const int str_m = get_racial_modifiers(rt, 0);
        const int dex_m = get_racial_modifiers(rt, 1);
        const int int_m = get_racial_modifiers(rt, 2);
        const int wis_m = get_racial_modifiers(rt, 3);
        const int con_m = get_racial_modifiers(rt, 4);
        const int cha_m = get_racial_modifiers(rt, 5);
        // default to 3-18 for stats
        const int strength_ = GetRandomValue(3, 18) + str_m;
        const int dexterity_ = GetRandomValue(3, 18) + dex_m;
        const int intelligence_ = GetRandomValue(3, 18) + int_m;
        const int wisdom_ = GetRandomValue(3, 18) + wis_m;
        const int constitution_ = GetRandomValue(3, 18) + con_m;
        const int charisma_ = GetRandomValue(3, 18) + cha_m;
        ct.set<strength>(id, strength_);
        ct.set<dexterity>(id, dexterity_);
        ct.set<intelligence>(id, intelligence_);
        ct.set<wisdom>(id, wisdom_);
        ct.set<constitution>(id, constitution_);
        ct.set<charisma>(id, charisma_);
        // set default hp/maxhp for now
        // later, we will decide this by race templating
        vec3 hitdie = {1, 8, 0};
        switch (rt) {
        case RACE_HUMAN: hitdie.y = 8; break;
        case RACE_ELF: hitdie.y = 6; break;
        case RACE_DWARF: hitdie.y = 10; break;
        case RACE_HALFLING: hitdie.y = 6; break;
        case RACE_GOBLIN: hitdie.y = 6; break;
        case RACE_ORC: hitdie.y = 8; break;
        case RACE_BAT: hitdie.y = 3; break;
        case RACE_GREEN_SLIME: hitdie.y = 4; break;
        case RACE_WOLF: hitdie.y = 6; break;
        case RACE_WARG: hitdie.y = 12; break;
        case RACE_RAT: hitdie.y = 4; break;
        case RACE_SKELETON: hitdie.y = 8; break;
        case RACE_ZOMBIE: hitdie.y = 8; break;
        default: break;
        }
        const int my_maxhp = GetRandomValue(1, hitdie.y) + get_stat_bonus(constitution_);
        const int my_hp = my_maxhp;
        ct.set<maxhp>(id, my_maxhp);
        ct.set<hp>(id, my_hp);
        ct.set<base_ac>(id, 10);
        ct.set<hd>(id, hitdie);
    }




    inline void set_npc_defaults(const entityid id) {
        ct.set<entitytype>(id, ENTITY_NPC);
        ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
        ct.set<dead>(id, false);
        ct.set<update>(id, true);
        ct.set<direction>(id, DIR_DOWN_RIGHT);
        ct.set<attacking>(id, false);
        ct.set<blocking>(id, false);
        ct.set<block_success>(id, false);
        ct.set<damaged>(id, false);
        ct.set<txalpha>(id, 0);
        ct.set<inventory>(id, make_shared<vector<entityid>>());
        ct.set<equipped_weapon>(id, ENTITYID_INVALID);
        ct.set<aggro>(id, false);
        ct.set<vision_distance>(id, 3);
        ct.set<hearing_distance>(id, 3);
        // here we have some hard decisions to make about how to template-out NPC creation
        // all NPCs ben at level 1. level-up mechanisms will be determined elsewhere
        ct.set<level>(id, 1);
        ct.set<xp>(id, 0);
    }




    const inline entityid create_npc_with(const race_t rt, with_fun npcInitFunction) {
        const auto id = add_entity();
        set_npc_defaults(id);
        ct.set<race>(id, rt);
        set_npc_starting_stats(id);
        npcInitFunction(ct, id);
        return id;
    }




    const inline entityid tile_has_box(const int x, const int y, const int z) {
        massert(z >= 0, "floor is out of bounds");
        massert((size_t)z < dungeon.floors.size(), "floor is out of bounds");
        auto df = d_get_floor(dungeon, z);
        auto t = df_tile_at(df, (vec3){x, y, z});
        for (int i = 0; (size_t)i < t.entities->size(); i++) {
            const entityid id = tile_get_entity(t, i);
            const entitytype_t type = ct.get<entitytype>(id).value_or(ENTITY_NONE);
            if (id != ENTITYID_INVALID && type == ENTITY_BOX)
                return id;
        }
        return ENTITYID_INVALID;
    }




    const inline entityid create_npc_at_with(const race_t rt, const vec3 loc, with_fun npcInitFunction) {
        dungeon_floor_t& df = d_get_floor(dungeon, loc.z);
        tile_t& tile = df_tile_at(df, loc);
        if (!tile_is_walkable(tile.type)) {
            merror("cannot create entity on non-walkable tile");
            return ENTITYID_INVALID;
        }
        if (tile_has_live_npcs(tile)) {
            merror("cannot create entity on tile with live NPCs");
            return ENTITYID_INVALID;
        }
        if (tile_has_box(loc.x, loc.y, loc.z) != ENTITYID_INVALID) {
            merror("cannot create entity on tile with box");
            return ENTITYID_INVALID;
        }
        const entityid id = create_npc_with(rt, npcInitFunction);
        if (!df_add_at(df, id, loc.x, loc.y)) {
            return ENTITYID_INVALID;
        }
        ct.set<location>(id, loc);
        return id;
    }




    const inline bool add_to_inventory(const entityid actor_id, const entityid item_id) {
        //minfo("add to inventory: %d %d", actor_id, item_id);
        auto maybe_inventory = ct.get<inventory>(actor_id);
        if (!maybe_inventory.has_value()) {
            merror("no inventory: %d", actor_id);
            return false;
        }
        auto my_inventory = maybe_inventory.value();
        my_inventory->push_back(item_id);
        msuccess("item id %d added successfully to actor %d's inventory", item_id, actor_id);
        msuccess("inventory size: %ld", my_inventory->size());
        return true;
    }




    const inline entityid create_random_monster_with(with_fun monsterInitFunction) {
        //const race_t r = random_monster_type();
        //minfo("create random monster with");
        const race_t r = RACE_ORC;
        const entityid id = create_npc_with(r, monsterInitFunction);
        //minfo("create weapon");

        const entityid wpn_id = create_weapon_with([](CT& ct, const entityid id) {
            ct.set<name>(id, "Dagger");
            ct.set<description>(id, "Stabby stabby.");
            ct.set<weapontype>(id, WEAPON_DAGGER);
            ct.set<damage>(id, (vec3){1, 4, 0});
            ct.set<durability>(id, 100);
            ct.set<max_durability>(id, 100);
            ct.set<rarity>(id, RARITY_COMMON);
        });

        //minfo("create potion");
        //const entityid potion_id = create_potion_at_with(df_get_random_loc(dungeon.floors[0]));
        const entityid potion_id = create_potion_with([](CT& ct, const entityid id) {
            ct.set<name>(id, "small healing potion");
            ct.set<description>(id, "a small healing potion");
            ct.set<potiontype>(id, POTION_HP_SMALL);
            ct.set<healing>(id, (vec3){1, 6, 0});
        });

        msuccess("created potion");
        //minfo("add to inventory");
        add_to_inventory(id, wpn_id);
        //minfo("add to inventory");
        add_to_inventory(id, potion_id);
        ct.set<equipped_weapon>(id, wpn_id);
        ct.set<name>(id, get_random_orc_name());
        return id;
    }




    const inline entityid create_random_monster_at_with(const vec3 loc, with_fun monsterInitFunction) {
        if (vec3_equal(loc, (vec3){-1, -1, -1}))
            return ENTITYID_INVALID;
        auto df = d_get_floor(dungeon, loc.z);
        auto tile = df_tile_at(df, loc);
        if (!tile_is_walkable(tile.type))
            return ENTITYID_INVALID;
        if (tile_has_live_npcs(tile))
            return ENTITYID_INVALID;
        const auto id = create_random_monster_with(monsterInitFunction);
        if (id == ENTITYID_INVALID)
            return ENTITYID_INVALID;
        if (!df_add_at(df, id, loc.x, loc.y))
            return ENTITYID_INVALID;
        ct.set<location>(id, loc);
        ct.set<update>(id, true);
        return id;
    }




    inline void add_message(const char* fmt, ...) {
        massert(fmt, "format string is NULL");
        char buffer[MAX_MSG_LENGTH];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, MAX_MSG_LENGTH - 1, fmt, args);
        va_end(args);
        string s(buffer);
        msg_system.push_back(s);
        msg_system_is_active = true;
    }




    inline void add_message_history(const char* fmt, ...) {
        massert(fmt, "format string is NULL");
        char buffer[MAX_MSG_LENGTH];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, MAX_MSG_LENGTH - 1, fmt, args);
        va_end(args);
        string s(buffer);
        msg_history.push_back(s);
    }




    inline void update_tile(tile_t& tile) {
        tile.explored = true;
        tile.visible = true;
    }




    inline void update_player_tiles_explored() {
        if (hero_id == ENTITYID_INVALID)
            return;
        auto df = d_get_current_floor(dungeon);
        auto maybe_loc = ct.get<location>(hero_id);
        if (!maybe_loc.has_value())
            return;
        const vec3 loc = maybe_loc.value();
        const int light_radius0 = ct.get<light_radius>(hero_id).value_or(1);
        // Precompute bounds for the loops
        const int min_x = std::max(0, loc.x - light_radius0);
        const int max_x = std::min(df.width - 1, loc.x + light_radius0);
        const int min_y = std::max(0, loc.y - light_radius0);
        const int max_y = std::min(df.height - 1, loc.y + light_radius0);
        for (int y = min_y; y <= max_y; y++) {
            for (int x = min_x; x <= max_x; x++) {
                // Calculate Manhattan distance for diamond shape
                if (abs(x - loc.x) + abs(y - loc.y) > light_radius0)
                    continue;
                update_tile(df_tile_at(df, (vec3){x, y, loc.z}));
            }
        }
    }




    inline void update_player_state() {
        if (hero_id == ENTITYID_INVALID)
            return;
        const unsigned char a = ct.get<txalpha>(hero_id).value_or(255);
        if (a < 255)
            ct.set<txalpha>(hero_id, a + 1);
        if (ct.get<dead>(hero_id).value_or(true)) {
            gameover = true;
            return;
        }
        ct.set<blocking>(hero_id, false);
        ct.set<block_success>(hero_id, false);
        ct.set<damaged>(hero_id, false);
    }




    inline void update_spells_state() {
        for (entityid id = 0; id < next_entityid; id++) {
            if (id == hero_id || ct.get<entitytype>(id).value_or(ENTITY_NONE) != ENTITY_SPELL)
                continue;
            unsigned char a = ct.get<txalpha>(id).value_or(255);
            if (a < 255)
                a++;
            ct.set<txalpha>(id, a);
            const bool is_complete = ct.get<spell_complete>(id).value_or(false);
            const bool is_destroyed = ct.get<destroyed>(id).value_or(false);
            if (is_complete && is_destroyed) {
                // remove it from the tile
                auto df = d_get_current_floor(dungeon);
                auto loc = ct.get<location>(id).value_or((vec3){-1, -1, -1});
                df_remove_at(df, id, loc.x, loc.y);
            }
        }
    }




    inline void update_npcs_state() {
        for (entityid id = 0; id < next_entityid; id++) {
            if (id == hero_id || ct.get<entitytype>(id).value_or(ENTITY_NONE) != ENTITY_NPC)
                continue;
            unsigned char a = ct.get<txalpha>(id).value_or(255);
            if (a < 255)
                a++;
            ct.set<txalpha>(id, a);
            ct.set<damaged>(id, false);
        }
    }




    inline void logic_init() {
        srand(time(NULL));
        SetRandomSeed(time(NULL));
        //minfo("gamestate.logic_init");

        init_dungeon(1);

        massert(dungeon.floors.size() > 0, "dungeon.floors.size is 0");

        place_doors();
        place_props();

        const vec3 loc = df_get_random_loc(dungeon.floors[0]);

        create_weapon_at_with(ct, loc, dagger_init());
        //create_shield_at_with(df_get_random_loc(dungeon.floors[0]), [](CT& ct, const entityid id) {
        create_shield_at_with(df_get_random_loc(dungeon.floors[0]), shield_init());
        create_potion_at_with(df_get_random_loc(dungeon.floors[0]), potion_init(POTION_HP_SMALL));

        //minfo("creating monsters...");
        for (int i = 0; i < (int)dungeon.floors.size(); i++) {
            for (int j = 1; j <= i + 4; j++) {
                const vec3 random_loc = df_get_random_loc(d_get_floor(dungeon, i));
                create_random_monster_at_with(random_loc, [](CT& ct, const entityid id) {});
            }
        }

        msuccess("end creating monsters...");
        add_message("Welcome to the game! Press enter to cycle messages.");
#ifdef START_MESSAGES
        add_message("To move around, press [q w e a d z x c]");
        add_message("To pick up items, press / ");
        add_message("To manage inventory, press i ");
        add_message("To equip/unequip an item, highlight and press e ");
        add_message("To drop an item, highlight and press q ");
        add_message("To attack, press ' ");
        add_message("To go up/down a floor, press . ");
        add_message("To wait a turn, press s s ");
        add_message("To change direction, press s and then [q w e a d z x c] ");
        add_message("To open a door, face it and press o ");
#endif
        msuccess("liblogic_init: Game state initialized");
    }




    inline void handle_input_title_scene(const inputstate& is) {
        if (inputstate_is_pressed(is, KEY_ESCAPE)) {
            do_quit = true;
            return;
        }

        if (inputstate_is_pressed(is, KEY_ENTER) || inputstate_is_pressed(is, KEY_SPACE)) {
            //minfo("Title screen input detected, switching to main menu");
            current_scene = SCENE_MAIN_MENU;
            frame_dirty = true;
            PlaySound(sfx[SFX_CONFIRM_01]);
        }
    }




    inline void handle_input_main_menu_scene(const inputstate& is) {
        if (inputstate_is_pressed(is, KEY_ENTER) || inputstate_is_pressed(is, KEY_SPACE)) {
            if (title_screen_selection == 0) {
                //minfo("Switching to character creation scene");
                current_scene = SCENE_CHARACTER_CREATION;
                frame_dirty = true;
            }
            PlaySound(sfx.at(SFX_CONFIRM_01));
        } else if (inputstate_is_pressed(is, KEY_DOWN)) {
            //minfo("Title screen selection++");
            title_screen_selection++;
            if (title_screen_selection >= max_title_screen_selections) {
                title_screen_selection = 0;
            }
            PlaySound(sfx.at(SFX_CONFIRM_01));
        } else if (inputstate_is_pressed(is, KEY_UP)) {
            //minfo("Title screen selection--");
            title_screen_selection--;
            if (title_screen_selection < 0) {
                title_screen_selection = max_title_screen_selections - 1;
            }
            PlaySound(sfx.at(SFX_CONFIRM_01));
        } else if (inputstate_is_pressed(is, KEY_ESCAPE)) {
            do_quit = true;
            PlaySound(sfx.at(SFX_CONFIRM_01));
        }
        frame_dirty = true;
    }




    const inline entityid create_player(const vec3 loc, const string n) {
        massert(n != "", "name is empty string");
        //minfo("Creating player...");
        race_t rt = chara_creation.race;
        //minfo("Race: %s", race2str(rt).c_str());
        const auto id = create_npc_at_with(rt, loc, [](CT& ct, const entityid id) {});
        const int hp_ = 10;
        const int maxhp_ = 10;
        const int vis_dist = 3;
        const int light_rad = 3;
        const int hear_dist = 3;
        const entitytype_t type = ENTITY_PLAYER;
        set_hero_id(id);
        ct.set<entitytype>(id, type);
        ct.set<txalpha>(id, 0);
        ct.set<hp>(id, hp_);
        ct.set<maxhp>(id, maxhp_);
        ct.set<vision_distance>(id, vis_dist);
        ct.set<light_radius>(id, light_rad);
        ct.set<hearing_distance>(id, hear_dist);
        ct.set<name>(id, n);
        //minfo("Adding inventory to entity id %d", id);
        msuccess("create_player successful, id: %d", id);
        return id;
    }




    const inline entityid create_box_with() {
        const auto id = add_entity();
        ct.set<entitytype>(id, ENTITY_BOX);
        ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
        ct.set<update>(id, true);
        ct.set<pushable>(id, true);
        //boxInitFunction(g, id);
        return id;
    }




    const inline entityid create_box_at_with(const vec3 loc) {
        auto df = d_get_floor(dungeon, loc.z);
        auto tile = df_tile_at(df, loc);
        if (!tile_is_walkable(tile.type)) {
            merror("cannot create entity on non-walkable tile");
            return ENTITYID_INVALID;
        }
        if (tile_has_live_npcs(tile)) {
            merror("cannot create entity on tile with live NPCs");
            return ENTITYID_INVALID;
        }
        if (tile_has_box(loc.x, loc.y, loc.z) != ENTITYID_INVALID) {
            merror("cannot create entity on tile with box");
            return ENTITYID_INVALID;
        }
        const auto id = create_box_with();
        if (id == ENTITYID_INVALID) {
            return ENTITYID_INVALID;
        }
        //minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);
        if (!df_add_at(df, id, loc.x, loc.y)) {
            merror("failed df_add_at: %d, %d, %d", id, loc.x, loc.y);
            return ENTITYID_INVALID;
        }
        ct.set<location>(id, loc);
        return id;
    }




    const inline entityid create_spell_with() {
        const auto id = add_entity();
        ct.set<entitytype>(id, ENTITY_SPELL);
        ct.set<spelltype>(id, SPELLTYPE_FIRE);
        ct.set<spellstate>(id, SPELLSTATE_NONE);
        return id;
    }




    const inline entityid create_spell_at_with(const vec3 loc) {
        auto df = d_get_floor(dungeon, loc.z);
        auto tile = df_tile_at(df, loc);
        if (!tile_is_walkable(tile.type))
            return ENTITYID_INVALID;
        const auto id = create_spell_with();
        if (id == ENTITYID_INVALID)
            return ENTITYID_INVALID;
        //minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);
        if (!df_add_at(df, id, loc.x, loc.y))
            return ENTITYID_INVALID;
        ct.set<location>(id, loc);
        ct.set<update>(id, true);
        return id;
    }




    inline void handle_input_character_creation_scene(const inputstate& is) {
        if (inputstate_is_pressed(is, KEY_ESCAPE)) {
            do_quit = true;
            return;
        }
        if (inputstate_is_pressed(is, KEY_ENTER)) {
            //minfo("Character creation confirmed");
            PlaySound(sfx.at(SFX_CONFIRM_01));
            //play_sound(SFX_CONFIRM_01);
            // we need to copy the character creation stats to the hero entity
            // hero has already been created, so its id is available
            const int myhd = chara_creation.hitdie;
            int maxhp_roll = -1;
            while (maxhp_roll < 1)
                maxhp_roll = do_roll_best_of_3((vec3){1, myhd, 0}) + get_stat_bonus(chara_creation.constitution);
            //const vec3 start_loc = g.dungeon.floors->at(g.dungeon.current_floor)->upstairs_loc;
            const vec3 start_loc = dungeon.floors[dungeon.current_floor].upstairs_loc;
            entity_turn = create_player(start_loc, "darkmage");
            massert(hero_id != ENTITYID_INVALID, "heroid is invalid");
            // set stats from char_creation
            ct.set<strength>(hero_id, chara_creation.strength);
            ct.set<dexterity>(hero_id, chara_creation.dexterity);
            ct.set<constitution>(hero_id, chara_creation.constitution);
            ct.set<intelligence>(hero_id, chara_creation.intelligence);
            ct.set<wisdom>(hero_id, chara_creation.wisdom);
            ct.set<charisma>(hero_id, chara_creation.charisma);

            ct.set<hd>(hero_id, (vec3){1, chara_creation.hitdie, 0});
            ct.set<hp>(hero_id, maxhp_roll);
            ct.set<maxhp>(hero_id, maxhp_roll);
            // temporary wedge-in code
            // set all the NPCs to target the hero
            //minfo("BEGIN Temporary wedge-in code");
            for (entityid id = 0; id < next_entityid; id++) {
                //minfo("Getting type for id %d", id);
                entitytype_t t = ct.get<entitytype>(id).value_or(ENTITY_NONE);
                if (t == ENTITY_NPC) {
                    //minfo("Setting target for id %d", id);
                    ct.set<target_id>(id, hero_id);
                }
            }
            //minfo("END Temporary wedge-in code");
            current_scene = SCENE_GAMEPLAY;
        } else if (inputstate_is_pressed(is, KEY_SPACE)) {
            // re-roll character creation stats
            //minfo("Re-rolling character creation stats");
            PlaySound(sfx.at(SFX_CONFIRM_01));
            chara_creation.strength = do_roll_best_of_3((vec3){3, 6, 0});
            chara_creation.dexterity = do_roll_best_of_3((vec3){3, 6, 0});
            chara_creation.intelligence = do_roll_best_of_3((vec3){3, 6, 0});
            chara_creation.wisdom = do_roll_best_of_3((vec3){3, 6, 0});
            chara_creation.constitution = do_roll_best_of_3((vec3){3, 6, 0});
            chara_creation.charisma = do_roll_best_of_3((vec3){3, 6, 0});
        } else if (inputstate_is_pressed(is, KEY_LEFT)) {
            //PlaySound(g.sfx->at(SFX_CONFIRM_01));
            //play_sound(SFX_CONFIRM_01);
            PlaySound(sfx.at(SFX_CONFIRM_01));
            int race = chara_creation.race;
            if (chara_creation.race > 1)
                race--;
            else
                race = RACE_COUNT - 1;
            chara_creation.race = (race_t)race;
            chara_creation.hitdie = get_racial_hd(chara_creation.race);
        } else if (inputstate_is_pressed(is, KEY_RIGHT)) {
            //PlaySound(g.sfx->at(SFX_CONFIRM_01));
            //play_sound(SFX_CONFIRM_01);
            PlaySound(sfx.at(SFX_CONFIRM_01));
            int race = chara_creation.race;
            if (race < RACE_COUNT - 1)
                race++;
            else
                race = RACE_NONE + 1;
            chara_creation.race = (race_t)race;
            chara_creation.hitdie = get_racial_hd(chara_creation.race);
        }
        frame_dirty = true;
    }




    const inline bool remove_from_inventory(const entityid actor_id, const entityid item_id) {
        //minfo("remove from inventory: %d %d", actor_id, item_id);
        auto maybe_inventory = ct.get<inventory>(actor_id);
        if (!maybe_inventory.has_value()) {
            merror("maybe_inventory has no value for actor id %d", actor_id);
            return false;
        }
        auto my_items = maybe_inventory.value();
        bool success = false;
        //minfo("Inventory size: %ld", my_items->size());
        for (auto it = my_items->begin(); it != my_items->end(); it++) {
            //minfo("item id %d", *it);
            if (*it == item_id) {
                my_items->erase(it);
                success = true;
                break;
            }
        }

        if (!success) {
            merror("Failed to find item id %d", item_id);
        } else {
            msuccess("Successfully removed item id %d", item_id);
        }
        return success;
    }




    const inline bool drop_from_inventory(const entityid actor_id, const entityid item_id) {
        //minfo("drop from inventory: %d %d", actor_id, item_id);
        if (remove_from_inventory(actor_id, item_id)) {
            auto maybe_loc = ct.get<location>(actor_id);
            if (!maybe_loc.has_value()) {
                merror("actor id %d has no location -- cannot drop item", actor_id);
                return false;
            }
            auto loc = maybe_loc.value();
            auto df = d_get_current_floor(dungeon);
            auto tile = df_tile_at(df, loc);
            const entityid retval = df_add_at(df, item_id, loc.x, loc.y);
            if (retval == ENTITYID_INVALID) {
                merror("Failed to add to tile");
                return false;
            }
            // update the item_id's location
            ct.set<location>(item_id, loc);
            msuccess("Drop item successful");
            return true;
        }
        merror("Remove from inventory failed for some reason");
        return false;
    }




    const inline bool drop_all_from_inventory(entityid actor_id) {
        //minfo("drop all from inventory: %d", actor_id);
        auto maybe_inventory = ct.get<inventory>(actor_id);
        if (!maybe_inventory.has_value()) {
            merror("no inventory");
            return false;
        }
        auto inventory = maybe_inventory.value();
        while (inventory->size() > 0) {
            auto id = inventory->back();
            drop_from_inventory(actor_id, id);
        }
        return true;
    }




    inline void handle_hero_inventory_equip_weapon(const entityid item_id) {
        // Check if this is the currently equipped weapon
        const entityid current_weapon = ct.get<equipped_weapon>(hero_id).value_or(ENTITYID_INVALID);
        // Unequip if it's already equipped
        if (current_weapon == item_id)
            ct.set<equipped_weapon>(hero_id, ENTITYID_INVALID);
        // Equip the new weapon
        else
            ct.set<equipped_weapon>(hero_id, item_id);
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
        controlmode = CONTROLMODE_PLAYER;
        display_inventory_menu = false;
    }




    inline void handle_hero_inventory_equip_shield(const entityid item_id) {
        // Check if this is the currently equipped weapon
        const entityid current_shield = ct.get<equipped_shield>(hero_id).value_or(ENTITYID_INVALID);
        // Unequip if it's already equipped
        if (current_shield == item_id)
            ct.set<equipped_shield>(hero_id, ENTITYID_INVALID);
        // Equip the new shield
        else
            ct.set<equipped_shield>(hero_id, item_id);
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
        controlmode = CONTROLMODE_PLAYER;
        display_inventory_menu = false;
    }




    inline void handle_hero_inventory_equip_item(const entityid item_id) {
        itemtype_t item_type = ct.get<itemtype>(item_id).value_or(ITEM_NONE);
        switch (item_type) {
        case ITEM_NONE: break;
        case ITEM_WEAPON: handle_hero_inventory_equip_weapon(item_id); break;
        case ITEM_SHIELD: handle_hero_inventory_equip_shield(item_id); break;
        default: break;
        }
    }




    inline void handle_hero_inventory_equip() {
        PlaySound(sfx.at(SFX_EQUIP_01));
        // equip item
        // get the item id of the current selection
        const size_t index = inventory_cursor.y * 7 + inventory_cursor.x;
        auto my_inventory = ct.get<inventory>(hero_id);
        if (!my_inventory)
            return;
        if (!my_inventory.has_value())
            return;
        auto unpacked_inventory = my_inventory.value();
        if (index < 0 || index >= unpacked_inventory->size())
            return;
        entityid item_id = unpacked_inventory->at(index);
        entitytype_t type = ct.get<entitytype>(item_id).value_or(ENTITY_NONE);
        if (type == ENTITY_ITEM) {
            handle_hero_inventory_equip_item(item_id);
        }
    }




    const inline bool drop_item_from_hero_inventory() {
        if (!ct.has<inventory>(hero_id))
            return false;
        size_t index = inventory_cursor.y * 7 + inventory_cursor.x;
        auto maybe_inventory = ct.get<inventory>(hero_id);
        if (!maybe_inventory.has_value())
            return false;
        auto inventory = maybe_inventory.value();
        if (index < 0 || index >= inventory->size())
            return false;
        entityid item_id = inventory->at(index);
        inventory->erase(inventory->begin() + index);
        if (item_id == ct.get<equipped_weapon>(hero_id).value_or(ENTITYID_INVALID))
            ct.set<equipped_weapon>(hero_id, ENTITYID_INVALID);
        // add it to the tile at the player's current location
        // get the player's location
        vec3 loc = ct.get<location>(hero_id).value();
        // get the tile at the player's location
        auto df = d_get_current_floor(dungeon);
        if (!df_add_at(df, item_id, loc.x, loc.y)) {
            merror("Failed to add to %d, %d, %d", loc.x, loc.y, loc.z);
            return false;
        }
        // update the entity's location
        ct.set<location>(item_id, loc);
        // add to tile
        return true;
    }




    const inline bool is_in_inventory(const entityid actor_id, const entityid item_id) {
        auto maybe_inventory = ct.get<inventory>(actor_id);
        if (!maybe_inventory.has_value()) {
            merror("maybe_inventory has no value for actor id %d", actor_id);
            return false;
        }
        auto my_items = maybe_inventory.value();
        for (auto it = my_items->begin(); it != my_items->end(); it++) {
            //minfo("item id %d", *it);
            if (*it == item_id)
                return true;
        }
        return false;
    }




    const inline bool use_potion(const entityid actor_id, const entityid item_id) {
        massert(actor_id != ENTITYID_INVALID, "actor_id is invalid");
        massert(item_id != ENTITYID_INVALID, "actor_id is invalid");

        const bool is_item = ct.get<entitytype>(item_id).value_or(ENTITY_NONE) == ENTITY_ITEM;
        const bool is_potion = ct.get<itemtype>(item_id).value_or(ITEM_NONE) == ITEM_POTION;
        const bool in_inventory = is_in_inventory(actor_id, item_id);

        if (is_item && is_potion && in_inventory) {
            // get the item's effects
            auto maybe_heal = ct.get<healing>(item_id);
            if (maybe_heal && maybe_heal.has_value()) {
                const auto heal = maybe_heal.value();
                const int amount = do_roll(heal);
                const int myhp = ct.get<hp>(actor_id).value_or(-1);
                const int mymaxhp = ct.get<maxhp>(actor_id).value_or(-1);
                ct.set<hp>(actor_id, mymaxhp ? mymaxhp : myhp + amount);

                if (actor_id == hero_id) {
                    string n = ct.get<name>(actor_id).value_or("no-name");
                    add_message_history("%s used a healing potion", n.c_str());
                    add_message_history("%s restored %d hp", n.c_str(), amount);
                }
            } else {
                merror("Potion has no healing component");
                return false;
            }
            // consume the potion by removing it
            remove_from_inventory(actor_id, item_id);
            return true;
        }
        merror("id %d is not an item, potion, or isnt in the inventory", item_id);
        return false;
    }




    inline void logic_close() {
        d_destroy(dungeon);
    }




    inline void handle_camera_move(const inputstate& is) {
        const float move = cam2d.zoom;
        if (inputstate_is_held(is, KEY_RIGHT)) {
            cam2d.offset.x += move;
        } else if (inputstate_is_held(is, KEY_LEFT)) {
            cam2d.offset.x -= move;
        } else if (inputstate_is_held(is, KEY_UP)) {
            cam2d.offset.y -= move;
        } else if (inputstate_is_held(is, KEY_DOWN)) {
            cam2d.offset.y += move;
        }
    }




    inline void handle_hero_potion_use(const entityid id) {
        const entitytype_t type = ct.get<entitytype>(id).value_or(ENTITY_NONE);
        if (type != ENTITY_ITEM)
            return;
        const itemtype_t i_type = ct.get<itemtype>(id).value_or(ITEM_NONE);
        if (i_type == ITEM_NONE || i_type != ITEM_POTION)
            return;
        if (use_potion(hero_id, id)) {
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            controlmode = CONTROLMODE_PLAYER;
            display_inventory_menu = false;
        }
    }




    inline void handle_hero_item_use() {
        const size_t index = inventory_cursor.y * 7 + inventory_cursor.x;
        if (index < 0)
            return;
        auto maybe_inventory = ct.get<inventory>(hero_id);
        if (!maybe_inventory || !maybe_inventory.has_value())
            return;
        auto inventory = maybe_inventory.value();
        if (index >= inventory->size())
            return;
        entityid item_id = inventory->at(index);
        entitytype_t type = ct.get<entitytype>(item_id).value_or(ENTITY_NONE);
        if (type != ENTITY_ITEM)
            return;
        itemtype_t i_type = ct.get<itemtype>(item_id).value_or(ITEM_NONE);
        if (i_type == ITEM_NONE)
            return;
        if (i_type == ITEM_POTION) {
            handle_hero_potion_use(item_id);
        }
    }




    inline void handle_input_inventory(const inputstate& is) {
        if (controlmode != CONTROLMODE_INVENTORY)
            return;
        if (!display_inventory_menu)
            return;
        if (inputstate_is_pressed(is, KEY_ESCAPE)) {
            do_quit = true;
            return;
        }
        if (inputstate_is_pressed(is, KEY_I)) {
            controlmode = CONTROLMODE_PLAYER;
            display_inventory_menu = false;
            PlaySound(sfx[SFX_BAG_CLOSE]);
            return;
        }
        if (inputstate_is_pressed(is, KEY_LEFT) || inputstate_is_pressed(is, KEY_A)) {
            PlaySound(sfx[SFX_CONFIRM_01]);
            if (inventory_cursor.x > 0)
                inventory_cursor.x--;
        }
        if (inputstate_is_pressed(is, KEY_RIGHT) || inputstate_is_pressed(is, KEY_D)) {
            PlaySound(sfx[SFX_CONFIRM_01]);
            inventory_cursor.x++;
        }
        if (inputstate_is_pressed(is, KEY_UP) || inputstate_is_pressed(is, KEY_W)) {
            PlaySound(sfx[SFX_CONFIRM_01]);
            if (inventory_cursor.y > 0)
                inventory_cursor.y--;
        }
        if (inputstate_is_pressed(is, KEY_DOWN) || inputstate_is_pressed(is, KEY_X)) {
            PlaySound(sfx[SFX_CONFIRM_01]);
            inventory_cursor.y++;
        }
        if (inputstate_is_pressed(is, KEY_E)) {
            handle_hero_inventory_equip();
        }
        if (inputstate_is_pressed(is, KEY_Q)) {
            // drop item
            PlaySound(sfx[SFX_DISCARD_ITEM]);
            drop_item_from_hero_inventory();
        }
        if (inputstate_is_pressed(is, KEY_ENTER)) {
            handle_hero_item_use();
            PlaySound(sfx[SFX_CONFIRM_01]);
        }
    }




    const inline bool handle_quit_pressed(const inputstate& is) {
        if (inputstate_is_pressed(is, KEY_ESCAPE)) {
            do_quit = true;
            return true;
        }
        return false;
    }




    inline void cycle_messages() {
        if (msg_system.size() > 0) {
            string msg = msg_system.front();
            int len = msg.length();
            // measure the length of the message as calculated by MeasureText
            if (len > msg_history_max_len_msg) {
                msg_history_max_len_msg = len;
                int font_size = 10;
                int measure = MeasureText(msg.c_str(), font_size);
                msg_history_max_len_msg_measure = measure;
            }
            msg_history.push_back(msg_system.front());
            msg_system.erase(msg_system.begin());
        }
        if (msg_system.size() == 0)
            msg_system_is_active = false;
    }




    const inline bool handle_cycle_messages(const inputstate& is) {
        if (msg_system_is_active && inputstate_is_pressed(is, KEY_ENTER)) {
            PlaySound(sfx[SFX_CONFIRM_01]);
            cycle_messages();
            return true;
        }
        return false;
    }




    const inline bool handle_camera_zoom(const inputstate& is) {
        if (inputstate_is_pressed(is, KEY_LEFT_BRACKET)) {
            cam2d.zoom += DEFAULT_ZOOM_INCR;
            frame_dirty = true;
            return true;
        } else if (inputstate_is_pressed(is, KEY_RIGHT_BRACKET)) {
            cam2d.zoom -= (cam2d.zoom > 1.0) ? DEFAULT_ZOOM_INCR : 0.0;
            frame_dirty = true;
            return true;
        }
        return false;
    }




    inline void change_player_dir(const direction_t dir) {
        if (ct.get<dead>(hero_id).value_or(true))
            return;
        ct.set<direction>(hero_id, dir);
        ct.set<update>(hero_id, true);
        player_changing_dir = false;
        frame_dirty = true;
    }




    const inline bool handle_change_dir(const inputstate& is) {
        if (player_changing_dir) {
            auto maybe_player_is_dead = ct.get<dead>(hero_id);
            if (!maybe_player_is_dead.has_value())
                return true;
            const bool is_dead = maybe_player_is_dead.value();
            // double 's' is wait one turn
            if (inputstate_is_pressed(is, KEY_S)) {
                player_changing_dir = false;
                flag = GAMESTATE_FLAG_PLAYER_ANIM;
            } else if (inputstate_is_pressed(is, KEY_UP) || inputstate_is_pressed(is, KEY_W)) {
                change_player_dir(DIR_UP);
            } else if (inputstate_is_pressed(is, KEY_DOWN) || inputstate_is_pressed(is, KEY_X)) {
                change_player_dir(DIR_DOWN);
            } else if (inputstate_is_pressed(is, KEY_LEFT) || inputstate_is_pressed(is, KEY_A)) {
                change_player_dir(DIR_LEFT);
            } else if (inputstate_is_pressed(is, KEY_RIGHT) || inputstate_is_pressed(is, KEY_D)) {
                change_player_dir(DIR_RIGHT);
            } else if (inputstate_is_pressed(is, KEY_Q)) {
                change_player_dir(DIR_UP_LEFT);
            } else if (inputstate_is_pressed(is, KEY_E)) {
                change_player_dir(DIR_UP_RIGHT);
            } else if (inputstate_is_pressed(is, KEY_Z)) {
                change_player_dir(DIR_DOWN_LEFT);
            } else if (inputstate_is_pressed(is, KEY_C)) {
                change_player_dir(DIR_DOWN_RIGHT);
            } else if (inputstate_is_pressed(is, KEY_APOSTROPHE)) {
                // this is incorrect
                // this should be a regular attack
                if (is_dead) {
                    add_message("You cannot attack while dead");
                    return true;
                }
                ct.set<attacking>(hero_id, true);
                ct.set<update>(hero_id, true);
                flag = GAMESTATE_FLAG_PLAYER_ANIM;
                player_changing_dir = false;
            }
            return true;
        }
        return false;
    }




    const inline bool handle_change_dir_intent(const inputstate& is) {
        if (inputstate_is_pressed(is, KEY_S)) {
            player_changing_dir = true;
            return true;
        }
        return false;
    }




    const inline bool handle_display_inventory(const inputstate& is) {
        if (inputstate_is_pressed(is, KEY_I)) {
            display_inventory_menu = true;
            controlmode = CONTROLMODE_INVENTORY;
            frame_dirty = true;
            PlaySound(sfx[SFX_BAG_OPEN]);
            return true;
        }
        return false;
    }




    const inline bool tile_has_solid(const int x, const int y, const int z) {
        massert(z >= 0, "floor is out of bounds");
        massert((size_t)z < dungeon.floors.size(), "floor is out of bounds");
        auto df = d_get_floor(dungeon, z);
        auto t = df_tile_at(df, (vec3){x, y, z});
        for (int i = 0; (size_t)i < t.entities->size(); i++) {
            const entityid id = tile_get_entity(t, i);
            const bool is_solid = ct.get<solid>(id).value_or(false);
            if (id != ENTITYID_INVALID && is_solid)
                return true;
        }
        return false;
    }




    const inline bool handle_box_push(const entityid id, const vec3 v) {
        const bool can_push = ct.get<pushable>(id).value_or(false);
        if (can_push) {
            return try_entity_move(id, v);
        }
        // box is not pushable
        //minfo("Box NOT pushable");
        return false;
    }




    const inline entityid tile_has_pushable(const int x, const int y, const int z) {
        massert(z >= 0, "floor is out of bounds");
        massert((size_t)z < dungeon.floors.size(), "floor is out of bounds");
        auto df = d_get_floor(dungeon, z);
        auto t = df_tile_at(df, (vec3){x, y, z});
        for (int i = 0; (size_t)i < t.entities->size(); i++) {
            const entityid id = tile_get_entity(t, i);
            const bool is_pushable = ct.get<pushable>(id).value_or(false);
            if (id != ENTITYID_INVALID && is_pushable)
                return id;
        }
        return ENTITYID_INVALID;
    }




    const inline entityid tile_has_door(const vec3 v) {
        auto df = d_get_current_floor(dungeon);
        auto t = df_tile_at(df, v);
        for (size_t i = 0; i < t.entities->size(); i++) {
            const entityid id = t.entities->at(i);
            const entitytype_t type = ct.get<entitytype>(id).value_or(ENTITY_NONE);
            if (type == ENTITY_DOOR)
                return id;
        }
        return ENTITYID_INVALID;
    }




    const inline bool check_hearing(const entityid id, const vec3 loc) {
        if (id == ENTITYID_INVALID || vec3_equal(loc, (vec3){-1, -1, -1}))
            return false;
        const vec3 hero_loc = ct.get<location>(hero_id).value_or((vec3){-1, -1, -1});
        if (vec3_equal(hero_loc, (vec3){-1, -1, -1}))
            return false;
        // is the hero on the same floor as loc?
        if (hero_loc.z != loc.z)
            return false;
        const Vector2 p0 = {static_cast<float>(hero_loc.x), static_cast<float>(hero_loc.y)}, p1 = {static_cast<float>(loc.x), static_cast<float>(loc.y)};
        const float dist = Vector2Distance(p0, p1);
        const float hearing = ct.get<hearing_distance>(hero_id).value_or(3);
        return dist <= hearing;
    }




    const inline bool try_entity_move(const entityid id, const vec3 v) {
        massert(id != ENTITYID_INVALID, "Entity ID is invalid!");
        //if (id == hero_id)
        //    minfo("try_entity_move: %d, (%d,%d,%d)", id, v.x, v.y, v.z);
        ct.set<update>(id, true);
        ct.set<direction>(id, get_dir_from_xy(v.x, v.y));
        // entity location
        massert(ct.has<location>(id), "id %d has no location", id);
        const vec3 loc = ct.get<location>(id).value_or((vec3){-1, -1, -1});
        massert(!vec3_equal(loc, (vec3){-1, -1, -1}), "id %d has no location", id);
        // entity's new location
        // we will have a special case for traversing floors so ignore v.z
        const vec3 aloc = {loc.x + v.x, loc.y + v.y, loc.z};
        auto df = d_get_floor(dungeon, loc.z);
        auto tile = df_tile_at(df, aloc);
        if (!tile_is_walkable(tile.type)) {
            return false;
        }


        const entityid box_id = tile_has_box(aloc.x, aloc.y, aloc.z);
        if (box_id != ENTITYID_INVALID) {
            return handle_box_push(box_id, v);
        }

        const entityid pushable_id = tile_has_pushable(aloc.x, aloc.y, aloc.z);
        if (pushable_id != ENTITYID_INVALID) {
            return handle_box_push(pushable_id, v);
        }

        const bool has_solid = tile_has_solid(aloc.x, aloc.y, aloc.z);
        if (has_solid) {
            return false;
        }

        if (tile_has_live_npcs(tile_at_cur_floor(aloc))) {
            return false;
        }

        const entityid door_id = tile_has_door(aloc);
        if (door_id != ENTITYID_INVALID) {
            massert(ct.has<door_open>(door_id), "door_id %d doesnt have a door_open component", door_id);
            if (!ct.get<door_open>(door_id).value_or(false))
                return false;
        }

        // if door, door is open
        // remove the entity from the current tile
        if (!df_remove_at(df, id, loc.x, loc.y)) {
            merror("Failed to remove entity %d from tile at (%d, %d, %d)", id, loc.x, loc.y, loc.z);
            return false;
        }

        // add the entity to the new tile
        if (!df_add_at(df, id, aloc.x, aloc.y)) {
            merror("Failed to add entity %d to tile at (%d, %d, %d)", id, aloc.x, aloc.y, aloc.z);
            return false;
        }
        ct.set<location>(id, aloc);
        const float mx = v.x * DEFAULT_TILE_SIZE, my = v.y * DEFAULT_TILE_SIZE;
        ct.set<spritemove>(id, (Rectangle){mx, my, 0, 0});
        if (check_hearing(hero_id, aloc))
            PlaySound(sfx[SFX_STEP_STONE_1]);
        if (id == hero_id)
            msuccess("try_entity_move: %d, (%d,%d,%d)", id, v.x, v.y, v.z);
        return true;
    }




    const inline bool handle_move_up(const inputstate& is, const bool is_dead) {
        if (inputstate_is_pressed(is, KEY_UP) || inputstate_is_pressed(is, KEY_W)) {
            if (is_dead) {
                add_message("You cannot move while dead");
                return true;
            }
            try_entity_move(hero_id, (vec3){0, -1, 0});
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }




    const inline bool handle_move_down(const inputstate& is, const bool is_dead) {
        if (inputstate_is_pressed(is, KEY_DOWN) || inputstate_is_pressed(is, KEY_X)) {
            if (is_dead) {
                add_message("You cannot move while dead");
                return true;
            }
            try_entity_move(hero_id, (vec3){0, 1, 0});
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }




    const inline bool handle_move_left(const inputstate& is, const bool is_dead) {
        if (inputstate_is_pressed(is, KEY_LEFT) || inputstate_is_pressed(is, KEY_A)) {
            if (is_dead) {
                add_message("You cannot move while dead");
                return true;
            }
            try_entity_move(hero_id, (vec3){-1, 0, 0});
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }




    const inline bool handle_move_right(const inputstate& is, const bool is_dead) {
        if (inputstate_is_pressed(is, KEY_RIGHT) || inputstate_is_pressed(is, KEY_D)) {
            if (is_dead) {
                add_message("You cannot move while dead");
                return true;
            }
            try_entity_move(hero_id, (vec3){1, 0, 0});
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }




    const inline bool handle_move_up_left(const inputstate& is, const bool is_dead) {
        if (inputstate_is_pressed(is, KEY_Q)) {
            if (is_dead) {
                add_message("You cannot move while dead");
                return true;
            }
            try_entity_move(hero_id, (vec3){-1, -1, 0});
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }




    const inline bool handle_move_up_right(const inputstate& is, const bool is_dead) {
        if (inputstate_is_pressed(is, KEY_E)) {
            if (is_dead) {
                add_message("You cannot move while dead");
                return true;
            }
            try_entity_move(hero_id, (vec3){1, -1, 0});
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }




    const inline bool handle_move_down_left(const inputstate& is, const bool is_dead) {
        if (inputstate_is_pressed(is, KEY_Z)) {
            if (is_dead) {
                add_message("You cannot move while dead");
                return true;
            }
            try_entity_move(hero_id, (vec3){-1, 1, 0});
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }




    const inline bool handle_move_down_right(const inputstate& is, const bool is_dead) {
        if (inputstate_is_pressed(is, KEY_C)) {
            if (is_dead) {
                add_message("You cannot move while dead");
                return true;
            }
            try_entity_move(hero_id, (vec3){1, 1, 0});
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }




    const inline vec3 get_loc_facing_player() {
        optional<vec3> maybe_loc = ct.get<location>(hero_id);
        if (maybe_loc.has_value()) {
            vec3 loc = ct.get<location>(hero_id).value();
            direction_t dir = ct.get<direction>(hero_id).value();
            if (dir == DIR_UP) {
                loc.y -= 1;
            } else if (dir == DIR_DOWN) {
                loc.y += 1;
            } else if (dir == DIR_LEFT) {
                loc.x -= 1;
            } else if (dir == DIR_RIGHT) {
                loc.x += 1;
            } else if (dir == DIR_UP_LEFT) {
                loc.x -= 1;
                loc.y -= 1;
            } else if (dir == DIR_UP_RIGHT) {
                loc.x += 1;
                loc.y -= 1;
            } else if (dir == DIR_DOWN_LEFT) {
                loc.x -= 1;
                loc.y += 1;
            } else if (dir == DIR_DOWN_RIGHT) {
                loc.x += 1;
                loc.y += 1;
            }
            return loc;
        }

        return (vec3){-1, -1, -1};
    }




    const inline entityid get_cached_npc(tile_t& t) {
        recompute_entity_cache(t); // Force update
        return t.cached_npc;
    }




    const inline int compute_armor_class(const entityid id) {
        massert(ENTITYID_INVALID != id, "id is invalid");
        const int base_armor_class = ct.get<base_ac>(id).value_or(10);
        const int dex_bonus = get_stat_bonus(ct.get<dexterity>(id).value_or(10));
        // here, we would also grab any armor pieces that we are wearing
        const int total_ac = base_armor_class + dex_bonus;
        return total_ac;
    }




    const inline bool compute_attack_roll(const entityid attacker, const entityid target) {
        const int str = ct.get<strength>(attacker).value_or(10);
        const int bonus = get_stat_bonus(str);
        const int roll = GetRandomValue(1, 20) + bonus;
        const int ac = compute_armor_class(target);
        return roll >= ac;
    }




    const inline int compute_attack_damage(const entityid attacker, const entityid target) {
        const int str = ct.get<strength>(attacker).value_or(10);
        const int bonus = std::max(0, get_stat_bonus(str));
        const entityid equipped_wpn = ct.get<equipped_weapon>(attacker).value_or(ENTITYID_INVALID);
        const vec3 dmg_range = ct.get<damage>(equipped_wpn).value_or(MINIMUM_DAMAGE);
        const int dmg = std::max(1, GetRandomValue(dmg_range.x, dmg_range.y));
        return dmg + bonus;
    }




    inline void handle_weapon_durability_loss(const entityid atk_id, const entityid tgt_id) {
        const entityid equipped_wpn = ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID);
        auto maybe_dura = ct.get<durability>(equipped_wpn);
        if (!maybe_dura.has_value()) {
            return;
        }
        const int dura = maybe_dura.value();
        ct.set<durability>(equipped_wpn, dura - 1 < 0 ? 0 : dura - 1);
        if (dura > 0)
            return;
        // unequip item
        ct.set<equipped_weapon>(atk_id, ENTITYID_INVALID);
        // remove item from attacker's inventory
        remove_from_inventory(atk_id, equipped_wpn);
        // item destroyed
        ct.set<destroyed>(equipped_wpn, true);
        const bool event_heard = check_hearing(hero_id, ct.get<location>(tgt_id).value_or((vec3){-1, -1, -1}));
        if (event_heard)
            PlaySound(sfx[SFX_05_ALCHEMY_GLASS_BREAK]);
        //add_message_history("%s broke!", get_entity_name(g, equipped_wpn).c_str());
        add_message_history("%s broke!", ct.get<name>(equipped_wpn).value_or("no-name-weapon").c_str());
    }




    inline void handle_shield_durability_loss(const entityid defender, const entityid attacker) {
        const entityid shield = ct.get<equipped_shield>(defender).value_or(ENTITYID_INVALID);
        auto maybe_dura = ct.get<durability>(shield);
        if (!maybe_dura.has_value())
            return;
        const int dura = maybe_dura.value();
        ct.set<durability>(shield, dura - 1 < 0 ? 0 : dura - 1);
        if (dura > 0)
            return;
        // unequip item
        ct.set<equipped_shield>(defender, ENTITYID_INVALID);
        // remove item from attacker's inventory
        remove_from_inventory(defender, shield);
        // item destroyed
        ct.set<destroyed>(shield, true);
        const bool event_heard = check_hearing(hero_id, ct.get<location>(defender).value_or((vec3){-1, -1, -1}));
        if (event_heard)
            PlaySound(sfx[SFX_05_ALCHEMY_GLASS_BREAK]);
        add_message_history("%s broke!", ct.get<name>(shield).value_or("no-name-shield").c_str());
    }




    const inline int get_npc_xp(const entityid id) {
        return ct.get<xp>(id).value_or(0);
    }




    inline void update_npc_xp(const entityid id, const entityid target_id) {
        const int old_xp = get_npc_xp(id);
        const int reward_xp = 1;
        const int new_xp = old_xp + reward_xp;
        ct.set<xp>(id, new_xp);
    }




    inline void process_attack_results(const entityid atk_id, const entityid tgt_id, const bool atk_successful) {
        massert(atk_id != ENTITYID_INVALID, "attacker entity id is invalid");
        massert(tgt_id != ENTITYID_INVALID, "target entity id is invalid");
        const string attacker_name = ct.get<name>(atk_id).value_or("no-name");
        const string target_name = ct.get<name>(tgt_id).value_or("no-name");
        const char* atk_name = attacker_name.c_str();
        const char* tgt_name = target_name.c_str();
        if (ct.get<dead>(tgt_id).value_or(false)) {
            //minfo("Target is dead");
            add_message_history("%s swings at a dead target", atk_name);
            return;
        }
        if (!atk_successful) {
            //minfo("Missed attack");
            add_message_history("%s swings at %s and misses!", atk_name, tgt_name);
            return;
        }
        const int dmg = compute_attack_damage(atk_id, tgt_id);
        ct.set<damaged>(tgt_id, true);
        ct.set<update>(tgt_id, true);
        auto maybe_tgt_hp = ct.get<hp>(tgt_id);
        if (!maybe_tgt_hp.has_value()) {
            merror("target has no HP component");
            return;
        }
        const int tgt_hp = maybe_tgt_hp.value() - dmg;
        //minfo("damage dealt");
        add_message_history("%s deals %d damage to %s", atk_name, dmg, tgt_name);
        ct.set<hp>(tgt_id, tgt_hp);
        // decrement weapon durability
        handle_weapon_durability_loss(atk_id, tgt_id);
        if (tgt_hp > 0)
            return;
        ct.set<dead>(tgt_id, true);
        switch (ct.get<entitytype>(tgt_id).value_or(ENTITY_NONE)) {
        case ENTITY_NPC: {
            update_npc_xp(atk_id, tgt_id);
            // handle item drops
            drop_all_from_inventory(tgt_id);
        } break;
        case ENTITY_PLAYER: add_message("You died"); break;
        default: break;
        }
    }




    inline void handle_shield_block_sfx(const entityid target_id) {
        const bool event_heard = check_hearing(hero_id, ct.get<location>(target_id).value_or((vec3){-1, -1, -1}));
        if (event_heard)
            PlaySound(sfx[SFX_HIT_METAL_ON_METAL]);
    }




    const inline attack_result_t process_attack_entity(tile_t& tile, const entityid attacker_id, const entityid target_id) {
        massert(attacker_id != ENTITYID_INVALID, "attacker is NULL");
        if (target_id == ENTITYID_INVALID)
            return ATTACK_RESULT_MISS;
        const entitytype_t type = ct.get<entitytype>(target_id).value_or(ENTITY_NONE);
        if (type != ENTITY_PLAYER && type != ENTITY_NPC)
            return ATTACK_RESULT_MISS;
        if (ct.get<dead>(target_id).value_or(true))
            return ATTACK_RESULT_MISS;
        const string attacker_name = ct.get<name>(attacker_id).value_or("no-name");
        const string target_name = ct.get<name>(target_id).value_or("no-name");
        const char* atk_name = attacker_name.c_str();
        const char* tgt_name = target_name.c_str();
        // they have a shield
        // still need to do attack successful check
        const bool attack_successful = compute_attack_roll(attacker_id, target_id);
        // attack unsuccessful
        if (!attack_successful) {
            process_attack_results(attacker_id, target_id, false);
            return ATTACK_RESULT_MISS;
        }
        // check for shield
        const entityid shield_id = ct.get<equipped_shield>(target_id).value_or(ENTITYID_INVALID);
        // if no shield
        if (shield_id == ENTITYID_INVALID) {
            process_attack_results(attacker_id, target_id, true);
            return ATTACK_RESULT_HIT;
        }
        // if has shield
        // compute chance to block
        const int roll = GetRandomValue(1, MAX_BLOCK_CHANCE);
        const int chance = ct.get<block_chance>(shield_id).value_or(MAX_BLOCK_CHANCE);
        const int low_roll = MAX_BLOCK_CHANCE - chance;
        if (roll <= low_roll) {
            // failed to block
            //minfo("%s failed to block", tgt_name);
            process_attack_results(attacker_id, target_id, true);
            return ATTACK_RESULT_HIT;
        }
        // decrement shield durability
        handle_shield_durability_loss(target_id, attacker_id);
        handle_shield_block_sfx(target_id);
        ct.set<block_success>(target_id, true);
        ct.set<update>(target_id, true);
        //minfo("attack blocked");
        add_message_history("%s blocked an attack from %s", tgt_name, atk_name);
        return ATTACK_RESULT_BLOCK;
    }




    inline void handle_attack_sfx(const entityid attacker, const attack_result_t result) {
        //minfo("handle_attack_sfx: %d %d", attacker, result);
        if (!check_hearing(hero_id, ct.get<location>(attacker).value_or((vec3){-1, -1, -1}))) {
            return;
        }
        int index = SFX_SLASH_ATTACK_SWORD_1;
        if (result == ATTACK_RESULT_BLOCK) {
            index = SFX_HIT_METAL_ON_METAL;
        } else if (result == ATTACK_RESULT_HIT) {
            index = SFX_HIT_METAL_ON_FLESH;
        } else if (result == ATTACK_RESULT_MISS) {
            const entityid weapon_id = ct.get<equipped_weapon>(attacker).value_or(ENTITYID_INVALID);
            const weapontype_t wpn_type = ct.get<weapontype>(weapon_id).value_or(WEAPON_NONE);
            index = wpn_type == WEAPON_SWORD    ? SFX_SLASH_ATTACK_SWORD_1
                    : wpn_type == WEAPON_AXE    ? SFX_SLASH_ATTACK_HEAVY_1
                    : wpn_type == WEAPON_DAGGER ? SFX_SLASH_ATTACK_LIGHT_1
                                                : SFX_SLASH_ATTACK_SWORD_1;
        }
        //minfo("playing attack sfx...");
        PlaySound(sfx[index]);
        msuccess("attack sfx played");
    }




    inline void set_gamestate_flag_for_attack_animation(const entitytype_t type) {
        massert(type == ENTITY_PLAYER || type == ENTITY_NPC, "type is not player or npc!");
        if (type == ENTITY_PLAYER)
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
        else if (type == ENTITY_NPC)
            flag = GAMESTATE_FLAG_NPC_ANIM;
        else
            merror("Unknown flag state, invalid type: %d", type);
    }


    inline void try_entity_attack(const entityid id, const int tgt_x, const int tgt_y) {
        massert(!ct.get<dead>(id).value_or(false), "attacker entity is dead");
        //if (id == hero_id)
        //minfo("try_entity_attack: %d, (%d,%d)", id, tgt_x, tgt_y);
        const vec3 loc = ct.get<location>(id).value();
        auto df = d_get_floor(dungeon, loc.z);
        auto tile = df_tile_at(df, (vec3){tgt_x, tgt_y, loc.z});
        // Calculate direction based on target position
        const int dx = tgt_x - loc.x;
        const int dy = tgt_y - loc.y;
        ct.set<direction>(id, get_dir_from_xy(dx, dy));
        ct.set<attacking>(id, true);
        ct.set<update>(id, true);
        const entityid npc_id = get_cached_npc(tile);
        const attack_result_t result = process_attack_entity(tile, id, npc_id);
        // did the hero hear this event?
        handle_attack_sfx(id, result);
        set_gamestate_flag_for_attack_animation(ct.get<entitytype>(id).value_or(ENTITY_NONE));
    }




    const inline bool handle_attack(const inputstate& is, const bool is_dead) {
        if (inputstate_is_pressed(is, KEY_APOSTROPHE)) {
            if (is_dead) {
                add_message("You cannot attack while dead");
                return true;
            }
            if (ct.has<location>(hero_id) && ct.has<direction>(hero_id)) {
                const vec3 loc = get_loc_facing_player();
                try_entity_attack(hero_id, loc.x, loc.y);
                flag = GAMESTATE_FLAG_PLAYER_ANIM;
                return true;
            }
        }
        return false;
    }




    const inline entityid tile_get_item(tile_t& t) {
        recompute_entity_cache(t);
        return t.cached_item;
    }




    const inline bool try_entity_pickup(const entityid id) {
        massert(id != ENTITYID_INVALID, "Entity is NULL!");
        ct.set<update>(id, true);
        // check if the player is on a tile with an item
        auto maybe_loc = ct.get<location>(id);
        if (!maybe_loc.has_value()) {
            merror("id %d has no location", id);
            return false;
        }
        const vec3 loc = maybe_loc.value();
        dungeon_floor_t& df = d_get_floor(dungeon, loc.z);
        tile_t& tile = df_tile_at(df, loc);
        bool item_picked_up = false;
        // lets try using our new cached_item via tile_get_item
        const entityid item_id = tile_get_item(tile);
        if (item_id != ENTITYID_INVALID && add_to_inventory(id, item_id)) {
            tile_remove(tile, item_id);
            PlaySound(sfx[SFX_CONFIRM_01]);
            item_picked_up = true;
            auto item_name = ct.get<name>(item_id).value_or("no-name-item");
            add_message_history("You picked up %s", item_name.c_str());
        } else if (item_id == ENTITYID_INVALID) {
            merror("No item cached");
        }
        auto t = ct.get<entitytype>(id).value_or(ENTITY_NONE);
        if (t == ENTITY_PLAYER) {
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
        }
        //minfo("returning item_picked_up: %d", item_picked_up);
        return item_picked_up;
    }




    const inline bool handle_pickup_item(const inputstate& is, const bool is_dead) {
        if (inputstate_is_pressed(is, KEY_SLASH)) {
            if (is_dead) {
                add_message("You cannot pick up items while dead");
                return true;
            }
            try_entity_pickup(hero_id);
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }




    const inline bool try_entity_stairs(const entityid id) {
        massert(id != ENTITYID_INVALID, "Entity ID is invalid!");
        ct.set<update>(id, true);
        const vec3 loc = ct.get<location>(id).value();
        // first, we prob want to get the tile at this location
        const int current_floor = dungeon.current_floor;
        //auto df = g.dungeon.floors->at(current_floor);
        auto df = dungeon.floors[current_floor];
        auto t = df_tile_at(df, loc);
        // check the tile type
        if (t.type == TILE_UPSTAIRS) {
            // can't go up on the top floor
            // otherwise...
            if (current_floor == 0) {
                add_message("You are already on the top floor!");
            } else {
                // go upstairs
                // we have to remove the player from the old tile
                df_remove_at(df, hero_id, loc.x, loc.y);
                dungeon.current_floor--;
                const int new_floor = dungeon.current_floor;
                auto df2 = dungeon.floors[new_floor];
                const vec3 uloc = df2.downstairs_loc;
                auto t2 = df_tile_at(df2, uloc);
                df_add_at(df2, hero_id, uloc.x, uloc.y);
                ct.set<location>(hero_id, uloc);
                flag = GAMESTATE_FLAG_PLAYER_ANIM;
                PlaySound(sfx.at(SFX_STEP_STONE_1));
                return true;
            }
        } else if (t.type == TILE_DOWNSTAIRS) {
            // can't go down on the bottom floor
            // otherwise...
            if ((size_t)current_floor < dungeon.floors.size() - 1) {
                // go downstairs
                // we have to remove the player from the old tile
                df_remove_at(df, hero_id, loc.x, loc.y);
                dungeon.current_floor++;
                const int new_floor = dungeon.current_floor;
                auto df2 = dungeon.floors[new_floor];
                const vec3 uloc = df2.upstairs_loc;
                auto t2 = df_tile_at(df2, uloc);
                df_add_at(df2, hero_id, uloc.x, uloc.y);
                ct.set<location>(hero_id, uloc);
                flag = GAMESTATE_FLAG_PLAYER_ANIM;
                PlaySound(sfx.at(SFX_STEP_STONE_1));
                return true;
            } else {
                // bottom floor
                add_message("You can't go downstairs anymore!");
            }
        }
        return false;
    }




    const inline bool handle_traverse_stairs(const inputstate& is, const bool is_dead) {
        if (inputstate_is_pressed(is, KEY_PERIOD)) {
            if (is_dead) {
                add_message("You cannot traverse stairs while dead");
                return true;
            }
            try_entity_stairs(hero_id);
            return true;
        }
        return false;
    }




    const inline bool try_entity_open_door(const entityid id, const vec3 loc) {
        massert(id != ENTITYID_INVALID, "id is invalid");
        if (tile_has_door(loc)) {
            auto df = d_get_current_floor(dungeon);
            auto t = df_tile_at(df, loc);
            for (size_t i = 0; i < t.entities->size(); i++) {
                const entityid myid = t.entities->at(i);
                const entitytype_t type = ct.get<entitytype>(myid).value_or(ENTITY_NONE);
                if (type == ENTITY_DOOR) {
                    auto maybe_is_open = ct.get<door_open>(myid);
                    if (maybe_is_open.has_value()) {
                        const bool is_open = maybe_is_open.value();
                        if (is_open) {
                            ct.set<door_open>(myid, false);
                        } else {
                            ct.set<door_open>(myid, true);
                        }
                        PlaySound(sfx.at(SFX_CHEST_OPEN));
                        return true;
                    }
                }
            }
        }
        return false;
    }




    const inline bool handle_open_door(const inputstate& is, const bool is_dead) {
        if (inputstate_is_pressed(is, KEY_O)) {
            if (is_dead) {
                add_message("You cannot open doors while dead");
                return true;
            }
            const vec3 loc = get_loc_facing_player();
            try_entity_open_door(hero_id, loc);
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }




    inline void try_entity_cast_spell(const entityid id, const int tgt_x, const int tgt_y) {
        //minfo("Trying to cast spell...");
        const auto maybe_loc = ct.get<location>(id);
        if (!maybe_loc.has_value()) {
            merror("no location for entity id %d", id);
            return;
        }
        const vec3 loc = maybe_loc.value();
        auto floor = d_get_floor(dungeon, loc.z);
        const vec3 spell_loc = {tgt_x, tgt_y, loc.z};
        auto tile = df_tile_at(floor, spell_loc);
        // Calculate direction based on target position
        bool ok = false;
        const int dx = tgt_x - loc.x;
        const int dy = tgt_y - loc.y;
        ct.set<direction>(id, get_dir_from_xy(dx, dy));
        ct.set<casting>(id, true);
        ct.set<update>(id, true);
        // ok...
        // we are hard-coding a spell cast
        // in this example, we will 'create' a 'spell entity' of type 'fire' and place it on a tile
        const entityid spell_id = create_spell_at_with(spell_loc);
        ct.set<spellstate>(id, SPELLSTATE_CAST);
        ct.set<spelltype>(id, SPELLTYPE_FIRE);
        ct.set<spell_casting>(id, true);
        if (spell_id != ENTITYID_INVALID) {
            ok = true;
            // lets do an example of processing a spell effect immediately
            // first we need to iterate the entities on the tile
            // if there's an NPC we damage it
            if (tile_has_live_npcs(tile)) {
                entityid npcid = ENTITYID_INVALID;
                for (auto id : *tile.entities) {
                    if (ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_NPC) {
                        npcid = id;
                        break;
                    }
                }
                const int dmg = GetRandomValue(1, 6);
                ct.set<damaged>(npcid, true);
                ct.set<update>(npcid, true);
                auto maybe_tgt_hp = ct.get<hp>(npcid);
                if (!maybe_tgt_hp.has_value()) {
                    merror("target has no HP component");
                    return;
                }
                int tgt_hp = maybe_tgt_hp.value();
                if (tgt_hp <= 0) {
                    merror("Target is already dead, hp was: %d", tgt_hp);
                    ct.set<dead>(npcid, true);
                    return;
                }
                tgt_hp -= dmg;
                ct.set<hp>(npcid, tgt_hp);
                if (tgt_hp > 0) {
                    ct.set<dead>(npcid, false);
                    return;
                }
                auto tgttype = ct.get<entitytype>(npcid).value_or(ENTITY_NONE);
                ct.set<dead>(npcid, true);
                tile_t& target_tile = df_tile_at(d_get_current_floor(dungeon), spell_loc);
                target_tile.dirty_entities = true;
                if (tgttype == ENTITY_NPC) {
                    // increment attacker's xp
                    const int old_xp = ct.get<xp>(id).value_or(0);
                    const int reward_xp = 1;
                    const int new_xp = old_xp + reward_xp;
                    ct.set<xp>(id, new_xp);
                    // handle item drops
                    drop_all_from_inventory(npcid);
                } else if (tgttype == ENTITY_PLAYER) {
                    add_message("You died");
                }
            }
            if (tile_has_door(spell_loc)) {
                // find the door id
                entityid doorid = ENTITYID_INVALID;
                for (auto id : *tile.entities) {
                    if (ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_DOOR) {
                        doorid = id;
                        break;
                    }
                }
                // mark it 'destroyed'
                // remove it from the tile
                if (doorid != ENTITYID_INVALID) {
                    ct.set<destroyed>(doorid, true);
                    df_remove_at(floor, doorid, spell_loc.x, spell_loc.y);
                }
            }
            ct.set<destroyed>(spell_id, true);
        }
        // did the hero hear this event?
        const bool event_heard = check_hearing(hero_id, (vec3){tgt_x, tgt_y, loc.z});
        if (ok) {
            // default metal on flesh
            //play_sound_if_heard(SFX_ITEM_FUSION, event_heard);
            if (event_heard)
                PlaySound(sfx[SFX_ITEM_FUSION]);
        } else {
            // need to select appropriate sound effect based on equipment- get attacker's equipped weapon if any
            //const entityid weapon_id = g.ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID);
            // attacker has equipped weapon - get its type
            //const weapontype_t wpn_type = g.ct.get<weapontype>(g.ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID)).value_or(WEAPON_NONE);
            //const int index = wpn_type == WEAPON_SWORD    ? SFX_SLASH_ATTACK_SWORD_1
            //                  : wpn_type == WEAPON_AXE    ? SFX_SLASH_ATTACK_HEAVY_1
            //                  : wpn_type == WEAPON_DAGGER ? SFX_SLASH_ATTACK_LIGHT_1
            //                                              : SFX_SLASH_ATTACK_SWORD_1;
            //play_sound_if_heard(index, event_heard);
        }
    }




    const inline bool handle_test_cast_spell(const inputstate& is, const bool is_dead) {
        if (inputstate_is_pressed(is, KEY_M)) {
            if (is_dead) {
                add_message("You cannot cast spells while dead (yet)");
                return true;
            }
            if (ct.get<location>(hero_id).has_value() && ct.get<direction>(hero_id).has_value()) {
                const vec3 loc = get_loc_facing_player();
                try_entity_cast_spell(hero_id, loc.x, loc.y);
                flag = GAMESTATE_FLAG_PLAYER_ANIM;
                return true;
            }
        }
        return false;
    }




    const inline bool handle_restart(const inputstate& is, const bool is_dead) {
        if (inputstate_is_pressed(is, KEY_R) && is_dead) {
            do_restart = true;
            //minfo("setting do_restart to true...");
            return true;
        }
        return false;
    }




    inline void handle_input_gameplay_controlmode_player(const inputstate& is) {
        if (flag != GAMESTATE_FLAG_PLAYER_INPUT)
            return;
        if (handle_quit_pressed(is))
            return;
        if (handle_cycle_messages(is))
            return;
        // make sure player isnt dead
        auto maybe_player_is_dead = ct.get<dead>(hero_id);
        if (!maybe_player_is_dead.has_value())
            return;
        const bool is_dead = maybe_player_is_dead.value();
        if (handle_camera_zoom(is))
            return;
        else if (handle_change_dir(is))
            return;
        else if (handle_change_dir_intent(is))
            return;
        else if (handle_display_inventory(is))
            return;
        else if (handle_move_up(is, is_dead))
            return;
        else if (handle_move_down(is, is_dead))
            return;
        else if (handle_move_left(is, is_dead))
            return;
        else if (handle_move_right(is, is_dead))
            return;
        else if (handle_move_up_left(is, is_dead))
            return;
        else if (handle_move_up_right(is, is_dead))
            return;
        else if (handle_move_down_left(is, is_dead))
            return;
        else if (handle_move_down_right(is, is_dead))
            return;
        else if (handle_attack(is, is_dead))
            return;
        else if (handle_pickup_item(is, is_dead))
            return;
        else if (handle_traverse_stairs(is, is_dead))
            return;
        else if (handle_open_door(is, is_dead))
            return;
        else if (handle_test_cast_spell(is, is_dead))
            return;
        else if (handle_restart(is, is_dead))
            return;
    }




    inline void handle_input_gameplay_scene(const inputstate& is) {
        if (inputstate_is_pressed(is, KEY_B)) {
            if (controlmode == CONTROLMODE_PLAYER) {
                controlmode = CONTROLMODE_CAMERA;
            } else if (controlmode == CONTROLMODE_CAMERA) {
                controlmode = CONTROLMODE_PLAYER;
            }
            frame_dirty = true;
        }
        if (controlmode == CONTROLMODE_CAMERA) {
            handle_camera_move(is);
            frame_dirty = true;
        }
        if (controlmode == CONTROLMODE_PLAYER) {
            handle_input_gameplay_controlmode_player(is);
            return;
        }
        if (controlmode == CONTROLMODE_INVENTORY) {
            handle_input_inventory(is);
            return;
        }
    }




    inline void handle_input(const inputstate& is) {
        // no matter which mode we are in, we can toggle the debug panel
        if (inputstate_is_pressed(is, KEY_P)) {
            debugpanelon = !debugpanelon;
            //minfo("Toggling debug panel: %s", debugpanelon ? "ON" : "OFF");
            return;
        }
        if (current_scene == SCENE_TITLE)
            handle_input_title_scene(is);
        else if (current_scene == SCENE_MAIN_MENU)
            handle_input_main_menu_scene(is);
        else if (current_scene == SCENE_CHARACTER_CREATION)
            handle_input_character_creation_scene(is);
        else if (current_scene == SCENE_GAMEPLAY)
            handle_input_gameplay_scene(is);
    }




    inline void update_debug_panel_buffer(const inputstate& is) {
        // Static buffers to avoid reallocating every frame
        //static const char* control_modes[] = {"Camera", "Player", "Unknown"};
        int message_count = msg_history.size();
        int inventory_count;
        vec3 loc = {0, 0, 0};
        inventory_count = -1;
        if (hero_id != ENTITYID_INVALID)
            loc = ct.get<location>(hero_id).value_or((vec3){-1, -1, -1});
        // Determine control mode and flag strings
        const char* control_mode = controlmode == CONTROLMODE_CAMERA ? "Camera" : controlmode == CONTROLMODE_PLAYER ? "Player" : "Unknown";
        // zero out the buffer
        memset(debugpanel.buffer, 0, sizeof(debugpanel.buffer));
        // Format the string in one pass
        snprintf(debugpanel.buffer,
                 sizeof(debugpanel.buffer),
                 "@evildojo666\n"
                 "project.rpg\n"
                 //"%s\n" // timebeganbuf
                 //"%s\n" // currenttimebuf
                 "frame : %d\n"
                 "update: %d\n"
                 "frame dirty: %d\n"
                 "draw time: %.1fms\n"
                 "cam: (%.0f,%.0f) Zoom: %.1f\n"
                 "controlmode: %s \n"
                 "floor: %d/%d \n"
                 "next_entity_id: %d\n"
                 "hero_id: %d\n"
                 "flag: %s\n"
                 "entity_turn: %d\n"
                 "hero: (%d,%d,%d)\n"
                 "weapon: %d\n"
                 "inventory: %d\n"
                 "message count: %d\n",
                 framecount,
                 frame_updates,
                 frame_dirty,
                 last_frame_time * 1000,
                 cam2d.offset.x,
                 cam2d.offset.y,
                 cam2d.zoom,
                 control_mode,
                 0,
                 0,
                 next_entityid,
                 hero_id,
                 flag == GAMESTATE_FLAG_NONE           ? "None"
                 : flag == GAMESTATE_FLAG_PLAYER_INPUT ? "Player Input"
                 : flag == GAMESTATE_FLAG_PLAYER_ANIM  ? "Player anim"
                 : flag == GAMESTATE_FLAG_NPC_TURN     ? "NPC Turn"
                 : flag == GAMESTATE_FLAG_NPC_ANIM     ? "NPC anim"
                                                       : "Unknown",

                 entity_turn,
                 loc.x,
                 loc.y,
                 loc.z,
                 ct.get<equipped_weapon>(hero_id).value_or(ENTITYID_INVALID),
                 inventory_count,
                 message_count);
    }




    const inline bool is_entity_adjacent(const entityid id0, const entityid id1) {
        massert(id0 != ENTITYID_INVALID, "id0 is invalid");
        massert(id1 != ENTITYID_INVALID, "id1 is invalid");
        massert(id0 != id1, "id0 and id1 are the same");
        // check if on same floor
        auto loc0 = ct.get<location>(id0).value_or((vec3){-1, -1, -1});
        auto loc1 = ct.get<location>(id1).value_or((vec3){-1, -1, -1});
        if (loc0.z == -1 || loc1.z == -1 || loc0.z != loc1.z)
            return false;
        // use e0 and check the surrounding 8 tiles
        for (int y = -1; y <= 1; y++) {
            for (int x = -1; x <= 1; x++) {
                if (x == 0 && y == 0)
                    continue;
                if (loc0.x + x == loc1.x && loc0.y + y == loc1.y)
                    return true;
            }
        }
        return false;
    }




    void handle_npc(const entityid id) {
        massert(id != ENTITYID_INVALID, "Entity is NULL!");
        if (id == 0 || id == hero_id)
            return;
        auto maybe_type = ct.get<entitytype>(id);
        if (!maybe_type.has_value())
            return;
        entitytype_t type = maybe_type.value();
        if (type != ENTITY_NPC)
            return;
        auto maybe_dead = ct.get<dead>(id);
        if (!maybe_dead.has_value())
            return;
        const bool is_dead = maybe_dead.value();
        if (is_dead)
            return;
        // this is a heuristic for handling entity actions
        // originally, we were just moving randomly
        // this example shows how, if the player is not adjacent to an NPC,
        // they will just move randomly. otherwise, they attack the player
        //const entityid target_id = g.ct.get<target>(id).value_or(g.hero_id);
        auto tgt_id = ct.get<target_id>(id).value_or(hero_id);
        if (is_entity_adjacent(id, tgt_id)) {
            // if id is adjacent to its target or the hero
            vec3 loc = ct.get<location>(tgt_id).value();
            try_entity_attack(id, loc.x, loc.y);
            return;
        }
        // else, randomly move
        try_entity_move(id, (vec3){rand() % 3 - 1, rand() % 3 - 1, 0});
    }




    inline void handle_npcs() {
        if (flag == GAMESTATE_FLAG_NPC_TURN) {
#ifndef NPCS_ALL_AT_ONCE
            if (entity_turn >= 0 && entity_turn < next_entityid) {
                handle_npc(entity_turn);
                flag = GAMESTATE_FLAG_NPC_ANIM;
            }
#else
            for (entityid id = 0; id < next_entityid; id++) {
                handle_npc(id);
            }
            flag = GAMESTATE_FLAG_NPC_ANIM;
#endif
        }
    }




    inline void tick(const inputstate& is) {
        // Spawn NPCs periodically
        //try_spawn_npc(g);
        update_player_tiles_explored();
        update_player_state();
        update_npcs_state();
        update_spells_state();
        handle_input(is);
        handle_npcs();
        update_debug_panel_buffer(is);
        currenttime = time(NULL);
        currenttimetm = localtime(&currenttime);
        strftime(currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", currenttimetm);
    }
};
