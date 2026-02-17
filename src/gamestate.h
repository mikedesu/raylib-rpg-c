#pragma once

#include "ComponentTable.h"
#include "attack_result.h"
#include "biome.h"
#include "calculate_linear_path.h"
#include "character_creation.h"
#include "controlmode.h"
#include "debugpanel.h"
#include "dungeon.h"
#include "dungeon_floor.h"
#include "dungeon_tile_type.h"
#include "entityid.h"
#include "entitytype.h"
#include "gamestate_flag.h"
#include "get_racial_hd.h"
#include "get_racial_modifiers.h"
#include "hunger_points.h"
#include "inputstate.h"
#include "libgame_defines.h"
#include "magic_values.h"
#include "option_menu.h"
#include "orc_names.h"
#include "potion.h"
#include "roll.h"
#include "scene.h"
#include "sfx.h"
#include "stat_bonus.h"
#include "tactics.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <memory>
#include <random>
#include <raylib.h>
#include <raymath.h>

#define DEFAULT_MASTER_VOLUME 1.0f
#define DEFAULT_MUSIC_VOLUME 0.35f
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
#define LAST_FRAME_TIMES_MAX 1000

typedef ComponentTable CT;

typedef function<void(CT& ct, const entityid)> with_fun;

using std::make_pair;
using std::mt19937;
using std::pair;
using std::round;
using std::seed_seq;
using std::uniform_int_distribution;
using std::uniform_real_distribution;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::system_clock;
using std::chrono::time_point;

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
    mt19937 mt;
    string seed;
    bool god_mode;
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
    bool display_action_menu;
    bool display_option_menu;
    bool do_quit;
    bool dirty_entities;
    bool display_help_menu;
    bool cam_changed;
    bool frame_dirty;
    bool music_volume_changed;
    bool do_restart;
    bool msg_system_is_active;
    unsigned int framecount;
    unsigned int fadealpha;
    unsigned int targetwidth;
    unsigned int targetheight;
    unsigned int windowwidth;
    unsigned int windowheight;
    int lock;
    unsigned int frame_updates;
    int pad;
    unsigned int inventory_menu_selection;
    unsigned int gameplay_settings_menu_selection;
    unsigned int title_screen_selection;
    unsigned int max_title_screen_selections;
    unsigned int msg_history_max_len_msg;
    unsigned int msg_history_max_len_msg_measure;
    unsigned int current_music_index;
    unsigned int restart_count;
    unsigned int font_size;
    unsigned int turn_count;
    unsigned long int ticks;
    size_t action_selection;
    float line_spacing;
    float music_volume;
    double last_frame_time;
    vector<double> last_frame_times;
    size_t last_frame_times_current;
    dungeon d;
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
    option_menu options_menu;

    void set_seed() {
        srand(time(NULL));
        SetRandomSeed(time(NULL));
        time_point<system_clock> now = system_clock::now();
        auto dur = duration_cast<nanoseconds>(now.time_since_epoch());
        long long nanoseconds_since_epoch = dur.count();
        seed = std::to_string(nanoseconds_since_epoch);
        seed_seq seedseq(seed.begin(), seed.end());
        mt.seed(seedseq);
    }

    gamestate() {
        minfo2("Initializing gamestate");
        set_seed();
        minfo2("The gamestate seed was set to: %s", seed.c_str());
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
                continue;
            }
            // copy into g->music_file_paths
            // need to pre-pend the folder path
            const string fullpath = "audio/music/" + string(buffer);
            music_file_paths.push_back(fullpath);
        }
        fclose(file);
    }

    inline double get_avg_last_frame_time() {
        double sum = 0;
        for (int i = 0; i < LAST_FRAME_TIMES_MAX; i++) {
            sum += last_frame_times[i];
        }
        return sum / LAST_FRAME_TIMES_MAX;
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
        hero_id = INVALID;
        entity_turn = 1;
        new_entityid_begin = new_entityid_end = ENTITYID_INVALID;
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
        display_action_menu = false;
        display_inventory_menu = false;
        display_help_menu = false;
        do_quit = false;
        cam_changed = false;
        gameover = dirty_entities = false;
        processing_actions = false;
        test_guard = false;
        player_changing_dir = false;
        msg_system_is_active = false;
        music_volume_changed = false;
#ifndef TEST
        test = false;
#else
        test = true;
#endif
#ifndef GODMODE
        god_mode = false;
#else
        god_mode = true;
#endif
#ifndef GOD_MODE
        god_mode = false;
#else
        god_mode = true;
#endif
        gameplay_settings_menu_selection = 0;
        cam2d.target = cam2d.offset = Vector2{0, 0};
        cam2d.zoom = DEFAULT_ZOOM_LEVEL;
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
        do_restart = 0;
        title_screen_selection = 0;
        lock = 0;
        frame_updates = framecount = restart_count = 0;
        last_frame_time = last_frame_times_current = 0;
        last_frame_times.reserve(LAST_FRAME_TIMES_MAX);
        for (size_t i = 0; i < last_frame_times.size(); i++) {
            last_frame_times[i] = 0;
        }
        turn_count = 0;
        action_selection = 0;
        inventory_menu_selection = 0;
        msg_history_max_len_msg_measure = 0;
        msg_history_max_len_msg = 0;
        debugpanel.pad_top = 0;
        debugpanel.pad_left = 0;
        debugpanel.pad_right = 0;
        debugpanel.pad_bottom = 0;
        max_title_screen_selections = 2;
        // initialize character creation
        chara_creation.name = "hero";
        chara_creation.strength = 10;
        chara_creation.dexterity = 10;
        chara_creation.intelligence = 10;
        chara_creation.wisdom = 10;
        chara_creation.constitution = 10;
        chara_creation.charisma = 10;
        chara_creation.race = RACE_HUMAN;
        chara_creation.hitdie = get_racial_hd(RACE_HUMAN);
        current_scene = SCENE_TITLE;
        music_volume = DEFAULT_MUSIC_VOLUME;
        last_click_screen_pos = Vector2{-1, -1};
        msg_system.clear();
        msg_history.clear();
        ct.clear();
        for (size_t i = 0; i < d.floors.size(); i++) {
            d.floors[i]->df_free();
        }
        d.floors.clear();
        d.is_initialized = false;
        init_music_paths();
    }

    inline bool set_hero_id(const entityid id) {
        massert(id != ENTITYID_INVALID, "id is invalid");
        hero_id = id;
        return true;
    }

    inline entityid add_entity() {
        const entityid id = next_entityid;
        if (!dirty_entities) {
            dirty_entities = true;
            new_entityid_begin = id;
            new_entityid_end = id + 1;
        }
        else {
            new_entityid_end = id + 1;
        }
        next_entityid++;
        return id;
    }

    inline shared_ptr<tile_t> tile_at_cur_floor(vec3 loc) {
        return d.get_current_floor()->tile_at(loc);
    }

    inline void create_and_add_df_0(biome_t type, int df_count, int w, int h, float parts) {
        auto df = d.create_floor(type, w, h);
        const float dw = df->get_width();
        const float dh = df->get_height();
        vector<room> rooms;
        room r(0, TextFormat("room-%d", 0), TextFormat("room-%d description", 0), Rectangle{0, 0, dw, dh});
        df->set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, r.get_area());
        d.add_floor(df);
    }

    inline void create_and_add_df_1(biome_t type, int df_count, int w, int h, float parts) {
        constexpr float min_room_w = 2;
        auto df = d.create_floor(type, w, h);
        const float dw = df->get_width();
        const float dh = df->get_height();
        vector<room> rooms;
        vector<room> doorways;
        minfo2("creating rooms");
        for (int i = 0; i < parts; i++) {
            for (int j = 0; j < parts; j++) {
                Rectangle q = {(dw / parts) * j, (dh / parts) * i, dw / parts, dh / parts};
                const float max_room_w = q.width / 2.0;
                uniform_int_distribution<int> pgx(q.x, q.x + q.width / 2.0 - 1);
                uniform_int_distribution<int> pgy(q.y, q.y + q.height / 2.0 - 1);
                uniform_int_distribution<int> sg(min_room_w, max_room_w);
                Rectangle ra1 = {static_cast<float>(pgx(mt)), static_cast<float>(pgy(mt)), static_cast<float>(sg(mt)), static_cast<float>(sg(mt))};
                room_id rid = i * parts + j;
                room r(rid, TextFormat("room-%d", rid), TextFormat("room-%d description", rid), ra1);
                rooms.push_back(r);
            }
        }
        minfo2("setting rooms");
        for (size_t i = 0; i < rooms.size(); i++) {
            if (!df->add_room(rooms[i])) {
                merror("Failed to add room");
                continue;
            }
            df->set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, rooms[i].get_area());
        }
        minfo2("sorting rooms");
        std::sort(rooms.begin(), rooms.end(), [](room& a, room& b) { return a.get_x() < b.get_x() && a.get_y() < b.get_y(); });
        for (size_t y = 0; y < parts; y++) {
            for (size_t x = 0; x < parts; x++) {
                const size_t index = y * parts + x;
                room& r = rooms[index];
                const int rx = r.get_x() + r.get_w() / 2;
                const int ry = r.get_y() + r.get_h() / 2;
                const float rix = rx;
                const float riy = ry;
                if (x < parts - 1) {
                    room& r2 = rooms[index + 1];
                    const int rx_e = r.get_x() + r.get_w();
                    const float rxf_e = r.get_x() + r.get_w();
                    const int rjx_e = r2.get_x();
                    const float rw = rjx_e - rx_e;
                    Rectangle c0 = {rxf_e, riy, rw, 1};
                    df->set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, c0);
                    shared_ptr<tile_t> first_tile = df->tile_at(vec3{rx_e, ry, -1});
                    first_tile->set_can_have_door(true);
                    const int rw_i = rw;
                    shared_ptr<tile_t> last_tile = df->tile_at(vec3{rx_e + rw_i - 1, ry, -1});
                    last_tile->set_can_have_door(true);
                }
                if (y < parts - 1) {
                    const size_t index2 = (y + 1) * parts + x;
                    room& r2 = rooms[index2];
                    const int ry_e = r.get_y() + r.get_h();
                    const float ryf_e = r.get_y() + r.get_h();
                    const int rjy_e = r2.get_y();
                    const float rh = rjy_e - ry_e;
                    Rectangle c0 = {rix, ryf_e, 1, rh};
                    df->set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, c0);
                    shared_ptr<tile_t> first_tile = df->tile_at(vec3{rx, ry_e, -1});
                    first_tile->set_can_have_door(true);
                }
            }
        }
        for (int y = 1; y < dh - 1; y++) {
            for (int x = 1; x < dw - 1; x++) {
                const vec3 loc = {x, y, -1};
                if (df->tile_is_good_for_upgrade(loc)) {
                    df->set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_10, Rectangle{static_cast<float>(loc.x), static_cast<float>(loc.y), 1, 1});
                }
            }
        }

        d.add_floor(df);
    }

    inline void init_dungeon(biome_t type, int df_count, int w, int h, float parts) {
        //constexpr float min_room_w = 2;
        minfo2("init_dungeon");
        massert(df_count > 0, "df_count is <= 0");
        massert(w > 0, "w == 0");
        massert(h > 0, "h == 0");
        massert(df_count > 0, "df_count == 0");
        massert(type > BIOME_NONE, "biome is invalid");
        massert(type < BIOME_COUNT, "biome is invalid 2");
        if (d.is_initialized) {
            merror("dungeon is already initialized");
            return;
        }
        for (int i = 0; i < df_count; i++) {
            create_and_add_df_0(type, df_count, w, h, parts);
            //create_and_add_df_1(type, df_count, w, h, parts);

            /*
            auto df = d.create_floor(type, w, h);
            const float dw = df->get_width();
            const float dh = df->get_height();
            vector<room> rooms;
            vector<room> doorways;
            minfo2("creating rooms");
            for (int i = 0; i < parts; i++) {
                for (int j = 0; j < parts; j++) {
                    Rectangle q = {(dw / parts) * j, (dh / parts) * i, dw / parts, dh / parts};
                    const float max_room_w = q.width / 2.0;
                    uniform_int_distribution<int> pgx(q.x, q.x + q.width / 2.0 - 1);
                    uniform_int_distribution<int> pgy(q.y, q.y + q.height / 2.0 - 1);
                    uniform_int_distribution<int> sg(min_room_w, max_room_w);
                    Rectangle ra1 = {static_cast<float>(pgx(mt)), static_cast<float>(pgy(mt)), static_cast<float>(sg(mt)), static_cast<float>(sg(mt))};
                    room_id rid = i * parts + j;
                    room r(rid, TextFormat("room-%d", rid), TextFormat("room-%d description", rid), ra1);
                    rooms.push_back(r);
                }
            }
            minfo2("setting rooms");
            for (size_t i = 0; i < rooms.size(); i++) {
                if (!df->add_room(rooms[i])) {
                    merror("Failed to add room");
                    continue;
                }
                df->set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, rooms[i].get_area());
            }
            minfo2("sorting rooms");
            std::sort(rooms.begin(), rooms.end(), [](room& a, room& b) { return a.get_x() < b.get_x() && a.get_y() < b.get_y(); });
            for (size_t y = 0; y < parts; y++) {
                for (size_t x = 0; x < parts; x++) {
                    const size_t index = y * parts + x;
                    room& r = rooms[index];
                    const int rx = r.get_x() + r.get_w() / 2;
                    const int ry = r.get_y() + r.get_h() / 2;
                    const float rix = rx;
                    const float riy = ry;
                    if (x < parts - 1) {
                        room& r2 = rooms[index + 1];
                        const int rx_e = r.get_x() + r.get_w();
                        const float rxf_e = r.get_x() + r.get_w();
                        const int rjx_e = r2.get_x();
                        const float rw = rjx_e - rx_e;
                        Rectangle c0 = {rxf_e, riy, rw, 1};
                        df->set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, c0);
                        shared_ptr<tile_t> first_tile = df->tile_at(vec3{rx_e, ry, -1});
                        first_tile->set_can_have_door(true);
                        const int rw_i = rw;
                        shared_ptr<tile_t> last_tile = df->tile_at(vec3{rx_e + rw_i - 1, ry, -1});
                        last_tile->set_can_have_door(true);
                    }
                    if (y < parts - 1) {
                        const size_t index2 = (y + 1) * parts + x;
                        room& r2 = rooms[index2];
                        const int ry_e = r.get_y() + r.get_h();
                        const float ryf_e = r.get_y() + r.get_h();
                        const int rjy_e = r2.get_y();
                        const float rh = rjy_e - ry_e;
                        Rectangle c0 = {rix, ryf_e, 1, rh};
                        df->set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_11, c0);
                        shared_ptr<tile_t> first_tile = df->tile_at(vec3{rx, ry_e, -1});
                        first_tile->set_can_have_door(true);
                    }
                }
            }
            for (int y = 1; y < dh - 1; y++) {
                for (int x = 1; x < dw - 1; x++) {
                    const vec3 loc = {x, y, -1};
                    if (df->tile_is_good_for_upgrade(loc))
                        df->set_area(TILE_FLOOR_STONE_00, TILE_FLOOR_STONE_10, Rectangle{static_cast<float>(loc.x), static_cast<float>(loc.y), 1, 1});
                }
            }

            minfo2("adding floor to dungeon...");
            d.add_floor(df);
            */
        }
        minfo2("END floor creation loop");
        d.is_initialized = true;
    }

    inline entityid create_door_with(with_fun doorInitFunction) {
        const entityid id = add_entity();
        ct.set<entitytype>(id, ENTITY_DOOR);
        doorInitFunction(ct, id);
        return id;
    }

    inline void recompute_entity_cache(shared_ptr<tile_t> t) {
        // Only recompute if cache is dirty
        if (t == nullptr) {
            return;
        }

        if (!t->get_dirty_entities()) {
            return;
        }
        // Reset counters
        t->set_cached_live_npcs(0);
        t->set_cached_item_count(0);
        t->set_cached_player_present(false);
        t->set_cached_npc(ENTITYID_INVALID);
        t->set_cached_item(ENTITYID_INVALID);
        // Iterate through all entities on the tile
        for (size_t i = 0; i < t->get_entity_count(); i++) {
            const entityid id = t->get_entity_at(i);
            // Skip dead entities
            if (ct.get<dead>(id).value_or(false)) {
                continue;
            }
            // Check entity type
            const entitytype_t type = ct.get<entitytype>(id).value_or(ENTITY_NONE);
            if (type == ENTITY_NPC) {
                t->set_cached_live_npcs(t->get_cached_live_npcs() + 1);
                t->set_cached_npc(id);
            }
            else if (type == ENTITY_PLAYER) {
                t->set_cached_player_present(true);
                t->set_cached_npc(id);
            }
            else if (type == ENTITY_ITEM) {
                t->set_cached_item_count(t->get_cached_item_count() + 1);
                t->set_cached_item(id);
            }
        }
        // Cache is now clean
        t->set_dirty_entities(false);
    }

    inline void recompute_entity_cache_at(vec3 l) {
        massert(l.x >= 0 && l.y >= 0 && l.z >= 0, "x, y, or z is out of bounds: %d, %d, %d", l.x, l.y, l.z);
        massert((size_t)l.z < d.floors.size(), "z is out of bounds");
        shared_ptr<dungeon_floor> df = d.floors[l.z];
        shared_ptr<tile_t> t = df->tile_at(l);
        recompute_entity_cache(t);
    }

    inline bool tile_has_live_npcs(shared_ptr<tile_t> t) {
        recompute_entity_cache(t);
        return t->get_cached_live_npcs() > 0;
    }

    inline bool tile_has_player(shared_ptr<tile_t> t) {
        recompute_entity_cache(t);
        return t->get_cached_player_present();
    }

    inline entityid create_door_at_with(const vec3 loc, with_fun doorInitFunction) {
        shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
        shared_ptr<tile_t> tile = df->tile_at(loc);
        if (!tile_is_walkable(tile->get_type())) {
            return INVALID;
        }
        if (tile_has_live_npcs(tile)) {
            return INVALID;
        }
        const entityid id = create_door_with(doorInitFunction);
        if (id == INVALID) {
            return INVALID;
        }
        if (!df->df_add_at(id, loc)) {
            return INVALID;
        }
        ct.set<location>(id, loc);
        ct.set<door_open>(id, false);
        ct.set<update>(id, true);
        return id;
    }

    inline size_t place_doors() {
        minfo2("gamestate.place_doors");
        size_t placed_doors = 0;
        for (size_t z = 0; z < d.floors.size(); z++) {
            shared_ptr<dungeon_floor> df = d.get_floor(z);
            for (int x = 0; x < df->get_width(); x++) {
                for (int y = 0; y < df->get_height(); y++) {
                    const vec3 loc = {x, y, static_cast<int>(z)};
                    shared_ptr<tile_t> tile = df->tile_at(loc);
                    if (!tile->get_can_have_door()) {
                        continue;
                    }
                    const entityid door_id = create_door_at_with(loc, [](CT& ct, const entityid id) {});
                    if (door_id == ENTITYID_INVALID) {
                        continue;
                    }
                    placed_doors++;
                }
            }
        }
        msuccess("placed %ld doors", placed_doors);
        return placed_doors;
    }

    inline entityid create_prop_with(proptype_t type, with_fun initFun) {
        const entityid id = add_entity();
        ct.set<entitytype>(id, ENTITY_PROP);
        ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
        ct.set<update>(id, true);
        ct.set<proptype>(id, type);
        initFun(ct, id);
        return id;
    }

    inline entityid create_prop_at_with(proptype_t type, vec3 loc, with_fun initFun) {
        shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
        const entityid id = create_prop_with(type, initFun);
        if (id == ENTITYID_INVALID) {
            return ENTITYID_INVALID;
        }
        const entityid result = df->df_add_at(id, loc);
        if (result == ENTITYID_INVALID) {
            return ENTITYID_INVALID;
        }
        ct.set<location>(id, loc);
        return id;
    }

    inline int place_props() {
        //auto mydefault = [](gamestate& g, entityid id) {};
        //auto set_solid = [](gamestate& g, entityid id) { g.ct.set<solid>(id, true); };
        //auto set_solid_and_pushable = [](gamestate& g, entityid id) {
        //    g.ct.set<solid>(id, true);
        //    g.ct.set<pushable>(id, true);
        //};
        auto defaultInit = [](CT& ct, const entityid id) {};
        int placed_props = 0;
        for (int z = 0; z < (int)d.floors.size(); z++) {
            shared_ptr<dungeon_floor> df = d.get_floor(z);
            for (int x = 0; x < df->get_width(); x++) {
                for (int y = 0; y < df->get_height(); y++) {
                    const vec3 loc = {x, y, z};
                    shared_ptr<tile_t> t = df->tile_at(loc);
                    if (t->get_type() == TILE_UPSTAIRS || t->get_type() == TILE_DOWNSTAIRS) {
                        continue;
                    }
                    if (t->get_can_have_door()) {
                        continue;
                    }
                    if (t->tile_is_wall()) {
                        uniform_int_distribution<int> gen(0, 20);
                        const int flip = gen(mt);
                        if (flip == 0) {
                            uniform_int_distribution<int> gen2(1, 3);
                            const int r = gen2(mt);
                            switch (r) {
                            case 1: {
                                const entityid id = create_prop_at_with(PROP_DUNGEON_BANNER_00, loc, defaultInit);
                                if (id != ENTITYID_INVALID) {
                                    placed_props++;
                                }
                            } break;
                            case 2: {
                                const entityid id = create_prop_at_with(PROP_DUNGEON_BANNER_01, loc, defaultInit);
                                if (id != ENTITYID_INVALID) {
                                    placed_props++;
                                }
                            } break;
                            case 3: {
                                const entityid id = create_prop_at_with(PROP_DUNGEON_BANNER_02, loc, defaultInit);
                                if (id != ENTITYID_INVALID) {
                                    placed_props++;
                                }
                            } break;
                            default: break;
                            }
                        }
                    }
                    else {
                        uniform_int_distribution<int> gen(0, 20);
                        //const int flip = GetRandomValue(0, 20);
                        const int flip = gen(mt);
                        if (flip == 0) {
                            const entityid id = create_prop_at_with(PROP_DUNGEON_WOODEN_TABLE_00, loc, defaultInit);
                            ct.set<solid>(id, true);
                            if (id != INVALID) {
                                placed_props++;
                            }
                        }
                        else if (flip == 1) {
                            const entityid id = create_prop_at_with(PROP_DUNGEON_WOODEN_TABLE_01, loc, defaultInit);
                            if (id != INVALID) {
                                placed_props++;
                                ct.set<solid>(id, true);
                            }
                        }
                        else if (flip == 2) {
                            const entityid id = create_prop_at_with(PROP_DUNGEON_WOODEN_CHAIR_00, loc, defaultInit);
                            if (id != INVALID) {
                                placed_props++;
                            }
                        }
                        else if (flip == 3) {
                            const entityid id = create_prop_at_with(PROP_DUNGEON_STATUE_00, loc, defaultInit);
                            if (id != INVALID) {
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

    inline entityid create_weapon_with(with_fun weaponInitFunction) {
        //minfo("create weapon with");
        const entityid id = add_entity();
        ct.set<entitytype>(id, ENTITY_ITEM);
        ct.set<itemtype>(id, ITEM_WEAPON);
        ct.set<spritemove>(id, Rectangle{0, 0, 0, 0});
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

    inline with_fun axe_init() {
        return [](CT& ct, const entityid id) {
            ct.set<name>(id, "axe");
            ct.set<description>(id, "Choppy choppy");
            ct.set<weapontype>(id, WEAPON_AXE);
            ct.set<damage>(id, (vec3){1, 8, 0});
            ct.set<durability>(id, 100);
            ct.set<max_durability>(id, 100);
            ct.set<rarity>(id, RARITY_COMMON);
        };
    }

    inline with_fun sword_init() {
        return [](CT& ct, const entityid id) {
            ct.set<name>(id, "short sword");
            ct.set<description>(id, "your basic soldier's short sword");
            ct.set<weapontype>(id, WEAPON_SHORT_SWORD);
            ct.set<damage>(id, vec3{1, 6, 0});
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
            ct.set<rarity>(id, RARITY_COMMON);
            ct.set<durability>(id, 100);
            ct.set<max_durability>(id, 100);
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

    inline with_fun player_init(const int maxhp_roll) {
        return [this, maxhp_roll](CT& ct, const entityid id) {
            ct.set<strength>(id, chara_creation.strength);
            ct.set<dexterity>(id, chara_creation.dexterity);
            ct.set<constitution>(id, chara_creation.constitution);
            ct.set<intelligence>(id, chara_creation.intelligence);
            ct.set<wisdom>(id, chara_creation.wisdom);
            ct.set<charisma>(id, chara_creation.charisma);
            ct.set<hd>(id, (vec3){1, chara_creation.hitdie, 0});
            ct.set<hp>(id, maxhp_roll);
            ct.set<maxhp>(id, maxhp_roll);
        };
    }

    inline entityid create_weapon_at_with(ComponentTable& ct, const vec3 loc, with_fun weaponInitFunction) {
        minfo2("create weapon at with: %d %d %d", loc.x, loc.y, loc.z);
        if (d.floors.size() == 0) {
            merror2("dungeon floors size is 0");
            return INVALID;
        }
        if (!d.is_initialized) {
            merror2("dungeon is_initialized flag not set");
            return INVALID;
        }
        if (vec3_invalid(loc)) {
            merror2("loc is invalid");
            return INVALID;
        }
        shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
        shared_ptr<tile_t> tile = df->tile_at(loc);
        if (!tile_is_walkable(tile->get_type())) {
            merror2("cannot create entity on non-walkable tile");
            return INVALID;
        }
        if (tile_has_live_npcs(tile)) {
            merror2("cannot create entity on tile with live NPCs");
            return INVALID;
        }
        const entityid id = create_weapon_with(weaponInitFunction);
        if (id == ENTITYID_INVALID) {
            minfo2("failed to create weapon");
            return INVALID;
        }
        if (df->df_add_at(id, loc) == ENTITYID_INVALID) {
            minfo2("failed to add weapon to df");
            return INVALID;
        }
        ct.set<location>(id, loc);
        return id;
    }

    inline entityid create_weapon_at_random_loc_with(CT& ct, with_fun weaponInitFunction) {
        const vec3 loc = d.floors[d.current_floor]->get_random_loc();
        if (vec3_invalid(loc)) {
            merror("loc is invalid");
            return INVALID;
        }
        return create_weapon_at_with(ct, loc, dagger_init());
    }

    inline entityid create_shield_with(ComponentTable& ct, with_fun shieldInitFunction) {
        const entityid id = add_entity();
        ct.set<entitytype>(id, ENTITY_ITEM);
        ct.set<itemtype>(id, ITEM_SHIELD);
        ct.set<durability>(id, 100);
        ct.set<max_durability>(id, 100);
        ct.set<rarity>(id, RARITY_COMMON);
        ct.set<update>(id, false);
        shieldInitFunction(ct, id);
        return id;
    }

    inline entityid create_shield_at_with(ComponentTable& ct, const vec3 loc, with_fun shieldInitFunction) {
        if (d.floors.size() == 0) {
            return INVALID;
        }
        const entityid id = create_shield_with(ct, shieldInitFunction);
        shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
        if (!df->df_add_at(id, loc)) {
            return INVALID;
        }
        ct.set<location>(id, loc);
        return id;
    }

    inline entityid create_potion_with(with_fun potionInitFunction) {
        const entityid id = add_entity();
        ct.set<entitytype>(id, ENTITY_ITEM);
        ct.set<itemtype>(id, ITEM_POTION);
        ct.set<update>(id, true);
        potionInitFunction(ct, id);
        return id;
    }

    inline entityid create_potion_at_with(const vec3 loc, with_fun potionInitFunction) {
        shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
        shared_ptr<tile_t> tile = df->tile_at(loc);
        if (!tile_is_walkable(tile->get_type())) {
            return INVALID;
        }
        if (tile_has_live_npcs(tile)) {
            return INVALID;
        }
        const entityid id = create_potion_with(potionInitFunction);
        if (id == INVALID) {
            return INVALID;
        }
        if (!df->df_add_at(id, loc)) {
            return INVALID;
        }
        ct.set<location>(id, loc);
        ct.set<update>(id, true);
        return id;
    }

    inline race_t random_monster_type() {
        const vector<race_t> monster_races = {RACE_GOBLIN, RACE_ORC, RACE_BAT, RACE_WOLF, RACE_WARG, RACE_ZOMBIE, RACE_SKELETON, RACE_RAT, RACE_GREEN_SLIME};
        uniform_int_distribution<int> gen(0, monster_races.size() - 1);
        const int random_index = gen(mt);
        return monster_races[random_index];
    }

    inline void set_npc_starting_stats(const entityid id) {
        const race_t rt = ct.get<race>(id).value_or(RACE_NONE);
        if (rt == RACE_NONE) {
            return;
        }
        // stats racial modifiers for stats
        const int str_m = get_racial_modifiers(rt, 0);
        const int dex_m = get_racial_modifiers(rt, 1);
        const int int_m = get_racial_modifiers(rt, 2);
        const int wis_m = get_racial_modifiers(rt, 3);
        const int con_m = get_racial_modifiers(rt, 4);
        const int cha_m = get_racial_modifiers(rt, 5);
        // default to 3-18 for stats
        uniform_int_distribution<int> gen(3, 18);
        //const int strength_ = GetRandomValue(3, 18) + str_m;
        //const int dexterity_ = GetRandomValue(3, 18) + dex_m;
        //const int intelligence_ = GetRandomValue(3, 18) + int_m;
        //const int wisdom_ = GetRandomValue(3, 18) + wis_m;
        //const int constitution_ = GetRandomValue(3, 18) + con_m;
        //const int charisma_ = GetRandomValue(3, 18) + cha_m;
        const int strength_ = gen(mt) + str_m;
        const int dexterity_ = gen(mt) + dex_m;
        const int intelligence_ = gen(mt) + int_m;
        const int wisdom_ = gen(mt) + wis_m;
        const int constitution_ = gen(mt) + con_m;
        const int charisma_ = gen(mt) + cha_m;
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
        uniform_int_distribution<int> gen2(1, hitdie.y);
        //const int my_maxhp = std::max(1, GetRandomValue(1, hitdie.y) + get_stat_bonus(constitution_));
        const int my_maxhp = std::max(1, gen2(mt) + get_stat_bonus(constitution_));
        const int my_hp = my_maxhp;
        ct.set<maxhp>(id, my_maxhp);
        ct.set<hp>(id, my_hp);
        ct.set<base_ac>(id, 10);
        ct.set<hd>(id, hitdie);
    }

    inline void set_npc_defaults(entityid id) {
        ct.set<entitytype>(id, ENTITY_NPC);
        ct.set<spritemove>(id, Rectangle{0, 0, 0, 0});
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
        // all NPCs begin at level 1. level-up mechanisms will be determined elsewhere
        ct.set<level>(id, 1);
        ct.set<xp>(id, 0);
        //tactic t = {tactic_target::enemy, tactic_condition::adjacent, tactic_action::wait};
        //vector<tactic> my_tactics = {
        //    {tactic_target::enemy, tactic_condition::adjacent, tactic_action::attack}, {tactic_target::nil, tactic_condition::any, tactic_action::move}};

        //vector<tactic> my_tactics = {
        //    {tactic_target::enemy, tactic_condition::adjacent, tactic_action::attack},
        //    {tactic_target::nil, tactic_condition::any, tactic_action::move}
        //};

        vector<tactic> my_tactics = {//{tactic_target::enemy, tactic_condition::adjacent, tactic_action::attack},
                                     {tactic_target::nil, tactic_condition::any, tactic_action::move}};




        //vector<tactic> my_tactics = {
        //    {tactic_target::enemy, tactic_condition::adjacent, tactic_action::attack},
        //};

        ct.set<tactics>(id, my_tactics);
        ct.set<hunger_points>(id, hunger_points_t{100, 100});
    }

    inline entityid create_npc_with(const race_t rt, with_fun npcInitFunction) {
        const entityid id = add_entity();
        set_npc_defaults(id);
        ct.set<race>(id, rt);
        set_npc_starting_stats(id);
        npcInitFunction(ct, id);
        return id;
    }




    inline entityid tile_has_box(int x, int y, int z) {
        massert(z >= 0, "floor is out of bounds");
        massert((size_t)z < d.floors.size(), "floor is out of bounds");
        auto df = d.get_floor(z);
        auto t = df->tile_at((vec3){x, y, z});
        for (int i = 0; (size_t)i < t->get_entity_count(); i++) {
            const entityid id = t->tile_get_entity(i);
            const entitytype_t type = ct.get<entitytype>(id).value_or(ENTITY_NONE);
            if (id != INVALID && type == ENTITY_BOX) {
                return id;
            }
        }
        return INVALID;
    }




    inline entityid tile_has_pullable(int x, int y, int z) {
        minfo("tile_has_pullable(%d, %d, %d)", x, y, z);
        massert(z >= 0, "floor is out of bounds");
        massert((size_t)z < d.floors.size(), "floor is out of bounds");
        shared_ptr<dungeon_floor> df = d.get_floor(z);
        shared_ptr<tile_t> t = df->tile_at(vec3{x, y, z});
        for (size_t i = 0; i < t->get_entity_count(); i++) {
            const entityid id = t->tile_get_entity(i);
            const bool is_pullable = ct.get<pullable>(id).value_or(false);
            if (id != ENTITYID_INVALID && is_pullable) {
                return id;
            }
        }
        return ENTITYID_INVALID;
    }




    inline entityid create_npc_at_with(race_t rt, vec3 loc, with_fun npcInitFunction) {
        minfo2("create npc at with: (%d, %d, %d)", loc.x, loc.y, loc.z);
        auto df = d.get_floor(loc.z);
        auto tile = df->tile_at(loc);
        if (!tile_is_walkable(tile->get_type())) {
            merror2("cannot create entity on non-walkable tile: tile.type: %s", tiletype2str(tile->get_type()).c_str());
            return INVALID;
        }
        if (tile_has_live_npcs(tile)) {
            merror2("cannot create entity on tile with live NPCs");
            return INVALID;
        }
        if (tile_has_box(loc.x, loc.y, loc.z) != ENTITYID_INVALID) {
            merror2("cannot create entity on tile with box");
            return INVALID;
        }
        if (tile_has_pushable(loc.x, loc.y, loc.z) != ENTITYID_INVALID) {
            merror2("cannot create entity on tile with pushable");
            return INVALID;
        }
        const entityid id = create_npc_with(rt, npcInitFunction);
        if (id == INVALID) {
            merror("failed to create npc");
            return INVALID;
        }
        if (df->df_add_at(id, loc) == INVALID) {
            merror("failed to add npc %d to %d, %d", id, loc.x, loc.y);
            return INVALID;
        }
        minfo2("setting location for %d", id);
        ct.set<location>(id, loc);
        msuccess2("created npc %d", id);
        return id;
    }

    inline bool add_to_inventory(entityid actor_id, entityid item_id) {
        minfo2("adding %d to %d's inventory", actor_id, item_id);
        auto maybe_inventory = ct.get<inventory>(actor_id);
        if (!maybe_inventory.has_value()) {
            merror2("%d has no inventory component", actor_id);
            return false;
        }
        auto inventory = maybe_inventory.value();
        inventory->push_back(item_id);
        msuccess2("added %d to %d's inventory", actor_id, item_id);
        return true;
    }

    inline entityid create_orc_with(with_fun monsterInitFunction) {
        constexpr race_t r = RACE_ORC;
        const entityid id = create_npc_with(r, monsterInitFunction);
        ct.set<name>(id, get_random_orc_name());
        return id;
    }

    inline entityid create_orc_at_with(vec3 loc, with_fun monsterInitFunction) {
        if (vec3_invalid(loc)) {
            return ENTITYID_INVALID;
        }
        auto df = d.get_floor(loc.z);
        auto t = df->tile_at(loc);
        if (!tile_is_walkable(t->get_type())) {
            return ENTITYID_INVALID;
        }
        if (tile_has_live_npcs(t)) {
            return ENTITYID_INVALID;
        }
        const entityid id = create_orc_with(monsterInitFunction);
        if (id == ENTITYID_INVALID) {
            return ENTITYID_INVALID;
        }
        if (!df->df_add_at(id, loc)) {
            return ENTITYID_INVALID;
        }
        df->add_living_npc(id);
        ct.set<location>(id, loc);
        ct.set<update>(id, true);
        return id;
    }

    inline bool add_message(const char* fmt, ...) {
        massert(fmt, "format string is NULL");
        char buffer[MAX_MSG_LENGTH];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, MAX_MSG_LENGTH - 1, fmt, args);
        va_end(args);
        string s(buffer);
        msg_system.push_back(s);
        msg_system_is_active = true;
        return true;
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

    inline void update_tile(shared_ptr<tile_t> tile) {
        tile->set_explored(true);
        tile->set_visible(true);
    }

    inline bool path_blocked(vec3 a, vec3 b) {
        vector<vec3> path = calculate_path_with_thickness(a, b);
        auto df = d.get_current_floor();
        for (auto loc : path) {
            auto t = df->tile_at(loc);
            if (tiletype_is_none(t->get_type())) {
                return true;
            }
            else if (tiletype_is_wall(t->get_type())) {
                return true;
            }
            // also need to check for a door or other blockades
            for (auto id : *t->get_entities()) {
                const bool door_present = ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_DOOR;
                if (door_present) {
                    const bool door_is_open = ct.get<door_open>(id).value_or(false);
                    if (!door_is_open) {
                        return true;
                    }
                }
            }
        }
        return false;
    }




    inline bool update_player_tiles_explored() {
        if (current_scene != SCENE_GAMEPLAY) {
            return false;
        }

        if (hero_id == ENTITYID_INVALID) {
            merror2("hero_id is invalid");
            return false;
        }

        auto df = d.get_current_floor();
        auto maybe_loc = ct.get<location>(hero_id);

        if (!maybe_loc.has_value()) {
            merror2("hero location lacks value");
            return false;
        }

        const vec3 hero_loc = maybe_loc.value();
        const int light_radius0 = ct.get<light_radius>(hero_id).value_or(1);
        // Precompute bounds for the loops
        const int min_x = std::max(0, hero_loc.x - light_radius0);
        const int max_x = std::min(df->get_width() - 1, hero_loc.x + light_radius0);
        const int min_y = std::max(0, hero_loc.y - light_radius0);
        const int max_y = std::min(df->get_height() - 1, hero_loc.y + light_radius0);

        for (int y = min_y; y <= max_y; y++) {
            for (int x = min_x; x <= max_x; x++) {
                // Calculate Manhattan distance for diamond shape
                if (abs(x - hero_loc.x) + abs(y - hero_loc.y) > light_radius0) {
                    continue;
                }
                // we need to see if there is anything blocking us between the player and this hero_location
                const vec3 loc = {x, y, hero_loc.z};
                const bool blocked = path_blocked(hero_loc, loc);
                if (blocked) {
                    continue;
                }
                auto t = df->tile_at(loc);
                update_tile(t);
            }
        }
        return true;
    }




    inline bool update_player_state() {
        if (hero_id == ENTITYID_INVALID) {
            merror2("hero_id is invalid");
            return false;
        }
        const unsigned char a = ct.get<txalpha>(hero_id).value_or(255);
        if (a < 255) {
            ct.set<txalpha>(hero_id, a + 1);
        }
        if (ct.get<dead>(hero_id).value_or(true)) {
            merror2("hero_id is dead");
            gameover = true;
            return true;
        }
        ct.set<blocking>(hero_id, false);
        ct.set<block_success>(hero_id, false);
        ct.set<damaged>(hero_id, false);
        return true;
    }

    inline void update_spells_state() {
        minfo2("update_spells_state");
        for (entityid id = 0; id < next_entityid; id++) {
            if (id == hero_id || ct.get<entitytype>(id).value_or(ENTITY_NONE) != ENTITY_SPELL) {
                continue;
            }
            unsigned char a = ct.get<txalpha>(id).value_or(255);
            if (a < 255) {
                a++;
            }
            ct.set<txalpha>(id, a);
            const bool is_complete = ct.get<spell_complete>(id).value_or(false);
            const bool is_destroyed = ct.get<destroyed>(id).value_or(false);
            if (is_complete && is_destroyed) {
                // remove it from the tile
                auto df = d.get_current_floor();
                const vec3 loc = ct.get<location>(id).value_or(vec3{-1, -1, -1});
                massert(!vec3_invalid(loc), "location is invalid");
                if (!df->df_remove_at(id, loc)) {
                    merror("failed to remove id %d at %d, %d", id, loc.x, loc.y);
                }
            }
        }
    }

    inline void update_npcs_state() {
        minfo2("update_npcs_state");
        auto df_npcs = d.get_current_floor()->get_living_npcs();
        for (entityid id : *df_npcs) {
            unsigned char a = ct.get<txalpha>(id).value_or(255);
            if (a < 255) {
                a++;
            }
            ct.set<txalpha>(id, a);
            ct.set<damaged>(id, false);
        }
    }

    inline size_t count_live_npcs_on_floor(size_t floor) {
        auto df = d.get_floor(floor);
        size_t count = 0;
        for (int i = 0; i < df->get_width(); i++) {
            for (int j = 0; j < df->get_height(); j++)
                if (tile_has_live_npcs(df->tile_at(vec3{i, j, -1}))) {
                    count++;
                }
        }
        return count;
    }

    inline void logic_init() {
        minfo("gamestate.logic_init");
        srand(time(NULL));
        SetRandomSeed(time(NULL));
        // 8x8   = 4 4x4 areas
        // 16x16 = 4 8x8 areas
        // 32x32 = 4 16x16 areas = 8 8x8 areas

        const int w = 32; // 4x4 4x4 areas
        const int h = 32;
        constexpr float parts = 1.0;

        //const int w = 32; // 4x4 4x4 areas
        //const int h = 32;
        //constexpr float parts = 4.0;

        init_dungeon(BIOME_STONE, 1, w, h, parts);
        massert(d.floors.size() > 0, "dungeon.floors.size is 0");
        place_doors();
        //place_props();
        //const vec3 loc0 = d.floors[0].df_get_random_loc();
        auto df = d.get_current_floor();
        //auto rl0 = df->df_get_random_loc();
        //create_weapon_at_with(ct, df->get_random_loc(), dagger_init());
        create_weapon_at_with(ct, df->get_random_loc(), axe_init());

        create_shield_at_with(ct, df->get_random_loc(), shield_init());
        //create_shield_at_with(ct, df->get_random_loc(), shield_init());
        //create_shield_at_with(ct, df->get_random_loc(), shield_init());

        //create_potion_at_with(d.floors[0].df_get_random_loc(), potion_init(POTION_HP_SMALL));
        //constexpr int num_boxes = 4;
        //for (int i = 0; i < num_boxes; i++) {
        //    create_box_at_with(df->get_random_loc());
        //}
        constexpr int monster_count = 1;
        for (int j = 0; j < monster_count; j++) {
            const vec3 random_loc = d.get_floor(0)->get_random_loc();
            create_orc_at_with(random_loc, [this](CT& ct, const entityid id) {
                const entityid wpn_id = create_weapon_with([](CT& ct, const entityid id) {
                    ct.set<name>(id, "Dagger");
                    ct.set<description>(id, "Stabby stabby.");
                    ct.set<weapontype>(id, WEAPON_DAGGER);
                    ct.set<damage>(id, (vec3){1, 4, 0});
                    ct.set<durability>(id, 100);
                    ct.set<max_durability>(id, 100);
                    ct.set<rarity>(id, RARITY_COMMON);
                });
                const entityid potion_id = create_potion_with([](CT& ct, const entityid id) {
                    ct.set<name>(id, "small healing potion");
                    ct.set<description>(id, "a small healing potion");
                    ct.set<potiontype>(id, POTION_HP_SMALL);
                    ct.set<healing>(id, (vec3){1, 6, 0});
                });
                add_to_inventory(id, wpn_id);
                add_to_inventory(id, potion_id);
                ct.set<equipped_weapon>(id, wpn_id);
            });
        }
        //    }
        //}
        msuccess("end creating monsters...");
        add_message("Welcome to the game! Press enter to cycle messages.");
        add_message("For help, press ?");
        // lets not deal with "multi-line" messages...
        //add_message("This is a multi-line test\nHow will my game handle newlines?");
        //add_message("This is a multi-line test\nHow will my game handle newlines again?\nOh no lol");
        //#ifdef START_MESSAGES
        //add_message("To pick up items, press / ");
        //add_message("To manage inventory, press i ");
        //add_message("To equip/unequip an item, highlight and press e ");
        //add_message("To drop an item, highlight and press q ");
        //add_message("To attack, press ' ");
        //add_message("To go up/down a floor, press . ");
        //add_message("To wait a turn, press s s ");
        //add_message("To change direction, press s and then [q w e a d z x c] ");
        //add_message("To open a door, face it and press o ");
        //#endif
        msuccess("liblogic_init: Game state initialized");
    }

    inline void handle_input_title_scene(const inputstate& is) {
        if (inputstate_is_pressed(is, KEY_ESCAPE)) {
            do_quit = true;
            return;
        }
        if (inputstate_is_pressed(is, KEY_ENTER) || inputstate_is_pressed(is, KEY_SPACE)) {
            current_scene = SCENE_MAIN_MENU;
            frame_dirty = true;
            PlaySound(sfx[SFX_CONFIRM_01]);
        }
    }

    inline void handle_input_main_menu_scene(const inputstate& is) {
        if (inputstate_is_pressed(is, KEY_ENTER) || inputstate_is_pressed(is, KEY_SPACE)) {
            if (title_screen_selection == 0) {
                current_scene = SCENE_CHARACTER_CREATION;
                frame_dirty = true;
            }
            PlaySound(sfx.at(SFX_CONFIRM_01));
        }
        else if (inputstate_is_pressed(is, KEY_DOWN)) {
            title_screen_selection++;
            if (title_screen_selection >= max_title_screen_selections) {
                title_screen_selection = 0;
            }
            PlaySound(sfx.at(SFX_CONFIRM_01));
        }
        else if (inputstate_is_pressed(is, KEY_UP)) {
            title_screen_selection--;
            if (title_screen_selection < 0) {
                title_screen_selection = max_title_screen_selections - 1;
            }
            PlaySound(sfx.at(SFX_CONFIRM_01));
        }
        else if (inputstate_is_pressed(is, KEY_ESCAPE)) {
            do_quit = true;
            PlaySound(sfx.at(SFX_CONFIRM_01));
        }
        frame_dirty = true;
    }

    inline entityid create_player_at_with(vec3 loc, string n, with_fun playerInitFunction) {
        minfo2("create player with: loc=(%d, %d, %d), n=%s", loc.x, loc.y, loc.z, n.c_str());
        massert(n != "", "name is empty string");
        const race_t rt = chara_creation.race;
        const entityid id = create_npc_at_with(rt, loc, [](CT& ct, const entityid id) {});
        massert(id != ENTITYID_INVALID, "id is invalid");
        constexpr int hp_ = 10, maxhp_ = 10;
        constexpr int vis_dist = 5, light_rad = 5, hear_dist = 3;
        constexpr entitytype_t type = ENTITY_PLAYER;
        set_hero_id(id);
        ct.set<entitytype>(id, type);
        ct.set<txalpha>(id, 0);
        ct.set<hp>(id, hp_);
        ct.set<maxhp>(id, maxhp_);
        ct.set<vision_distance>(id, vis_dist);
        ct.set<light_radius>(id, light_rad);
        ct.set<hearing_distance>(id, hear_dist);
        ct.set<name>(id, n);
        ct.set<dead>(id, false); // Hero should never start dead
        playerInitFunction(ct, id);
        return id;
    }

    inline entityid create_box_with() {
        const entityid id = add_entity();
        ct.set<entitytype>(id, ENTITY_BOX);
        ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
        ct.set<update>(id, true);
        ct.set<pushable>(id, true);
        ct.set<pullable>(id, true);
        //boxInitFunction(g, id);
        return id;
    }

    inline entityid create_box_at_with(vec3 loc) {
        shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
        shared_ptr<tile_t> tile = df->tile_at(loc);
        if (!tile_is_walkable(tile->get_type())) {
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
        const entityid id = create_box_with();
        if (df->df_add_at(id, loc) == ENTITYID_INVALID) {
            merror("failed df_add_at: %d, %d, %d", id, loc.x, loc.y);
            return ENTITYID_INVALID;
        }
        ct.set<location>(id, loc);
        return id;
    }

    inline entityid create_spell_with() {
        const entityid id = add_entity();
        ct.set<entitytype>(id, ENTITY_SPELL);
        ct.set<spelltype>(id, SPELLTYPE_FIRE);
        ct.set<spellstate>(id, SPELLSTATE_NONE);
        return id;
    }

    inline entityid create_spell_at_with(vec3 loc) {
        auto df = d.get_floor(loc.z);
        auto tile = df->tile_at(loc);
        if (!tile_is_walkable(tile->get_type())) {
            return ENTITYID_INVALID;
        }
        const entityid id = create_spell_with();
        if (df->df_add_at(id, loc) == ENTITYID_INVALID) {
            return ENTITYID_INVALID;
        }
        ct.set<location>(id, loc);
        ct.set<update>(id, true);
        return id;
    }

    inline void make_all_npcs_target_player() {
        massert(hero_id != ENTITYID_INVALID, "hero_id is invalid");
        for (entityid id = 0; id < next_entityid; id++) {
            const entitytype_t t = ct.get<entitytype>(id).value_or(ENTITY_NONE);
            if (t != ENTITY_NPC) {
                continue;
            }
            ct.set<target_id>(id, hero_id);
        }
    }

    inline void handle_input_character_creation_scene(const inputstate& is) {
        if (inputstate_is_pressed(is, KEY_ESCAPE)) {
            do_quit = true;
            return;
        }
        if (inputstate_is_pressed(is, KEY_ENTER)) {
            PlaySound(sfx.at(SFX_CONFIRM_01));
            // we need to copy the character creation stats to the hero entity
            // hero has already been created, so its id is available
            const int myhd = chara_creation.hitdie;
            int maxhp_roll = -1;
            while (maxhp_roll < 1) {
                maxhp_roll = do_roll_best_of_3(vec3{1, myhd, 0}) + get_stat_bonus(chara_creation.constitution);
            }
            shared_ptr<dungeon_floor> df = d.floors[0];
            const vec3 start_loc = df->get_random_loc();
            massert(!vec3_invalid(start_loc), "start_loc is (-1,-1,-1)");
            entity_turn = create_player_at_with(start_loc, "darkmage", [this, maxhp_roll](CT& ct, const entityid id) {
                // set stats from char_creation
                ct.set<strength>(id, chara_creation.strength);
                ct.set<dexterity>(id, chara_creation.dexterity);
                ct.set<constitution>(id, chara_creation.constitution);
                ct.set<intelligence>(id, chara_creation.intelligence);
                ct.set<wisdom>(id, chara_creation.wisdom);
                ct.set<charisma>(id, chara_creation.charisma);
                ct.set<hd>(id, (vec3){1, chara_creation.hitdie, 0});
                ct.set<hp>(hero_id, maxhp_roll);
                ct.set<maxhp>(hero_id, maxhp_roll);
            });
            massert(hero_id != ENTITYID_INVALID, "heroid is invalid");
            // temporary wedge-in code
            // set all the NPCs to target the hero
            make_all_npcs_target_player();
            current_scene = SCENE_GAMEPLAY;
        }
        else if (inputstate_is_pressed(is, KEY_SPACE)) {
            // re-roll character creation stats
            PlaySound(sfx.at(SFX_CONFIRM_01));
            chara_creation.strength = do_roll_best_of_3((vec3){3, 6, 0});
            chara_creation.dexterity = do_roll_best_of_3((vec3){3, 6, 0});
            chara_creation.intelligence = do_roll_best_of_3((vec3){3, 6, 0});
            chara_creation.wisdom = do_roll_best_of_3((vec3){3, 6, 0});
            chara_creation.constitution = do_roll_best_of_3((vec3){3, 6, 0});
            chara_creation.charisma = do_roll_best_of_3((vec3){3, 6, 0});
        }
        else if (inputstate_is_pressed(is, KEY_LEFT)) {
            PlaySound(sfx.at(SFX_CONFIRM_01));
            int race = chara_creation.race;
            if (chara_creation.race > 1) {
                race--;
            }
            else {
                race = RACE_COUNT - 1;
            }
            chara_creation.race = (race_t)race;
            chara_creation.hitdie = get_racial_hd(chara_creation.race);
        }
        else if (inputstate_is_pressed(is, KEY_RIGHT)) {
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

    inline bool remove_from_inventory(entityid actor_id, entityid item_id) {
        auto maybe_inventory = ct.get<inventory>(actor_id);
        if (!maybe_inventory.has_value()) {
            merror("maybe_inventory has no value for actor id %d", actor_id);
            return false;
        }
        auto my_items = maybe_inventory.value();
        bool success = false;
        for (auto it = my_items->begin(); it != my_items->end(); it++) {
            if (*it == item_id) {
                my_items->erase(it);
                success = true;
                break;
            }
        }
        if (!success)
            merror("Failed to find item id %d", item_id);
        else
            msuccess("Successfully removed item id %d", item_id);
        return success;
    }

    inline bool drop_from_inventory(entityid actor_id, entityid item_id) {
        if (remove_from_inventory(actor_id, item_id)) {
            auto maybe_loc = ct.get<location>(actor_id);
            if (!maybe_loc.has_value()) {
                merror("actor id %d has no location -- cannot drop item", actor_id);
                return false;
            }
            vec3 loc = maybe_loc.value();
            auto df = d.get_current_floor();
            const entityid retval = df->df_add_at(item_id, loc);
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

    inline bool drop_all_from_inventory(entityid actor_id) {
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

    inline void handle_hero_inventory_equip_weapon(entityid item_id) {
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

    inline void handle_hero_inventory_equip_shield(entityid item_id) {
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

    inline void handle_hero_inventory_equip_item(entityid item_id) {
        const itemtype_t item_type = ct.get<itemtype>(item_id).value_or(ITEM_NONE);
        switch (item_type) {
        case ITEM_NONE: break;
        case ITEM_WEAPON: handle_hero_inventory_equip_weapon(item_id); break;
        case ITEM_SHIELD: handle_hero_inventory_equip_shield(item_id); break;
        default: break;
        }
    }

    inline void handle_hero_inventory_equip() {
        PlaySound(sfx.at(SFX_EQUIP_01));
        // equip item: get the item id of the current selection
        const size_t index = inventory_cursor.y * 7 + inventory_cursor.x;
        auto my_inventory = ct.get<inventory>(hero_id);
        if (!my_inventory)
            return;
        if (!my_inventory.has_value())
            return;
        auto inventory = my_inventory.value();
        if (index < 0 || index >= inventory->size())
            return;
        const entityid item_id = inventory->at(index);
        const entitytype_t type = ct.get<entitytype>(item_id).value_or(ENTITY_NONE);
        if (type == ENTITY_ITEM)
            handle_hero_inventory_equip_item(item_id);
    }

    inline bool drop_item_from_hero_inventory() {
        if (!ct.has<inventory>(hero_id))
            return false;
        const size_t index = inventory_cursor.y * 7 + inventory_cursor.x;
        auto maybe_inventory = ct.get<inventory>(hero_id);
        if (!maybe_inventory.has_value())
            return false;
        auto inventory = maybe_inventory.value();
        if (index < 0 || index >= inventory->size())
            return false;
        const entityid item_id = inventory->at(index);
        inventory->erase(inventory->begin() + index);
        if (item_id == ct.get<equipped_weapon>(hero_id).value_or(ENTITYID_INVALID))
            ct.set<equipped_weapon>(hero_id, ENTITYID_INVALID);
        // add it to the tile at the player's current location
        // get the player's location
        const vec3 loc = ct.get<location>(hero_id).value();
        // get the tile at the player's location
        auto df = d.get_current_floor();
        if (!df->df_add_at(item_id, loc)) {
            merror("Failed to add to %d, %d, %d", loc.x, loc.y, loc.z);
            return false;
        }
        // update the entity's location
        ct.set<location>(item_id, loc);
        // add to tile
        return true;
    }

    inline bool is_in_inventory(entityid actor_id, entityid item_id) {
        auto maybe_inventory = ct.get<inventory>(actor_id);
        if (!maybe_inventory.has_value()) {
            merror("maybe_inventory has no value for actor id %d", actor_id);
            return false;
        }
        auto my_items = maybe_inventory.value();
        for (auto it = my_items->begin(); it != my_items->end(); it++) {
            if (*it == item_id)
                return true;
        }
        return false;
    }

    inline bool use_potion(entityid actor_id, entityid item_id) {
        massert(actor_id != ENTITYID_INVALID, "actor_id is invalid");
        massert(item_id != ENTITYID_INVALID, "actor_id is invalid");
        const bool is_item = ct.get<entitytype>(item_id).value_or(ENTITY_NONE) == ENTITY_ITEM;
        const bool is_potion = ct.get<itemtype>(item_id).value_or(ITEM_NONE) == ITEM_POTION;
        const bool in_inventory = is_in_inventory(actor_id, item_id);
        if (is_item && is_potion && in_inventory) {
            // get the item's effects
            optional<vec3> maybe_heal = ct.get<healing>(item_id);
            if (maybe_heal && maybe_heal.has_value()) {
                const vec3 heal = maybe_heal.value();
                const int amount = do_roll(heal);
                const int myhp = ct.get<hp>(actor_id).value_or(-1);
                const int mymaxhp = ct.get<maxhp>(actor_id).value_or(-1);
                ct.set<hp>(actor_id, mymaxhp ? mymaxhp : myhp + amount);
                if (actor_id == hero_id) {
                    const string n = ct.get<name>(actor_id).value_or("no-name");
                    add_message_history("%s used a healing potion", n.c_str());
                    add_message_history("%s restored %d hp", n.c_str(), amount);
                }
            }
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

    inline void logic_close() {
        d.floors.clear();
    }

    inline void handle_camera_move(inputstate& is) {
        if (inputstate_is_held(is, KEY_RIGHT)) {
            cam2d.offset.x += cam2d.zoom;
            frame_dirty = true;
        }
        else if (inputstate_is_held(is, KEY_LEFT)) {
            cam2d.offset.x -= cam2d.zoom;
            frame_dirty = true;
        }
        else if (inputstate_is_held(is, KEY_UP)) {
            cam2d.offset.y -= cam2d.zoom;
            frame_dirty = true;
        }
        else if (inputstate_is_held(is, KEY_DOWN)) {
            cam2d.offset.y += cam2d.zoom;
            frame_dirty = true;
        }
    }

    inline void handle_hero_potion_use(entityid id) {
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
        optional<shared_ptr<vector<entityid>>> maybe_inventory = ct.get<inventory>(hero_id);
        if (!maybe_inventory || !maybe_inventory.has_value())
            return;
        shared_ptr<vector<entityid>> inventory = maybe_inventory.value();
        if (index >= inventory->size())
            return;
        const entityid item_id = inventory->at(index);
        const entitytype_t type = ct.get<entitytype>(item_id).value_or(ENTITY_NONE);
        if (type != ENTITY_ITEM)
            return;
        const itemtype_t i_type = ct.get<itemtype>(item_id).value_or(ITEM_NONE);
        if (i_type == ITEM_NONE)
            return;
        if (i_type == ITEM_POTION)
            handle_hero_potion_use(item_id);
    }

    inline void handle_input_inventory(const inputstate& is) {
        if (controlmode != CONTROLMODE_INVENTORY || !display_inventory_menu) {
            return;
        }

        //if (inputstate_is_pressed(is, KEY_ESCAPE)) {
        //    do_quit = true;
        //    return;
        //}

        if (inputstate_is_pressed(is, KEY_I) || inputstate_is_pressed(is, KEY_ESCAPE)) {
            controlmode = CONTROLMODE_PLAYER;
            display_inventory_menu = false;
            PlaySound(sfx[SFX_BAG_CLOSE]);
            return;
        }

        if (inputstate_is_pressed(is, KEY_LEFT)) {
            PlaySound(sfx[SFX_CONFIRM_01]);
            if (inventory_cursor.x > 0) {
                inventory_cursor.x--;
            }
        }
        else if (inputstate_is_pressed(is, KEY_RIGHT)) {
            PlaySound(sfx[SFX_CONFIRM_01]);
            inventory_cursor.x++;
        }
        else if (inputstate_is_pressed(is, KEY_UP)) {
            PlaySound(sfx[SFX_CONFIRM_01]);
            if (inventory_cursor.y > 0) {
                inventory_cursor.y--;
            }
        }
        else if (inputstate_is_pressed(is, KEY_DOWN)) {
            PlaySound(sfx[SFX_CONFIRM_01]);
            inventory_cursor.y++;
        }
        else if (inputstate_is_pressed(is, KEY_E)) {
            handle_hero_inventory_equip();
        }
        else if (inputstate_is_pressed(is, KEY_Q)) {
            // drop item
            PlaySound(sfx[SFX_DISCARD_ITEM]);
            drop_item_from_hero_inventory();
        }
        else if (inputstate_is_pressed(is, KEY_ENTER)) {
            handle_hero_item_use();
            PlaySound(sfx[SFX_CONFIRM_01]);
        }
    }

    //inlinline_count_0 + 2bool handle_quit_pressed(const inputstate& is) {
    //    if (inputstate_is_pressed(is, KEY_ESCAPE)) {
    //        do_quit = true;
    //        return true;
    //    }
    //    return false;
    //}

    inline void cycle_messages() {
        if (msg_system.size() > 0) {
            const string msg = msg_system.front();
            const unsigned int len = msg.length();
            // measure the length of the message as calculated by MeasureText
            if (len > msg_history_max_len_msg) {
                msg_history_max_len_msg = len;
                constexpr int font_size = 10;
                const int measure = MeasureText(msg.c_str(), font_size);
                msg_history_max_len_msg_measure = measure;
            }
            msg_history.push_back(msg_system.front());
            msg_system.erase(msg_system.begin());
        }
        if (msg_system.size() == 0)
            msg_system_is_active = false;
    }

    inline bool handle_cycle_messages(const inputstate& is) {
        if (msg_system_is_active && inputstate_is_pressed(is, KEY_ENTER)) {
            PlaySound(sfx[SFX_CONFIRM_01]);
            cycle_messages();
            return true;
        }
        return false;
    }

    inline bool handle_cycle_messages_test() {
        if (!msg_system_is_active)
            return false;
        PlaySound(sfx[SFX_CONFIRM_01]);
        cycle_messages();
        return true;
    }

    inline void handle_camera_zoom(inputstate& is) {
        if (inputstate_is_pressed(is, KEY_LEFT_BRACKET)) {
            minfo("camera zoom in");
            cam2d.zoom += DEFAULT_ZOOM_INCR;
            frame_dirty = true;
        }
        else if (inputstate_is_pressed(is, KEY_RIGHT_BRACKET)) {
            minfo("camera zoom out");
            cam2d.zoom -= (cam2d.zoom > 1.0) * DEFAULT_ZOOM_INCR;
            frame_dirty = true;
        }
    }

    inline void change_player_dir(direction_t dir) {
        if (ct.get<dead>(hero_id).value_or(true))
            return;
        ct.set<direction>(hero_id, dir);
        ct.set<update>(hero_id, true);
        player_changing_dir = false;
        frame_dirty = true;
    }

    inline bool handle_change_dir(inputstate& is) {
        if (!player_changing_dir) {
            return false;
        }
        optional<bool> maybe_player_is_dead = ct.get<dead>(hero_id);
        if (!maybe_player_is_dead.has_value()) {
            return true;
        }
        const bool is_dead = maybe_player_is_dead.value();
        // double 's' is wait one turn
        if (inputstate_is_pressed(is, KEY_S) || inputstate_is_pressed(is, KEY_KP_5)) {
            player_changing_dir = false;
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
        }
        else if (inputstate_is_pressed(is, KEY_UP) || inputstate_is_pressed(is, KEY_W) || inputstate_is_pressed(is, KEY_KP_8)) {
            change_player_dir(DIR_UP);
        }
        else if (inputstate_is_pressed(is, KEY_DOWN) || inputstate_is_pressed(is, KEY_X) || inputstate_is_pressed(is, KEY_KP_2)) {
            change_player_dir(DIR_DOWN);
        }
        else if (inputstate_is_pressed(is, KEY_LEFT) || inputstate_is_pressed(is, KEY_A) || inputstate_is_pressed(is, KEY_KP_4)) {
            change_player_dir(DIR_LEFT);
        }
        else if (inputstate_is_pressed(is, KEY_RIGHT) || inputstate_is_pressed(is, KEY_D) || inputstate_is_pressed(is, KEY_KP_6)) {
            change_player_dir(DIR_RIGHT);
        }
        else if (inputstate_is_pressed(is, KEY_Q) || inputstate_is_pressed(is, KEY_KP_7)) {
            change_player_dir(DIR_UP_LEFT);
        }
        else if (inputstate_is_pressed(is, KEY_E) || inputstate_is_pressed(is, KEY_KP_9)) {
            change_player_dir(DIR_UP_RIGHT);
        }
        else if (inputstate_is_pressed(is, KEY_Z) || inputstate_is_pressed(is, KEY_KP_1)) {
            change_player_dir(DIR_DOWN_LEFT);
        }
        else if (inputstate_is_pressed(is, KEY_C) || inputstate_is_pressed(is, KEY_KP_3)) {
            change_player_dir(DIR_DOWN_RIGHT);
        }
        else if (inputstate_is_pressed(is, KEY_APOSTROPHE)) {
            // this is incorrect
            // this should be a regular attack
            if (is_dead) {
                return add_message("You cannot attack while dead");
            }
            ct.set<attacking>(hero_id, true);
            ct.set<update>(hero_id, true);
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            player_changing_dir = false;
        }
        return true;
    }

    inline bool handle_change_dir_intent(const inputstate& is) {
        //if (inputstate_is_pressed(is, KEY_S) || inputstate_is_pressed(is, KEY_KP_5)) {
        if (inputstate_is_pressed(is, KEY_KP_5)) {
            player_changing_dir = true;
            return true;
        }
        return false;
    }

    inline bool handle_display_inventory(const inputstate& is) {
        if (inputstate_is_pressed(is, KEY_I)) {
            display_inventory_menu = true;
            controlmode = CONTROLMODE_INVENTORY;
            frame_dirty = true;
            PlaySound(sfx[SFX_BAG_OPEN]);
            return true;
        }
        return false;
    }

    inline bool tile_has_solid(int x, int y, int z) {
        massert(z >= 0, "floor is out of bounds");
        massert((size_t)z < d.floors.size(), "floor is out of bounds");
        shared_ptr<dungeon_floor> df = d.get_floor(z);
        shared_ptr<tile_t> t = df->tile_at(vec3{x, y, z});
        for (int i = 0; (size_t)i < t->get_entity_count(); i++) {
            const entityid id = t->tile_get_entity(i);
            const bool is_solid = ct.get<solid>(id).value_or(false);
            if (id != ENTITYID_INVALID && is_solid) {
                return true;
            }
        }
        return false;
    }

    inline bool handle_box_push(entityid id, vec3 v) {
        const bool can_push = ct.get<pushable>(id).value_or(false);
        if (!can_push) {
            return false;
        }
        return try_entity_move(id, v);
    }

    inline entityid tile_has_pushable(int x, int y, int z) {
        massert(z >= 0, "floor is out of bounds");
        massert((size_t)z < d.floors.size(), "floor is out of bounds");
        shared_ptr<dungeon_floor> df = d.get_floor(z);
        shared_ptr<tile_t> t = df->tile_at(vec3{x, y, z});
        for (int i = 0; (size_t)i < t->get_entity_count(); i++) {
            const entityid id = t->tile_get_entity(i);
            const bool is_pushable = ct.get<pushable>(id).value_or(false);
            if (id != ENTITYID_INVALID && is_pushable) {
                return id;
            }
        }
        return ENTITYID_INVALID;
    }

    inline entityid tile_has_door(vec3 v) {
        shared_ptr<dungeon_floor> df = d.get_current_floor();
        shared_ptr<tile_t> t = df->tile_at(v);
        for (size_t i = 0; i < t->get_entity_count(); i++) {
            const entityid id = t->get_entity_at(i);
            const entitytype_t type = ct.get<entitytype>(id).value_or(ENTITY_NONE);
            if (type == ENTITY_DOOR) {
                return id;
            }
        }
        return ENTITYID_INVALID;
    }

    inline bool check_hearing(entityid id, vec3 loc) {
        if (id == ENTITYID_INVALID || vec3_invalid(loc)) {
            return false;
        }
        const vec3 hero_loc = ct.get<location>(hero_id).value_or(vec3{-1, -1, -1});
        // is the hero on the same floor as loc?
        if (vec3_invalid(hero_loc) || hero_loc.z != loc.z) {
            return false;
        }

        const float x0 = hero_loc.x;
        const float y0 = hero_loc.y;
        const float x1 = loc.x;
        const float y1 = loc.y;
        const Vector2 p0 = {x0, y0};
        const Vector2 p1 = {x1, y1};

        const float dist = Vector2Distance(p0, p1);

        const float hearing = ct.get<hearing_distance>(hero_id).value_or(3);

        return dist <= hearing;
    }

    inline bool try_entity_move(entityid id, vec3 v) {
        massert(id != ENTITYID_INVALID, "Entity ID is invalid!");
        minfo2("entity %d is trying to move: (%d,%d,%d)", id, v.x, v.y, v.z);
        ct.set<direction>(id, get_dir_from_xy(v.x, v.y));
        ct.set<update>(id, true);
        // entity location
        massert(ct.has<location>(id), "id %d has no location", id);
        const vec3 loc = ct.get<location>(id).value_or((vec3){-1, -1, -1});
        massert(!vec3_invalid(loc), "id %d location invalid", id);
        // entity's new location
        // we will have a special case for traversing floors so ignore v.z
        const vec3 aloc = {loc.x + v.x, loc.y + v.y, loc.z};
        minfo2("entity %d is trying to move to (%d,%d,%d)", id, aloc.x, aloc.y, aloc.z);
        shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
        if (aloc.x < 0 || aloc.x >= df->get_width() || aloc.y < 0 || aloc.y >= df->get_height()) {
            merror2("destination is invalid: (%d, %d, %d)", aloc.x, aloc.y, aloc.z);
            return false;
        }
        shared_ptr<tile_t> tile = df->tile_at(aloc);
        if (!tile_is_walkable(tile->get_type())) {
            if (!(god_mode && id == hero_id)) {
                merror2("tile is not walkable");
                return false;
            }
        }
        const entityid box_id = tile_has_box(aloc.x, aloc.y, aloc.z);
        if (box_id != ENTITYID_INVALID) {
            merror2("box present, trying to push");
            return handle_box_push(box_id, v);
        }
        const entityid pushable_id = tile_has_pushable(aloc.x, aloc.y, aloc.z);
        if (pushable_id != ENTITYID_INVALID) {
            merror2("pushable present, trying to push");
            return handle_box_push(pushable_id, v);
        }
        const bool has_solid = tile_has_solid(aloc.x, aloc.y, aloc.z);
        if (has_solid) {
            merror2("solid present, cannot move");
            return false;
        }
        else if (tile_has_live_npcs(tile_at_cur_floor(aloc))) {
            merror2("live npcs present, cannot move");
            return false;
        }
        else if (tile_has_player(tile_at_cur_floor(aloc))) {
            merror2("player present, cannot move");
            return false;
        }
        const entityid door_id = tile_has_door(aloc);
        if (door_id != ENTITYID_INVALID) {
            massert(ct.has<door_open>(door_id), "door_id %d doesnt have a door_open component", door_id);
            if (!ct.get<door_open>(door_id).value_or(false)) {
                merror2("door is closed");
                return false;
            }
        }
        // if door, door is open
        // remove the entity from the current tile
        if (!df->df_remove_at(id, loc)) {
            merror2("Failed to remove %d from (%d, %d)", id, loc.x, loc.y);
            return false;
        }
        // force cache update
        recompute_entity_cache_at(loc);
        // add the entity to the new tile
        if (df->df_add_at(id, aloc) == ENTITYID_INVALID) {
            merror2("Failed to add %d to (%d, %d)", id, aloc.x, aloc.y);
            return false;
        }
        // force cache update
        recompute_entity_cache_at(aloc);
        ct.set<location>(id, aloc);
        const float mx = v.x * DEFAULT_TILE_SIZE;
        const float my = v.y * DEFAULT_TILE_SIZE;
        ct.set<spritemove>(id, (Rectangle){mx, my, 0, 0});
        if (check_hearing(hero_id, aloc)) {
            // crashes in unittest if missing this check
            if (IsAudioDeviceReady())
                PlaySound(sfx[SFX_STEP_STONE_1]);
        }
        ct.set<steps_taken>(id, ct.get<steps_taken>(id).value_or(0) + 1);
        msuccess2("npc %d moved to (%d,%d,%d)", id, aloc.x, aloc.y, aloc.z);
        return true;
    }

    inline bool handle_move_up(const inputstate& is, bool is_dead) {
        //if (inputstate_is_pressed(is, KEY_UP) || inputstate_is_pressed(is, KEY_W) || inputstate_is_pressed(is, KEY_KP_8)) {
        if (inputstate_is_pressed(is, KEY_UP) || inputstate_is_pressed(is, KEY_KP_8)) {
            if (is_dead) {
                return add_message("You cannot move while dead");
            }
            try_entity_move(hero_id, (vec3){0, -1, 0});
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }

    inline bool handle_move_down(const inputstate& is, bool is_dead) {
        //if (inputstate_is_pressed(is, KEY_DOWN) || inputstate_is_pressed(is, KEY_X) || inputstate_is_pressed(is, KEY_KP_2)) {
        if (inputstate_is_pressed(is, KEY_DOWN) || inputstate_is_pressed(is, KEY_KP_2)) {
            if (is_dead)
                return add_message("You cannot move while dead");
            try_entity_move(hero_id, (vec3){0, 1, 0});
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }

    inline bool handle_move_left(const inputstate& is, bool is_dead) {
        //if (inputstate_is_pressed(is, KEY_LEFT) || inputstate_is_pressed(is, KEY_A) || inputstate_is_pressed(is, KEY_KP_4)) {
        if (inputstate_is_pressed(is, KEY_LEFT) || inputstate_is_pressed(is, KEY_KP_4)) {
            if (is_dead)
                return add_message("You cannot move while dead");
            try_entity_move(hero_id, (vec3){-1, 0, 0});
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }

    inline bool handle_move_right(const inputstate& is, bool is_dead) {
        //if (inputstate_is_pressed(is, KEY_RIGHT) || inputstate_is_pressed(is, KEY_D) || inputstate_is_pressed(is, KEY_KP_6)) {
        if (inputstate_is_pressed(is, KEY_RIGHT) || inputstate_is_pressed(is, KEY_KP_6)) {
            if (is_dead)
                return add_message("You cannot move while dead");
            try_entity_move(hero_id, (vec3){1, 0, 0});
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }

    inline bool handle_move_up_left(const inputstate& is, bool is_dead) {
        //if (inputstate_is_pressed(is, KEY_Q) || inputstate_is_pressed(is, KEY_KP_7)) {
        if (inputstate_is_pressed(is, KEY_KP_7)) {
            if (is_dead) {
                return add_message("You cannot move while dead");
            }
            try_entity_move(hero_id, (vec3){-1, -1, 0});
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }

    inline bool handle_move_up_right(const inputstate& is, bool is_dead) {
        //if (inputstate_is_pressed(is, KEY_E) || inputstate_is_pressed(is, KEY_KP_9)) {
        if (inputstate_is_pressed(is, KEY_KP_9)) {
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

    inline bool handle_move_down_left(const inputstate& is, bool is_dead) {
        //if (inputstate_is_pressed(is, KEY_Z) || inputstate_is_pressed(is, KEY_KP_1)) {
        if (inputstate_is_pressed(is, KEY_KP_1)) {
            if (is_dead)
                return add_message("You cannot move while dead");
            try_entity_move(hero_id, (vec3){-1, 1, 0});
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }

    inline bool handle_move_down_right(const inputstate& is, bool is_dead) {
        //if (inputstate_is_pressed(is, KEY_C) || inputstate_is_pressed(is, KEY_KP_3)) {
        if (inputstate_is_pressed(is, KEY_KP_3)) {
            if (is_dead)
                return add_message("You cannot move while dead");
            try_entity_move(hero_id, (vec3){1, 1, 0});
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }

    inline vec3 get_loc_facing_player() {
        optional<vec3> maybe_loc = ct.get<location>(hero_id);
        if (maybe_loc.has_value()) {
            vec3 loc = ct.get<location>(hero_id).value();
            direction_t dir = ct.get<direction>(hero_id).value();
            if (dir == DIR_UP)
                loc.y -= 1;
            else if (dir == DIR_DOWN)
                loc.y += 1;
            else if (dir == DIR_LEFT)
                loc.x -= 1;
            else if (dir == DIR_RIGHT)
                loc.x += 1;
            else if (dir == DIR_UP_LEFT) {
                loc.x -= 1;
                loc.y -= 1;
            }
            else if (dir == DIR_UP_RIGHT) {
                loc.x += 1;
                loc.y -= 1;
            }
            else if (dir == DIR_DOWN_LEFT) {
                loc.x -= 1;
                loc.y += 1;
            }
            else if (dir == DIR_DOWN_RIGHT) {
                loc.x += 1;
                loc.y += 1;
            }
            return loc;
        }
        return vec3{-1, -1, -1};
    }

    inline entityid get_cached_npc(shared_ptr<tile_t> t) {
        if (t == nullptr) {
            return INVALID;
        }
        recompute_entity_cache(t); // Force update
        return t->get_cached_npc();
    }

    inline int compute_armor_class(entityid id) {
        massert(ENTITYID_INVALID != id, "id is invalid");
        const int base_armor_class = ct.get<base_ac>(id).value_or(10);
        const int dex_bonus = get_stat_bonus(ct.get<dexterity>(id).value_or(10));
        // here, we would also grab any armor pieces that we are wearing
        const int total_ac = base_armor_class + dex_bonus;
        return total_ac;
    }

    inline bool compute_attack_roll(entityid attacker, entityid target) {
        const int str = ct.get<strength>(attacker).value_or(10);
        const int bonus = get_stat_bonus(str);
        uniform_int_distribution<int> gen(1, 20);
        const int roll = gen(mt) + bonus;
        const int ac = compute_armor_class(target);
        return roll >= ac;
    }

    inline int compute_attack_damage(entityid attacker, entityid target) {
        const int str = ct.get<strength>(attacker).value_or(10);
        const int bonus = std::max(0, get_stat_bonus(str));
        const entityid equipped_wpn = ct.get<equipped_weapon>(attacker).value_or(ENTITYID_INVALID);
        const vec3 dmg_range = ct.get<damage>(equipped_wpn).value_or(MINIMUM_DAMAGE);
        uniform_int_distribution<int> gen(dmg_range.x, dmg_range.y);
        const int dmg = std::max(1, gen(mt));
        return dmg + bonus;
    }

    inline void handle_weapon_durability_loss(entityid atk_id, entityid tgt_id) {
        const entityid equipped_wpn = ct.get<equipped_weapon>(atk_id).value_or(ENTITYID_INVALID);
        optional<int> maybe_dura = ct.get<durability>(equipped_wpn);
        if (!maybe_dura.has_value()) {
            return;
        }
        const int dura = maybe_dura.value();
        ct.set<durability>(equipped_wpn, dura - 1 < 0 ? 0 : dura - 1);
        if (dura > 0) {
            return;
        }
        // unequip item
        ct.set<equipped_weapon>(atk_id, ENTITYID_INVALID);
        // remove item from attacker's inventory
        remove_from_inventory(atk_id, equipped_wpn);
        // item destroyed
        ct.set<destroyed>(equipped_wpn, true);
        const bool event_heard = check_hearing(hero_id, ct.get<location>(tgt_id).value_or((vec3){-1, -1, -1}));
        if (event_heard) {
            PlaySound(sfx[SFX_05_ALCHEMY_GLASS_BREAK]);
        }
        add_message_history("%s broke!", ct.get<name>(equipped_wpn).value_or("no-name-weapon").c_str());
    }

    inline void handle_shield_durability_loss(entityid defender, entityid attacker) {
        const entityid shield = ct.get<equipped_shield>(defender).value_or(ENTITYID_INVALID);
        optional<int> maybe_dura = ct.get<durability>(shield);
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

    inline int get_npc_xp(entityid id) {
        return ct.get<xp>(id).value_or(0);
    }

    inline void update_npc_xp(entityid id, entityid target_id) {
        const int old_xp = get_npc_xp(id);
        const int reward_xp = 1;
        const int new_xp = old_xp + reward_xp;
        ct.set<xp>(id, new_xp);
    }




    inline void process_attack_results(entityid atk_id, entityid tgt_id, bool atk_successful) {
        massert(atk_id != ENTITYID_INVALID, "attacker entity id is invalid");
        massert(tgt_id != ENTITYID_INVALID, "target entity id is invalid");

        const string attacker_name = ct.get<name>(atk_id).value_or("no-name");
        const string target_name = ct.get<name>(tgt_id).value_or("no-name");
        const char* atk_name = attacker_name.c_str();
        const char* tgt_name = target_name.c_str();

        if (ct.get<dead>(tgt_id).value_or(false)) {
            add_message_history("%s swings at a dead target", atk_name);
            return;
        }

        if (!atk_successful) {
            add_message_history("%s swings at %s and misses!", atk_name, tgt_name);
            return;
        }

        const int dmg = compute_attack_damage(atk_id, tgt_id);

        ct.set<damaged>(tgt_id, true);
        ct.set<update>(tgt_id, true);

        optional<int> maybe_tgt_hp = ct.get<hp>(tgt_id);
        if (!maybe_tgt_hp.has_value()) {
            merror("target has no HP component");
            return;
        }

        const int tgt_hp = maybe_tgt_hp.value() - dmg;

        add_message_history("%s deals %d damage to %s", atk_name, dmg, tgt_name);

        ct.set<hp>(tgt_id, tgt_hp);

        // decrement weapon durability
        handle_weapon_durability_loss(atk_id, tgt_id);
        if (tgt_hp > 0) {
            return;
        }

        ct.set<dead>(tgt_id, true);

        // testing marking corpses as pullable objects
        ct.set<pullable>(tgt_id, true);

        // we need to remove tgt_id from the floor's living npcs and add it to dead npcs
        shared_ptr<dungeon_floor> df = d.get_current_floor();
        df->remove_living_npc(tgt_id);
        df->add_dead_npc(tgt_id);

        // when an npc target is killed, the attacker gains xp
        // when an npc target is killed, it drops its inventory
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




    inline void handle_shield_block_sfx(entityid target_id) {
        const bool event_heard = check_hearing(hero_id, ct.get<location>(target_id).value_or((vec3){-1, -1, -1}));
        if (event_heard) {
            PlaySound(sfx[SFX_HIT_METAL_ON_METAL]);
        }
    }




    inline attack_result_t process_attack_entity(shared_ptr<tile_t> tile, entityid attacker_id, entityid target_id) {
        massert(attacker_id != ENTITYID_INVALID, "attacker is NULL");

        if (tile == nullptr) {
            return ATTACK_RESULT_MISS;
        }

        if (target_id == INVALID) {
            return ATTACK_RESULT_MISS;
        }

        const entitytype_t type = ct.get<entitytype>(target_id).value_or(ENTITY_NONE);

        if (type != ENTITY_PLAYER && type != ENTITY_NPC) {
            return ATTACK_RESULT_MISS;
        }

        if (ct.get<dead>(target_id).value_or(true)) {
            return ATTACK_RESULT_MISS;
        }

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
        uniform_int_distribution<int> gen(1, MAX_BLOCK_CHANCE);
        const int roll = gen(mt);
        const int chance = ct.get<block_chance>(shield_id).value_or(MAX_BLOCK_CHANCE);
        const int low_roll = MAX_BLOCK_CHANCE - chance;
        if (roll <= low_roll) {
            // failed to block
            process_attack_results(attacker_id, target_id, true);
            return ATTACK_RESULT_HIT;
        }

        // decrement shield durability
        handle_shield_durability_loss(target_id, attacker_id);
        handle_shield_block_sfx(target_id);
        ct.set<block_success>(target_id, true);
        ct.set<update>(target_id, true);

        const string atk_name = ct.get<name>(attacker_id).value_or("no-name");
        const string tgt_name = ct.get<name>(target_id).value_or("no-name");
        add_message_history("%s blocked an attack from %s", tgt_name.c_str(), atk_name.c_str());

        return ATTACK_RESULT_BLOCK;
    }




    inline void handle_attack_sfx(entityid attacker, attack_result_t result) {
        if (test) {
            return;
        }

        vec3 loc = ct.get<location>(attacker).value_or(vec3{-1, -1, -1});
        massert(vec3_valid(loc), "loc is invalid");

        if (!check_hearing(hero_id, loc)) {
            return;
        }

        int index = SFX_SLASH_ATTACK_SWORD_1;
        if (result == ATTACK_RESULT_BLOCK) {
            index = SFX_HIT_METAL_ON_METAL;
        }
        else if (result == ATTACK_RESULT_HIT) {
            index = SFX_HIT_METAL_ON_FLESH;
        }
        else if (result == ATTACK_RESULT_MISS) {
            const entityid weapon_id = ct.get<equipped_weapon>(attacker).value_or(ENTITYID_INVALID);
            const weapontype_t wpn_type = ct.get<weapontype>(weapon_id).value_or(WEAPON_NONE);
            index = wpn_type == WEAPON_SHORT_SWORD ? SFX_SLASH_ATTACK_SWORD_1
                    : wpn_type == WEAPON_AXE       ? SFX_SLASH_ATTACK_HEAVY_1
                    : wpn_type == WEAPON_DAGGER    ? SFX_SLASH_ATTACK_LIGHT_1
                                                   : SFX_SLASH_ATTACK_SWORD_1;
        }
        PlaySound(sfx[index]);
        msuccess("attack sfx played");
    }

    inline void set_gamestate_flag_for_attack_animation(entitytype_t type) {
        massert(type == ENTITY_PLAYER || type == ENTITY_NPC, "type is not player or npc!");
        if (type == ENTITY_PLAYER) {
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
        }
        else if (type == ENTITY_NPC) {
            flag = GAMESTATE_FLAG_NPC_ANIM;
        }
    }




    inline attack_result_t try_entity_attack(entityid id, int x, int y) {
        minfo2("entity %d is attacking location %d, %d", id, x, y);
        massert(!ct.get<dead>(id).value_or(false), "attacker entity is dead");
        massert(ct.has<location>(id), "entity %d has no location", id);

        const vec3 loc = ct.get<location>(id).value();
        shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
        shared_ptr<tile_t> tile = df->tile_at(vec3{x, y, loc.z});

        // Calculate direction based on target position
        const int dx = x - loc.x;
        const int dy = y - loc.y;

        ct.set<direction>(id, get_dir_from_xy(dx, dy));
        ct.set<attacking>(id, true);
        ct.set<update>(id, true);

        const entityid npc_id = get_cached_npc(tile);
        const attack_result_t result = process_attack_entity(tile, id, npc_id);

        // did the hero hear this event?
        handle_attack_sfx(id, result);

        if (!test) {
            set_gamestate_flag_for_attack_animation(ct.get<entitytype>(id).value_or(ENTITY_NONE));
        }

        return result;
    }




    inline bool handle_attack(const inputstate& is, bool is_dead) {
        //if (inputstate_is_pressed(is, KEY_APOSTROPHE)) {
        if (inputstate_is_pressed(is, KEY_A)) {
            if (is_dead) {
                return add_message("You cannot attack while dead");
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

    inline entityid tile_get_item(shared_ptr<tile_t> t) {
        recompute_entity_cache(t);
        return t->get_cached_item();
    }




    inline bool try_entity_pull(entityid id) {
        minfo("try_entity_pull(%d)", id);

        massert(id != ENTITYID_INVALID, "Entity is NULL!");

        ct.set<update>(id, true);
        vec3 loc = ct.get<location>(id).value_or(vec3{-1, -1, -1});

        massert(!vec3_invalid(loc), "loc is invalid");

        auto df = d.get_floor(loc.z);
        auto tile = df->tile_at(loc);
        // get the id's direction
        direction_t facing_d = ct.get<direction>(id).value_or(DIR_NONE);

        massert(facing_d != DIR_NONE, "direction d is none");

        // opposite dir
        direction_t d = get_opposite_dir(facing_d);
        vec3 v = get_loc_from_dir(d);
        vec3 aloc = {loc.x + v.x, loc.y + v.y, loc.z};
        vec3 fv = get_loc_from_dir(facing_d);
        vec3 bloc = {loc.x + fv.x, loc.y + fv.y, loc.z};
        auto tile_dest = df->tile_at(aloc);
        if (aloc.x < 0 || aloc.x >= df->get_width() || aloc.y < 0 || aloc.y >= df->get_height()) {
            merror2("destination is invalid: (%d, %d, %d)", aloc.x, aloc.y, aloc.z);
            return false;
        }
        if (!tile_is_walkable(tile_dest->get_type())) {
            //if (!(god_mode && id == hero_id)) {
            merror2("tile is not walkable");
            return false;
            //}
        }
        const entityid box_id = tile_has_box(aloc.x, aloc.y, aloc.z);
        if (box_id != ENTITYID_INVALID) {
            merror2("box present, can't push and pull simultaneously");
            return false;
        }
        const bool has_solid = tile_has_solid(aloc.x, aloc.y, aloc.z);
        if (has_solid) {
            merror2("solid present, cannot move");
            return false;
        }
        else if (tile_has_live_npcs(tile_at_cur_floor(aloc))) {
            merror2("live npcs present, cannot move");
            return false;
        }
        else if (tile_has_player(tile_at_cur_floor(aloc))) {
            merror2("player present, cannot move");
            return false;
        }
        const entityid door_id = tile_has_door(aloc);
        if (door_id != ENTITYID_INVALID) {
            massert(ct.has<door_open>(door_id), "door_id %d doesnt have a door_open component", door_id);
            if (!ct.get<door_open>(door_id).value_or(false)) {
                merror2("door is closed");
                return false;
            }
        }




        //const entityid box_id2 = tile_has_box(bloc.x, bloc.y, bloc.z);
        const entityid box_id2 = tile_has_pullable(bloc.x, bloc.y, bloc.z);
        if (box_id2 == ENTITYID_INVALID) {
            return false;
        }




        // remove the entity from the current tile
        if (!df->df_remove_at(id, loc)) {
            merror2("Failed to remove %d from (%d, %d)", id, loc.x, loc.y);
            return false;
        }
        // force cache update
        recompute_entity_cache_at(loc);
        // add the entity to the new tile
        if (df->df_add_at(id, aloc) == ENTITYID_INVALID) {
            merror2("Failed to add %d to (%d, %d)", id, aloc.x, aloc.y);
            return false;
        }
        // force cache update
        recompute_entity_cache_at(aloc);
        ct.set<location>(id, aloc);
        const float mx = v.x * DEFAULT_TILE_SIZE;
        const float my = v.y * DEFAULT_TILE_SIZE;
        ct.set<spritemove>(id, (Rectangle){mx, my, 0, 0});
        if (check_hearing(hero_id, aloc)) {
            // crashes in unittest if missing this check
            if (IsAudioDeviceReady()) {
                PlaySound(sfx[SFX_STEP_STONE_1]);
            }
        }
        ct.set<steps_taken>(id, ct.get<steps_taken>(id).value_or(0) + 1);

        msuccess2("npc %d moved to (%d,%d,%d)", id, aloc.x, aloc.y, aloc.z);

        // check to see if pullable
        // for now i will be lazy
        // and skip this check
        try_entity_move(box_id2, v);
        msuccess("try_entity_pull(%d)", id);
        return true;
    }




    inline bool try_entity_pickup(entityid id) {
        massert(id != ENTITYID_INVALID, "Entity is NULL!");
        ct.set<update>(id, true);
        // check if the player is on a tile with an item
        optional<vec3> maybe_loc = ct.get<location>(id);
        if (!maybe_loc.has_value()) {
            merror("id %d has no location", id);
            return false;
        }
        const vec3 loc = maybe_loc.value();
        shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
        shared_ptr<tile_t> tile = df->tile_at(loc);
        bool item_picked_up = false;
        // lets try using our new cached_item via tile_get_item
        const entityid item_id = tile_get_item(tile);
        if (item_id != ENTITYID_INVALID && add_to_inventory(id, item_id)) {
            tile->tile_remove(item_id);
            PlaySound(sfx[SFX_CONFIRM_01]);
            item_picked_up = true;
            const string item_name = ct.get<name>(item_id).value_or("no-name-item");
            add_message_history("You picked up %s", item_name.c_str());
        }
        else if (item_id == ENTITYID_INVALID) {
            mwarning("No item cached");
        }
        const entitytype_t t = ct.get<entitytype>(id).value_or(ENTITY_NONE);
        if (t == ENTITY_PLAYER) {
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
        }
        return item_picked_up;
    }

    inline bool handle_pickup_item(const inputstate& is, bool is_dead) {
        //if (!inputstate_is_pressed(is, KEY_SLASH)) {
        if (!inputstate_is_pressed(is, KEY_S)) {
            return false;
        }
        if (is_dead) {
            return add_message("You cannot pick up items while dead");
        }
        try_entity_pickup(hero_id);
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
        return true;
    }

    inline bool try_entity_stairs(entityid id) {
        massert(id != ENTITYID_INVALID, "Entity ID is invalid!");
        ct.set<update>(id, true);
        const vec3 loc = ct.get<location>(id).value();
        // first, we prob want to get the tile at this location
        const int current_floor = d.current_floor;
        shared_ptr<dungeon_floor> df = d.floors[current_floor];
        shared_ptr<tile_t> t = df->tile_at(loc);
        // check the tile type
        if (t->get_type() == TILE_UPSTAIRS) {
            // can't go up on the top floor
            // otherwise...
            if (current_floor == 0) {
                add_message("You are already on the top floor!");
            }
            else {
                // go upstairs
                // we have to remove the player from the old tile
                df->df_remove_at(hero_id, loc);
                d.current_floor--;
                const int new_floor = d.current_floor;
                shared_ptr<dungeon_floor> df2 = d.floors[new_floor];
                const vec3 uloc = df2->get_downstairs_loc();
                df2->df_add_at(hero_id, uloc);
                ct.set<location>(hero_id, uloc);
                flag = GAMESTATE_FLAG_PLAYER_ANIM;
                PlaySound(sfx.at(SFX_STEP_STONE_1));
                return true;
            }
        }
        else if (t->get_type() == TILE_DOWNSTAIRS) {
            // can't go down on the bottom floor
            // otherwise...
            if ((size_t)current_floor < d.floors.size() - 1) {
                // go downstairs
                // we have to remove the player from the old tile
                df->df_remove_at(hero_id, loc);
                d.current_floor++;
                const int new_floor = d.current_floor;
                shared_ptr<dungeon_floor> df2 = d.floors[new_floor];
                const vec3 uloc = df2->get_upstairs_loc();
                df2->df_add_at(hero_id, uloc);
                ct.set<location>(hero_id, uloc);
                flag = GAMESTATE_FLAG_PLAYER_ANIM;
                PlaySound(sfx.at(SFX_STEP_STONE_1));
                return true;
            }
            // bottom floor
            else {
                add_message("You can't go downstairs anymore!");
            }
        }
        return false;
    }

    inline bool handle_traverse_stairs(const inputstate& is, bool is_dead) {
        //if (inputstate_is_pressed(is, KEY_PERIOD)) {
        if (inputstate_is_pressed(is, KEY_Z)) {
            if (is_dead) {
                return add_message("You cannot traverse stairs while dead");
            }
            try_entity_stairs(hero_id);
            return true;
        }
        return false;
    }

    inline bool try_entity_open_door(const entityid id, const vec3 loc) {
        massert(id != ENTITYID_INVALID, "id is invalid");
        if (!tile_has_door(loc)) {
            return false;
        }
        shared_ptr<dungeon_floor> df = d.get_current_floor();
        shared_ptr<tile_t> t = df->tile_at(loc);
        for (size_t i = 0; i < t->get_entity_count(); i++) {
            const entityid myid = t->get_entity_at(i);
            const entitytype_t type = ct.get<entitytype>(myid).value_or(ENTITY_NONE);
            if (type != ENTITY_DOOR) {
                continue;
            }
            optional<bool> maybe_is_open = ct.get<door_open>(myid);
            massert(maybe_is_open.has_value(), "door %d has no `is_open` component", myid);
            ct.set<door_open>(myid, !maybe_is_open.value());
            PlaySound(sfx.at(SFX_CHEST_OPEN));
            return true;
        }
        return false;
    }

    inline bool handle_open_door(const inputstate& is, bool is_dead) {
        if (inputstate_is_pressed(is, KEY_O)) {
            if (is_dead) {
                return add_message("You cannot open doors while dead");
            }
            const vec3 loc = get_loc_facing_player();
            try_entity_open_door(hero_id, loc);
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }

    inline void try_entity_cast_spell(entityid id, int tgt_x, int tgt_y) {
        optional<vec3> maybe_loc = ct.get<location>(id);
        if (!maybe_loc.has_value()) {
            merror("no location for entity id %d", id);
            return;
        }
        const vec3 loc = maybe_loc.value();
        shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
        const vec3 spell_loc = {tgt_x, tgt_y, loc.z};
        shared_ptr<tile_t> tile = df->tile_at(spell_loc);
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
                for (const entityid id : *tile->get_entities()) {
                    if (ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_NPC) {
                        npcid = id;
                        break;
                    }
                }

                uniform_int_distribution<int> gen(1, 6);
                const int dmg = gen(mt);
                ct.set<damaged>(npcid, true);
                ct.set<update>(npcid, true);

                optional<int> maybe_tgt_hp = ct.get<hp>(npcid);
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

                const entitytype_t tgttype = ct.get<entitytype>(npcid).value_or(ENTITY_NONE);
                ct.set<dead>(npcid, true);
                shared_ptr<dungeon_floor> df = d.get_current_floor();
                shared_ptr<tile_t> target_tile = df->tile_at(spell_loc);
                target_tile->set_dirty_entities(true);
                if (tgttype == ENTITY_NPC) {
                    // increment attacker's xp
                    const int old_xp = ct.get<xp>(id).value_or(0);
                    const int reward_xp = 1;
                    const int new_xp = old_xp + reward_xp;
                    ct.set<xp>(id, new_xp);
                    // handle item drops
                    drop_all_from_inventory(npcid);
                }
                else if (tgttype == ENTITY_PLAYER) {
                    add_message("You died");
                }
            }
            if (tile_has_door(spell_loc)) {
                // find the door id
                entityid doorid = ENTITYID_INVALID;
                for (auto id : *tile->get_entities()) {
                    if (ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_DOOR) {
                        doorid = id;
                        break;
                    }
                }
                // mark it 'destroyed'
                // remove it from the tile
                if (doorid != ENTITYID_INVALID) {
                    ct.set<destroyed>(doorid, true);
                    df->df_remove_at(doorid, spell_loc);
                }
            }
            ct.set<destroyed>(spell_id, true);
        }
        // did the hero hear this event?
        const bool event_heard = check_hearing(hero_id, (vec3){tgt_x, tgt_y, loc.z});
        if (ok && event_heard) {
            PlaySound(sfx[SFX_ITEM_FUSION]);
        }
    }

    inline bool handle_test_cast_spell(const inputstate& is, bool is_dead) {
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

    inline bool handle_restart(const inputstate& is) {
        if (inputstate_is_pressed(is, KEY_R)) {
            do_restart = true;
            return true;
        }
        return false;
    }

    inline void handle_input_gameplay_controlmode_player(inputstate& is) {
        if (flag != GAMESTATE_FLAG_PLAYER_INPUT) {
            return;
        }

        if (inputstate_is_pressed(is, KEY_ESCAPE)) {
            do_quit = true;
            return;
        }

        handle_camera_zoom(is);
        // make sure player isnt dead

        if (handle_cycle_messages(is)) {
            return;
        }

        if (inputstate_is_pressed(is, KEY_SLASH)) {
            display_help_menu = true;
            controlmode = CONTROLMODE_HELP_MENU;
        }

        const bool is_dead = ct.get<dead>(hero_id).value_or(true);
        if (is_dead) {
            handle_restart(is);
            //static bool showed_msg = false;
            //if (!showed_msg) {
            //    minfo("Player is dead");
            //    showed_msg = true;
            //}
            return;
        }

        if (test && framecount % 20 == 0) {
            if (handle_cycle_messages_test()) {
                return;
            }
            // special handler
            // move randomly for now
            const bool is_dead = ct.get<dead>(hero_id).value_or(true);
            if (is_dead)
                return;
            //minfo2("hero random move");
            uniform_int_distribution<int> dist(-1, 1);
            // during normal testing, move randomly
            int vx = dist(mt);
            int vy = dist(mt);
            while (vx == 0 && vy == 0) {
                vx = dist(mt);
                vy = dist(mt);
            }
            if (try_entity_move(hero_id, vec3{vx, vy, 0})) {
                msuccess2("hero moved randomly successfully");
            }
            else {
                minfo2("hero failed to move randomly");
            }
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return;
        }
        else if (test) {
            return;
        }
        else if (inputstate_is_pressed(is, KEY_GRAVE)) {
            display_option_menu = true;
            controlmode = CONTROLMODE_OPTION_MENU;
            return;
        }
        else if (inputstate_is_pressed(is, KEY_SPACE)) {
            try_entity_pull(hero_id);
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return;
        }
        else if (inputstate_is_pressed(is, KEY_ESCAPE)) {
            do_quit = true;
            return;
        }
        //else if (handle_cycle_messages(is)) {
        //    return;
        //}
        if (handle_change_dir(is) || handle_change_dir_intent(is) || handle_display_inventory(is)) {
            return;
        }
        else if (handle_move_up(is, is_dead) || handle_move_down(is, is_dead) || handle_move_left(is, is_dead) || handle_move_right(is, is_dead)) {
            return;
        }
        else if (
            handle_move_up_left(is, is_dead) || handle_move_up_right(is, is_dead) || handle_move_down_left(is, is_dead) ||
            handle_move_down_right(is, is_dead)) {
            return;
        }
        else if (handle_attack(is, is_dead) || handle_pickup_item(is, is_dead) || handle_traverse_stairs(is, is_dead) || handle_open_door(is, is_dead)) {
            return;
        }
        //else if (handle_test_cast_spell(is, is_dead)) {
        //    return;
        //}
        //else if (handle_restart(is, is_dead)) {
        //    return;
        //}
    }

    inline void handle_input_action_menu(const inputstate& is) {
        massert(controlmode == CONTROLMODE_ACTION_MENU, "controlmode isnt in action menu: %d", controlmode);
        if (inputstate_is_pressed(is, KEY_SPACE)) {
            display_action_menu = false;
            controlmode = CONTROLMODE_PLAYER;
            return;
        }
        else if (inputstate_is_pressed(is, KEY_DOWN))
            action_selection++;
        else if (inputstate_is_pressed(is, KEY_UP))
            action_selection--;
    }




    inline void handle_input_option_menu(inputstate& is) {
        massert(controlmode == CONTROLMODE_OPTION_MENU, "controlmode isnt in option menu: %d", controlmode);
        if (inputstate_is_pressed(is, KEY_GRAVE)) {
            display_option_menu = false;
            controlmode = CONTROLMODE_PLAYER;
        }
        else if (inputstate_is_pressed(is, KEY_UP))
            options_menu.decr_selection();
        else if (inputstate_is_pressed(is, KEY_DOWN))
            options_menu.incr_selection();
        else if (inputstate_is_pressed(is, KEY_ENTER))
            minfo("Enter pressed");
    }




    inline void handle_input_help_menu(inputstate& is) {
        massert(controlmode == CONTROLMODE_HELP_MENU, "controlmode isnt in help menu: %d", controlmode);

        //if (inputstate_is_pressed(is, KEY_GRAVE)) {
        if (inputstate_any_pressed(is)) {
            display_help_menu = false;
            controlmode = CONTROLMODE_PLAYER;
        }

        //if (inputstate_is_pressed(is, KEY_GRAVE)) {
        //    display_option_menu = false;
        //    controlmode = CONTROLMODE_PLAYER;
        //}

        //else if (inputstate_is_pressed(is, KEY_UP))
        //    options_menu.decr_selection();
        //else if (inputstate_is_pressed(is, KEY_DOWN))
        //    options_menu.incr_selection();
        //else if (inputstate_is_pressed(is, KEY_ENTER))
        //    minfo("Enter pressed");
    }




    inline void handle_input_gameplay_scene(inputstate& is) {
        minfo2("handle input gameplay scene");
        if (inputstate_is_pressed(is, KEY_B)) {
            if (controlmode == CONTROLMODE_PLAYER)
                controlmode = CONTROLMODE_CAMERA;
            else if (controlmode == CONTROLMODE_CAMERA)
                controlmode = CONTROLMODE_PLAYER;
            frame_dirty = true;
        }

        if (controlmode == CONTROLMODE_CAMERA) {
            handle_camera_move(is);
        }
        else if (controlmode == CONTROLMODE_PLAYER) {
            handle_input_gameplay_controlmode_player(is);
        }
        else if (controlmode == CONTROLMODE_INVENTORY) {
            handle_input_inventory(is);
        }
        else if (controlmode == CONTROLMODE_ACTION_MENU) {
            handle_input_action_menu(is);
        }
        else if (controlmode == CONTROLMODE_OPTION_MENU) {
            handle_input_option_menu(is);
        }
        else if (controlmode == CONTROLMODE_HELP_MENU) {
            handle_input_help_menu(is);
        }
    }

    inline void handle_input(inputstate& is) {
        // no matter which mode we are in, we can toggle the debug panel
        minfo2("handle input: current_scene: %d", current_scene);
        if (inputstate_is_pressed(is, KEY_P)) {
            debugpanelon = !debugpanelon;
            minfo2("Toggling debug panel: %s", debugpanelon ? "ON" : "OFF");
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
        minfo2("update debug panel buffer");
        // Static buffers to avoid reallocating every frame
        const int message_count = msg_history.size();
        int inventory_count;
        vec3 loc = {0, 0, 0};
        inventory_count = -1;
        if (hero_id != ENTITYID_INVALID) {
            loc = ct.get<location>(hero_id).value_or((vec3){-1, -1, -1});
        }
        // current df
        shared_ptr<dungeon_floor> df = d.get_current_floor();
        const int df_w = df->get_width();
        const int df_h = df->get_height();
        // Determine control mode and flag strings
        const char* control_mode = controlmode == CONTROLMODE_CAMERA ? "Camera" : controlmode == CONTROLMODE_PLAYER ? "Player" : "Unknown";
        // zero out the buffer
        //memset(debugpanel.buffer, 0, sizeof(debugpanel.buffer));

        direction_t player_dir = ct.get<direction>(hero_id).value_or(DIR_NONE);

        bzero(debugpanel.buffer, sizeof(debugpanel.buffer));
        // Format the string in one pass
        snprintf(
            debugpanel.buffer,
            sizeof(debugpanel.buffer),
            "@evildojo666\n"
            "project.rpg\n"
            //"%s\n" // timebeganbuf
            //"%s\n" // currenttimebuf
            "frame : %d\n"
            "update: %d\n"
            "frame dirty: %d\n"
            "draw time: %.1fms\n"
            "avg last %d draw time: %.1fms\n"
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
            "message count: %d\n"
            "df.width x height: %dx%d\n"
            "living npcs on floor: %lu\n"
            "dead npcs on floor: %lu\n"
            "god_mode: %d\n"
            "player direction: %d\n"
            "\n",
            framecount,
            frame_updates,
            frame_dirty,
            last_frame_time * 1000,
            LAST_FRAME_TIMES_MAX,
            (get_avg_last_frame_time() * 1000),
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
            message_count,
            df_w,
            df_h,
            d.get_current_floor()->get_living_npcs()->size(),
            d.get_current_floor()->get_dead_npcs()->size(),
            god_mode,
            player_dir);

        msuccess2("successfully updated debug panel buffer");
    }

    inline bool is_entity_adjacent(entityid id0, entityid id1) {
        massert(id0 != ENTITYID_INVALID, "id0 is invalid");
        massert(id1 != ENTITYID_INVALID, "id1 is invalid");
        massert(id0 != id1, "id0 and id1 are the same");
        // check if on same floor
        const vec3 a = ct.get<location>(id0).value_or((vec3){-1, -1, -1});
        const vec3 b = ct.get<location>(id1).value_or((vec3){-1, -1, -1});
        if (a.z == -1 || b.z == -1 || a.z != b.z)
            return false;
        // compute all as one function, no loops
        return (a.x - 1 == b.x && a.y - 1 == b.y) || (a.x == b.x && a.y - 1 == b.y) || (a.x + 1 == b.x && a.y - 1 == b.y) || (a.x - 1 == b.x && a.y == b.y) ||
               (a.x + 1 == b.x && a.y == b.y) || (a.x - 1 == b.x && a.y + 1 == b.y) || (a.x == b.x && a.y + 1 == b.y) || (a.x + 1 == b.x && a.y + 1 == b.y);
    }

    inline bool handle_npc(entityid id) {
        minfo2("handle npc %d", id);
        massert(id != ENTITYID_INVALID, "Entity is NULL!");

        auto id_name = ct.get<name>(id).value_or("no-name");

        auto maybe_dead = ct.get<dead>(id);
        massert(maybe_dead.has_value(), "npc id %d name %s has no dead component", id, id_name.c_str());

        const bool is_dead = maybe_dead.value();
        if (is_dead) {
            return false;
        }

        // this is a heuristic for handling entity actions
        // originally, we were just moving randomly
        // this example shows how, if the player is not adjacent to an NPC,
        // they will just move randomly. otherwise, they attack the player
        const entityid tgt_id = ct.get<target_id>(id).value_or(hero_id);
        vector<tactic> default_tactics = {{tactic_target::nil, tactic_condition::any, tactic_action::move}};
        const vector<tactic> npc_tactics = ct.get<tactics>(id).value_or(default_tactics);
        uniform_int_distribution<int> dist(-1, 1);
        for (tactic t : npc_tactics) {
            if (t.target == tactic_target::nil && t.condition == tactic_condition::any && t.action == tactic_action::move) {
                // handle random move
                if (try_entity_move(id, vec3{dist(mt), dist(mt), 0})) {
                    msuccess2("try entity move succeeded");
                }
                else {
                    merror2("try entity move FAILED");
                }
                return true;
            }
            else if (t.target == tactic_target::enemy && t.condition == tactic_condition::adjacent && t.action == tactic_action::attack) {
                // handle attack adjacent enemy
                if (is_entity_adjacent(id, tgt_id)) {
                    const vec3 loc = ct.get<location>(tgt_id).value();
                    try_entity_attack(id, loc.x, loc.y);
                    //const attack_result_t result = try_entity_attack(id, loc.x, loc.y);
                    //if (result == ATTACK_RESULT_BLOCK) {
                    //    minfo2("attack result: blocked");
                    //} else if (result == ATTACK_RESULT_HIT) {
                    //    minfo2("attack result: hit");
                    //} else if (result == ATTACK_RESULT_MISS) {
                    //    minfo2("attack result: miss");
                    //} else if (result == ATTACK_RESULT_NONE) {
                    //    minfo2("attack result: none");
                    //} else if (result == ATTACK_RESULT_COUNT) {
                    //    minfo2("attack result: count");
                    //} else {
                    //    minfo2("attack result: unknown");
                    //}
                    return true;
                }
            }
            else if (t.target == tactic_target::nil && t.condition == tactic_condition::nil && t.action == tactic_action::attack) {
                // just attack directly in front of you
                massert(ct.has<direction>(id), "id %d has no direction", id);
                direction_t dir = ct.get<direction>(id).value_or(DIR_NONE);
                massert(dir != DIR_NONE, "dir cannot be none");

                vec3 loc = ct.get<location>(id).value_or(vec3{-1, -1, -1});
                massert(vec3_valid(loc), "id %d location is invalid", id);

                vec3 dir_vec = get_loc_from_dir(dir);
                vec3 new_loc = vec3_add(loc, dir_vec);

                try_entity_attack(id, new_loc.x, new_loc.y);
            }
        }
        merror2("failed to handle npc %d", id);
        return false;
    }

    inline void handle_npcs() {
        minfo2("handle npcs");
        if (flag == GAMESTATE_FLAG_NPC_TURN) {
#ifndef NPCS_ALL_AT_ONCE
            //if (entity_turn >= 0 && entity_turn < next_entityid) {
            if (entity_turn >= 1 && entity_turn < next_entityid) {
                //const bool result = handle_npc(entity_turn);
                handle_npc(entity_turn);
                //if (result) {
                //    msuccess("npc %d handled successfully", entity_turn);
                //} else {
                //    merror("npc %d handle failed", entity_turn);
                //}
                flag = GAMESTATE_FLAG_NPC_ANIM;
            }
#else
            auto df_npcs = d.get_current_floor()->get_living_npcs();
            for (entityid id : *df_npcs) {
                const bool result = handle_npc(id);
                if (result) {
                    msuccess2("npc %d handled successfully", entity_turn);
                }
                else {
                    merror2("npc %d handle failed", entity_turn);
                }
            }
            flag = GAMESTATE_FLAG_NPC_ANIM;
#endif
        }
    }

    inline void handle_test_flag() {
        minfo2(
            "handle test flag: %s",
            flag == GAMESTATE_FLAG_PLAYER_ANIM    ? "player anim"
            : flag == GAMESTATE_FLAG_PLAYER_INPUT ? "player input"
            : flag == GAMESTATE_FLAG_NPC_TURN     ? "npc turn"
            : flag == GAMESTATE_FLAG_NPC_ANIM     ? "npc anim"
                                                  : "Unknown");
        if (flag == GAMESTATE_FLAG_PLAYER_ANIM) {
            //minfo("player anim");
#ifndef NPCS_ALL_AT_ONCE
            entity_turn++;
            if (entity_turn >= next_entityid) {
                entity_turn = 1;
            }
#endif
            //minfo("handle test flag: %d", turn_count);
            flag = GAMESTATE_FLAG_NPC_TURN;
        }
        else if (flag == GAMESTATE_FLAG_NPC_ANIM) {
            //minfo("npc anim");
#ifndef NPCS_ALL_AT_ONCE
            entity_turn++;

            if (entity_turn >= next_entityid) {
                entity_turn = 1;
            }

            if (entity_turn == hero_id) {
                flag = GAMESTATE_FLAG_PLAYER_INPUT;
                turn_count++;
            }
            else {
                flag = GAMESTATE_FLAG_NPC_TURN;
            }
#else
            flag = GAMESTATE_FLAG_PLAYER_INPUT;
            //minfo("handle test flag: %d", turn_count);
            turn_count++;
#endif
        }
    }

    inline void tick(inputstate& is) {
        minfo3("tick");
        // Spawn NPCs periodically
        //try_spawn_npc(g);
        //massert(r0, "update player tiles explored failed");
        if (!update_player_tiles_explored()) {
            merror3("update player tiles explored failed");
        }
        if (!update_player_state()) {
            merror3("update player state failed");
        }
        update_npcs_state();
        update_spells_state();
        handle_input(is);
        handle_npcs();
#ifdef DEBUG
        update_debug_panel_buffer(is);
#endif
        currenttime = time(NULL);
        currenttimetm = localtime(&currenttime);
        strftime(currenttimebuf, GAMESTATE_SIZEOFTIMEBUF, "Current Time: %Y-%m-%d %H:%M:%S", currenttimetm);
        if (test) {
            handle_test_flag();
        }
        ticks++;
        minfo2("end tick");
    }
};
