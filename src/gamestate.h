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

    entityid tile_has_pullable(int x, int y, int z);

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

    bool drop_from_inventory(entityid actor_id, entityid item_id);

    bool drop_all_from_inventory(entityid actor_id);

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

    bool drop_item_from_hero_inventory();

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

    bool tile_has_solid(int x, int y, int z);

    bool handle_box_push(entityid id, vec3 v);

    entityid tile_has_pushable(int x, int y, int z);

    entityid tile_has_door(vec3 v);

    bool check_hearing(entityid id, vec3 loc);

    bool try_entity_move(entityid id, vec3 v);

    bool handle_move_up(inputstate& is, bool is_dead);

    bool handle_move_down(inputstate& is, bool is_dead);

    bool handle_move_left(inputstate& is, bool is_dead);

    bool handle_move_right(inputstate& is, bool is_dead);

    bool handle_move_up_left(inputstate& is, bool is_dead);

    bool handle_move_up_right(inputstate& is, bool is_dead);

    bool handle_move_down_left(inputstate& is, bool is_dead);

    bool handle_move_down_right(inputstate& is, bool is_dead);

    vec3 get_loc_facing_player();

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

    entityid tile_get_item(shared_ptr<tile_t> t);

    bool try_entity_pull(entityid id);

    bool try_entity_pickup(entityid id);

    bool handle_pickup_item(inputstate& is, bool is_dead);

    bool try_entity_stairs(entityid id);

    bool handle_traverse_stairs(inputstate& is, bool is_dead);

    bool try_entity_open_door(entityid id, vec3 loc);

    bool handle_open_door(inputstate& is, bool is_dead);

    void try_entity_cast_spell(entityid id, int tgt_x, int tgt_y);

    bool handle_test_cast_spell(inputstate& is, bool is_dead);

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
#include "gamestate_world_interaction_impl.h"
#include "gamestate_entity_factory_impl.h"
