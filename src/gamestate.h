#pragma once

#include "ComponentTable.h"
#include "ComponentTraits.h"
#include "attack_result.h"
#include "biome.h"
#include "calculate_linear_path.h"
#include "character_creation.h"
#include "controlmode.h"
#include "debugpanel.h"
#include "dungeon.h"
#include "gamestate_flag.h"
#include "get_racial_hd.h"
#include "get_racial_modifiers.h"
#include "inputstate.h"
#include "libgame_defines.h"
#include "magic_values.h"
#include "option_menu.h"
#include "orc_names.h"
#include "roll.h"
#include "scene.h"
#include "sfx.h"
#include "stat_bonus.h"
#include <chrono>
#include <map>
#include <queue>
#include <random>
#include <raylib.h>
#include <raymath.h>

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
using std::map;
using std::mt19937;
using std::pair;
using std::priority_queue;
using std::round;
using std::seed_seq;
using std::uniform_int_distribution;
using std::uniform_real_distribution;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::system_clock;
using std::chrono::time_point;

extern float music_volume;
extern float master_volume;

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
    int lock;
    int pad;
    unsigned int framecount;
    unsigned int fadealpha;
    unsigned int targetwidth;
    unsigned int targetheight;
    unsigned int windowwidth;
    unsigned int windowheight;
    unsigned int frame_updates;
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
    double max_frame_time;
    size_t last_frame_times_current;
    vector<double> last_frame_times;
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

    void init_music_paths() {
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
            string fullpath = "audio/music/" + string(buffer);
            music_file_paths.push_back(fullpath);
        }
        fclose(file);
    }

    double get_avg_last_frame_time() {
        double sum = 0;
        for (int i = 0; i < LAST_FRAME_TIMES_MAX; i++) {
            sum += last_frame_times[i];
        }
        return sum / LAST_FRAME_TIMES_MAX;
    }

    void reset() {
        version = GAME_VERSION;
        debugpanel.x = GAMESTATE_DEBUGPANEL_DEFAULT_X;
        debugpanel.y = GAMESTATE_DEBUGPANEL_DEFAULT_Y;
        debugpanel.w = GAMESTATE_DEBUGPANEL_DEFAULT_WIDTH;
        debugpanel.h = GAMESTATE_DEBUGPANEL_DEFAULT_HEIGHT;
        debugpanel.fg_color = RAYWHITE;
        debugpanel.bg_color = RED;
        debugpanel.font_size = GAMESTATE_DEBUGPANEL_DEFAULT_FONT_SIZE;
        targetwidth = -1;
        targetheight = -1;
        windowwidth = -1;
        windowheight = -1;
        hero_id = INVALID;
        entity_turn = 1;
        new_entityid_begin = ENTITYID_INVALID;
        new_entityid_end = ENTITYID_INVALID;
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
        frame_updates = 0;
        framecount = 0;
        restart_count = 0;
        last_frame_time = 0;
        max_frame_time = 0;
        last_frame_times_current = 0;
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

    bool set_hero_id(entityid id) {
        massert(id != ENTITYID_INVALID, "id is invalid");
        hero_id = id;
        return true;
    }

    entityid add_entity() {
        entityid id = next_entityid;
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

    tile_t& tile_at_cur_floor(vec3 loc) {
        return d.get_current_floor()->tile_at(loc);
    }

    void create_and_add_df_0(biome_t type, int w, int h, int df_count, float parts);

    void create_and_add_df_1(biome_t type, int w, int h, int df_count, float parts);

    void init_dungeon(biome_t type, int df_count, float parts, int width = DEFAULT_DUNGEON_FLOOR_WIDTH, int height = DEFAULT_DUNGEON_FLOOR_HEIGHT);

    entityid create_door_with(with_fun doorInitFunction);

    bool tile_has_player(shared_ptr<tile_t> t) {
        return t->get_cached_player_present();
    }

    entityid create_door_at_with(vec3 loc, with_fun doorInitFunction);

    size_t place_doors();

    entityid create_prop_with(proptype_t type, with_fun initFun);

    entityid create_prop_at_with(proptype_t type, vec3 loc, with_fun initFun);

    int place_props();

    entityid create_weapon_with(with_fun weaponInitFunction);

    with_fun dagger_init();

    with_fun axe_init();

    with_fun sword_init();

    with_fun shield_init();

    with_fun potion_init(potiontype_t pt);

    with_fun player_init(int maxhp_roll);

    entityid create_weapon_at_with(ComponentTable& ct, vec3 loc, with_fun weaponInitFunction);

    entityid create_weapon_at_random_loc_with(CT& ct, with_fun weaponInitFunction);

    entityid create_shield_with(ComponentTable& ct, with_fun shieldInitFunction);

    entityid create_shield_at_with(ComponentTable& ct, vec3 loc, with_fun shieldInitFunction);

    entityid create_potion_with(with_fun potionInitFunction);

    entityid create_potion_at_with(vec3 loc, with_fun potionInitFunction);

    race_t random_monster_type();

    void set_npc_starting_stats(entityid id);

    void set_npc_defaults(entityid id);

    entityid create_npc_with(race_t rt, with_fun npcInitFunction);

    entityid tile_has_box(int x, int y, int z) {
        massert(z >= 0, "floor is out of bounds");
        massert((size_t)z < d.floors.size(), "floor is out of bounds");
        auto df = d.get_floor(z);
        tile_t& t = df->tile_at((vec3){x, y, z});
        return t.get_cached_box();
    }

    entityid tile_has_pullable(int x, int y, int z) {
        minfo("tile_has_pullable(%d, %d, %d)", x, y, z);
        massert(z >= 0, "floor is out of bounds");
        massert((size_t)z < d.floors.size(), "floor is out of bounds");
        shared_ptr<dungeon_floor> df = d.get_floor(z);
        tile_t& t = df->tile_at(vec3{x, y, z});
        entityid box_id = t.get_cached_box();
        if (box_id != INVALID) {
            bool is_pullable = ct.get<pullable>(box_id).value_or(false);
            if (is_pullable) {
                return box_id;
            }
        }
        return ENTITYID_INVALID;
    }

    entityid create_npc_at_with(race_t rt, vec3 loc, with_fun npcInitFunction);

    bool add_to_inventory(entityid actor_id, entityid item_id);

    entityid create_orc_with(with_fun monsterInitFunction);

    entityid create_orc_at_with(vec3 loc, with_fun monsterInitFunction);

    bool add_message(const char* fmt, ...) {
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

    void add_message_history(const char* fmt, ...) {
        massert(fmt, "format string is NULL");
        char buffer[MAX_MSG_LENGTH];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, MAX_MSG_LENGTH - 1, fmt, args);
        va_end(args);
        string s(buffer);
        msg_history.push_back(s);
    }

    void update_tile(tile_t& tile);

    bool path_blocked(vec3 a, vec3 b);

    bool update_player_tiles_explored();

    bool update_player_state();

    void update_spells_state();

    void update_npcs_state();

    void logic_init();

    void handle_input_title_scene(inputstate& is);

    void handle_input_main_menu_scene(inputstate& is);

    entityid create_player_at_with(vec3 loc, string n, with_fun playerInitFunction);

    entityid create_box_with();

    entityid create_box_at_with(vec3 loc);

    entityid create_spell_with();

    entityid create_spell_at_with(vec3 loc);

    void make_all_npcs_target_player();

    void handle_input_character_creation_scene(inputstate& is);

    void handle_test_flag();

    void tick(inputstate& is);

    bool remove_from_inventory(entityid actor_id, entityid item_id) {
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

        if (!success) {
            merror("Failed to find item id %d", item_id);
        }
        else {
            msuccess("Successfully removed item id %d", item_id);
        }
        return success;
    }

    bool drop_from_inventory(entityid actor_id, entityid item_id) {
        if (remove_from_inventory(actor_id, item_id)) {
            auto maybe_loc = ct.get<location>(actor_id);
            if (!maybe_loc.has_value()) {
                merror("actor id %d has no location -- cannot drop item", actor_id);
                return false;
            }
            vec3 loc = maybe_loc.value();
            auto df = d.get_current_floor();
            entityid retval = df->df_add_at(item_id, ENTITY_ITEM, loc);
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
        entityid current_weapon = ct.get<equipped_weapon>(hero_id).value_or(ENTITYID_INVALID);
        // Unequip if it's already equipped
        if (current_weapon == item_id) {
            ct.set<equipped_weapon>(hero_id, ENTITYID_INVALID);
        }
        // Equip the new weapon
        else {
            ct.set<equipped_weapon>(hero_id, item_id);
        }
        flag = GAMESTATE_FLAG_PLAYER_ANIM;
        controlmode = CONTROLMODE_PLAYER;
        display_inventory_menu = false;
    }

    void handle_hero_inventory_equip_shield(entityid item_id) {
        // Check if this is the currently equipped weapon
        entityid current_shield = ct.get<equipped_shield>(hero_id).value_or(ENTITYID_INVALID);
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
        // equip item: get the item id of the current selection
        size_t index = inventory_cursor.y * 7 + inventory_cursor.x;
        auto my_inventory = ct.get<inventory>(hero_id);
        if (!my_inventory) {
            return;
        }
        if (!my_inventory.has_value()) {
            return;
        }
        auto inventory = my_inventory.value();
        if (index < 0 || index >= inventory->size()) {
            return;
        }
        entityid item_id = inventory->at(index);
        entitytype_t type = ct.get<entitytype>(item_id).value_or(ENTITY_NONE);
        if (type == ENTITY_ITEM) {
            handle_hero_inventory_equip_item(item_id);
        }
    }

    bool drop_item_from_hero_inventory() {
        if (!ct.has<inventory>(hero_id)) {
            return false;
        }
        size_t index = inventory_cursor.y * 7 + inventory_cursor.x;
        auto maybe_inventory = ct.get<inventory>(hero_id);
        if (!maybe_inventory.has_value()) {
            return false;
        }
        auto inventory = maybe_inventory.value();
        if (index < 0 || index >= inventory->size()) {
            return false;
        }
        entityid item_id = inventory->at(index);
        inventory->erase(inventory->begin() + index);
        if (item_id == ct.get<equipped_weapon>(hero_id).value_or(ENTITYID_INVALID)) {
            ct.set<equipped_weapon>(hero_id, ENTITYID_INVALID);
        }
        // add it to the tile at the player's current location
        // get the player's location
        vec3 loc = ct.get<location>(hero_id).value();
        // get the tile at the player's location
        auto df = d.get_current_floor();
        if (!df->df_add_at(item_id, ENTITY_ITEM, loc)) {
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
            if (*it == item_id) {
                return true;
            }
        }
        return false;
    }

    bool use_potion(entityid actor_id, entityid item_id) {
        massert(actor_id != ENTITYID_INVALID, "actor_id is invalid");
        massert(item_id != ENTITYID_INVALID, "actor_id is invalid");
        bool is_item = ct.get<entitytype>(item_id).value_or(ENTITY_NONE) == ENTITY_ITEM;
        bool is_potion = ct.get<itemtype>(item_id).value_or(ITEM_NONE) == ITEM_POTION;
        bool in_inventory = is_in_inventory(actor_id, item_id);
        if (is_item && is_potion && in_inventory) {
            // get the item's effects
            optional<vec3> maybe_heal = ct.get<healing>(item_id);
            if (maybe_heal && maybe_heal.has_value()) {
                vec3 heal = maybe_heal.value();
                int amount = do_roll(heal);
                int myhp = ct.get<hp>(actor_id).value_or(-1);
                int mymaxhp = ct.get<maxhp>(actor_id).value_or(-1);
                ct.set<hp>(actor_id, mymaxhp ? mymaxhp : myhp + amount);
                if (actor_id == hero_id) {
                    string n = ct.get<name>(actor_id).value_or("no-name");
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

    void logic_close() {
        d.floors.clear();
    }

    void handle_camera_move(inputstate& is);

    void handle_hero_potion_use(entityid id);

    void handle_hero_item_use();

    void handle_input_inventory(inputstate& is);

    //inl_count_0 + 2bool handle_quit_pressed(const inputstate& is) {
    //    if (inputstate_is_pressed(is, KEY_ESCAPE)) {
    //        do_quit = true;
    //        return true;
    //    }
    //    return false;
    //}

    void cycle_messages();

    bool handle_cycle_messages(inputstate& is);

    bool handle_cycle_messages_test();

    void handle_camera_zoom(inputstate& is);

    void change_player_dir(direction_t dir);

    bool handle_change_dir(inputstate& is);

    bool handle_change_dir_intent(inputstate& is);

    bool handle_display_inventory(inputstate& is);

    bool tile_has_solid(int x, int y, int z) {
        massert(z >= 0, "floor is out of bounds");
        massert((size_t)z < d.floors.size(), "floor is out of bounds");
        shared_ptr<dungeon_floor> df = d.get_floor(z);
        //shared_ptr<tile_t> t = df->tile_at(vec3{x, y, z});
        tile_t& t = df->tile_at(vec3{x, y, z});

        //for (int i = 0; (size_t)i < t->get_entity_count(); i++) {
        //entityid id = t->get_cached_live_npc();
        entityid id = t.get_cached_live_npc();
        bool is_solid = ct.get<solid>(id).value_or(false);
        if (id != ENTITYID_INVALID && is_solid) {
            return true;
        }

        id = t.get_cached_box();
        is_solid = ct.get<solid>(id).value_or(false);
        if (id != ENTITYID_INVALID && is_solid) {
            return true;
        }

        id = t.get_cached_door();
        is_solid = ct.get<solid>(id).value_or(false);
        if (id != ENTITYID_INVALID && is_solid) {
            return true;
        }

        id = t.get_cached_item();
        is_solid = ct.get<solid>(id).value_or(false);
        if (id != ENTITYID_INVALID && is_solid) {
            return true;
        }

        return false;
    }

    bool handle_box_push(entityid id, vec3 v) {
        bool can_push = ct.get<pushable>(id).value_or(false);
        if (!can_push) {
            return false;
        }
        return try_entity_move(id, v);
    }

    entityid tile_has_pushable(int x, int y, int z) {
        massert(z >= 0, "floor is out of bounds");
        massert((size_t)z < d.floors.size(), "floor is out of bounds");
        shared_ptr<dungeon_floor> df = d.get_floor(z);
        //shared_ptr<tile_t> t = df->tile_at(vec3{x, y, z});
        tile_t& t = df->tile_at(vec3{x, y, z});

        //for (int i = 0; (size_t)i < t->get_entity_count(); i++) {
        //const entityid id = t->get_cached_box();
        entityid id = t.get_cached_box();
        bool is_pushable = ct.get<pushable>(id).value_or(false);
        if (id != ENTITYID_INVALID && is_pushable) {
            return id;
        }
        //}

        return ENTITYID_INVALID;
    }

    entityid tile_has_door(vec3 v) {
        shared_ptr<dungeon_floor> df = d.get_current_floor();
        //shared_ptr<tile_t> t = df->tile_at(v);
        tile_t& t = df->tile_at(v);
        //for (size_t i = 0; i < t->get_entity_count(); i++) {
        //const entityid id = t->get_cached_door();
        entityid id = t.get_cached_door();
        //const entitytype_t type = ct.get<entitytype>(id).value_or(ENTITY_NONE);
        //if (type == ENTITY_DOOR) {
        return id;
        //}
        //}
        //return ENTITYID_INVALID;
    }

    bool check_hearing(entityid id, vec3 loc) {
        if (id == ENTITYID_INVALID || vec3_invalid(loc)) {
            return false;
        }
        vec3 hero_loc = ct.get<location>(hero_id).value_or(vec3{-1, -1, -1});
        // is the hero on the same floor as loc?
        if (vec3_invalid(hero_loc) || hero_loc.z != loc.z) {
            return false;
        }

        float x0 = hero_loc.x;
        float y0 = hero_loc.y;
        float x1 = loc.x;
        float y1 = loc.y;
        Vector2 p0 = {x0, y0};
        Vector2 p1 = {x1, y1};

        float dist = Vector2Distance(p0, p1);

        float hearing = ct.get<hearing_distance>(hero_id).value_or(3);

        return dist <= hearing;
    }

    bool try_entity_move(entityid id, vec3 v) {
        massert(id != ENTITYID_INVALID, "Entity ID is invalid!");
        minfo2("entity %d is trying to move: (%d,%d,%d)", id, v.x, v.y, v.z);
        ct.set<direction>(id, get_dir_from_xy(v.x, v.y));
        ct.set<update>(id, true);
        // entity location
        massert(ct.has<location>(id), "id %d has no location", id);

        vec3 loc = ct.get<location>(id).value_or((vec3){-1, -1, -1});

        massert(!vec3_invalid(loc), "id %d location invalid", id);
        // entity's new location
        // we will have a special case for traversing floors so ignore v.z
        vec3 aloc = {loc.x + v.x, loc.y + v.y, loc.z};

        minfo2("entity %d is trying to move to (%d,%d,%d)", id, aloc.x, aloc.y, aloc.z);

        shared_ptr<dungeon_floor> df = d.get_floor(loc.z);

        if (aloc.x < 0 || aloc.x >= df->get_width() || aloc.y < 0 || aloc.y >= df->get_height()) {
            merror2("destination is invalid: (%d, %d, %d)", aloc.x, aloc.y, aloc.z);
            return false;
        }

        //shared_ptr<tile_t> tile = df->tile_at(aloc);
        tile_t& tile = df->tile_at(aloc);
        //if (!tile_is_walkable(tile->get_type())) {
        if (!tile_is_walkable(tile.get_type())) {
            if (!(god_mode && id == hero_id)) {
                merror2("tile is not walkable");
                return false;
            }
        }
        entityid box_id = tile_has_box(aloc.x, aloc.y, aloc.z);
        if (box_id != ENTITYID_INVALID) {
            merror2("box present, trying to push");
            return handle_box_push(box_id, v);
        }
        entityid pushable_id = tile_has_pushable(aloc.x, aloc.y, aloc.z);
        if (pushable_id != ENTITYID_INVALID) {
            merror2("pushable present, trying to push");
            return handle_box_push(pushable_id, v);
        }
        bool has_solid = tile_has_solid(aloc.x, aloc.y, aloc.z);
        if (has_solid) {
            merror2("solid present, cannot move");
            return false;
        }
        //else if (get_cached_live_npc(tile) != INVALID) {
        else if (tile.get_cached_live_npc() != INVALID) {
            merror2("live npcs present, cannot move");
            return false;
        }

        //else if (tile_has_live_npcs(tile_at_cur_floor(aloc))) {
        //    merror2("live npcs present, cannot move");
        //    return false;
        //}
        //else if (tile_has_player(tile_at_cur_floor(aloc))) {
        //    merror2("player present, cannot move");
        //    return false;
        //}
        entityid door_id = tile_has_door(aloc);
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
        //recompute_entity_cache_at(loc);
        // add the entity to the new tile

        auto type = ct.get<entitytype>(id).value_or(ENTITY_NONE);

        if (df->df_add_at(id, type, aloc) == ENTITYID_INVALID) {
            merror2("Failed to add %d to (%d, %d)", id, aloc.x, aloc.y);
            return false;
        }

        // force cache update
        //recompute_entity_cache_at(aloc);
        ct.set<location>(id, aloc);
        float mx = v.x * DEFAULT_TILE_SIZE;
        float my = v.y * DEFAULT_TILE_SIZE;
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

    bool handle_move_up(inputstate& is, bool is_dead) {
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

    bool handle_move_down(inputstate& is, bool is_dead) {
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

    bool handle_move_left(inputstate& is, bool is_dead) {
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

    bool handle_move_right(inputstate& is, bool is_dead) {
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

    bool handle_move_up_left(inputstate& is, bool is_dead) {
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

    bool handle_move_up_right(inputstate& is, bool is_dead) {
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

    bool handle_move_down_left(inputstate& is, bool is_dead) {
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

    bool handle_move_down_right(inputstate& is, bool is_dead) {
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

    vec3 get_loc_facing_player() {
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

    entityid get_cached_live_npc(shared_ptr<tile_t> t) {
        if (t == nullptr) {
            return INVALID;
        }
        //recompute_entity_cache(t); // Force update
        return t->get_cached_live_npc();
    }

    int compute_armor_class(entityid id);

    bool compute_attack_roll(entityid attacker, entityid target);

    int compute_attack_damage(entityid attacker, entityid target);

    void handle_weapon_durability_loss(entityid atk_id, entityid tgt_id);

    void handle_shield_durability_loss(entityid defender, entityid attacker);

    int get_npc_xp(entityid id);

    void update_npc_xp(entityid id, entityid target_id);

    void process_attack_results(tile_t& tile, entityid atk_id, entityid tgt_id, bool atk_successful);

    void handle_shield_block_sfx(entityid target_id);

    attack_result_t process_attack_entity(tile_t& tile, entityid attacker_id, entityid target_id);

    void handle_attack_sfx(entityid attacker, attack_result_t result);

    void set_gamestate_flag_for_attack_animation(entitytype_t type);

    attack_result_t try_entity_attack(entityid id, int x, int y);

    bool handle_attack(inputstate& is, bool is_dead) {
        //if (inputstate_is_pressed(is, KEY_APOSTROPHE)) {
        if (inputstate_is_pressed(is, KEY_A)) {
            if (is_dead) {
                return add_message("You cannot attack while dead");
            }
            if (ct.has<location>(hero_id) && ct.has<direction>(hero_id)) {
                vec3 loc = get_loc_facing_player();
                try_entity_attack(hero_id, loc.x, loc.y);
                flag = GAMESTATE_FLAG_PLAYER_ANIM;
                return true;
            }
        }
        return false;
    }

    entityid tile_get_item(shared_ptr<tile_t> t) {
        //recompute_entity_cache(t);
        return t->get_cached_item();
    }

    bool try_entity_pull(entityid id) {
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
        tile_t& tile_dest = df->tile_at(aloc);
        if (aloc.x < 0 || aloc.x >= df->get_width() || aloc.y < 0 || aloc.y >= df->get_height()) {
            merror("destination is invalid: (%d, %d, %d)", aloc.x, aloc.y, aloc.z);
            return false;
        }
        if (!tile_is_walkable(tile_dest.get_type())) {
            //if (!(god_mode && id == hero_id)) {
            merror("tile is not walkable");
            return false;
        }
        entityid box_id = tile_has_box(aloc.x, aloc.y, aloc.z);
        if (box_id != ENTITYID_INVALID) {
            merror("box present, can't push and pull simultaneously");
            return false;
        }
        bool has_solid = tile_has_solid(aloc.x, aloc.y, aloc.z);
        if (has_solid) {
            merror("solid present, cannot move");
            return false;
        }
        //else if (get_cached_live_npc(tile_dest)) {
        else if (tile_dest.get_cached_live_npc()) {
            merror("live npcs present, cannot move");
            return false;
        }
        //else if (tile_has_live_npcs(tile_at_cur_floor(aloc))) {
        //    merror2("live npcs present, cannot move");
        //    return false;
        //}
        //else if (tile_has_player(tile_at_cur_floor(aloc))) {
        else if (tile_at_cur_floor(aloc).get_cached_player_present()) {
            merror("player present, cannot move");
            return false;
        }
        entityid door_id = tile_has_door(aloc);
        if (door_id != ENTITYID_INVALID) {
            massert(ct.has<door_open>(door_id), "door_id %d doesnt have a door_open component", door_id);
            if (!ct.get<door_open>(door_id).value_or(false)) {
                merror("door is closed");
                return false;
            }
        }

        entityid box_id2 = tile_has_pullable(bloc.x, bloc.y, bloc.z);
        if (box_id2 == ENTITYID_INVALID) {
            return false;
        }

        // remove the entity from the current tile
        if (!df->df_remove_at(id, loc)) {
            merror("Failed to remove %d from (%d, %d)", id, loc.x, loc.y);
            return false;
        }
        // force cache update
        //recompute_entity_cache_at(loc);
        // add the entity to the new tile
        auto type = ct.get<entitytype>(id).value_or(ENTITY_NONE);
        if (df->df_add_at(id, type, aloc) == ENTITYID_INVALID) {
            merror("Failed to add %d to (%d, %d)", id, aloc.x, aloc.y);
            return false;
        }
        // force cache update
        //recompute_entity_cache_at(aloc);
        ct.set<location>(id, aloc);
        float mx = v.x * DEFAULT_TILE_SIZE;
        float my = v.y * DEFAULT_TILE_SIZE;
        ct.set<spritemove>(id, (Rectangle){mx, my, 0, 0});
        if (check_hearing(hero_id, aloc)) {
            // crashes in unittest if missing this check
            if (IsAudioDeviceReady()) {
                PlaySound(sfx[SFX_STEP_STONE_1]);
            }
        }
        ct.set<steps_taken>(id, ct.get<steps_taken>(id).value_or(0) + 1);

        msuccess("npc %d moved to (%d,%d,%d)", id, aloc.x, aloc.y, aloc.z);

        // check to see if pullable
        // for now i will be lazy
        // and skip this check
        try_entity_move(box_id2, v);
        msuccess("try_entity_pull(%d)", id);
        return true;
    }

    bool try_entity_pickup(entityid id) {
        massert(id != ENTITYID_INVALID, "Entity is NULL!");
        ct.set<update>(id, true);
        // check if the player is on a tile with an item
        optional<vec3> maybe_loc = ct.get<location>(id);
        if (!maybe_loc.has_value()) {
            merror("id %d has no location", id);
            return false;
        }
        vec3 loc = maybe_loc.value();
        shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
        tile_t& tile = df->tile_at(loc);
        bool item_picked_up = false;
        // lets try using our new cached_item via tile_get_item
        entityid item_id = tile.get_cached_item();
        if (item_id != ENTITYID_INVALID && add_to_inventory(id, item_id)) {
            //tile->tile_remove(item_id);
            tile.tile_remove(item_id);
            PlaySound(sfx[SFX_CONFIRM_01]);
            item_picked_up = true;
            string item_name = ct.get<name>(item_id).value_or("no-name-item");
            add_message_history("You picked up %s", item_name.c_str());
        }
        else if (item_id == ENTITYID_INVALID) {
            mwarning("No item cached");
        }
        entitytype_t t = ct.get<entitytype>(id).value_or(ENTITY_NONE);
        if (t == ENTITY_PLAYER) {
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
        }
        return item_picked_up;
    }

    bool handle_pickup_item(inputstate& is, bool is_dead) {
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

    bool try_entity_stairs(entityid id) {
        massert(id != ENTITYID_INVALID, "Entity ID is invalid!");
        ct.set<update>(id, true);
        vec3 loc = ct.get<location>(id).value();
        // first, we prob want to get the tile at this location
        int current_floor = d.current_floor;
        shared_ptr<dungeon_floor> df = d.floors[current_floor];
        tile_t& t = df->tile_at(loc);
        // check the tile type
        if (t.get_type() == TILE_UPSTAIRS) {
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
                int new_floor = d.current_floor;
                shared_ptr<dungeon_floor> df2 = d.floors[new_floor];
                vec3 uloc = df2->get_downstairs_loc();
                df2->df_add_at(hero_id, ENTITY_PLAYER, uloc);
                ct.set<location>(hero_id, uloc);
                flag = GAMESTATE_FLAG_PLAYER_ANIM;
                PlaySound(sfx.at(SFX_STEP_STONE_1));
                return true;
            }
        }
        else if (t.get_type() == TILE_DOWNSTAIRS) {
            // can't go down on the bottom floor
            // otherwise...
            if ((size_t)current_floor < d.floors.size() - 1) {
                // go downstairs
                // we have to remove the player from the old tile
                df->df_remove_at(hero_id, loc);
                d.current_floor++;
                int new_floor = d.current_floor;
                shared_ptr<dungeon_floor> df2 = d.floors[new_floor];
                vec3 uloc = df2->get_upstairs_loc();
                df2->df_add_at(hero_id, ENTITY_PLAYER, uloc);
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

    bool handle_traverse_stairs(inputstate& is, bool is_dead) {
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

    bool try_entity_open_door(entityid id, vec3 loc) {
        massert(id != ENTITYID_INVALID, "id is invalid");
        if (!tile_has_door(loc)) {
            return false;
        }
        shared_ptr<dungeon_floor> df = d.get_current_floor();
        //shared_ptr<tile_t> t = df->tile_at(loc);
        tile_t& t = df->tile_at(loc);

        //entityid door_id = t->get_cached_door();
        entityid door_id = t.get_cached_door();
        if (door_id != INVALID) {
            optional<bool> maybe_is_open = ct.get<door_open>(door_id);
            massert(maybe_is_open.has_value(), "door %d has no `is_open` component", door_id);
            ct.set<door_open>(door_id, !maybe_is_open.value());
            PlaySound(sfx.at(SFX_CHEST_OPEN));
            return true;
        }

        //for (size_t i = 0; i < t->get_entity_count(); i++) {
        //const entityid myid = t->get_entity_at(i);
        //const entitytype_t type = ct.get<entitytype>(myid).value_or(ENTITY_NONE);
        //if (type != ENTITY_DOOR) {
        //    continue;
        //}
        //optional<bool> maybe_is_open = ct.get<door_open>(myid);
        //massert(maybe_is_open.has_value(), "door %d has no `is_open` component", myid);
        //ct.set<door_open>(myid, !maybe_is_open.value());
        //PlaySound(sfx.at(SFX_CHEST_OPEN));
        //return true;
        //}
        return false;
    }

    bool handle_open_door(inputstate& is, bool is_dead) {
        if (inputstate_is_pressed(is, KEY_O)) {
            if (is_dead) {
                return add_message("You cannot open doors while dead");
            }
            vec3 loc = get_loc_facing_player();
            try_entity_open_door(hero_id, loc);
            flag = GAMESTATE_FLAG_PLAYER_ANIM;
            return true;
        }
        return false;
    }

    void try_entity_cast_spell(entityid id, int tgt_x, int tgt_y) {
        optional<vec3> maybe_loc = ct.get<location>(id);
        if (!maybe_loc.has_value()) {
            merror("no location for entity id %d", id);
            return;
        }
        vec3 loc = maybe_loc.value();
        shared_ptr<dungeon_floor> df = d.get_floor(loc.z);
        vec3 spell_loc = {tgt_x, tgt_y, loc.z};
        //shared_ptr<tile_t> tile = df->tile_at(spell_loc);
        tile_t& tile = df->tile_at(spell_loc);
        // Calculate direction based on target position
        bool ok = false;
        int dx = tgt_x - loc.x;
        int dy = tgt_y - loc.y;
        ct.set<direction>(id, get_dir_from_xy(dx, dy));
        ct.set<casting>(id, true);
        ct.set<update>(id, true);
        // ok...
        // we are hard-coding a spell cast
        // in this example, we will 'create' a 'spell entity' of type 'fire' and place it on a tile
        entityid spell_id = create_spell_at_with(spell_loc);
        ct.set<spellstate>(id, SPELLSTATE_CAST);
        ct.set<spelltype>(id, SPELLTYPE_FIRE);
        ct.set<spell_casting>(id, true);
        if (spell_id != ENTITYID_INVALID) {
            ok = true;
            // lets do an example of processing a spell effect immediately
            // first we need to iterate the entities on the tile
            // if there's an NPC we damage it

            //if (tile_has_live_npcs(tile)) {
            //    entityid npcid = ENTITYID_INVALID;
            //    for (const entityid id : *tile->get_entities()) {
            //        if (ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_NPC) {
            //            npcid = id;
            //            break;
            //        }
            //    }
            //    uniform_int_distribution<int> gen(1, 6);
            //    const int dmg = gen(mt);
            //    ct.set<damaged>(npcid, true);
            //    ct.set<update>(npcid, true);
            //    optional<int> maybe_tgt_hp = ct.get<hp>(npcid);
            //    if (!maybe_tgt_hp.has_value()) {
            //        merror("target has no HP component");
            //        return;
            //    }
            //    int tgt_hp = maybe_tgt_hp.value();
            //    if (tgt_hp <= 0) {
            //        merror("Target is already dead, hp was: %d", tgt_hp);
            //        ct.set<dead>(npcid, true);
            //        return;
            //    }
            //    tgt_hp -= dmg;
            //    ct.set<hp>(npcid, tgt_hp);
            //    if (tgt_hp > 0) {
            //        ct.set<dead>(npcid, false);
            //        return;
            //    }
            //    const entitytype_t tgttype = ct.get<entitytype>(npcid).value_or(ENTITY_NONE);
            //    ct.set<dead>(npcid, true);
            //    shared_ptr<dungeon_floor> df = d.get_current_floor();
            //    shared_ptr<tile_t> target_tile = df->tile_at(spell_loc);
            //    target_tile->set_dirty_entities(true);
            //    if (tgttype == ENTITY_NPC) {
            //        // increment attacker's xp
            //        const int old_xp = ct.get<xp>(id).value_or(0);
            //        const int reward_xp = 1;
            //        const int new_xp = old_xp + reward_xp;
            //        ct.set<xp>(id, new_xp);
            //        // handle item drops
            //        drop_all_from_inventory(npcid);
            //    }
            //    else if (tgttype == ENTITY_PLAYER) {
            //        add_message("You died");
            //    }
            //}

            if (tile_has_door(spell_loc)) {
                // find the door id
                //entityid doorid = tile->get_cached_door();
                entityid doorid = tile.get_cached_door();
                //for (auto id : *tile->get_entities()) {
                //if (ct.get<entitytype>(id).value_or(ENTITY_NONE) == ENTITY_DOOR) {
                //doorid = id;
                //break;
                //}
                //}
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
        bool event_heard = check_hearing(hero_id, (vec3){tgt_x, tgt_y, loc.z});
        if (ok && event_heard) {
            PlaySound(sfx[SFX_ITEM_FUSION]);
        }
    }

    bool handle_test_cast_spell(inputstate& is, bool is_dead) {
        if (inputstate_is_pressed(is, KEY_M)) {
            if (is_dead) {
                add_message("You cannot cast spells while dead (yet)");
                return true;
            }
            if (ct.get<location>(hero_id).has_value() && ct.get<direction>(hero_id).has_value()) {
                vec3 loc = get_loc_facing_player();
                try_entity_cast_spell(hero_id, loc.x, loc.y);
                flag = GAMESTATE_FLAG_PLAYER_ANIM;
                return true;
            }
        }
        return false;
    }

    bool handle_restart(inputstate& is);

    void handle_input_gameplay_controlmode_player(inputstate& is);

    void handle_input_action_menu(inputstate& is);

    void handle_input_option_menu(inputstate& is);

    void handle_input_help_menu(inputstate& is);

    void handle_input_gameplay_scene(inputstate& is);

    void handle_input(inputstate& is);

    void update_debug_panel_buffer(inputstate& is) {
        minfo2("update debug panel buffer");
        // Static buffers to avoid reallocating every frame
        int message_count = msg_history.size();
        int inventory_count;
        vec3 loc = {0, 0, 0};
        inventory_count = -1;
        if (hero_id != ENTITYID_INVALID) {
            loc = ct.get<location>(hero_id).value_or((vec3){-1, -1, -1});
        }
        // current df
        shared_ptr<dungeon_floor> df = d.get_current_floor();
        int df_w = df->get_width();
        int df_h = df->get_height();
        // Determine control mode and flag strings
        const char* control_mode = controlmode == CONTROLMODE_CAMERA ? "Camera" : controlmode == CONTROLMODE_PLAYER ? "Player" : "Unknown";
        // zero out the buffer
        //memset(debugpanel.buffer, 0, sizeof(debugpanel.buffer));

        direction_t player_dir = ct.get<direction>(hero_id).value_or(DIR_NONE);

        bzero(debugpanel.buffer, sizeof(debugpanel.buffer));
        // Format the string in one pass

        minfo2("calling snprintf...");

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
            "draw time: %.2fms\n"
            "avg last %d draw time: %.2fms\n"
            "max draw time: %.2fms\n"
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
            "master volume: %0.1f\n"
            "\n",
            framecount,
            frame_updates,
            frame_dirty,
            last_frame_time * 1000,
            LAST_FRAME_TIMES_MAX,
            (get_avg_last_frame_time() * 1000),
            max_frame_time * 1000,
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
            0L,
            0L,
            god_mode,
            player_dir,
            master_volume);

        msuccess2("successfully updated debug panel buffer");
    }

    bool is_entity_adjacent(entityid id0, entityid id1);

    void update_path_to_target(entityid id);

    bool try_entity_move_to_target(entityid id);

    bool try_entity_move_random(entityid id);

    bool handle_npc(entityid id);

    void handle_npcs();

};

#include "gamestate_lifecycle_impl.h"
#include "gamestate_scene_impl.h"
#include "gamestate_input_impl.h"
#include "gamestate_npc_combat_impl.h"
#include "gamestate_world_impl.h"
#include "gamestate_entity_factory_impl.h"
