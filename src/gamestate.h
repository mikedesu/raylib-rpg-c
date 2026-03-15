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
#include "event_type.h"
#include "gameplay_keybindings.h"
#include "gamestate_flag.h"
#include "get_racial_hd.h"
#include "get_racial_modifiers.h"
#include "inputstate.h"
#include "libdraw_context.h"
#include "libgame_defines.h"
#include "magic_values.h"
#include "option_menu.h"
#include "orc_names.h"
#include "roll.h"
#include "scene.h"
#include "sfx.h"
#include "stat_bonus.h"
#include "texture_ids.h"
#include <array>
#include <algorithm>
#include <chrono>
#include <map>
#include <queue>
#include <random>
#include <raylib.h>
#include <raymath.h>

#define GAMESTATE_SIZEOFTIMEBUF 64
#define GAMESTATE_SIZEOFDEBUGPANELBUF 1024
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

constexpr Color DEFAULT_WINDOW_BOX_BGCOLOR = Color{0, 0, 255, 128};
constexpr Color DEFAULT_WINDOW_BOX_FGCOLOR = Color{255, 255, 255, 255};
constexpr float AUDIO_VOLUME_STEP = 0.1f;

typedef enum {
    CONFIRM_ACTION_NONE = 0,
    CONFIRM_ACTION_QUIT,
} confirm_action_t;

struct damage_popup_t {
    entityid target_id;
    int amount;
    bool critical;
    int floor;
    Vector2 world_anchor;
    float age_seconds;
    float lifetime_seconds;
    float drift_x;
    float rise_distance;
};

struct floor_pressure_plate_t {
    vec3 loc;
    entityid linked_door_id;
    bool active;
    bool destroyed;
    int txkey_up;
    int txkey_down;
};

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
    entityid active_chest_id;
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
    bool display_chest_menu;
    bool display_action_menu;
    bool display_option_menu;
    bool display_sound_menu;
    bool display_window_color_menu;
    bool display_controls_menu;
    bool display_keyboard_profile_prompt;
    bool display_confirm_prompt;
    bool display_interaction_modal;
    bool display_level_up_modal;
    bool do_quit;
    bool dirty_entities;
    bool display_help_menu;
    bool cam_changed;
    bool frame_dirty;
    bool do_restart;
    bool msg_system_is_active;
    bool chest_deposit_mode;
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
    unsigned int level_up_selection;
    unsigned int gameplay_settings_menu_selection;
    unsigned int sound_menu_selection;
    unsigned int window_color_menu_selection;
    unsigned int keyboard_profile_selection;
    unsigned int mini_inventory_visible_count;
    unsigned int mini_inventory_scroll_offset;
    unsigned int title_screen_selection;
    unsigned int max_title_screen_selections;
    unsigned int msg_history_max_len_msg;
    unsigned int msg_history_max_len_msg_measure;
    unsigned int restart_count;
    unsigned int font_size;
    unsigned int turn_count;
    unsigned long int ticks;
    size_t action_selection;
    float line_spacing;
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
    Color window_box_bgcolor;
    Color window_box_fgcolor;
    Vector2 last_click_screen_pos;
    Vector2 inventory_cursor = {0, 0};
    Camera2D cam2d;
    Font font;
    ComponentTable ct;
    option_menu options_menu;
    keyboard_profile_t keyboard_profile = KEYBOARD_PROFILE_FULL;
    bool keyboard_profile_confirmed = false;
    bool controls_menu_waiting_for_key;
    bool prefer_mini_inventory_menu;
    size_t controls_menu_selection;
    gameplay_input_action_t controls_menu_pending_action;
    std::array<std::array<gameplay_keybinding_t, INPUT_ACTION_COUNT>, KEYBOARD_PROFILE_COUNT> keybindings;
    confirm_action_t confirm_action;
    string confirm_prompt_message;
    entityid active_interaction_entity_id;
    string interaction_title;
    string interaction_body;
    int pending_level_ups;
    vector<damage_popup_t> damage_popups;
    vector<floor_pressure_plate_t> floor_pressure_plates;
    vector<gameplay_event_t> gameplay_events;
    vec3 floor_four_tutorial_orc_spawn = vec3{-1, -1, -1};

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
        active_chest_id = INVALID;
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
        display_chest_menu = false;
        display_option_menu = false;
        display_sound_menu = false;
        display_window_color_menu = false;
        display_help_menu = false;
        display_controls_menu = false;
        display_keyboard_profile_prompt = false;
        display_confirm_prompt = false;
        display_interaction_modal = false;
        display_level_up_modal = false;
        do_quit = false;
        cam_changed = false;
        gameover = dirty_entities = false;
        processing_actions = false;
        test_guard = false;
        player_changing_dir = false;
        msg_system_is_active = false;
        chest_deposit_mode = false;
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
        sound_menu_selection = 0;
        window_color_menu_selection = 0;
        keyboard_profile_selection = 0;
        mini_inventory_visible_count = 10;
        mini_inventory_scroll_offset = 0;
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
        level_up_selection = 0;
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
        chara_creation.alignment = ALIGNMENT_NEUTRAL_NEUTRAL;
        chara_creation.hitdie = get_racial_hd(RACE_HUMAN);
        current_scene = SCENE_TITLE;
        libdraw_ctx.audio.master_volume = DEFAULT_MASTER_VOLUME;
        libdraw_ctx.audio.music_volume = DEFAULT_MUSIC_VOLUME;
        libdraw_ctx.audio.sfx_volume = DEFAULT_MASTER_VOLUME;
        libdraw_ctx.audio.music_volume_changed = false;
        libdraw_ctx.audio.current_music_index = 0;
        window_box_bgcolor = DEFAULT_WINDOW_BOX_BGCOLOR;
        window_box_fgcolor = DEFAULT_WINDOW_BOX_FGCOLOR;
        message_history_bgcolor = DEFAULT_WINDOW_BOX_BGCOLOR;
        last_click_screen_pos = Vector2{-1, -1};
        confirm_action = CONFIRM_ACTION_NONE;
        confirm_prompt_message.clear();
        active_interaction_entity_id = ENTITYID_INVALID;
        interaction_title.clear();
        interaction_body.clear();
        pending_level_ups = 0;
        damage_popups.clear();
        floor_pressure_plates.clear();
        gameplay_events.clear();
        floor_four_tutorial_orc_spawn = vec3{-1, -1, -1};
        prefer_mini_inventory_menu = false;
        controls_menu_waiting_for_key = false;
        controls_menu_selection = 0;
        controls_menu_pending_action = INPUT_ACTION_MOVE_UP;
        reset_default_keybindings();
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

    /** @brief Create a treasure chest entity and apply additional component initialization. */
    entityid create_chest_with(with_fun chestInitFunction);

    /** @brief Create and place a treasure chest entity on a floor tile. */
    entityid create_chest_at_with(vec3 loc, with_fun chestInitFunction);

    /** @brief Place one empty treasure chest on floor `0` if a valid tile exists. */
    entityid place_first_floor_chest();

    /** @brief Create a prop entity of the requested logical type. */
    entityid create_prop_with(proptype_t type, with_fun initFun);

    /** @brief Create and place a prop entity on a tile. */
    entityid create_prop_at_with(proptype_t type, vec3 loc, with_fun initFun);

    /** @brief Populate generated floors with props after layout generation completes. */
    int place_props();

    /** @brief Place deterministic pullable props in the floor-3 tutorial room. */
    int place_floor_three_pullable_props();

    /** @brief Place the floor-3 tutorial sign that explains pullables. */
    entityid place_floor_three_pullable_sign();

    /** @brief Register a floor pressure plate that controls a specific door. */
    bool create_floor_pressure_plate(vec3 loc, entityid linked_door_id);

    /** @brief Destroy a floor pressure plate and sever any linked event connection. */
    bool destroy_floor_pressure_plate(vec3 loc);

    /** @brief Create the floor-4 tutorial pressure-plate setup around the arrival room. */
    bool setup_floor_four_pressure_plate_tutorial();

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

    /** @brief Return the default alignment to assign for a race. */
    alignment_t default_alignment_for_race(race_t rt);

    /** @brief Return whether an alignment should default to hostile behavior. */
    bool alignment_is_aggressive(alignment_t alignment);

    /** @brief Return an initializer that assigns an explicit alignment component. */
    with_fun npc_alignment_init(alignment_t alignment);

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

    entityid tile_has_chest(int x, int y, int z) {
        massert(z >= 0, "floor is out of bounds");
        massert((size_t)z < d.floors.size(), "floor is out of bounds");
        auto df = d.get_floor(z);
        tile_t& t = df->tile_at((vec3){x, y, z});
        return t.get_cached_chest();
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

    /** @brief Return whether line-of-sight to a tile is blocked, excluding a closed door on the target tile itself. */
    bool visibility_path_blocked(vec3 a, vec3 b);

    /** @brief Refresh explored and visible tile state from the player's current position. */
    bool update_player_tiles_explored();

    /** @brief Recompute player-facing derived runtime state for the current tick. */
    bool update_player_state();

    /** @brief Refresh NPC state before or during their turn processing. */
    void update_npcs_state();

    /** @brief Recompute one NPC's target and default action from its aggression state. */
    void update_npc_behavior(entityid id);

    /** @brief Initialize gameplay state, generate floors, and seed initial entities. */
    void logic_init();

    /** @brief Perform an in-process gameplay restart without recreating the OS window. */
    void restart_game();

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

    /** @brief Retarget all active NPCs toward the player actor. */
    void make_all_npcs_target_player();

    /** @brief Append one printable character to the character-creation name buffer. */
    bool try_append_character_creation_char(int codepoint);

    /** @brief Remove the last character from the character-creation name buffer. */
    bool backspace_character_creation_name();

    /** @brief Pull typed characters from Raylib into the character-creation name buffer. */
    bool handle_character_creation_text_input(inputstate& is);

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

    /** @brief Return whether inventory/chest UIs should use the compact list presentation. */
    bool use_mini_inventory_menu() const;

    /** @brief Return the currently selected inventory/chest item index for the active menu mode. */
    size_t get_inventory_selection_index() const;

    /** @brief Move the active inventory/chest selection by a signed delta. */
    void move_inventory_selection(int delta);

    /** @brief Clamp inventory/chest selection state to the current active list size. */
    void clamp_inventory_selection(size_t item_count);

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

    /** @brief Return whether the active profile maps the requested gameplay action to a pressed key. */
    bool is_action_pressed(const inputstate& is, gameplay_input_action_t action) const;

    /** @brief Return whether the active profile maps the requested gameplay action to a held key. */
    bool is_action_held(const inputstate& is, gameplay_input_action_t action) const;

    /** @brief Restore one profile/action pair to its built-in defaults. */
    void set_default_keybinding(keyboard_profile_t profile, gameplay_input_action_t action);

    /** @brief Restore all gameplay profiles to their built-in defaults. */
    void reset_default_keybindings();

    /** @brief Restore one keyboard profile to its built-in defaults. */
    void reset_profile_keybindings(keyboard_profile_t profile);

    /** @brief Replace the primary key for one action within one profile. */
    void set_keybinding_primary(keyboard_profile_t profile, gameplay_input_action_t action, int key);

    /** @brief Read the primary key assigned to one action in one profile. */
    int get_keybinding_primary(keyboard_profile_t profile, gameplay_input_action_t action) const;

    /** @brief Build a display string for one action's active keybinding. */
    string get_keybinding_label(keyboard_profile_t profile, gameplay_input_action_t action) const;

    /** @brief Open the mandatory profile-selection prompt shown after character creation. */
    void open_keyboard_profile_prompt();

    /** @brief Apply the currently highlighted profile prompt selection. */
    void apply_keyboard_profile_selection();

    /** @brief Handle input while the keyboard-profile prompt is active. */
    void handle_input_keyboard_profile_prompt(inputstate& is);

    /** @brief Open the controls menu for rebinding gameplay actions. */
    void open_controls_menu();

    /** @brief Close the controls menu and restore direct player control. */
    void close_controls_menu();

    /** @brief Handle input while the controls menu is active. */
    void handle_input_controls_menu(inputstate& is);

    /** @brief Apply hero-facing potion usage behavior for the selected item. */
    void handle_hero_potion_use(entityid id);

    /** @brief Use the currently selected item in the hero inventory UI. */
    void handle_hero_item_use();

    /** @brief Handle input while the inventory UI is active. */
    void handle_input_inventory(inputstate& is);

    /** @brief Transfer one item between two inventory-owning entities. */
    bool transfer_inventory_item(entityid from_id, entityid to_id, entityid item_id);

    /** @brief Open the active chest UI for the provided chest entity. */
    bool open_chest_menu(entityid chest_id);

    /** @brief Close the active chest UI and restore player control. */
    void close_chest_menu();

    /** @brief Toggle between chest contents view and hero deposit view. */
    void toggle_chest_menu_mode();

    /** @brief Handle confirm/transfer behavior for the active chest UI selection. */
    void handle_chest_menu_confirm();

    /** @brief Handle input while the chest UI is active. */
    void handle_input_chest(inputstate& is);

    /** @brief Open the yes/no confirmation prompt with a formatted message. */
    void open_confirm_prompt(confirm_action_t action, const char* fmt, ...);

    /** @brief Resolve the active confirmation prompt with the chosen answer. */
    void resolve_confirm_prompt(bool confirmed);

    /** @brief Handle input while a yes/no confirmation prompt is active. */
    void handle_input_confirm_prompt(inputstate& is);

    /** @brief Execute the quit action associated with the active confirmation prompt. */
    void handle_confirm_quit();

    /** @brief Open the current interaction modal for an NPC or prop. */
    void open_interaction_modal(entityid id, const string& title, const string& body);

    /** @brief Close the current interaction modal and restore previous control. */
    void close_interaction_modal();

    /** @brief Handle input while an interaction modal is active. */
    void handle_input_interaction(inputstate& is);

    /** @brief Return the XP threshold required for the next level-up. */
    int get_level_up_xp_threshold(entityid id);

    /** @brief Open the level-up picker and suspend normal gameplay input. */
    void open_level_up_modal();

    /** @brief Apply a permanent attribute increase outside or inside level-up flow. */
    bool apply_permanent_attribute_increase(entityid id, unsigned int attribute_index, int amount = 1);

    /** @brief Roll one level-up hit-die reward for the entity's maximum HP. */
    int roll_level_up_max_hp_gain(entityid id);

    /** @brief Apply generic rewards that happen whenever an entity gains a level. */
    void apply_level_up_rewards(entityid id);

    /** @brief Apply the currently selected permanent attribute increase. */
    void apply_level_up_selection();

    /** @brief Award pending level-ups when the actor has reached the XP threshold. */
    void maybe_unlock_level_up(entityid id);

    /** @brief Handle input while the level-up picker is active. */
    void handle_input_level_up(inputstate& is);

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

    /** @brief Attempt to push one pushable entity by a delta vector. */
    bool try_entity_push(entityid id, vec3 v);

    /** @brief Return the pushable entity cached on a tile, if any. */
    entityid tile_has_pushable(int x, int y, int z);

    /** @brief Return the door entity cached at a dungeon location, if any. */
    entityid tile_has_door(vec3 v);

    /** @brief Return whether an entity can perceive activity at the target location by hearing. */
    bool check_hearing(entityid id, vec3 loc);

    /** @brief Remove all queued gameplay events for the current turn slice. */
    void clear_gameplay_events();

    /** @brief Append one gameplay event to the current turn queue. */
    bool queue_gameplay_event(const gameplay_event_t& event);

    /** @brief Queue a movement intent for ordered turn resolution. */
    bool queue_move_event(entityid id, vec3 v);

    /** @brief Queue a push intent for ordered turn resolution. */
    bool queue_push_event(entityid id, vec3 v);

    /** @brief Queue an attack intent for ordered turn resolution. */
    bool queue_attack_event(entityid id, vec3 loc);

    /** @brief Queue shield-block follow-up resolution for an attack exchange. */
    bool queue_attack_block_event(entityid attacker_id, entityid target_id);

    /** @brief Queue damage follow-up resolution for an attack exchange. */
    bool queue_attack_damage_event(entityid attacker_id, entityid target_id, int damage);

    /** @brief Queue death cleanup follow-up for a defeated combat target. */
    bool queue_attack_death_event(entityid attacker_id, entityid target_id);

    /** @brief Queue a pull intent for ordered turn resolution. */
    bool queue_pull_event(entityid id);

    /** @brief Queue a manual door-toggle intent for ordered turn resolution. */
    bool queue_open_door_event(entityid id, vec3 loc);

    /** @brief Queue a chest-toggle intent for ordered turn resolution. */
    bool queue_open_chest_event(entityid id, vec3 loc);

    /** @brief Queue an interaction intent for ordered turn resolution. */
    bool queue_interact_event(entityid id, vec3 loc);

    /** @brief Queue a pickup intent for ordered turn resolution. */
    bool queue_pickup_event(entityid id);

    /** @brief Queue a stairs-traversal intent for ordered turn resolution. */
    bool queue_traverse_stairs_event(entityid id);

    /** @brief Queue a floor-local pressure-plate refresh follow-up. */
    bool queue_pressure_plate_refresh_event(int z);

    /** @brief Queue a pressure-plate-driven door-state follow-up. */
    bool queue_pressure_plate_set_door_event(entityid door_id, bool should_open);

    /** @brief Resolve one queued gameplay event. */
    gameplay_event_result_t process_gameplay_event(const gameplay_event_t& event);

    /** @brief Drain queued gameplay events in FIFO order. */
    gameplay_event_result_t process_gameplay_events();

    /** @brief Queue and immediately resolve one movement intent plus follow-up events. */
    bool run_move_action(entityid id, vec3 v);

    /** @brief Queue and immediately resolve one push intent plus follow-up events. */
    bool run_push_action(entityid id, vec3 v);

    /** @brief Queue and immediately resolve one attack intent. */
    attack_result_t run_attack_action(entityid id, vec3 loc);

    /** @brief Queue and immediately resolve one pull intent plus follow-up events. */
    bool run_pull_action(entityid id);

    /** @brief Queue and immediately resolve one manual door-toggle intent. */
    bool run_open_door_action(entityid id, vec3 loc);

    /** @brief Queue and immediately resolve one chest-toggle intent. */
    bool run_open_chest_action(entityid id, vec3 loc);

    /** @brief Queue and immediately resolve one interaction intent. */
    bool run_interact_action(entityid id, vec3 loc);

    /** @brief Queue and immediately resolve one pickup intent. */
    bool run_pickup_action(entityid id);

    /** @brief Queue and immediately resolve one stairs-traversal intent. */
    bool run_traverse_stairs_action(entityid id);

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

    /** @brief Flip an NPC into hostile state in response to a violation. */
    void provoke_npc(entityid npc_id, entityid source_id);

    /** @brief Queue a floating damage-number popup at the target entity's current world position. */
    void add_damage_popup(entityid target_id, int amount, bool critical = false);

    /** @brief Advance and expire active floating damage-number popups. */
    void update_damage_popups(float dt_seconds);

    /** @brief Apply gameplay side effects after resolving an attack attempt. */
    void process_attack_results(tile_t& tile, entityid atk_id, entityid tgt_id, bool atk_successful);

    /** @brief Play shield-block audio feedback for a defending entity. */
    void handle_shield_block_sfx(entityid target_id);

    /** @brief Resolve queued attack intent and schedule follow-up combat events. */
    attack_result_t resolve_attack_intent(entityid attacker_id, vec3 target_loc);

    /** @brief Apply queued shield-block consequences for a defended attack. */
    void resolve_attack_block_event(entityid attacker_id, entityid target_id);

    /** @brief Apply queued damage consequences for a successful attack hit. */
    void resolve_attack_damage_event(entityid attacker_id, entityid target_id, int damage);

    /** @brief Apply queued death cleanup and rewards for a defeated target. */
    void resolve_attack_death_event(entityid attacker_id, entityid target_id);

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
        if (is_action_pressed(is, INPUT_ACTION_ATTACK)) {
            if (is_dead) {
                return add_message("You cannot attack while dead");
            }
            if (ct.has<location>(hero_id) && ct.has<direction>(hero_id)) {
                vec3 loc = get_loc_facing_player();
                run_attack_action(hero_id, loc);
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

    /** @brief Return whether the requested tile currently has an entity activating a pressure plate. */
    bool tile_has_pressure_plate_occupant(vec3 loc);

    /** @brief Recompute all pressure plates on one floor and apply linked door states. */
    void update_pressure_plates_for_floor(int z);

    /** @brief Apply one pressure-plate-driven linked-door open/close state. */
    void resolve_pressure_plate_set_door_event(entityid door_id, bool should_open);

    /** @brief Recompute every registered pressure plate in the dungeon. */
    void refresh_pressure_plates();

    /** @brief Return whether a door is controlled by a pressure plate. */
    bool door_is_pressure_plate_controlled(entityid door_id) const;

    /** @brief Return the pressure plate registered at a location, if one exists. */
    floor_pressure_plate_t* get_floor_pressure_plate(vec3 loc);

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

    /** @brief Attempt to open a chest entity at the requested location. */
    bool try_entity_open_chest(entityid id, vec3 loc);

    /** @brief Attempt to interact with the front-facing NPC or prop. */
    bool try_entity_interact(entityid id, vec3 loc);

    /** @brief Handle door-opening input for the active actor. */
    bool handle_open_door(inputstate& is, bool is_dead);

    /** @brief Handle NPC/prop interaction input for the active actor. */
    bool handle_interact(inputstate& is, bool is_dead);

    /** @brief Handle input that requests a full gameplay restart. */
    bool handle_restart(inputstate& is);

    /** @brief Handle gameplay-scene input while in direct player control mode. */
    void handle_input_gameplay_controlmode_player(inputstate& is);

    /** @brief Handle input while the action menu is active. */
    void handle_input_action_menu(inputstate& is);

    /** @brief Handle input while the options menu is active. */
    void handle_input_option_menu(inputstate& is);
    void handle_input_sound_menu(inputstate& is);
    void handle_input_window_color_menu(inputstate& is);

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
            get_master_volume());

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

    void open_sound_menu();
    void close_sound_menu();
    void open_window_color_menu();
    void close_window_color_menu();
    void apply_audio_settings();
    void adjust_master_volume(int dir);
    void adjust_music_volume(int dir);
    void adjust_sfx_volume(int dir);
    float get_master_volume() const;
    float get_music_volume() const;
    float get_sfx_volume() const;
    bool get_music_volume_changed() const;
    unsigned int get_current_music_index() const;
    void set_master_volume(float value);
    void set_music_volume(float value);
    void set_sfx_volume(float value);
    void set_music_volume_changed(bool value);
    void set_current_music_index(unsigned int value);
    void adjust_window_box_bg_channel(size_t channel, int dir);
    void adjust_window_box_fg_channel(size_t channel, int dir);
    void reset_window_box_colors();
    Color get_debug_panel_bgcolor() const;

};

#include "gamestate_lifecycle_impl.h"
#include "gamestate_scene_impl.h"
#include "gamestate_inventory_impl.h"
#include "gamestate_keybinding_impl.h"
#include "gamestate_options_impl.h"
#include "gamestate_input_impl.h"
#include "gamestate_npc_combat_impl.h"
#include "gamestate_damage_popups_impl.h"
#include "gamestate_world_impl.h"
#include "gamestate_world_interaction_impl.h"
#include "gamestate_entity_factory_impl.h"
