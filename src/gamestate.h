#pragma once

#include "ComponentTable.h"
#include "character_creation.h"
#include "controlmode.h"
#include "debugpanel.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "entity_actions.h"
#include "entityid.h"
#include "entitytype.h"
#include "gamestate_flag.h"
#include "get_racial_hd.h"
#include "get_racial_modifiers.h"
#include "inputstate.h"
#include "libgame_version.h"
#include "orc_names.h"
#include "roll.h"
#include "scene.h"
#include "sfx.h"
#include "stat_bonus.h"
#include <ctime>
#include <raylib.h>

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
        msuccess("Gamestate initialized successfully");
    }


    void init_music_paths() {
        //minfo("Initializing music paths...");
        const char* music_path_file = "music.txt";
        FILE* file = fopen(music_path_file, "r");
        massert(file, "Could not open music path file: %s", music_path_file);
        char buffer[128];
        music_file_paths.clear();
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            // Remove newline character if present
            //minfo("Removing newline character...");
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
            }
            // if it begins with a #, skip for now
            //minfo("Checking for comment...");
            if (buffer[0] == '#') {
                mwarning("Comment received, skipping...");
                continue;
            }
            // copy into g->music_file_paths
            //minfo("Pushing back %s", buffer);
            // need to pre-pend the folder path
            string fullpath = "audio/music/" + string(buffer);
            music_file_paths.push_back(fullpath);
        }
        fclose(file);
        //msuccess("Music paths initialized!");
    }


    void reset() {
        //minfo("gamestate reset");
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
        init_music_paths();
    }


    bool set_hero_id(entityid id) {
        massert(id != ENTITYID_INVALID, "id is invalid");
        if (id == ENTITYID_INVALID) {
            merror("g_set_hero_id: id is invalid");
            return false;
        }
        hero_id = id;
        msuccess("Hero ID set to %d", id);
        return true;
    }


    entityid add_entity() {
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


    tile_t& tile_at_cur_floor(vec3 loc) {
        return df_tile_at(d_get_current_floor(dungeon), loc);
    }


    void init_dungeon(int df_count) {
        massert(df_count > 0, "df_count is <= 0");
        d_create(dungeon);
        msuccess("dungeon initialized successfully");
        minfo("adding floors...");
        // max size of 128x128 for now to maintain 60fps
        // dungeon floors, tiles etc will require re-write/re-design for optimization
        const int w = 32;
        const int h = 32;
        const biome_t type = BIOME_STONE;
        //dungeon_floor_type_t type = DUNGEON_FLOOR_TYPE_GRASS;
        for (int i = 0; i < df_count; i++) {
            d_add_floor(dungeon, type, w, h);
        }
        msuccess("added %d floors to dungeon", df_count);
    }


    entityid create_door_with() {
        //const auto id = g_add_entity(g);
        const entityid id = add_entity();
        ct.set<entitytype>(id, ENTITY_DOOR);
        //doorInitFunction(g, id);
        return id;
    }


    void recompute_entity_cache(tile_t& t) {
        // Only recompute if cache is dirty
        //minfo("recompute entity cache...");
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


    void recompute_entity_cache_at(int x, int y, int z) {
        massert(x >= 0 && y >= 0 && z >= 0, "x, y, or z is out of bounds: %d, %d, %d", x, y, z);
        massert((size_t)z < dungeon.floors.size(), "z is out of bounds");
        auto df = dungeon.floors[z];
        auto t = df_tile_at(df, (vec3){x, y, z});
        recompute_entity_cache(t);
    }


    bool tile_has_live_npcs(tile_t& t) {
        recompute_entity_cache(t);
        return t.cached_live_npcs > 0;
    }


    entityid create_door_at_with(vec3 loc) {
        dungeon_floor_t& df = d_get_floor(dungeon, loc.z);
        tile_t& tile = df_tile_at(df, loc);
        if (!tile_is_walkable(tile.type))
            return ENTITYID_INVALID;
        if (tile_has_live_npcs(tile))
            return ENTITYID_INVALID;
        const auto id = create_door_with();
        if (id == ENTITYID_INVALID)
            return ENTITYID_INVALID;
        minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);
        if (!df_add_at(df, id, loc.x, loc.y))
            return ENTITYID_INVALID;
        ct.set<location>(id, loc);
        ct.set<door_open>(id, false);
        ct.set<update>(id, true);
        return id;
    }


    void place_doors() {
        for (int z = 0; z < (int)dungeon.floors.size(); z++) {
            auto df = d_get_floor(dungeon, z);
            for (int x = 0; x < df.width; x++) {
                for (int y = 0; y < df.height; y++) {
                    const vec3 loc = {x, y, z};
                    auto tile = df_tile_at(df, loc);
                    if (!tile.can_have_door)
                        continue;
                    create_door_at_with(loc);
                }
            }
        }
    }


    entityid create_prop_with(proptype_t type) {
        const auto id = add_entity();
        ct.set<entitytype>(id, ENTITY_PROP);
        ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
        ct.set<update>(id, true);
        ct.set<proptype>(id, type);
        //propInitFunction(g, id);
        return id;
    }


    entityid create_prop_at_with(proptype_t type, vec3 loc) {
        auto df = d_get_floor(dungeon, loc.z);
        auto tile = df_tile_at(df, loc);
        const entityid id = create_prop_with(type);
        if (id == ENTITYID_INVALID) {
            return ENTITYID_INVALID;
        }
        minfo("attempting prop df_add_at: %d, %d, %d", id, loc.x, loc.y);
        if (!df_add_at(df, id, loc.x, loc.y)) {
            merror("failed df_add_at: %d, %d, %d", id, loc.x, loc.y);
            return ENTITYID_INVALID;
        }
        msuccess("prop add success");
        ct.set<location>(id, loc);
        return id;
    }


    inline void place_props() {
        //auto mydefault = [](gamestate& g, entityid id) {};
        //auto set_solid = [](gamestate& g, entityid id) { g.ct.set<solid>(id, true); };
        //auto set_solid_and_pushable = [](gamestate& g, entityid id) {
        //    g.ct.set<solid>(id, true);
        //    g.ct.set<pushable>(id, true);
        //};
        for (int z = 0; z < (int)dungeon.floors.size(); z++) {
            auto df = d_get_floor(dungeon, z);
            for (int x = 0; x < df.width; x++) {
                for (int y = 0; y < df.height; y++) {
                    const vec3 loc = {x, y, z};
                    auto tile = df_tile_at(df, loc);
                    //if (!tile.can_have_door)
                    //    continue;
                    //create_door_at_with(g, loc, [](gamestate& g, entityid id) {});
                    if (tile.type == TILE_UPSTAIRS || tile.type == TILE_DOWNSTAIRS)
                        continue;
                    if (tile.can_have_door)
                        continue;
                    if (tile_is_wall(tile)) {
                        const int flip = GetRandomValue(0, 20);
                        if (flip == 0) {
                            const int r = GetRandomValue(1, 3);
                            switch (r) {
                            case 1: create_prop_at_with(PROP_DUNGEON_BANNER_00, loc);
                            case 2: create_prop_at_with(PROP_DUNGEON_BANNER_01, loc);
                            case 3: create_prop_at_with(PROP_DUNGEON_BANNER_02, loc);
                            default: break;
                            }
                        }
                    } else {
                        const int flip = GetRandomValue(0, 20);
                        if (flip == 0) {
                            const entityid id = create_prop_at_with(PROP_DUNGEON_WOODEN_TABLE_00, loc);
                            ct.set<solid>(id, true);

                        } else if (flip == 1) {
                            const entityid id = create_prop_at_with(PROP_DUNGEON_WOODEN_TABLE_01, loc);
                            ct.set<solid>(id, true);
                        } else if (flip == 2) {
                            //const entityid id = create_prop_at_with(PROP_DUNGEON_WOODEN_CHAIR_00, loc);
                            create_prop_at_with(PROP_DUNGEON_WOODEN_CHAIR_00, loc);
                        } else if (flip == 3) {
                            const entityid id = create_prop_at_with(PROP_DUNGEON_STATUE_00, loc);
                            ct.set<solid>(id, true);
                            ct.set<pushable>(id, true);
                        }
                    }
                }
            }
        }
    }


    entityid create_weapon_with() {
        const entityid id = add_entity();
        ct.set<entitytype>(id, ENTITY_ITEM);
        ct.set<itemtype>(id, ITEM_WEAPON);
        ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
        ct.set<update>(id, true);
        //weaponInitFunction(g, id);
        return id;
    }


    entityid create_weapon_at_with(vec3 loc) {
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
        const auto id = create_weapon_with();
        if (id == ENTITYID_INVALID) {
            return ENTITYID_INVALID;
        }
        minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);
        if (!df_add_at(df, id, loc.x, loc.y)) {
            return ENTITYID_INVALID;
        }
        ct.set<location>(id, loc);
        return id;
    }


    inline entityid create_shield_with() {
        const auto id = add_entity();
        ct.set<entitytype>(id, ENTITY_ITEM);
        ct.set<itemtype>(id, ITEM_SHIELD);
        ct.set<durability>(id, 100);
        ct.set<max_durability>(id, 100);
        ct.set<rarity>(id, RARITY_COMMON);
        //shieldInitFunction(g, id);
        return id;
    }


    inline entityid create_shield_at_with(vec3 loc) {
        const auto id = create_shield_with();
        minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);
        auto df = d_get_floor(dungeon, loc.z);
        if (!df_add_at(df, id, loc.x, loc.y))
            return ENTITYID_INVALID;
        ct.set<location>(id, loc);
        return id;
    }


    inline entityid create_potion_with() {
        const auto id = add_entity();
        ct.set<entitytype>(id, ENTITY_ITEM);
        ct.set<itemtype>(id, ITEM_POTION);
        //potionInitFunction(g, id);
        return id;
    }


    inline entityid create_potion_at_with(vec3 loc) {
        auto df = d_get_floor(dungeon, loc.z);
        auto tile = df_tile_at(df, loc);
        if (!tile_is_walkable(tile.type))
            return ENTITYID_INVALID;
        if (tile_has_live_npcs(tile))
            return ENTITYID_INVALID;
        const auto id = create_potion_with();
        if (id == ENTITYID_INVALID)
            return ENTITYID_INVALID;
        minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);
        if (!df_add_at(df, id, loc.x, loc.y))
            return ENTITYID_INVALID;
        ct.set<location>(id, loc);
        ct.set<update>(id, true);
        return id;
    }


    inline race_t random_monster_type() {
        const vector<race_t> monster_races = {RACE_GOBLIN, RACE_ORC, RACE_BAT, RACE_WOLF, RACE_WARG, RACE_ZOMBIE, RACE_SKELETON, RACE_RAT, RACE_GREEN_SLIME};
        const int random_index = GetRandomValue(0, monster_races.size() - 1);
        return monster_races[random_index];
    }


    inline void set_npc_starting_stats(entityid id) {
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


    inline void set_npc_defaults(entityid id) {
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


    inline entityid create_npc_with(race_t rt) {
        minfo("begin create npc");
        //const entityid id = g_add_entity(g);
        const auto id = add_entity();
        set_npc_defaults(id);
        ct.set<race>(id, rt);
        set_npc_starting_stats(id);
        //npcInitFunction(g, id);
        minfo("end create npc");
        return id;
    }


    inline entityid tile_has_box(int x, int y, int z) {
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


    inline entityid create_npc_at_with(race_t rt, vec3 loc) {
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
        const entityid id = create_npc_with(rt);
        if (!df_add_at(df, id, loc.x, loc.y)) {
            return ENTITYID_INVALID;
        }
        ct.set<location>(id, loc);
        return id;
    }


    bool add_to_inventory(entityid actor_id, entityid item_id) {
        minfo("add to inventory: %d %d", actor_id, item_id);
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


    inline entityid create_random_monster_with() {
        //const race_t r = random_monster_type();
        minfo("create random monster with");
        const race_t r = RACE_ORC;
        const entityid id = create_npc_with(r);
        minfo("create weapon");
        const entityid wpn_id = create_weapon_with();
        ct.set<name>(wpn_id, "Dagger");
        ct.set<description>(wpn_id, "Stabby stabby.");
        ct.set<weapontype>(wpn_id, WEAPON_DAGGER);
        ct.set<damage>(wpn_id, (vec3){1, 4, 0});
        ct.set<durability>(wpn_id, 100);
        ct.set<max_durability>(wpn_id, 100);
        ct.set<rarity>(wpn_id, RARITY_COMMON);
        minfo("create potion");
        //const entityid potion_id = create_potion_at_with(df_get_random_loc(dungeon.floors[0]));
        const entityid potion_id = create_potion_with();
        msuccess("created potion");
        ct.set<name>(potion_id, "small healing potion");
        ct.set<description>(potion_id, "a small healing potion");
        ct.set<potiontype>(potion_id, POTION_HP_SMALL);
        ct.set<healing>(potion_id, (vec3){1, 6, 0});
        minfo("add to inventory");
        add_to_inventory(id, wpn_id);
        minfo("add to inventory");
        add_to_inventory(id, potion_id);
        ct.set<equipped_weapon>(id, wpn_id);
        ct.set<name>(id, get_random_orc_name());
        return id;
    }


    inline entityid create_random_monster_at_with(vec3 loc) {
        minfo("create random monster at with: %d, %d, %d", loc.x, loc.y, loc.z);
        minfo("getting floor...");
        auto df = d_get_floor(dungeon, loc.z);
        minfo("getting tile...");
        auto tile = df_tile_at(df, loc);
        minfo("getting tile is walkable...");
        if (!tile_is_walkable(tile.type))
            return ENTITYID_INVALID;
        minfo("getting tile has live npcs...");
        if (tile_has_live_npcs(tile))
            return ENTITYID_INVALID;
        //const auto id = create_random_monster_with(g, monsterInitFunction);
        minfo("create random monster with...");
        const auto id = create_random_monster_with();
        if (id == ENTITYID_INVALID)
            return ENTITYID_INVALID;
        minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);
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


    void update_tile(tile_t& tile) {
        tile.explored = tile.visible = true;
    }


    void update_player_tiles_explored() {
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


    void update_player_state() {
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


    void update_spells_state() {
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


    void update_npcs_state() {
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


    void logic_init() {
        srand(time(NULL));
        SetRandomSeed(time(NULL));
        minfo("gamestate.logic_init");

        init_dungeon(10);
        place_doors();
        place_props();

        const entityid dagger_id = create_weapon_at_with(df_get_random_loc(dungeon.floors[0]));
        ct.set<name>(dagger_id, "Dagger");
        ct.set<description>(dagger_id, "Stabby stabby.");
        ct.set<weapontype>(dagger_id, WEAPON_DAGGER);
        ct.set<damage>(dagger_id, (vec3){1, 4, 0});
        ct.set<durability>(dagger_id, 100);
        ct.set<max_durability>(dagger_id, 100);
        ct.set<rarity>(dagger_id, RARITY_COMMON);

        const entityid shield_id = create_shield_at_with(df_get_random_loc(dungeon.floors[0]));
        ct.set<name>(shield_id, "Kite Shield");
        ct.set<description>(shield_id, "Standard knight's shield");
        ct.set<shieldtype>(shield_id, SHIELD_KITE);
        ct.set<block_chance>(shield_id, 90);

        const entityid potion_id = create_potion_at_with(df_get_random_loc(dungeon.floors[0]));
        ct.set<name>(potion_id, "small healing potion");
        ct.set<description>(potion_id, "a small healing potion");
        ct.set<potiontype>(potion_id, POTION_HP_SMALL);
        ct.set<healing>(potion_id, (vec3){1, 6, 0});

        minfo("creating monsters...");
        for (int i = 0; i < (int)dungeon.floors.size(); i++) {
            for (int j = 1; j <= i + 1; j++) {
                const vec3 random_loc = df_get_random_loc(d_get_floor(dungeon, i));
                create_random_monster_at_with(random_loc);
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

    void handle_input_title_scene(inputstate& is) {
        if (inputstate_is_pressed(is, KEY_ESCAPE)) {
            do_quit = true;
            return;
        }

        if (inputstate_is_pressed(is, KEY_ENTER) || inputstate_is_pressed(is, KEY_SPACE)) {
            minfo("Title screen input detected, switching to main menu");
            current_scene = SCENE_MAIN_MENU;
            frame_dirty = true;
            PlaySound(sfx[SFX_CONFIRM_01]);
        }
    }

    void handle_input_main_menu_scene(inputstate& is) {
        if (inputstate_is_pressed(is, KEY_ENTER) || inputstate_is_pressed(is, KEY_SPACE)) {
            if (title_screen_selection == 0) {
                minfo("Switching to character creation scene");
                current_scene = SCENE_CHARACTER_CREATION;
                frame_dirty = true;
            }
            PlaySound(sfx.at(SFX_CONFIRM_01));
        } else if (inputstate_is_pressed(is, KEY_DOWN)) {
            minfo("Title screen selection++");
            title_screen_selection++;
            if (title_screen_selection >= max_title_screen_selections) {
                title_screen_selection = 0;
            }
            PlaySound(sfx.at(SFX_CONFIRM_01));
        } else if (inputstate_is_pressed(is, KEY_UP)) {
            minfo("Title screen selection--");
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


    entityid create_player(vec3 loc, string n) {
        massert(n != "", "name is empty string");
        minfo("Creating player...");
        race_t rt = chara_creation.race;
        minfo("Race: %s", race2str(rt).c_str());
        const auto id = create_npc_at_with(rt, loc); // [n](gamestate& g, entityid id) {
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
        minfo("Adding inventory to entity id %d", id);
        msuccess("create_player successful, id: %d", id);
        return id;
    }


    entityid create_box_with() {
        const auto id = add_entity();
        ct.set<entitytype>(id, ENTITY_BOX);
        ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
        ct.set<update>(id, true);
        ct.set<pushable>(id, true);
        //boxInitFunction(g, id);
        return id;
    }


    entityid create_box_at_with(vec3 loc) {
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
        minfo("attempting df_add_at: %d, %d, %d", id, loc.x, loc.y);
        if (!df_add_at(df, id, loc.x, loc.y)) {
            merror("failed df_add_at: %d, %d, %d", id, loc.x, loc.y);
            return ENTITYID_INVALID;
        }
        ct.set<location>(id, loc);
        return id;
    }


    entityid create_spell_with() {
        const auto id = add_entity();
        ct.set<entitytype>(id, ENTITY_SPELL);
        ct.set<spelltype>(id, SPELLTYPE_FIRE);
        ct.set<spellstate>(id, SPELLSTATE_NONE);
        return id;
    }


    entityid create_spell_at_with(vec3 loc) {
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


    void handle_input_character_creation_scene(inputstate& is) {
        if (inputstate_is_pressed(is, KEY_ESCAPE)) {
            do_quit = true;
            return;
        }
        if (inputstate_is_pressed(is, KEY_ENTER)) {
            minfo("Character creation confirmed");
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
            minfo("BEGIN Temporary wedge-in code");
            for (entityid id = 0; id < next_entityid; id++) {
                //minfo("Getting type for id %d", id);
                entitytype_t t = ct.get<entitytype>(id).value_or(ENTITY_NONE);
                if (t == ENTITY_NPC) {
                    minfo("Setting target for id %d", id);
                    ct.set<target_id>(id, hero_id);
                }
            }
            minfo("END Temporary wedge-in code");
            current_scene = SCENE_GAMEPLAY;
        } else if (inputstate_is_pressed(is, KEY_SPACE)) {
            // re-roll character creation stats
            minfo("Re-rolling character creation stats");
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


    bool remove_from_inventory(entityid actor_id, entityid item_id) {
        minfo("remove from inventory: %d %d", actor_id, item_id);
        auto maybe_inventory = ct.get<inventory>(actor_id);
        if (!maybe_inventory.has_value()) {
            merror("maybe_inventory has no value for actor id %d", actor_id);
            return false;
        }
        auto my_items = maybe_inventory.value();
        bool success = false;
        minfo("Inventory size: %ld", my_items->size());
        for (auto it = my_items->begin(); it != my_items->end(); it++) {
            minfo("item id %d", *it);
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

    bool drop_from_inventory(entityid actor_id, entityid item_id) {
        minfo("drop from inventory: %d %d", actor_id, item_id);
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

    bool drop_all_from_inventory(entityid actor_id) {
        minfo("drop all from inventory: %d", actor_id);
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


    void handle_hero_inventory_equip_weapon(entityid item_id) {
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


    void handle_hero_inventory_equip_shield(entityid item_id) {
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


    void handle_hero_inventory_equip_item(entityid item_id) {
        itemtype_t item_type = ct.get<itemtype>(item_id).value_or(ITEM_NONE);
        switch (item_type) {
        case ITEM_NONE: break;
        case ITEM_WEAPON: handle_hero_inventory_equip_weapon(item_id); break;
        case ITEM_SHIELD: handle_hero_inventory_equip_shield(item_id); break;
        default: break;
        }
    }


    void handle_hero_inventory_equip() {
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


    bool drop_item_from_hero_inventory() {
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
        //auto tile = df_tile_at(g.dungeon->floors->at(loc.z), loc);
        //if (!tile)
        //    return false;
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


    bool is_in_inventory(entityid actor_id, entityid item_id) {
        auto maybe_inventory = ct.get<inventory>(actor_id);
        if (!maybe_inventory.has_value()) {
            merror("maybe_inventory has no value for actor id %d", actor_id);
            return false;
        }
        auto my_items = maybe_inventory.value();
        for (auto it = my_items->begin(); it != my_items->end(); it++) {
            minfo("item id %d", *it);
            if (*it == item_id)
                return true;
        }
        return false;
    }


    bool use_potion(entityid actor_id, entityid item_id) {
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
            }
            //====
            else {
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


    void logic_close() {
        d_destroy(dungeon);
    }


    void handle_camera_move(inputstate& is) {
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
};
