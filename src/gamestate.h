#pragma once

/** @file gamestate.h
 *  @brief Top-level gameplay state, orchestration, and API surface for the game runtime.
 */

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

typedef enum {
    CONFIRM_ACTION_NONE = 0,
    CONFIRM_ACTION_QUIT,
} confirm_action_t;

/**
 * @brief Central runtime object for gameplay, world state, entities, UI state, and high-level flow.
 *
 * The project currently treats `gamestate` as the main coordination surface for
 * dungeon generation, entity creation, input handling, combat, inventory, and
 * renderer-facing state preparation.
 */
class gamestate {
public:
    controlmode_t controlmode;
    controlmode_t controlmode_before_confirm;
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
    bool display_confirm_prompt;
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
    confirm_action_t confirm_action;
    string confirm_prompt_message;

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
        display_confirm_prompt = false;
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
        controlmode_before_confirm = CONTROLMODE_PLAYER;
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
        confirm_action = CONFIRM_ACTION_NONE;
        confirm_prompt_message.clear();
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

    /** @brief Create and add a simple floor using the legacy one-room bootstrap path. */
    void create_and_add_df_0(biome_t type, int w, int h, int df_count, float parts);

    /** @brief Create and add a generated floor using the current room-and-corridor generator. */
    void create_and_add_df_1(biome_t type, int w, int h, int df_count, float parts);

    /** @brief Randomly assign upstairs and downstairs locations on a single floor. */
    bool assign_random_stairs_to_floor(shared_ptr<dungeon_floor> df);

    /** @brief Randomly assign stairs across all currently created floors. */
    bool assign_random_stairs();

    /**
     * @brief Initialize a dungeon with generated floors and stairs.
     *
     * @param type Biome to apply to created floors.
     * @param df_count Number of floors to create.
     * @param parts Density/style tuning input for generation.
     * @param width Floor width.
     * @param height Floor height.
     */
    void init_dungeon(biome_t type, int df_count, float parts, int width = DEFAULT_DUNGEON_FLOOR_WIDTH, int height = DEFAULT_DUNGEON_FLOOR_HEIGHT);

    /** @brief Create a door entity and apply additional component initialization. */
    entityid create_door_with(with_fun doorInitFunction);

    bool tile_has_player(shared_ptr<tile_t> t) {
        return t->get_cached_player_present();
    }

    /** @brief Create and place a door entity on a floor tile. */
    entityid create_door_at_with(vec3 loc, with_fun doorInitFunction);

    /** @brief Scan current floors for valid door candidates and place door entities. */
    size_t place_doors();

    /** @brief Create a prop entity of the requested logical type. */
    entityid create_prop_with(proptype_t type, with_fun initFun);

    /** @brief Create and place a prop entity on a tile. */
    entityid create_prop_at_with(proptype_t type, vec3 loc, with_fun initFun);

    /** @brief Populate generated floors with props after layout generation completes. */
    int place_props();

    /** @brief Create a weapon item entity and apply weapon-specific initialization. */
    entityid create_weapon_with(with_fun weaponInitFunction);

    /** @brief Return the default component initializer for dagger items. */
    with_fun dagger_init();

    /** @brief Return the default component initializer for axe items. */
    with_fun axe_init();

    /** @brief Return the default component initializer for sword items. */
    with_fun sword_init();

    /** @brief Return the default component initializer for shield items. */
    with_fun shield_init();

    /**
     * @brief Return the default component initializer for a potion type.
     *
     * @param pt Logical potion type to assign to the created item.
     */
    with_fun potion_init(potiontype_t pt);

    /**
     * @brief Return the default component initializer for the player actor.
     *
     * @param maxhp_roll Rolled maximum hit point seed to apply at creation.
     */
    with_fun player_init(int maxhp_roll);

    /** @brief Create and place a weapon item at a specific dungeon location. */
    entityid create_weapon_at_with(ComponentTable& ct, vec3 loc, with_fun weaponInitFunction);

    /**
     * @brief Create and place a weapon item at a random valid location.
     *
     * @param ct Component table that receives the created entity components.
     * @param weaponInitFunction Additional item-specific initialization.
     * @return The created entity id, or `ENTITYID_INVALID` on failure.
     */
    entityid create_weapon_at_random_loc_with(CT& ct, with_fun weaponInitFunction);

    /**
     * @brief Create a shield item entity and apply shield-specific initialization.
     *
     * @param ct Component table that receives the created entity components.
     * @param shieldInitFunction Additional item-specific initialization.
     * @return The created entity id, or `ENTITYID_INVALID` on failure.
     */
    entityid create_shield_with(ComponentTable& ct, with_fun shieldInitFunction);

    /** @brief Create and place a shield item at a specific dungeon location. */
    entityid create_shield_at_with(ComponentTable& ct, vec3 loc, with_fun shieldInitFunction);

    /**
     * @brief Create a potion item entity and apply potion-specific initialization.
     *
     * @param potionInitFunction Additional item-specific initialization.
     * @return The created entity id, or `ENTITYID_INVALID` on failure.
     */
    entityid create_potion_with(with_fun potionInitFunction);

    /** @brief Create and place a potion item at a specific dungeon location. */
    entityid create_potion_at_with(vec3 loc, with_fun potionInitFunction);

    /** @brief Return a random monster race suitable for generic floor population. */
    race_t random_monster_type();

    /** @brief Roll and assign starting stat values for an NPC actor. */
    void set_npc_starting_stats(entityid id);

    /** @brief Apply baseline NPC components and default runtime values. */
    void set_npc_defaults(entityid id);

    /** @brief Create an NPC entity of the requested race and apply extra initialization. */
    entityid create_npc_with(race_t rt, with_fun npcInitFunction);

    entityid tile_has_box(int x, int y, int z) {
        massert(z >= 0, "floor is out of bounds");
        massert((size_t)z < d.floors.size(), "floor is out of bounds");
        auto df = d.get_floor(z);
        tile_t& t = df->tile_at((vec3){x, y, z});
        return t.get_cached_box();
    }

    /** @brief Return the pullable entity cached on a tile, if any. */
    entityid tile_has_pullable(int x, int y, int z);

    /** @brief Create and place an NPC entity on the dungeon floor. */
    entityid create_npc_at_with(race_t rt, vec3 loc, with_fun npcInitFunction);

    /**
     * @brief Add an item entity to an actor's inventory.
     *
     * @return `true` on success.
     */
    bool add_to_inventory(entityid actor_id, entityid item_id);

    /**
     * @brief Create an orc NPC entity with additional initialization.
     *
     * @param monsterInitFunction Additional NPC-specific initialization.
     * @return The created entity id, or `ENTITYID_INVALID` on failure.
     */
    entityid create_orc_with(with_fun monsterInitFunction);

    /**
     * @brief Create and place an orc NPC entity at a dungeon location.
     *
     * @param loc Dungeon-space location, including floor in `z`.
     * @param monsterInitFunction Additional NPC-specific initialization.
     * @return The created entity id, or `ENTITYID_INVALID` on failure.
     */
    entityid create_orc_at_with(vec3 loc, with_fun monsterInitFunction);

    /**
     * @brief Push a formatted message into the active message queue.
     *
     * @return `true` after the message is queued.
     */
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

    /**
     * @brief Append a formatted message to the persistent message history.
     *
     * @warning This does not activate the transient on-screen message queue.
     */
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

    /** @brief Refresh derived per-tile state after gameplay mutations. */
    void update_tile(tile_t& tile);

    /** @brief Return whether movement or line travel between two points is blocked. */
    bool path_blocked(vec3 a, vec3 b);

    /** @brief Refresh explored and visible tile state from the player's current position. */
    bool update_player_tiles_explored();

    /** @brief Recompute player-facing derived runtime state for the current tick. */
    bool update_player_state();

    /** @brief Advance active spell state and spell-related runtime effects. */
    void update_spells_state();

    /** @brief Refresh NPC state before or during their turn processing. */
    void update_npcs_state();

    /** @brief Initialize gameplay state, generate floors, and seed initial entities. */
    void logic_init();

    /** @brief Handle input while the title scene is active. */
    void handle_input_title_scene(inputstate& is);

    /** @brief Handle input while the main menu scene is active. */
    void handle_input_main_menu_scene(inputstate& is);

    /**
     * @brief Create and place the player actor at a dungeon location.
     *
     * @param loc Dungeon-space location, including floor in `z`.
     * @param n Player-visible name to assign.
     * @param playerInitFunction Additional player-specific initialization.
     * @return The created entity id, or `ENTITYID_INVALID` on failure.
     */
    entityid create_player_at_with(vec3 loc, string n, with_fun playerInitFunction);

    /** @brief Create a box entity with default components. */
    entityid create_box_with();

    /**
     * @brief Create and place a box entity at a dungeon location.
     *
     * @param loc Dungeon-space location, including floor in `z`.
     * @return The created entity id, or `ENTITYID_INVALID` on failure.
     */
    entityid create_box_at_with(vec3 loc);

    /** @brief Create a spell entity with default components. */
    entityid create_spell_with();

    /**
     * @brief Create and place a spell entity at a dungeon location.
     *
     * @param loc Dungeon-space location, including floor in `z`.
     * @return The created entity id, or `ENTITYID_INVALID` on failure.
     */
    entityid create_spell_at_with(vec3 loc);

    /** @brief Retarget all active NPCs toward the player actor. */
    void make_all_npcs_target_player();

    /** @brief Handle input while the character creation scene is active. */
    void handle_input_character_creation_scene(inputstate& is);

    /** @brief Apply any enabled test or debug flags for the current tick. */
    void handle_test_flag();

    /**
     * @brief Advance one gameplay tick from the latest input snapshot.
     *
     * This is the main per-frame game update entry point.
     */
    void tick(inputstate& is);

    /**
     * @brief Remove an item entity from an actor inventory.
     *
     * @return `true` on success.
     */
    bool remove_from_inventory(entityid actor_id, entityid item_id);

    /** @brief Drop an item from an actor inventory back into the world. */
    bool drop_from_inventory(entityid actor_id, entityid item_id);

    /** @brief Drop every inventory item currently carried by an actor. */
    bool drop_all_from_inventory(entityid actor_id);

    /** @brief Equip a weapon from the hero inventory. */
    void handle_hero_inventory_equip_weapon(entityid item_id);

    /** @brief Equip a shield from the hero inventory. */
    void handle_hero_inventory_equip_shield(entityid item_id);

    /** @brief Equip the selected hero inventory item according to its type. */
    void handle_hero_inventory_equip_item(entityid item_id);

    /** @brief Equip the currently selected item in the hero inventory UI. */
    void handle_hero_inventory_equip();

    /** @brief Drop the currently selected item from the hero inventory UI. */
    bool drop_item_from_hero_inventory();

    /** @brief Return whether an actor inventory currently contains an item entity. */
    bool is_in_inventory(entityid actor_id, entityid item_id);

    /** @brief Consume a potion item from an actor inventory and apply its effect. */
    bool use_potion(entityid actor_id, entityid item_id);

    void logic_close() {
        d.floors.clear();
    }

    /** @brief Handle free-camera movement input. */
    void handle_camera_move(inputstate& is);

    /** @brief Apply hero-facing potion usage behavior for the selected item. */
    void handle_hero_potion_use(entityid id);

    /** @brief Use the currently selected item in the hero inventory UI. */
    void handle_hero_item_use();

    /** @brief Handle input while the inventory UI is active. */
    void handle_input_inventory(inputstate& is);

    /** @brief Open the yes/no confirmation prompt with a formatted message. */
    void open_confirm_prompt(confirm_action_t action, const char* fmt, ...);

    /** @brief Resolve the active confirmation prompt with the chosen answer. */
    void resolve_confirm_prompt(bool confirmed);

    /** @brief Handle input while a yes/no confirmation prompt is active. */
    void handle_input_confirm_prompt(inputstate& is);

    /** @brief Execute the quit action associated with the active confirmation prompt. */
    void handle_confirm_quit();

    //inl_count_0 + 2bool handle_quit_pressed(const inputstate& is) {
    //    if (inputstate_is_pressed(is, KEY_ESCAPE)) {
    //        do_quit = true;
    //        return true;
    //    }
    //    return false;
    //}

    /** @brief Advance the visible message queue to the next pending message. */
    void cycle_messages();

    /** @brief Handle player input that advances the message queue. */
    bool handle_cycle_messages(inputstate& is);

    /** @brief Test helper for message queue cycling behavior. */
    bool handle_cycle_messages_test();

    /** @brief Handle free-camera zoom input. */
    void handle_camera_zoom(inputstate& is);

    /** @brief Update the player actor's facing direction. */
    void change_player_dir(direction_t dir);

    /** @brief Handle direct player facing-direction input. */
    bool handle_change_dir(inputstate& is);

    /** @brief Handle input that begins a direction-selection intent. */
    bool handle_change_dir_intent(inputstate& is);

    /** @brief Handle the per-floor debug full-light toggle input. */
    bool handle_toggle_full_light(inputstate& is);

    /** @brief Handle input that opens or toggles the inventory UI. */
    bool handle_display_inventory(inputstate& is);

    /** @brief Return whether the addressed tile currently contains a solid blocking entity. */
    bool tile_has_solid(int x, int y, int z);

    /** @brief Attempt to push a box-like entity in the requested direction. */
    bool handle_box_push(entityid id, vec3 v);

    /** @brief Return the pushable entity cached on a tile, if any. */
    entityid tile_has_pushable(int x, int y, int z);

    /** @brief Return the door entity cached at a dungeon location, if any. */
    entityid tile_has_door(vec3 v);

    /** @brief Return whether an entity can perceive activity at the target location by hearing. */
    bool check_hearing(entityid id, vec3 loc);

    /**
     * @brief Attempt to move an entity by a delta vector.
     *
     * @return `true` when the move succeeds.
     */
    bool try_entity_move(entityid id, vec3 v);

    /** @brief Handle upward movement input for the active actor. */
    bool handle_move_up(inputstate& is, bool is_dead);

    /** @brief Handle downward movement input for the active actor. */
    bool handle_move_down(inputstate& is, bool is_dead);

    /** @brief Handle leftward movement input for the active actor. */
    bool handle_move_left(inputstate& is, bool is_dead);

    /** @brief Handle rightward movement input for the active actor. */
    bool handle_move_right(inputstate& is, bool is_dead);

    /** @brief Handle up-left diagonal movement input for the active actor. */
    bool handle_move_up_left(inputstate& is, bool is_dead);

    /** @brief Handle up-right diagonal movement input for the active actor. */
    bool handle_move_up_right(inputstate& is, bool is_dead);

    /** @brief Handle down-left diagonal movement input for the active actor. */
    bool handle_move_down_left(inputstate& is, bool is_dead);

    /** @brief Handle down-right diagonal movement input for the active actor. */
    bool handle_move_down_right(inputstate& is, bool is_dead);

    /** @brief Return the dungeon location directly in front of the player actor. */
    vec3 get_loc_facing_player();

    entityid get_cached_live_npc(shared_ptr<tile_t> t) {
        if (t == nullptr) {
            return INVALID;
        }
        //recompute_entity_cache(t); // Force update
        return t->get_cached_live_npc();
    }

    /** @brief Compute the current armor class for an entity. */
    int compute_armor_class(entityid id);

    /** @brief Resolve whether an attack roll hits the target. */
    bool compute_attack_roll(entityid attacker, entityid target);

    /** @brief Compute attack damage from attacker to target after current modifiers. */
    int compute_attack_damage(entityid attacker, entityid target);

    /** @brief Apply weapon durability loss after an attack exchange. */
    void handle_weapon_durability_loss(entityid atk_id, entityid tgt_id);

    /** @brief Apply shield durability loss after a defended attack exchange. */
    void handle_shield_durability_loss(entityid defender, entityid attacker);

    /** @brief Return the experience value granted by defeating an NPC. */
    int get_npc_xp(entityid id);

    /** @brief Award NPC-defeat experience to the acting entity. */
    void update_npc_xp(entityid id, entityid target_id);

    /** @brief Apply gameplay side effects after resolving an attack attempt. */
    void process_attack_results(tile_t& tile, entityid atk_id, entityid tgt_id, bool atk_successful);

    /** @brief Play shield-block audio feedback for a defending entity. */
    void handle_shield_block_sfx(entityid target_id);

    /**
     * @brief Resolve a direct attack against a target entity on a tile.
     *
     * @return Detailed attack result describing hit, miss, or block outcome.
     */
    attack_result_t process_attack_entity(tile_t& tile, entityid attacker_id, entityid target_id);

    /** @brief Play attack-result audio feedback for an entity action. */
    void handle_attack_sfx(entityid attacker, attack_result_t result);

    /** @brief Set the active gamestate flag for the next attack animation phase. */
    void set_gamestate_flag_for_attack_animation(entitytype_t type);

    /**
     * @brief Attempt a melee attack from an entity toward a target tile.
     *
     * @return Detailed attack result describing the resolved outcome.
     */
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

    /** @brief Return the top visible item cached on a tile, if any. */
    entityid tile_get_item(shared_ptr<tile_t> t);

    /** @brief Attempt to pull an adjacent pullable entity with the acting entity. */
    bool try_entity_pull(entityid id);

    /** @brief Attempt to pick up the top item at the acting entity's location. */
    bool try_entity_pickup(entityid id);

    /** @brief Handle pickup input for the active actor. */
    bool handle_pickup_item(inputstate& is, bool is_dead);

    /** @brief Attempt to traverse stairs at the acting entity's current location. */
    bool try_entity_stairs(entityid id);

    /** @brief Handle stairs traversal input for the active actor. */
    bool handle_traverse_stairs(inputstate& is, bool is_dead);

    /** @brief Attempt to open a door entity at the requested location. */
    bool try_entity_open_door(entityid id, vec3 loc);

    /** @brief Handle door-opening input for the active actor. */
    bool handle_open_door(inputstate& is, bool is_dead);

    /** @brief Attempt to cast a spell from an entity toward a target tile. */
    void try_entity_cast_spell(entityid id, int tgt_x, int tgt_y);

    /** @brief Handle the current spell-cast test input path. */
    bool handle_test_cast_spell(inputstate& is, bool is_dead);

    /** @brief Handle input that requests a full gameplay restart. */
    bool handle_restart(inputstate& is);

    /** @brief Handle gameplay-scene input while in direct player control mode. */
    void handle_input_gameplay_controlmode_player(inputstate& is);

    /** @brief Handle input while the action menu is active. */
    void handle_input_action_menu(inputstate& is);

    /** @brief Handle input while the options menu is active. */
    void handle_input_option_menu(inputstate& is);

    /** @brief Handle input while the help menu is active. */
    void handle_input_help_menu(inputstate& is);

    /** @brief Dispatch input handling for the active gameplay scene UI state. */
    void handle_input_gameplay_scene(inputstate& is);

    /** @brief Dispatch input handling for the active scene and control mode. */
    void handle_input(inputstate& is);

    /**
     * @brief Rebuild the debug panel text buffer from current runtime state.
     *
     * @warning This routine is debug-facing and reads broadly from renderer and gameplay state.
     */
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
        const int full_light = df->get_full_light();
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
            "full light: %d\n"
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
            full_light,
            god_mode,
            player_dir,
            master_volume);

        msuccess2("successfully updated debug panel buffer");
    }

    /** @brief Return whether two entities occupy adjacent dungeon tiles. */
    bool is_entity_adjacent(entityid id0, entityid id1);

    /** @brief Refresh the current pathfinding route for an entity's active target. */
    void update_path_to_target(entityid id);

    /** @brief Attempt to move an entity one step along its current target path. */
    bool try_entity_move_to_target(entityid id);

    /** @brief Attempt to move an entity using a random roaming step. */
    bool try_entity_move_random(entityid id);

    /** @brief Execute one NPC turn or behavior step for the given entity. */
    bool handle_npc(entityid id);

    /** @brief Process the current batch of NPC turns for the gameplay tick. */
    void handle_npcs();

};

#include "gamestate_lifecycle_impl.h"
#include "gamestate_scene_impl.h"
#include "gamestate_inventory_impl.h"
#include "gamestate_input_impl.h"
#include "gamestate_npc_combat_impl.h"
#include "gamestate_world_impl.h"
#include "gamestate_world_interaction_impl.h"
#include "gamestate_entity_factory_impl.h"
