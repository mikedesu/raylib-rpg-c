/** @file libdraw.cpp
 *  @brief Top-level renderer entry points and process-lifetime renderer globals.
 */

#include "libdraw.h"
#include "ComponentTraits.h"
#include "create_sg_byid.h"
#include "camera_lock_on.h"
#include "draw_action_menu.h"
#include "draw_character_creation_screen.h"
#include "draw_chest_menu.h"
#include "draw_controls_menu.h"
#include "draw_damage_numbers.h"
#include "draw_dungeon_floor.h"
#include "draw_entity_sprite.h"
#include "draw_sprite.h"
#include "draw_handle_gamestate_flag.h"
#include "draw_handle_debug_panel.h"
#include "draw_help_menu.h"
#include "draw_hud.h"
#include "draw_interaction_modal.h"
#include "draw_inventory_menu.h"
#include "draw_keyboard_profile_prompt.h"
#include "draw_level_up_modal.h"
#include "draw_look_panel.h"
#include "draw_message_box.h"
#include "draw_message_history.h"
#include "draw_mini_inventory_menu.h"
#include "draw_option_menu.h"
#include "draw_sound_menu.h"
#include "draw_title_screen.h"
#include "draw_window_color_menu.h"
#include "entitytype.h"
#include "get_txkey_for_tiletype.h"
#include "get_shield_sprite.h"
#include "get_weapon_sprite.h"
#include "item.h"
#include "libdraw_frame_stats.h"
#include "libdraw_from_texture.h"
#include "libdraw_create_spritegroup.h"
#include "libdraw_update_shield_for_entity.h"
#include "libdraw_to_texture.h"
#include "libdraw_update_sprites.h"
#include "libdraw_update_weapon_for_entity.h"
#include "load_music.h"
#include "load_sfx.h"
#include "load_textures.h"
#include "set_sg.h"
#include "shaders.h"
#include "spritegroup_anim.h"
#include "stat_bonus.h"
#include "tx_keys_boxes.h"
#include "tx_keys_chests.h"
#include "tx_keys_doors.h"
#include "tx_keys_monsters.h"
#include "tx_keys_npcs.h"
#include "tx_keys_potions.h"
#include "tx_keys_props.h"
#include "tx_keys_shields.h"
#include "tx_keys_weapons.h"
#include "unload_textures.h"
#include "update_sprite.h"
#include <algorithm>
#include <array>
#include <cstdio>

libdraw_context_t libdraw_ctx;

bool create_spritegroup(gamestate& g, entityid id, int* keys, int num_keys, int offset_x, int offset_y) {
    minfo("BEGIN create_spritegroup");
    massert(libdraw_ctx.txinfo, "txinfo is null");
    spritegroup* group = new spritegroup(SPRITEGROUP_DEFAULT_SIZE);

    massert(group, "spritegroup is NULL");
    auto maybe_loc = g.ct.get<location>(id);

    minfo("checking if has location");
    if (maybe_loc.has_value()) {
        minfo("it DOES have a location");
        const vec3 loc = maybe_loc.value();
        massert(loc.z >= 0 && static_cast<size_t>(loc.z) < g.d.get_floor_count(), "location z out of bounds: %d", loc.z);
        auto df = g.d.get_floor(loc.z);
        massert(df, "dungeon floor is NULL");
        massert(loc.x >= 0 && loc.x < df->get_width(), "location x out of bounds: %d", loc.x);
        massert(loc.y >= 0 && loc.y < df->get_height(), "location y out of bounds: %d", loc.y);

        minfo2("creating spritegroups...");
        minfo2("num_keys: %d", num_keys);
        int count = 0;
        for (int i = 0; i < num_keys; i++) {
            const int k = keys[i];
            minfo("k: %d", k);
            Texture2D* tex = &libdraw_ctx.txinfo[k].texture;
            auto s = make_shared<sprite>(tex, libdraw_ctx.txinfo[k].contexts, libdraw_ctx.txinfo[k].num_frames);
            massert(s, "s is NULL for some reason!");
            group->add(s);
            count++;
        }
        msuccess2("spritegroups created");
        minfo2("count: %d", count);
        minfo2("setting id: %d", id);
        group->id = id;

        string n = g.ct.get<name>(id).value_or("no-name");
        minfo2("name: %s", n.c_str());
        entitytype_t t = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
        string t_s = entitytype_to_str(t);
        minfo2("type: %s", t_s.c_str());
        minfo2("group->get(0)");
        minfo2("group->count(): %lu", group->count());

        auto s = group->get(0);
        massert(s, "sprite is NULL");

        minfo2("group->current = 0");
        group->current = 0;

        float x = loc.x * DEFAULT_TILE_SIZE + offset_x;
        float y = loc.y * DEFAULT_TILE_SIZE + offset_y;
        minfo("getting width");
        float w = s->get_width();
        minfo("getting height");
        float h = s->get_height();

        minfo("setting destination...");
        group->dest = Rectangle{x, y, w, h};
        group->off_x = offset_x;
        group->off_y = offset_y;
        libdraw_ctx.spritegroups[id] = group;

        msuccess("END create spritegroup");
        return true;
    }

    minfo("it does NOT have a location");
    for (int i = 0; i < num_keys; i++) {
        int k = keys[i];
        Texture2D* tex = &libdraw_ctx.txinfo[k].texture;
        auto s = make_shared<sprite>(tex, libdraw_ctx.txinfo[k].contexts, libdraw_ctx.txinfo[k].num_frames);
        group->add(s);
    }
    group->id = id;
    group->current = 0;
    auto s = group->get(0);
    massert(s, "sprite is NULL");
    float x = -DEFAULT_TILE_SIZE + offset_x;
    float y = -DEFAULT_TILE_SIZE + offset_y;
    float w = s->get_width();
    float h = s->get_height();
    group->dest = Rectangle{x, y, w, h};
    group->off_x = offset_x;
    group->off_y = offset_y;
    libdraw_ctx.spritegroups[id] = group;
    msuccess("END create spritegroup");
    return true;
}

bool create_sg(gamestate& g, entityid id, int* keys, int num_keys) {
    return create_spritegroup(g, id, keys, num_keys, -12, -12);
}

void draw_hud(gamestate& g) {
    constexpr int font_size = DEFAULT_HUD_FONT_SIZE;
    constexpr int line_thickness = 2;
    constexpr int line_gap = 2;
    constexpr int line_count = 5;
    constexpr int inner_pad = 8;

    const int turn = g.turn_count;
    const vec2 hp_value = g.ct.get<hp>(g.hero_id).value_or(vec2{-1, -1});
    const int myhp = hp_value.x;
    const int mymaxhp = hp_value.y;
    const int mylevel = g.ct.get<level>(g.hero_id).value_or(0);
    const int myxp = g.ct.get<xp>(g.hero_id).value_or(0);
    const int attack_bonus = get_stat_bonus(g.ct.get<strength>(g.hero_id).value_or(10));
    const int ac = g.compute_armor_class(g.hero_id);
    const int str = g.ct.get<strength>(g.hero_id).value_or(-1);
    const int dex = g.ct.get<dexterity>(g.hero_id).value_or(-1);
    const int con = g.ct.get<constitution>(g.hero_id).value_or(-1);
    const int int_ = g.ct.get<intelligence>(g.hero_id).value_or(-1);
    const int wis = g.ct.get<wisdom>(g.hero_id).value_or(-1);
    const int cha = g.ct.get<charisma>(g.hero_id).value_or(-1);
    const string n = g.ct.get<name>(g.hero_id).value_or("no-name");
    const vec3 loc = g.ct.get<location>(g.hero_id).value_or(vec3{-1, -1, -1});
    const alignment_t hero_alignment = g.ct.get<alignment>(g.hero_id).value_or(ALIGNMENT_NONE);
    const std::array<std::string, line_count> lines = {
        n,
        TextFormat("Lvl %d HP %d/%d  Atk: %d  AC: %d", mylevel, myhp, mymaxhp, attack_bonus, ac),
        TextFormat("Str %d Dex %d Con %d Int %d Wis %d Cha %d", str, dex, con, int_, wis, cha),
        TextFormat("Location: (%d, %d, %d) Turn %d  XP %d", loc.x, loc.y, loc.z, turn, myxp),
        TextFormat("Alignment: %s", alignment_to_str(hero_alignment).c_str()),
    };

    int max_w = 0;
    for (const std::string& line : lines) {
        max_w = std::max(max_w, MeasureText(line.c_str(), font_size));
    }

    const float box_w = max_w + inner_pad * 2;
    const float box_h = line_count * font_size + (line_count - 1) * line_gap + inner_pad * 2;
    const float box_x = g.targetwidth / 2.0f - (box_w / 2.0f);
    const float box_y = g.targetheight - box_h - g.pad;

    DrawRectangleRec(Rectangle{box_x, box_y, box_w, box_h}, g.window_box_bgcolor);
    DrawRectangleLinesEx(Rectangle{box_x, box_y, box_w, box_h}, line_thickness, g.window_box_fgcolor);

    const int text_x = box_x + inner_pad;
    int text_y = box_y + inner_pad;
    for (const std::string& line : lines) {
        DrawText(line.c_str(), text_x, text_y, font_size, g.window_box_fgcolor);
        text_y += font_size + line_gap;
    }
}

void draw_action_menu(gamestate& g) {
    constexpr int fontsize = 10;
    constexpr int hp = 20;
    constexpr float x = 10;
    constexpr float y = 10;
    constexpr float rotation = 0;
    constexpr int thickness = 1;
    constexpr float xp = 10;
    constexpr float yp = 10;
    constexpr float wp = 20;
    constexpr Vector2 origin = {0, 0};
    const vector<string> actions = {"Attack", "Open", "Inventory"};
    const float h = fontsize * actions.size();
    const string s = "> Inventory";
    const float w = MeasureText(s.c_str(), fontsize);
    const Rectangle r = {x, y, w + wp, h + hp};
    DrawRectanglePro(r, origin, rotation, g.window_box_bgcolor);
    DrawRectangleLinesEx(r, thickness, g.window_box_fgcolor);
    for (size_t i = 0; i < actions.size(); i++) {
        if (g.action_selection == i) {
            DrawText(TextFormat("> %s", actions[i].c_str()), x + xp, y + yp + (fontsize * i), fontsize, YELLOW);
        }
        else {
            DrawText(TextFormat("  %s", actions[i].c_str()), x + xp, y + yp + (fontsize * i), fontsize, g.window_box_fgcolor);
        }
    }
}

void draw_option_menu(gamestate& g) {
    constexpr float x = 10, y = 10, p = 20, pa = 10, rotation = 0;
    constexpr int fsize = 10, thickness = 1;
    constexpr Vector2 origin = {0, 0};
    float max_w = 0;
    for (size_t i = 0; i < g.options_menu.get_option_count(); i++) {
        const option_type option = g.options_menu.get_option(i);
        const string option_label =
            option == OPTION_INVENTORY_MENU
                ? TextFormat("inventory menu: %s", g.prefer_mini_inventory_menu ? "mini" : "full")
                : g.options_menu.get_option_str(option);
        const char* spaced_str = TextFormat("  %s", option_label.c_str());
        const float w = MeasureText(spaced_str, fsize);
        if (w > max_w) {
            max_w = w;
        }
    }
    const float h = fsize * g.options_menu.get_option_count();
    const float padded_w = max_w + p, padded_h = h + p;
    const Rectangle r = {x, y, padded_w, padded_h};
    DrawRectanglePro(r, origin, rotation, g.window_box_bgcolor);
    DrawRectangleLinesEx(r, thickness, g.window_box_fgcolor);
    for (size_t i = 0; i < g.options_menu.get_option_count(); i++) {
        constexpr int x0 = x + pa;
        const int y0 = y + pa + fsize * i;
        const option_type otype = g.options_menu.get_option(i);
        const string ostr =
            otype == OPTION_INVENTORY_MENU
                ? TextFormat("inventory menu: %s", g.prefer_mini_inventory_menu ? "mini" : "full")
                : g.options_menu.get_option_str(otype);
        const char* cstr = ostr.c_str();
        if (g.options_menu.get_selection() == i) {
            DrawText(TextFormat("> %s", cstr), x0, y0, fsize, YELLOW);
        }
        else {
            DrawText(TextFormat("  %s", cstr), x0, y0, fsize, g.window_box_fgcolor);
        }
    }
}

void draw_sound_menu(gamestate& g) {
    if (!g.display_sound_menu) {
        return;
    }

    constexpr int font_size = 12;
    constexpr int title_font_size = 18;
    constexpr int line_height = 16;
    constexpr int padding = 12;
    const int box_w = 360;
    const int box_h = 140;
    const int box_x = (DEFAULT_TARGET_WIDTH - box_w) / 2;
    const int box_y = (DEFAULT_TARGET_HEIGHT - box_h) / 2;
    const Rectangle box = {(float)box_x, (float)box_y, (float)box_w, (float)box_h};

    DrawRectangleRec(box, g.window_box_bgcolor);
    DrawRectangleLinesEx(box, 2.0f, g.window_box_fgcolor);
    DrawText("Sound", box_x + padding, box_y + padding, title_font_size, g.window_box_fgcolor);
    DrawText("Up/Down select, Left/Right adjust, Esc returns", box_x + padding, box_y + padding + 22, font_size, g.window_box_fgcolor);

    const char* labels[] = {"Master", "Music", "Sound Effects"};
    const float values[] = {g.get_master_volume(), g.get_music_volume(), g.get_sfx_volume()};
    int y = box_y + padding + 48;
    for (size_t i = 0; i < 3; i++) {
        const bool selected = g.sound_menu_selection == i;
        DrawText(selected ? TextFormat("> %-14s %3d%%", labels[i], (int)(values[i] * 100.0f))
                          : TextFormat("  %-14s %3d%%", labels[i], (int)(values[i] * 100.0f)),
            box_x + padding,
            y,
            font_size,
            selected ? YELLOW : g.window_box_fgcolor);
        y += line_height;
    }
}

void draw_window_color_menu(gamestate& g) {
    if (!g.display_window_color_menu) {
        return;
    }

    constexpr int font_size = 11;
    constexpr int title_font_size = 18;
    constexpr int line_height = 14;
    constexpr int padding = 12;
    const int box_w = 420;
    const int box_h = 220;
    const int box_x = (DEFAULT_TARGET_WIDTH - box_w) / 2;
    const int box_y = (DEFAULT_TARGET_HEIGHT - box_h) / 2;
    const Rectangle box = {(float)box_x, (float)box_y, (float)box_w, (float)box_h};

    DrawRectangleRec(box, g.window_box_bgcolor);
    DrawRectangleLinesEx(box, 2.0f, g.window_box_fgcolor);
    DrawText("Window Box Colors", box_x + padding, box_y + padding, title_font_size, g.window_box_fgcolor);
    DrawText("Up/Down select, Left/Right adjust, Enter reset, Esc returns", box_x + padding, box_y + padding + 22, font_size, g.window_box_fgcolor);

    const unsigned char values[] = {
        g.window_box_bgcolor.r, g.window_box_bgcolor.g, g.window_box_bgcolor.b, g.window_box_bgcolor.a,
        g.window_box_fgcolor.r, g.window_box_fgcolor.g, g.window_box_fgcolor.b, g.window_box_fgcolor.a
    };
    const char* labels[] = {
        "Background R", "Background G", "Background B", "Background A",
        "Foreground R", "Foreground G", "Foreground B", "Foreground A",
        "Reset Defaults"
    };

    int y = box_y + padding + 48;
    for (size_t i = 0; i < 9; i++) {
        const bool selected = g.window_color_menu_selection == i;
        const char* text = i < 8 ? TextFormat("%c %-14s %3d", selected ? '>' : ' ', labels[i], values[i])
                                 : (selected ? "> Reset Defaults" : "  Reset Defaults");
        DrawText(text, box_x + padding, y, font_size, selected ? YELLOW : g.window_box_fgcolor);
        y += line_height;
    }

    Rectangle preview = {(float)(box_x + box_w - 112), (float)(box_y + 60), 84.0f, 84.0f};
    DrawRectangleRec(preview, g.window_box_bgcolor);
    DrawRectangleLinesEx(preview, 2.0f, g.window_box_fgcolor);
    DrawText("Preview", (int)preview.x + 10, (int)preview.y + 32, font_size, g.window_box_fgcolor);
}

void draw_controls_menu(gamestate& g) {
    if (!g.display_controls_menu) {
        return;
    }

    constexpr int font_size = 10;
    constexpr int line_height = 12;
    constexpr int padding = 12;
    const int visible_rows = INPUT_ACTION_COUNT + 4;
    const int box_w = 500;
    const int box_h = visible_rows * line_height + padding * 2 + 24;
    const int box_x = (DEFAULT_TARGET_WIDTH - box_w) / 2;
    const int box_y = (DEFAULT_TARGET_HEIGHT - box_h) / 2;
    const Rectangle box = {(float)box_x, (float)box_y, (float)box_w, (float)box_h};

    DrawRectangleRec(box, g.window_box_bgcolor);
    DrawRectangleLinesEx(box, 2.0f, g.window_box_fgcolor);
    DrawText("Keyboard Controls", box_x + padding, box_y + padding, 18, g.window_box_fgcolor);
    const char* subtitle = g.controls_menu_waiting_for_key
        ? TextFormat("Press a new key for %s. Esc cancels.", gameplay_input_action_label(g.controls_menu_pending_action))
        : "Up/Down select, Enter edit, Left/Right swap profile, Esc closes";
    DrawText(subtitle, box_x + padding, box_y + padding + 20, font_size, g.window_box_fgcolor);

    int y = box_y + padding + 38;
    const bool profile_selected = g.controls_menu_selection == 0;
    DrawText(profile_selected ? TextFormat("> Profile: %s", keyboard_profile_label(g.keyboard_profile))
                              : TextFormat("  Profile: %s", keyboard_profile_label(g.keyboard_profile)),
        box_x + padding, y, font_size, profile_selected ? YELLOW : g.window_box_fgcolor);
    y += line_height;

    const bool reset_selected = g.controls_menu_selection == 1;
    DrawText(reset_selected ? "> Reset Current Profile To Defaults" : "  Reset Current Profile To Defaults",
        box_x + padding, y, font_size, reset_selected ? YELLOW : g.window_box_fgcolor);
    y += line_height;

    for (int action = 0; action < INPUT_ACTION_COUNT; action++) {
        const bool selected = g.controls_menu_selection == static_cast<size_t>(action + 2);
        const char* action_label = gameplay_input_action_label(static_cast<gameplay_input_action_t>(action));
        const string binding = g.get_keybinding_label(g.keyboard_profile, static_cast<gameplay_input_action_t>(action));
        DrawText(selected ? TextFormat("> %-22s %s", action_label, binding.c_str())
                          : TextFormat("  %-22s %s", action_label, binding.c_str()),
            box_x + padding, y, font_size, selected ? YELLOW : g.window_box_fgcolor);
        y += line_height;
    }
}

void draw_help_menu(gamestate& g) {
    const string text = TextFormat(
        "Help Menu\n"
        "\n"
        "Profile: %s\n"
        "Move Up: %s\n"
        "Move Down: %s\n"
        "Move Left / Right: %s / %s\n"
        "Move Diagonals: %s %s %s %s\n"
        "Face Mode: %s\n"
        "Face Attack: %s\n"
        "Camera mode: %s\n"
        "Zoom: %s and %s\n"
        "\n"
        "Pick up item: %s\n"
        "Attack: %s\n"
        "Pull: %s\n"
        "Open door / chest: %s\n"
        "Interact / examine: %s\n"
        "Use stairs: %s\n"
        "Toggle full light: %s\n"
        "Level-up select: arrows + enter\n"
        "\n"
        "Inventory: %s\n"
        "Inventory equip/use: e or enter\n"
        "Inventory drop: q\n"
        "Inventory close: esc or inventory key\n"
        "\n"
        "Options: %s\n"
        "Open this help menu: %s\n"
        "Debug panel: p\n"
        "Quit prompt: esc\n"
        "\n"
        "@evildojo666",
        keyboard_profile_label(g.keyboard_profile),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_MOVE_UP).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_MOVE_DOWN).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_MOVE_LEFT).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_MOVE_RIGHT).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_MOVE_UP_LEFT).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_MOVE_UP_RIGHT).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_MOVE_DOWN_LEFT).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_MOVE_DOWN_RIGHT).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_DIRECTION_MODE).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_FACE_ATTACK).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_CAMERA_TOGGLE).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_ZOOM_IN).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_ZOOM_OUT).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_PICKUP).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_ATTACK).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_PULL).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_OPEN).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_INTERACT).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_STAIRS).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_TOGGLE_FULL_LIGHT).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_INVENTORY).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_OPTIONS).c_str(),
        g.get_keybinding_label(g.keyboard_profile, INPUT_ACTION_HELP).c_str());

    constexpr int font_size = 20;
    constexpr int line_spacing = 4;
    const int padding = DEFAULT_PAD * 2;

    int max_text_width = 0;
    int line_count = 0;
    size_t line_start = 0;
    while (line_start <= text.size()) {
        const size_t line_end = text.find('\n', line_start);
        const string line = line_end == string::npos ? text.substr(line_start) : text.substr(line_start, line_end - line_start);
        const int line_width = MeasureText(line.c_str(), font_size);
        if (line_width > max_text_width) {
            max_text_width = line_width;
        }
        line_count++;
        if (line_end == string::npos) {
            break;
        }
        line_start = line_end + 1;
    }

    const int text_height = line_count * font_size + (line_count - 1) * line_spacing;
    const int box_width = max_text_width + padding * 2;
    const int box_height = text_height + padding * 2;
    const int box_x = (DEFAULT_TARGET_WIDTH - box_width) / 2;
    const int box_y = (DEFAULT_TARGET_HEIGHT - box_height) / 2;

    DrawRectangle(box_x, box_y, box_width, box_height, g.window_box_bgcolor);
    DrawRectangleLinesEx({(float)box_x, (float)box_y, (float)box_width, (float)box_height}, 2, g.window_box_fgcolor);

    const int text_x = box_x + padding;
    int text_y = box_y + padding;
    line_start = 0;
    while (line_start <= text.size()) {
        const size_t line_end = text.find('\n', line_start);
        const string line = line_end == string::npos ? text.substr(line_start) : text.substr(line_start, line_end - line_start);
        DrawText(line.c_str(), text_x, text_y, font_size, g.window_box_fgcolor);
        text_y += font_size + line_spacing;
        if (line_end == string::npos) {
            break;
        }
        line_start = line_end + 1;
    }
}

void draw_keyboard_profile_prompt(gamestate& g) {
    if (!g.display_keyboard_profile_prompt) {
        return;
    }

    const int box_w = 420;
    const int box_h = 150;
    const int box_x = (DEFAULT_TARGET_WIDTH - box_w) / 2;
    const int box_y = (DEFAULT_TARGET_HEIGHT - box_h) / 2;
    const Rectangle box = {(float)box_x, (float)box_y, (float)box_w, (float)box_h};

    DrawRectangleRec(box, g.window_box_bgcolor);
    DrawRectangleLinesEx(box, 2.0f, g.window_box_fgcolor);
    DrawText("Choose Keyboard Layout", box_x + 16, box_y + 14, 20, g.window_box_fgcolor);
    DrawText("Select a default gameplay profile before entering the dungeon.", box_x + 16, box_y + 42, 10, g.window_box_fgcolor);
    DrawText("You can change this later in ` -> controls.", box_x + 16, box_y + 56, 10, g.window_box_fgcolor);

    for (int i = 0; i < KEYBOARD_PROFILE_COUNT; i++) {
        const bool selected = g.keyboard_profile_selection == (unsigned int)i;
        const int option_y = box_y + 88 + i * 18;
        const char* label = keyboard_profile_label(static_cast<keyboard_profile_t>(i));
        DrawText(selected ? TextFormat("> %s", label) : TextFormat("  %s", label), box_x + 24, option_y, 12, selected ? YELLOW : g.window_box_fgcolor);
    }

    DrawText("Arrows to choose, Enter to confirm", box_x + 16, box_y + box_h - 20, 10, g.window_box_fgcolor);
}

void draw_entity_sprite(gamestate& g, spritegroup* sg) {
    (void)g;
    massert(sg, "spritegroup is NULL");
    Rectangle dest = {sg->dest.x, sg->dest.y, sg->dest.width, sg->dest.height};
    massert(dest.width > 0, "dest.width is 0");
    massert(dest.height > 0, "dest.height is 0");
    shared_ptr<sprite> s = sg->get_current();
    massert(s, "sprite is NULL");
    DrawTexturePro(*s->get_texture(), s->get_src(), dest, Vector2{0, 0}, 0, WHITE);
#ifdef ENTITY_BORDER
    DrawRectangleLinesEx(dest, 1, Color{255, 0, 0, 255});
#endif
}

void draw_weapon_sprite_back(gamestate& g, entityid id, spritegroup* sg) {
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(sg, "spritegroup is NULL");
    auto weapon_back_s = get_weapon_back_sprite(g, id, sg);
    if (weapon_back_s) {
        DrawTexturePro(*weapon_back_s->get_texture(), weapon_back_s->get_src(), sg->dest, Vector2{0, 0}, 0, WHITE);
    }
}

void draw_weapon_sprite_front(gamestate& g, entityid id, spritegroup* sg) {
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(sg, "spritegroup is NULL");
    auto weapon_front_s = get_weapon_front_sprite(g, id, sg);
    if (weapon_front_s) {
        DrawTexturePro(*weapon_front_s->get_texture(), weapon_front_s->get_src(), sg->dest, Vector2{0, 0}, 0, WHITE);
    }
}

void draw_shield_sprite_back(gamestate& g, entityid id, spritegroup* sg) {
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(sg, "spritegroup is NULL");
    auto shield_back_s = get_shield_back_sprite(g, id, sg);
    if (shield_back_s) {
        DrawTexturePro(*shield_back_s->get_texture(), shield_back_s->get_src(), sg->dest, Vector2{0, 0}, 0, WHITE);
    }
    else {
        merror3("no shield back sprite");
    }
}

void draw_shield_sprite_front(gamestate& g, entityid id, spritegroup* sg) {
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(sg, "spritegroup is NULL");
    auto shield_front_s = get_shield_front_sprite(g, id, sg);
    if (shield_front_s) {
        DrawTexturePro(*shield_front_s->get_texture(), shield_front_s->get_src(), sg->dest, Vector2{0, 0}, 0, WHITE);
    }
    else {
        merror3("no shield front sprite");
    }
}

void draw_sprite_and_shadow(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "id is invalid");
    massert(libdraw_ctx.spritegroups.find(id) != libdraw_ctx.spritegroups.end(), "NO SPRITE GROUP FOR ID %d", id);
    spritegroup* sg = libdraw_ctx.spritegroups[id];
    massert(sg, "sg is NULL");
    draw_shield_sprite_back(g, id, sg);
    draw_weapon_sprite_back(g, id, sg);
    draw_entity_sprite(g, sg);
    draw_shield_sprite_front(g, id, sg);
    draw_weapon_sprite_front(g, id, sg);
}

void draw_message_box(gamestate& g) {
    if (!g.display_confirm_prompt && (!g.msg_system_is_active || g.msg_system.size() == 0)) {
        return;
    }

    constexpr int w = DEFAULT_TARGET_WIDTH;
    constexpr int font_size = DEFAULT_MSG_WINDOW_FONT_SIZE;
    constexpr float text_height = font_size;
    constexpr float y = (DEFAULT_TARGET_HEIGHT - text_height) / 2.0 - DEFAULT_PAD;

    const string msg = g.display_confirm_prompt ? g.confirm_prompt_message : g.msg_system.at(0);
    char tmp[1024] = {0};
    snprintf(tmp, sizeof(tmp), "%s", msg.c_str());

    const int measure = MeasureText(tmp, font_size);
    const float text_width = measure;
    const float x = (w - text_width) / 2.0 - DEFAULT_PAD;
    const float box_width = text_width + DEFAULT_PAD * 2;
    const float box_height = text_height + g.pad * 2;
    const Rectangle box = {x, y, box_width, box_height};

    DrawRectangleRec(box, g.window_box_bgcolor);
    DrawRectangleLinesEx(box, 1, g.window_box_fgcolor);

    const int text_x = box.x + g.pad;
    const int text_y = box.y + g.pad;
    DrawText(tmp, text_x, text_y, font_size, g.window_box_fgcolor);
}

void draw_message_history(gamestate& g) {
    char tmp[1024] = {0};
    const int font_size = DEFAULT_MSG_HISTORY_FONT_SIZE;
    constexpr int max_messages = 30;
    const int msg_count = g.msg_history.size();
    if (msg_count == 0) {
        return;
    }
    const int max_measure = g.msg_history_max_len_msg_measure;

    const float w = max_measure + g.pad;
    const float h = (font_size + 2) * std::min(msg_count, max_messages) + g.pad;
    const float x = g.targetwidth - w;
    constexpr float y = 0;
    const Rectangle box = {x, y, w, h};
    DrawRectangleRec(box, g.window_box_bgcolor);
    DrawRectangleLinesEx(box, 1, g.window_box_fgcolor);

    if (msg_count > max_messages) {
        int outer_count = 0;
        for (int i = msg_count - max_messages; i < msg_count; i++) {
            const string msg = g.msg_history.at(i);
            bzero(tmp, 1024);
            snprintf(tmp, sizeof(tmp), "%s", msg.c_str());
            const float msg_x = box.x + g.pad / 2.0;
            const float msg_y = box.y + g.pad / 2.0 + (outer_count * (font_size + 2));
            DrawText(tmp, msg_x, msg_y, font_size, g.window_box_fgcolor);
            outer_count++;
        }
        return;
    }
    for (int i = 0; i < msg_count; i++) {
        const string msg = g.msg_history.at(i);
        bzero(tmp, 1024);
        snprintf(tmp, sizeof(tmp), "%s", msg.c_str());
        const float msg_x = box.x + g.pad / 2.0;
        const float msg_y = box.y + g.pad / 2.0 + (i * (font_size + 2));
        DrawText(tmp, msg_x, msg_y, font_size, g.window_box_fgcolor);
    }
}

void draw_look_panel(gamestate& g) {
    auto loc = g.ct.get<location>(g.hero_id).value_or((vec3){-1, -1, -1});
    auto df = g.d.get_floor(loc.z);
    tile_t& tile = df->tile_at(loc);
    const int entity_count = tile.entity_count() - 1;
    const int texts_size = 7;
    const char* dummy_text = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    const int font_size = DEFAULT_LOOK_PANEL_FONT_SIZE;
    const int pad_w = 10;
    const int pad_h = 10;
    const int m = MeasureText(dummy_text, font_size);
    const float w = m + pad_w * 2;
    const float h = font_size * texts_size + pad_h * (texts_size + 2);
    const float base_x = 10;
    const float base_y = g.targetheight - h;
    Rectangle r = {base_x, base_y, w, h};

    DrawRectanglePro(r, Vector2{0, 0}, 0.0f, g.window_box_bgcolor);
    DrawRectangleLinesEx(r, 1, g.window_box_fgcolor);
    DrawText("Look Panel", r.x + pad_w, r.y + pad_h + (font_size + 5) * 0, font_size, g.window_box_fgcolor);
    DrawText(TextFormat("There are %d things here", entity_count), r.x + pad_w, r.y + pad_h + (font_size + 5) * 1, font_size, g.window_box_fgcolor);
    DrawText(TextFormat("living npc %d", tile.get_cached_live_npc()), r.x + pad_w, r.y + pad_h + (font_size + 5) * 2, font_size, g.window_box_fgcolor);
    DrawText(TextFormat("dead npc %d", tile.get_cached_dead_npc()), r.x + pad_w, r.y + pad_h + (font_size + 5) * 3, font_size, g.window_box_fgcolor);
    DrawText(TextFormat("items %d (%d)", (int)tile.get_item_count(), tile.get_cached_item()), r.x + pad_w, r.y + pad_h + (font_size + 5) * 4, font_size, g.window_box_fgcolor);
    DrawText(TextFormat("door %d", tile.get_cached_door()), r.x + pad_w, r.y + pad_h + (font_size + 5) * 5, font_size, g.window_box_fgcolor);
    DrawText(TextFormat("box %d", tile.get_cached_box()), r.x + pad_w, r.y + pad_h + (font_size + 5) * 6, font_size, g.window_box_fgcolor);
}

void draw_interaction_modal(gamestate& g) {
    if (!g.display_interaction_modal || g.interaction_body.empty()) {
        return;
    }

    constexpr int title_font_size = 24;
    constexpr int body_font_size = DEFAULT_MSG_WINDOW_FONT_SIZE;
    constexpr int line_spacing = 6;
    constexpr int padding = DEFAULT_PAD;
    constexpr int min_width = 360;
    vector<string> lines;
    if (!g.interaction_title.empty()) {
        lines.push_back(g.interaction_title);
    }

    size_t line_start = 0;
    while (line_start <= g.interaction_body.size()) {
        const size_t line_end = g.interaction_body.find('\n', line_start);
        const string line = line_end == string::npos ? g.interaction_body.substr(line_start) : g.interaction_body.substr(line_start, line_end - line_start);
        lines.push_back(line);
        if (line_end == string::npos) {
            break;
        }
        line_start = line_end + 1;
    }

    int max_text_width = 0;
    for (size_t i = 0; i < lines.size(); i++) {
        const int font_size = i == 0 && !g.interaction_title.empty() ? title_font_size : body_font_size;
        max_text_width = std::max(max_text_width, MeasureText(lines[i].c_str(), font_size));
    }

    const int box_width = std::max(min_width, max_text_width + padding * 2);
    const int title_height = g.interaction_title.empty() ? 0 : title_font_size;
    const int body_lines = static_cast<int>(lines.size()) - static_cast<int>(!g.interaction_title.empty());
    const int body_height = body_lines > 0 ? body_lines * body_font_size + (body_lines - 1) * line_spacing : 0;
    const int section_gap = (!g.interaction_title.empty() && body_lines > 0) ? line_spacing + 4 : 0;
    const int box_height = padding * 2 + title_height + body_height + section_gap;
    const int box_x = (DEFAULT_TARGET_WIDTH - box_width) / 2;
    const int box_y = (DEFAULT_TARGET_HEIGHT - box_height) / 2;

    DrawRectangle(box_x, box_y, box_width, box_height, g.window_box_bgcolor);
    DrawRectangleLinesEx(Rectangle{static_cast<float>(box_x), static_cast<float>(box_y), static_cast<float>(box_width), static_cast<float>(box_height)}, 2, g.window_box_fgcolor);

    int text_y = box_y + padding;
    if (!g.interaction_title.empty()) {
        const int title_width = MeasureText(g.interaction_title.c_str(), title_font_size);
        const int title_x = box_x + (box_width - title_width) / 2;
        DrawText(g.interaction_title.c_str(), title_x, text_y, title_font_size, g.window_box_fgcolor);
        text_y += title_font_size + section_gap;
    }

    const int body_x = box_x + padding;
    for (size_t i = g.interaction_title.empty() ? 0 : 1; i < lines.size(); i++) {
        DrawText(lines[i].c_str(), body_x, text_y, body_font_size, g.window_box_fgcolor);
        text_y += body_font_size + line_spacing;
    }
}

void draw_level_up_modal(gamestate& g) {
    if (!g.display_level_up_modal) {
        return;
    }

    constexpr int title_font_size = 26;
    constexpr int body_font_size = 18;
    constexpr int cell_font_size = 20;
    constexpr int padding = DEFAULT_PAD;
    constexpr int cols = 2;
    constexpr int rows = 3;
    constexpr int cell_w = 200;
    constexpr int cell_h = 52;
    constexpr int cell_gap = 12;

    const std::array<const char*, 6> labels = {
        "Strength",
        "Dexterity",
        "Constitution",
        "Intelligence",
        "Wisdom",
        "Charisma",
    };

    const int grid_w = cols * cell_w + (cols - 1) * cell_gap;
    const int grid_h = rows * cell_h + (rows - 1) * cell_gap;
    const int box_w = grid_w + padding * 2;
    const int box_h = padding * 2 + title_font_size + body_font_size + 20 + grid_h;
    const int box_x = (DEFAULT_TARGET_WIDTH - box_w) / 2;
    const int box_y = (DEFAULT_TARGET_HEIGHT - box_h) / 2;

    DrawRectangle(box_x, box_y, box_w, box_h, g.window_box_bgcolor);
    DrawRectangleLinesEx(Rectangle{static_cast<float>(box_x), static_cast<float>(box_y), static_cast<float>(box_w), static_cast<float>(box_h)}, 2, g.window_box_fgcolor);

    const char* title = "Level Up";
    const char* subtitle = "Choose one attribute to permanently increase by 1";
    const int title_x = box_x + (box_w - MeasureText(title, title_font_size)) / 2;
    const int subtitle_x = box_x + (box_w - MeasureText(subtitle, body_font_size)) / 2;
    DrawText(title, title_x, box_y + padding, title_font_size, g.window_box_fgcolor);
    DrawText(subtitle, subtitle_x, box_y + padding + title_font_size + 8, body_font_size, g.window_box_fgcolor);

    const int grid_x = box_x + padding;
    const int grid_y = box_y + padding + title_font_size + body_font_size + 20;
    for (int i = 0; i < 6; i++) {
        const int col = i % cols;
        const int row = i / cols;
        const int cell_x = grid_x + col * (cell_w + cell_gap);
        const int cell_y = grid_y + row * (cell_h + cell_gap);
        const bool selected = static_cast<int>(g.level_up_selection % 6) == i;
        const Color fill = selected ? Color{0x00, 0x88, 0x44, 220} : Color{0x11, 0x11, 0x66, 180};
        DrawRectangle(cell_x, cell_y, cell_w, cell_h, fill);
        DrawRectangleLinesEx(Rectangle{static_cast<float>(cell_x), static_cast<float>(cell_y), static_cast<float>(cell_w), static_cast<float>(cell_h)}, 2, g.window_box_fgcolor);

        const int text_w = MeasureText(labels[i], cell_font_size);
        const int text_x = cell_x + (cell_w - text_w) / 2;
        const int text_y = cell_y + (cell_h - cell_font_size) / 2;
        DrawText(labels[i], text_x, text_y, cell_font_size, g.window_box_fgcolor);
    }
}

void draw_hud_to_texture(gamestate& g) {
    BeginTextureMode(libdraw_ctx.hud_target_texture);
    draw_hud(g);
    EndTextureMode();
}

void draw_char_creation_to_texture(gamestate& g) {
    minfo3("draw char creation to texture");
    BeginTextureMode(libdraw_ctx.char_creation_target_texture);
    draw_character_creation_screen(g);
    handle_debug_panel(g);
    EndTextureMode();
    msuccess3("draw char creation to texture");
}

void draw_title_screen_to_texture(gamestate& g, bool show_menu) {
    BeginTextureMode(libdraw_ctx.title_target_texture);
    draw_title_screen(g, show_menu);
    handle_debug_panel(g);
    EndTextureMode();
}

void libdraw_drawframe_2d(gamestate& g, int vision_dist, int light_rad) {
    camera_lock_on(g);

    BeginMode2D(g.cam2d);
    ClearBackground(BLACK);
    draw_dungeon_floor(g, vision_dist, light_rad);
    draw_damage_numbers(g);
    EndMode2D();

    draw_hud(g);
    draw_look_panel(g);
    draw_message_history(g);
    draw_message_box(g);
    draw_interaction_modal(g);
    draw_level_up_modal(g);

    if (g.display_inventory_menu) {
        if (g.use_mini_inventory_menu()) {
            auto maybe_inventory = g.ct.get<inventory>(g.hero_id);
            auto items = maybe_inventory.value_or(nullptr);
            if (items) {
                draw_mini_inventory_menu(g, items, "Inventory", "E equip  Enter use  Q drop  Esc close", true);
            }
        }
        else {
            draw_inventory_menu(g);
        }
    }

    if (g.display_chest_menu) {
        if (g.use_mini_inventory_menu()) {
            const entityid source_id = g.chest_deposit_mode ? g.hero_id : g.active_chest_id;
            auto maybe_inventory = g.ct.get<inventory>(source_id);
            auto items = maybe_inventory.value_or(nullptr);
            if (items) {
                draw_mini_inventory_menu(
                    g,
                    items,
                    g.chest_deposit_mode ? "Chest Deposit" : "Chest",
                    g.chest_deposit_mode ? "Tab chest  Enter deposit  D close" : "Tab hero  Enter take  D close",
                    g.chest_deposit_mode);
            }
        }
        else {
            draw_chest_menu(g);
        }
    }

    if (g.display_action_menu) {
        draw_action_menu(g);
    }
    if (g.display_option_menu) {
        draw_option_menu(g);
    }
    if (g.display_sound_menu) {
        draw_sound_menu(g);
    }
    if (g.display_window_color_menu) {
        draw_window_color_menu(g);
    }
    if (g.display_controls_menu) {
        draw_controls_menu(g);
    }
    if (g.display_help_menu) {
        draw_help_menu(g);
    }

    draw_keyboard_profile_prompt(g);
#ifdef DEBUG
    handle_debug_panel(g);
#endif
}

void libdraw_drawframe_2d_to_texture(gamestate& g, int vision_dist, int light_rad) {
    BeginTextureMode(libdraw_ctx.main_game_target_texture);
    libdraw_drawframe_2d(g, vision_dist, light_rad);
    EndTextureMode();
}

void draw_title_screen_from_texture(gamestate& g) {
    (void)g;
    DrawTexturePro(libdraw_ctx.title_target_texture.texture, libdraw_ctx.target_src, libdraw_ctx.target_dest, Vector2{0, 0}, 0.0f, WHITE);
}

void draw_hud_from_texture(gamestate& g) {
    (void)g;
    DrawTexturePro(libdraw_ctx.hud_target_texture.texture, libdraw_ctx.target_src, libdraw_ctx.target_dest, Vector2{0, 0}, 0.0f, WHITE);
}

void draw_char_creation_from_texture(gamestate& g) {
    (void)g;
    DrawTexturePro(libdraw_ctx.char_creation_target_texture.texture, libdraw_ctx.target_src, libdraw_ctx.target_dest, Vector2{0, 0}, 0.0f, WHITE);
}

void libdraw_drawframe_2d_from_texture(gamestate& g) {
    (void)g;
    DrawTexturePro(libdraw_ctx.main_game_target_texture.texture, libdraw_ctx.target_src, libdraw_ctx.target_dest, Vector2{0, 0}, 0.0f, WHITE);
}

bool libdraw_draw_player_target_box(gamestate& g) {
    entityid id = g.hero_id;
    if (id == -1) {
        return false;
    }
    direction_t dir = g.ct.get<direction>(id).value();
    vec3 loc = g.ct.get<location>(id).value();
    float x = loc.x + get_x_from_dir(dir);
    float y = loc.y + get_y_from_dir(dir);
    float w = DEFAULT_TILE_SIZE;
    float h = DEFAULT_TILE_SIZE;
    float a = 0.75f;
    if (g.player_changing_dir) {
        a = 0.9f;
    }
    float time = (float)GetTime();
    SetShaderValue(libdraw_ctx.shaders[1], GetShaderLocation(libdraw_ctx.shaders[1], "time"), &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(libdraw_ctx.shaders[1], GetShaderLocation(libdraw_ctx.shaders[1], "alpha"), &a, SHADER_UNIFORM_FLOAT);
    BeginShaderMode(libdraw_ctx.shaders[1]);
    DrawRectangleLinesEx((Rectangle){x * w, y * h, w, h}, 1, Fade(GREEN, a));
    EndShaderMode();
    return true;
}

constexpr int manhattan_distance(vec3 a, vec3 b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

bool draw_dungeon_floor_tile(gamestate& g, int x, int y, int z, int light_dist, vec3 hero_loc, int distance) {
    (void)light_dist;
    (void)hero_loc;
    (void)distance;
    massert(z >= 0 && static_cast<size_t>(z) < g.d.get_floor_count(), "z is oob");
    const float px = x * DEFAULT_TILE_SIZE + DEFAULT_OFFSET;
    const float py = y * DEFAULT_TILE_SIZE + DEFAULT_OFFSET;
    const Rectangle src = {0, 0, DEFAULT_TILE_SIZE_SCALED, DEFAULT_TILE_SIZE_SCALED};
    const Rectangle dest = {px, py, DEFAULT_TILE_SIZE_FLOAT, DEFAULT_TILE_SIZE_FLOAT};
    auto df = g.d.get_floor(z);
    massert(df, "dungeon_floor is NULL");
    massert(x >= 0 && x < df->get_width(), "x is oob");
    massert(y >= 0 && y < df->get_height(), "y is oob");
    massert(!vec3_invalid(vec3{x, y, z}), "loc is invalid");
    tile_t& tile = df->tile_at(vec3{x, y, z});
    const bool full_light = df->get_full_light();
    if (tile.get_type() == TILE_NONE) {
        return true;
    }
    if (!full_light && !tile.get_explored()) {
        return true;
    }
    const int txkey = get_txkey_for_tiletype(tile.get_type());
    massert(txkey >= 0, "txkey is invalid");
    Texture2D* texture = &libdraw_ctx.txinfo[txkey].texture;
    massert(texture->id > 0, "texture->id is <= 0");
    const bool tile_visible = full_light || tile.get_visible();
    const unsigned char a = tile_visible ? 255 : 102;
    const Color draw_color = Color{255, 255, 255, a};
    DrawTexturePro(*texture, src, dest, Vector2{0, 0}, 0, draw_color);
    return true;
}

void draw_dungeon_floor_pressure_plates(gamestate& g, int light_rad) {
    auto df = g.d.get_current_floor();
    const int z = g.d.current_floor;
    const vec3 hero_loc = g.ct.get<location>(g.hero_id).value_or(vec3{-1, -1, -1});
    const bool full_light = df->get_full_light();
    const Rectangle src = {0, 0, DEFAULT_TILE_SIZE_SCALED, DEFAULT_TILE_SIZE_SCALED};

    for (const floor_pressure_plate_t& plate : g.floor_pressure_plates) {
        if (plate.destroyed || plate.loc.z != z) {
            continue;
        }

        if (!full_light) {
            if (abs(plate.loc.x - hero_loc.x) + abs(plate.loc.y - hero_loc.y) > light_rad) {
                continue;
            }

            tile_t& tile = df->tile_at(plate.loc);
            if (!tile.get_explored() || !tile.get_visible()) {
                continue;
            }
        }

        const int txkey = plate.active ? plate.txkey_down : plate.txkey_up;
        Texture2D* texture = &libdraw_ctx.txinfo[txkey].texture;
        massert(texture->id > 0, "pressure plate texture->id is <= 0");

        const float px = plate.loc.x * DEFAULT_TILE_SIZE + DEFAULT_OFFSET;
        const float py = plate.loc.y * DEFAULT_TILE_SIZE + DEFAULT_OFFSET;
        const Rectangle dest = {px, py, DEFAULT_TILE_SIZE_FLOAT, DEFAULT_TILE_SIZE_FLOAT};
        DrawTexturePro(*texture, src, dest, Vector2{0, 0}, 0, WHITE);
    }
}

void draw_dungeon_floor_entitytype(gamestate& g, entitytype_t type_0, int vision_dist, int light_rad, function<bool(gamestate&, entityid)> extra_check) {
    (void)vision_dist;
    auto df = g.d.get_current_floor();
    auto hero_loc = g.ct.get<location>(g.hero_id).value_or(vec3{-1, -1, -1});
    const bool full_light = df->get_full_light();
    const int min_x = full_light ? 0 : std::max(0, hero_loc.x - light_rad);
    const int max_x = full_light ? df->get_width() - 1 : std::min(df->get_width() - 1, hero_loc.x + light_rad);
    const int min_y = full_light ? 0 : std::max(0, hero_loc.y - light_rad);
    const int max_y = full_light ? df->get_height() - 1 : std::min(df->get_height() - 1, hero_loc.y + light_rad);
    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            if (!full_light && abs(x - hero_loc.x) + abs(y - hero_loc.y) > light_rad) {
                continue;
            }
            const vec3 loc = {x, y, g.d.current_floor};
            tile_t& tile = df->tile_at(loc);
            auto tiletype = tile.get_type();
            if (tiletype_is_none(tiletype) || tiletype_is_wall(tiletype)) {
                continue;
            }
            if (!full_light && (!tile.get_visible() || !tile.get_explored())) {
                continue;
            }
            if (tile.entity_count() == 0) {
                continue;
            }

            if (type_0 == ENTITY_NPC) {
                entityid dead_npc_id = tile.get_cached_dead_npc();
                if (dead_npc_id != INVALID && extra_check(g, dead_npc_id)) {
                    draw_sprite_and_shadow(g, dead_npc_id);
                }
                entityid npc_id = tile.get_cached_live_npc();
                if (npc_id != INVALID && extra_check(g, npc_id)) {
                    draw_sprite_and_shadow(g, npc_id);
                }
            }
            else if (type_0 == ENTITY_PLAYER) {
                bool player_present = tile.get_cached_player_present();
                if (player_present && extra_check(g, g.hero_id)) {
                    draw_sprite_and_shadow(g, g.hero_id);
                }
            }
            else if (type_0 == ENTITY_BOX) {
                entityid box_id = tile.get_cached_box();
                if (box_id != INVALID && extra_check(g, box_id)) {
                    draw_sprite_and_shadow(g, box_id);
                }
            }
            else if (type_0 == ENTITY_CHEST) {
                entityid chest_id = tile.get_cached_chest();
                if (chest_id != INVALID && extra_check(g, chest_id)) {
                    draw_sprite_and_shadow(g, chest_id);
                }
            }
            else if (type_0 == ENTITY_PROP) {
                entityid prop_id = tile.get_cached_prop();
                if (prop_id != INVALID && extra_check(g, prop_id)) {
                    draw_sprite_and_shadow(g, prop_id);
                }
            }
            else if (type_0 == ENTITY_ITEM) {
                entityid item_id = tile.get_cached_item();
                if (item_id != INVALID && extra_check(g, item_id)) {
                    draw_sprite_and_shadow(g, item_id);
                }
            }
            else if (type_0 == ENTITY_DOOR) {
                entityid door_id = tile.get_cached_door();
                if (door_id != INVALID && extra_check(g, door_id)) {
                    draw_sprite_and_shadow(g, door_id);
                }
            }
        }
    }
}

bool draw_dungeon_floor(gamestate& g, int vision_dist, int light_rad) {
    shared_ptr<dungeon_floor> df = g.d.get_current_floor();
    const int z = g.d.current_floor;
    auto maybe_hero_loc = g.ct.get<location>(g.hero_id);
    if (!maybe_hero_loc.has_value()) {
        return false;
    }

    const vec3 hero_loc = maybe_hero_loc.value();
    for (int y = 0; y < df->get_height(); y++) {
        for (int x = 0; x < df->get_width(); x++) {
            const vec3 loc = {x, y, z};
            const int distance = manhattan_distance(loc, hero_loc);
            draw_dungeon_floor_tile(g, x, y, z, light_rad, hero_loc, distance);
        }
    }
    draw_dungeon_floor_pressure_plates(g, light_rad);

    auto mydefault = [](gamestate& g, entityid id) { return true; };
    auto alive_check = [](gamestate& g, entityid id) {
        auto maybe_dead = g.ct.get<dead>(id);
        if (maybe_dead.has_value()) {
            return !maybe_dead.value();
        }
        return false;
    };

    auto dead_check = [](gamestate& g, entityid id) {
        auto maybe_dead = g.ct.get<dead>(id);
        if (maybe_dead.has_value()) {
            return maybe_dead.value();
        }
        return false;
    };

    draw_dungeon_floor_entitytype(g, ENTITY_DOOR, vision_dist, light_rad, mydefault);
    draw_dungeon_floor_entitytype(g, ENTITY_PROP, vision_dist, light_rad, mydefault);
    draw_dungeon_floor_entitytype(g, ENTITY_CHEST, vision_dist, light_rad, mydefault);
    libdraw_draw_player_target_box(g);
    draw_dungeon_floor_entitytype(g, ENTITY_ITEM, vision_dist, light_rad, mydefault);
    draw_dungeon_floor_entitytype(g, ENTITY_NPC, vision_dist, light_rad, dead_check);
    draw_dungeon_floor_entitytype(g, ENTITY_BOX, vision_dist, light_rad, mydefault);
    draw_dungeon_floor_entitytype(g, ENTITY_NPC, vision_dist, light_rad, alive_check);
    draw_dungeon_floor_entitytype(g, ENTITY_PLAYER, vision_dist, light_rad, mydefault);
    return true;
}

void create_npc_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");

    const race_t r = g.ct.get<race>(id).value_or(RACE_NONE);
    massert(r != RACE_NONE, "race is none for id %d", id);

    int* keys = NULL;
    int key_count = 0;

    switch (r) {
    case RACE_HUMAN: keys = TX_HUMAN_KEYS; key_count = TX_HUMAN_COUNT; break;
    case RACE_ORC: keys = TX_ORC_KEYS; key_count = TX_ORC_COUNT; break;
    case RACE_ELF: keys = TX_ELF_KEYS; key_count = TX_ELF_COUNT; break;
    case RACE_DWARF: keys = TX_DWARF_KEYS; key_count = TX_DWARF_COUNT; break;
    case RACE_HALFLING: keys = TX_HALFLING_KEYS; key_count = TX_HALFLING_COUNT; break;
    case RACE_GOBLIN: keys = TX_GOBLIN_KEYS; key_count = TX_GOBLIN_COUNT; break;
    case RACE_WOLF: keys = TX_WOLF_KEYS; key_count = TX_WOLF_COUNT; break;
    case RACE_BAT: keys = TX_BAT_KEYS; key_count = TX_BAT_COUNT; break;
    case RACE_WARG: keys = TX_WARG_KEYS; key_count = TX_WARG_COUNT; break;
    case RACE_GREEN_SLIME: keys = TX_GREEN_SLIME_KEYS; key_count = TX_GREEN_SLIME_COUNT; break;
    case RACE_SKELETON: keys = TX_SKELETON_KEYS; key_count = TX_SKELETON_COUNT; break;
    case RACE_RAT: keys = TX_RAT_KEYS; key_count = TX_RAT_COUNT; break;
    case RACE_ZOMBIE: keys = TX_ZOMBIE_KEYS; key_count = TX_ZOMBIE_COUNT; break;
    default: break;
    }

    massert(keys != NULL, "keys is null");
    massert(key_count > 0, "key_count is not > 0");
    create_sg(g, id, keys, key_count);
}

void create_door_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    minfo("create_door_sg_byid: %d", id);
    create_sg(g, id, TX_WOODEN_DOOR_KEYS, TX_WOODEN_DOOR_COUNT);
}

void create_box_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    create_sg(g, id, TX_WOODEN_BOX_KEYS, TX_WOODEN_BOX_COUNT);
}

void create_chest_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    create_sg(g, id, TX_TREASURE_CHEST_KEYS, TX_TREASURE_CHEST_COUNT);
}

void create_potion_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<potiontype>(id).value_or(POTION_NONE)) {
    case POTION_HP_SMALL: create_sg(g, id, TX_POTION_HP_SMALL_KEYS, TX_POTION_HP_SMALL_COUNT); break;
    case POTION_HP_MEDIUM: create_sg(g, id, TX_POTION_HP_MEDIUM_KEYS, TX_POTION_HP_MEDIUM_COUNT); break;
    case POTION_HP_LARGE: create_sg(g, id, TX_POTION_HP_LARGE_KEYS, TX_POTION_HP_LARGE_COUNT); break;
    case POTION_MP_SMALL: create_sg(g, id, TX_POTION_MP_SMALL_KEYS, TX_POTION_MP_SMALL_COUNT); break;
    case POTION_MP_MEDIUM: create_sg(g, id, TX_POTION_MP_MEDIUM_KEYS, TX_POTION_MP_MEDIUM_COUNT); break;
    case POTION_MP_LARGE: create_sg(g, id, TX_POTION_MP_LARGE_KEYS, TX_POTION_MP_LARGE_COUNT); break;
    default: break;
    }
}

void create_weapon_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<weapontype>(id).value_or(WEAPON_NONE)) {
    case WEAPON_DAGGER: create_sg(g, id, TX_DAGGER_KEYS, TX_DAGGER_COUNT); break;
    case WEAPON_SHORT_SWORD: create_sg(g, id, TX_SHORT_SWORD_KEYS, TX_SHORT_SWORD_COUNT); break;
    case WEAPON_AXE: create_sg(g, id, TX_AXE_KEYS, TX_AXE_COUNT); break;
    default: break;
    }
}

void create_shield_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<shieldtype>(id).value_or(SHIELD_NONE)) {
    case SHIELD_BUCKLER: create_sg(g, id, TX_BUCKLER_KEYS, TX_BUCKLER_COUNT); break;
    case SHIELD_KITE: create_sg(g, id, TX_KITE_SHIELD_KEYS, TX_BUCKLER_COUNT); break;
    case SHIELD_TOWER: create_sg(g, id, TX_TOWER_SHIELD_KEYS, TX_TOWER_SHIELD_COUNT); break;
    default: break;
    }
}

void create_item_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<itemtype>(id).value_or(ITEM_NONE)) {
    case ITEM_POTION: create_potion_sg_byid(g, id); break;
    case ITEM_WEAPON: create_weapon_sg_byid(g, id); break;
    case ITEM_SHIELD: create_shield_sg_byid(g, id); break;
    default: break;
    }
}

void create_prop_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<proptype>(id).value_or(PROP_NONE)) {
    case PROP_DUNGEON_BANNER_00: create_sg(g, id, TX_PROP_DUNGEON_BANNER_00_KEYS, TX_PROP_DUNGEON_BANNER_00_COUNT); break;
    case PROP_DUNGEON_BANNER_01: create_sg(g, id, TX_PROP_DUNGEON_BANNER_01_KEYS, TX_PROP_DUNGEON_BANNER_01_COUNT); break;
    case PROP_DUNGEON_BANNER_02: create_sg(g, id, TX_PROP_DUNGEON_BANNER_02_KEYS, TX_PROP_DUNGEON_BANNER_02_COUNT); break;
    case PROP_DUNGEON_WOODEN_TABLE_00: create_sg(g, id, TX_PROP_DUNGEON_WOODEN_TABLE_00_KEYS, TX_PROP_DUNGEON_WOODEN_TABLE_00_COUNT); break;
    case PROP_DUNGEON_WOODEN_TABLE_01: create_sg(g, id, TX_PROP_DUNGEON_WOODEN_TABLE_01_KEYS, TX_PROP_DUNGEON_WOODEN_TABLE_01_COUNT); break;
    case PROP_DUNGEON_WOODEN_CHAIR_00: create_sg(g, id, TX_PROP_DUNGEON_WOODEN_CHAIR_00_KEYS, TX_PROP_DUNGEON_WOODEN_CHAIR_00_COUNT); break;
    case PROP_DUNGEON_STATUE_00: create_sg(g, id, TX_PROP_DUNGEON_STATUE_00_KEYS, TX_PROP_DUNGEON_STATUE_00_COUNT); break;
    case PROP_DUNGEON_TORCH_00: create_sg(g, id, TX_PROP_DUNGEON_TORCH_00_KEYS, TX_PROP_DUNGEON_TORCH_00_COUNT); break;
    case PROP_DUNGEON_CANDLE_00: create_sg(g, id, TX_PROP_DUNGEON_CANDLE_00_KEYS, TX_PROP_DUNGEON_CANDLE_00_COUNT); break;
    case PROP_DUNGEON_JAR_00: create_sg(g, id, TX_PROP_DUNGEON_JAR_00_KEYS, TX_PROP_DUNGEON_JAR_00_COUNT); break;
    case PROP_DUNGEON_PLATE_00: create_sg(g, id, TX_PROP_DUNGEON_PLATE_00_KEYS, TX_PROP_DUNGEON_PLATE_00_COUNT); break;
    case PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_EMPTY:
        create_sg(g, id, TX_PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_EMPTY_KEYS, TX_PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_EMPTY_COUNT);
        break;
    case PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_WATER:
        create_sg(g, id, TX_PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_WATER_KEYS, TX_PROP_DUNGEON_WOODEN_BARREL_OPEN_TOP_WATER_COUNT);
        break;
    default: break;
    }
}

void create_sg_byid(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    switch (g.ct.get<entitytype>(id).value_or(ENTITY_NONE)) {
    case ENTITY_PLAYER:
    case ENTITY_NPC: create_npc_sg_byid(g, id); break;
    case ENTITY_DOOR: create_door_sg_byid(g, id); break;
    case ENTITY_BOX: create_box_sg_byid(g, id); break;
    case ENTITY_CHEST: create_chest_sg_byid(g, id); break;
    case ENTITY_ITEM: create_item_sg_byid(g, id); break;
    case ENTITY_PROP: create_prop_sg_byid(g, id); break;
    default: break;
    }
}

void libdraw_set_sg_block_success(gamestate& g, entityid id, spritegroup* const sg) {
    minfo("set sg block success");
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");
    const race_t r = g.ct.get<race>(id).value_or(RACE_NONE);
    if (r == RACE_GREEN_SLIME) {
        minfo("setting SG_ANIM_SLIME_IDLE");
        sg->set_current(SG_ANIM_SLIME_IDLE);
    }
    else if (r == RACE_WOLF) {
        minfo("setting SG_ANIM_WOLF_IDLE");
        sg->set_current(SG_ANIM_WOLF_IDLE);
    }
    else {
        minfo("setting SG_ANIM_NPC_GUARD_SUCCESS");
        sg->set_current(SG_ANIM_NPC_GUARD_SUCCESS);
    }
    update_shield_for_entity(g, id, sg);
    g.ct.set<block_success>(id, false);
}

void libdraw_set_sg_is_damaged(gamestate& g, entityid id, spritegroup* const sg) {
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");
    const race_t r = g.ct.get<race>(id).value_or(RACE_NONE);
    const int anim_index = r == RACE_GREEN_SLIME ? SG_ANIM_SLIME_DMG : SG_ANIM_NPC_DMG;
    sg->set_current(anim_index);
}

void libdraw_set_sg_is_dead(gamestate& g, entityid id, spritegroup* const sg) {
    massert(id != ENTITYID_INVALID, "entity id is -1");
    massert(sg, "spritegroup is NULL");

    if (!g.ct.has<dead>(id)) {
        return;
    }

    if (!g.ct.get<dead>(id).value()) {
        return;
    }

    const race_t r = g.ct.get<race>(id).value();
    if (r == RACE_NONE) {
        return;
    }

    const int anim_index = r == RACE_BAT           ? SG_ANIM_BAT_DIE
                           : r == RACE_GREEN_SLIME ? SG_ANIM_SLIME_DIE
                           : r == RACE_SKELETON    ? SG_ANIM_SKELETON_DIE
                           : r == RACE_RAT         ? SG_ANIM_RAT_DIE
                           : r == RACE_ZOMBIE      ? SG_ANIM_ZOMBIE_DIE
                                                   : SG_ANIM_NPC_SPINDIE;
    if (sg->current == anim_index) {
        return;
    }

    sg->set_default_anim(anim_index);
    sg->set_current(sg->default_anim);
    sg->set_stop_on_last_frame(true);
}

void libdraw_set_sg_is_attacking(gamestate& g, entityid id, spritegroup* const sg) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");
    const race_t r = g.ct.get<race>(id).value_or(RACE_NONE);
    massert(r != RACE_NONE, "race cant be none");
    sg->set_current(
        r == RACE_GREEN_SLIME ? SG_ANIM_SLIME_JUMP_ATTACK
        : r == RACE_SKELETON  ? SG_ANIM_SKELETON_ATTACK
        : r == RACE_RAT       ? SG_ANIM_RAT_ATTACK
        : r == RACE_ZOMBIE    ? SG_ANIM_ZOMBIE_ATTACK
        : r == RACE_BAT       ? SG_ANIM_BAT_ATTACK
                              : SG_ANIM_NPC_ATTACK);
    update_weapon_for_entity(g, id, sg);
    g.ct.set<attacking>(id, false);
}

void libdraw_update_sprite_context_ptr(gamestate& g, spritegroup* group, direction_t dir) {
    massert(group != NULL, "group is NULL");
    int old_ctx = group->sprites2->at(group->current)->get_currentcontext();
    int ctx = old_ctx;
    ctx = dir == DIR_NONE                                      ? old_ctx
          : dir == DIR_DOWN_RIGHT                              ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_DOWN_LEFT                               ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_UP_RIGHT                                ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_UP_LEFT                                 ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIR_DOWN && ctx == SPRITEGROUP_CONTEXT_R_D  ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_DOWN && ctx == SPRITEGROUP_CONTEXT_L_D  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_DOWN && ctx == SPRITEGROUP_CONTEXT_R_U  ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_DOWN && ctx == SPRITEGROUP_CONTEXT_L_U  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_UP && ctx == SPRITEGROUP_CONTEXT_R_D    ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_UP && ctx == SPRITEGROUP_CONTEXT_L_D    ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIR_UP && ctx == SPRITEGROUP_CONTEXT_R_U    ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_UP && ctx == SPRITEGROUP_CONTEXT_L_U    ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIR_RIGHT && ctx == SPRITEGROUP_CONTEXT_R_D ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_RIGHT && ctx == SPRITEGROUP_CONTEXT_L_D ? SPRITEGROUP_CONTEXT_R_D
          : dir == DIR_RIGHT && ctx == SPRITEGROUP_CONTEXT_R_U ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_RIGHT && ctx == SPRITEGROUP_CONTEXT_L_U ? SPRITEGROUP_CONTEXT_R_U
          : dir == DIR_LEFT && ctx == SPRITEGROUP_CONTEXT_R_D  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_LEFT && ctx == SPRITEGROUP_CONTEXT_L_D  ? SPRITEGROUP_CONTEXT_L_D
          : dir == DIR_LEFT && ctx == SPRITEGROUP_CONTEXT_R_U  ? SPRITEGROUP_CONTEXT_L_U
          : dir == DIR_LEFT && ctx == SPRITEGROUP_CONTEXT_L_U  ? SPRITEGROUP_CONTEXT_L_U
                                                               : old_ctx;
    if (ctx != old_ctx) {
        g.frame_dirty = true;
    }
    group->setcontexts(ctx);
}

void libdraw_update_sprite_position(gamestate& g, entityid id, spritegroup* sg) {
    massert(sg, "spritegroup is NULL");
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    auto maybe_sprite_move = g.ct.get<spritemove>(id);
    if (!maybe_sprite_move.has_value()) {
        return;
    }
    Rectangle sprite_move = g.ct.get<spritemove>(id).value();
    if (sprite_move.x != 0 || sprite_move.y != 0) {
        sg->move.x = sprite_move.x;
        sg->move.y = sprite_move.y;
        g.ct.set<spritemove>(id, (Rectangle){0, 0, 0, 0});
        entitytype_t type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
        massert(type != ENTITY_NONE, "entity type is none");
        if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
            race_t r = g.ct.get<race>(id).value_or(RACE_NONE);
            if (r == RACE_BAT) {
                sg->current = SG_ANIM_BAT_IDLE;
            }
            else if (r == RACE_GREEN_SLIME) {
                sg->current = SG_ANIM_SLIME_IDLE;
            }
            else {
                sg->current = SG_ANIM_NPC_WALK;
            }
        }
        g.frame_dirty = true;
    }
}

void libdraw_update_sprite_ptr(gamestate& g, entityid id, spritegroup* sg) {
    minfo3("Begin update sprite ptr: %d", id);
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    massert(sg, "spritegroup is NULL");
    massert(g.ct.has<update>(id), "id %d has no update component, name %s", id, g.ct.get<name>(id).value().c_str());

    const bool do_update = g.ct.get<update>(id).value();

    if (do_update) {
        if (g.ct.has<direction>(id)) {
            const direction_t d = g.ct.get<direction>(id).value();
            libdraw_update_sprite_context_ptr(g, sg, d);
        }
        g.ct.set<update>(id, false);
    }

    libdraw_update_sprite_position(g, id, sg);

    if (g.ct.get<block_success>(id).value_or(false)) {
        libdraw_set_sg_block_success(g, id, sg);
    }

    if (g.ct.get<attacking>(id).value_or(false)) {
        libdraw_set_sg_is_attacking(g, id, sg);
    }

    const entitytype_t type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);

    if (g.ct.get<dead>(id).value_or(false)) {
        libdraw_set_sg_is_dead(g, id, sg);
    }
    else if (g.ct.get<damaged>(id).value_or(false)) {
        libdraw_set_sg_is_damaged(g, id, sg);
    }

    if (type == ENTITY_DOOR || type == ENTITY_CHEST) {
        auto maybe_door_open = g.ct.get<door_open>(id);
        if (maybe_door_open.has_value()) {
            sg->set_current(maybe_door_open.value() ? 1 : 0);
        }
    }

    if (sg->update_dest()) {
        g.frame_dirty = true;
    }

    auto maybe_loc = g.ct.get<location>(id);
    if (!maybe_loc.has_value()) {
        return;
    }
    const vec3 loc = maybe_loc.value();
    sg->snap_dest(loc.x, loc.y);
}

void libdraw_update_sprite_pre(gamestate& g, entityid id) {
    massert(id != ENTITYID_INVALID, "entityid is invalid");
    if (libdraw_ctx.spritegroups.find(id) != libdraw_ctx.spritegroups.end()) {
        libdraw_update_sprite_ptr(g, id, libdraw_ctx.spritegroups[id]);
    }
}

void libdraw_handle_dirty_entities(gamestate& g) {
    if (!g.dirty_entities) {
        return;
    }
    for (entityid i = g.new_entityid_begin; i < g.new_entityid_end; i++) {
        create_sg_byid(g, i);
        libdraw_update_sprite_pre(g, i);
    }
    g.dirty_entities = false;
    g.new_entityid_begin = ENTITYID_INVALID;
    g.new_entityid_end = ENTITYID_INVALID;
    g.frame_dirty = true;
}

void libdraw_update_sprites_pre(gamestate& g) {
    minfo2("BEGIN update sprites pre");
    handle_music_stream(g);
    if (g.current_scene == SCENE_GAMEPLAY) {
        libdraw_handle_dirty_entities(g);
        for (entityid id = 0; id < g.next_entityid; id++) {
            libdraw_update_sprite_pre(g, id);
        }
    }
    msuccess2("END update sprites pre");
}

void libdraw_update_sprites_post(gamestate& g) {
    if (g.current_scene != SCENE_GAMEPLAY) {
        g.frame_dirty = false;
        return;
    }

    if (g.framecount % libdraw_ctx.anim_speed != 0) {
        libdraw_handle_gamestate_flag(g);
        return;
    }

    libdraw_handle_dirty_entities(g);
    g.frame_dirty = true;

    for (entityid id = 0; id < g.next_entityid; id++) {
        const entitytype_t type = g.ct.get<entitytype>(id).value_or(ENTITY_NONE);
        if (type == ENTITY_NONE) {
            continue;
        }

        spritegroup* sg = libdraw_ctx.spritegroups[id];
        if (!sg) {
            continue;
        }

        auto s = sg->sprites2->at(sg->current);
        if (!s) {
            continue;
        }

        s->incr_frame();

        if (type == ENTITY_PLAYER || type == ENTITY_NPC) {
            if (s->get_num_loops() >= 1) {
                sg->current = sg->default_anim;
                s->set_num_loops(0);
            }
        }
        else if (type == ENTITY_ITEM) {
            const itemtype_t itype = g.ct.get<itemtype>(id).value_or(ITEM_NONE);
            switch (itype) {
            case ITEM_WEAPON: {
                if (sg->current == 0) {
                    break;
                }
                auto s2 = sg->sprites2->at(sg->current + 1);
                if (!s2) {
                    break;
                }
                s2->incr_frame();
                g.frame_dirty = true;
                if (s2->get_num_loops() >= 1) {
                    sg->current = sg->default_anim;
                    s2->set_num_loops(0);
                }
            } break;
            case ITEM_SHIELD: {
                if (sg->current == 0) {
                    break;
                }
                minfo("shield appears to be equipped...");
                auto s2 = sg->sprites2->at(sg->current + 1);
                if (!s2) {
                    break;
                }
                s2->incr_frame();
                g.frame_dirty = true;
                if (s2->get_num_loops() >= 1) {
                    sg->current = sg->default_anim;
                    s2->set_num_loops(0);
                }
            } break;
            default: break;
            }
        }
    }
    libdraw_handle_gamestate_flag(g);
}

void libdraw_render_current_scene_to_scene_texture(gamestate& g) {
    switch (g.current_scene) {
    case SCENE_TITLE:
        draw_title_screen_to_texture(g, false);
        break;
    case SCENE_MAIN_MENU:
        draw_title_screen_to_texture(g, true);
        break;
    case SCENE_CHARACTER_CREATION:
        minfo3("draw character creation scene to texture");
        draw_char_creation_to_texture(g);
        break;
    case SCENE_GAMEPLAY: {
        const int vision_dist = g.ct.get<vision_distance>(g.hero_id).value_or(0);
        const int light_rad = g.ct.get<light_radius>(g.hero_id).value_or(0);
        libdraw_drawframe_2d_to_texture(g, vision_dist, light_rad);
    } break;
    default:
        break;
    }
}

void libdraw_draw_current_scene_from_scene_texture(gamestate& g) {
    switch (g.current_scene) {
    case SCENE_TITLE:
    case SCENE_MAIN_MENU:
        draw_title_screen_from_texture(g);
        break;
    case SCENE_CHARACTER_CREATION:
        draw_char_creation_from_texture(g);
        break;
    case SCENE_GAMEPLAY:
        libdraw_drawframe_2d_from_texture(g);
        break;
    default:
        break;
    }
}

void libdraw_refresh_dirty_scene(gamestate& g) {
    if (!g.frame_dirty) {
        return;
    }

    minfo3("frame is dirty");
    libdraw_render_current_scene_to_scene_texture(g);
    g.frame_dirty = false;
    g.frame_updates++;
    msuccess3("frame is no longer dirty");
}

void libdraw_compose_scene_to_window_target(gamestate& g) {
    BeginTextureMode(libdraw_ctx.target);
    ClearBackground(BLUE);
    libdraw_draw_current_scene_from_scene_texture(g);
    EndTextureMode();
}

void libdraw_present_window_target(gamestate& g) {
    (void)g;
    libdraw_ctx.win_dest.width = GetScreenWidth();
    libdraw_ctx.win_dest.height = GetScreenHeight();
    DrawTexturePro(libdraw_ctx.target.texture, libdraw_ctx.target_src, libdraw_ctx.win_dest, Vector2{0, 0}, 0.0f, WHITE);
    DrawFPS(10, 10);
}

void drawframe(gamestate& g) {
    minfo3("drawframe");
    const double start_time = libdraw_frame_begin_time();
    libdraw_update_sprites_pre(g);

    BeginDrawing();
    ClearBackground(RED);
    libdraw_refresh_dirty_scene(g);
    libdraw_compose_scene_to_window_target(g);
    libdraw_present_window_target(g);
    EndDrawing();
    libdraw_finish_frame_stats(g, start_time);

    libdraw_update_sprites_post(g);
    msuccess3("drawframe");
}

void libdraw_init_render_targets(gamestate& g) {
    g.windowwidth = DEFAULT_WIN_WIDTH;
    g.windowheight = DEFAULT_WIN_HEIGHT;
    g.targetwidth = DEFAULT_TARGET_WIDTH;
    g.targetheight = DEFAULT_TARGET_HEIGHT;

    const TextureFilter filter = TEXTURE_FILTER_POINT;
    libdraw_ctx.target = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    libdraw_ctx.title_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    libdraw_ctx.char_creation_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    libdraw_ctx.main_game_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);
    libdraw_ctx.hud_target_texture = LoadRenderTexture(DEFAULT_TARGET_WIDTH, DEFAULT_TARGET_HEIGHT);

    SetTextureFilter(libdraw_ctx.target.texture, filter);
    SetTextureFilter(libdraw_ctx.title_target_texture.texture, filter);
    SetTextureFilter(libdraw_ctx.char_creation_target_texture.texture, filter);
    SetTextureFilter(libdraw_ctx.main_game_target_texture.texture, filter);
    SetTextureFilter(libdraw_ctx.hud_target_texture.texture, filter);

    libdraw_ctx.target_src = Rectangle{0, 0, DEFAULT_TARGET_WIDTH * 1.0f, -DEFAULT_TARGET_HEIGHT * 1.0f};
    libdraw_ctx.target_dest = Rectangle{0, 0, DEFAULT_TARGET_WIDTH * 1.0f, DEFAULT_TARGET_HEIGHT * 1.0f};
}

void libdraw_init_resources(gamestate& g) {
    load_textures(libdraw_ctx.txinfo);
    load_shaders();

    const float x = DEFAULT_TARGET_WIDTH / 4.0f;
    const float y = DEFAULT_TARGET_HEIGHT / 4.0f;
    g.cam2d.offset = Vector2{x, y};

    draw_title_screen_to_texture(g, false);
    draw_char_creation_to_texture(g);

    InitAudioDevice();
    while (!IsAudioDeviceReady()) {
    }
    libdraw_load_music(g);
    libdraw_load_sfx(g);
}

void libdraw_init_rest(gamestate& g) {
    SetExitKey(KEY_ESCAPE);
    SetTargetFPS(60);
    libdraw_init_render_targets(g);
    libdraw_init_resources(g);
}

void libdraw_init(gamestate& g) {
    const int w = DEFAULT_WIN_WIDTH;
    const int h = DEFAULT_WIN_HEIGHT;
    const char* title = WINDOW_TITLE;
    char full_title[1024] = {0};
    snprintf(full_title, sizeof(full_title), "%s - %s", title, g.version.c_str());
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(w, h, full_title);
    g.windowwidth = w;
    g.windowheight = h;
    SetWindowMinSize(320, 240);
    libdraw_init_rest(g);
}

bool libdraw_windowshouldclose(gamestate& g) {
    return g.do_quit;
}

void libdraw_close_partial() {
    UnloadMusicStream(libdraw_ctx.audio.music);
    CloseAudioDevice();
    libdraw_unload_textures(libdraw_ctx.txinfo);
    unload_shaders();
    UnloadRenderTexture(libdraw_ctx.title_target_texture);
    UnloadRenderTexture(libdraw_ctx.char_creation_target_texture);
    UnloadRenderTexture(libdraw_ctx.main_game_target_texture);
    UnloadRenderTexture(libdraw_ctx.hud_target_texture);
    UnloadRenderTexture(libdraw_ctx.target);
}

void libdraw_close() {
    libdraw_close_partial();
    CloseWindow();
}
