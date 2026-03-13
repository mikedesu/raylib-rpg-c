#pragma once

#include "../draw_damage_numbers.h"
#include "../get_txkey_for_tiletype.h"
#include "../spritegroup.h"
#include <cxxtest/TestSuite.h>

class RendererSeamsTestSuite : public CxxTest::TestSuite {
private:
    Texture2D make_texture(int width, int height) {
        Texture2D texture = {};
        texture.width = width;
        texture.height = height;
        return texture;
    }

public:
    void testGetTxkeyForRepresentativeTileTypes() {
        TS_ASSERT_EQUALS(get_txkey_for_tiletype(TILE_NONE), -1);
        TS_ASSERT_EQUALS(get_txkey_for_tiletype(TILE_UPSTAIRS), TX_TILES_UPSTAIRS_00);
        TS_ASSERT_EQUALS(get_txkey_for_tiletype(TILE_DOWNSTAIRS), TX_TILES_DOWNSTAIRS_00);
        TS_ASSERT_EQUALS(get_txkey_for_tiletype(TILE_STONE_WALL_00), TX_TILES_WALL_STONE_00);
        TS_ASSERT_EQUALS(get_txkey_for_tiletype(TILE_FLOOR_STONE_10), TX_TILES_STONE_10);
        TS_ASSERT_EQUALS(get_txkey_for_tiletype(TILE_FLOOR_GRASS_19), TX_TILES_GRASS_19);
    }

    void testSpriteIncrFrameLoopsWhenAnimating() {
        Texture2D texture = make_texture(96, 64);
        sprite s(&texture, 2, 3);

        TS_ASSERT_EQUALS(s.get_width(), 32);
        TS_ASSERT_EQUALS(s.get_height(), 32);
        TS_ASSERT_EQUALS(s.get_currentframe(), 0);
        TS_ASSERT_EQUALS(s.get_num_loops(), 0);

        s.incr_frame();
        TS_ASSERT_EQUALS(s.get_currentframe(), 1);
        TS_ASSERT_EQUALS(static_cast<int>(s.get_src().x), 32);

        s.incr_frame();
        TS_ASSERT_EQUALS(s.get_currentframe(), 2);
        TS_ASSERT_EQUALS(static_cast<int>(s.get_src().x), 64);

        s.incr_frame();
        TS_ASSERT_EQUALS(s.get_currentframe(), 0);
        TS_ASSERT_EQUALS(s.get_num_loops(), 1);
        TS_ASSERT_EQUALS(static_cast<int>(s.get_src().x), 0);
    }

    void testSpriteStopOnLastFrameHaltsAnimation() {
        Texture2D texture = make_texture(96, 32);
        sprite s(&texture, 1, 3);
        s.set_stop_on_last_frame(true);

        s.incr_frame();
        TS_ASSERT(s.get_is_animating());
        TS_ASSERT_EQUALS(s.get_currentframe(), 1);

        s.incr_frame();
        TS_ASSERT(!s.get_is_animating());
        TS_ASSERT_EQUALS(s.get_currentframe(), 2);
        TS_ASSERT_EQUALS(static_cast<int>(s.get_src().x), 64);

        s.incr_frame();
        TS_ASSERT_EQUALS(s.get_currentframe(), 2);
        TS_ASSERT_EQUALS(s.get_num_loops(), 0);
    }

    void testSpriteSetContextResetsFrameOnlyWhileAnimating() {
        Texture2D texture = make_texture(128, 64);
        sprite s(&texture, 2, 4);

        s.set_currentframe(3);
        s.set_src_x(96.0f);
        s.set_context(1);
        TS_ASSERT_EQUALS(s.get_currentcontext(), 1);
        TS_ASSERT_EQUALS(s.get_currentframe(), 0);
        TS_ASSERT_EQUALS(static_cast<int>(s.get_src().x), 0);
        TS_ASSERT_EQUALS(static_cast<int>(s.get_src().y), 32);

        s.set_is_animating(false);
        s.set_currentframe(2);
        s.set_src_x(64.0f);
        s.set_context(0);
        TS_ASSERT_EQUALS(s.get_currentcontext(), 0);
        TS_ASSERT_EQUALS(s.get_currentframe(), 2);
        TS_ASSERT_EQUALS(static_cast<int>(s.get_src().x), 64);
        TS_ASSERT_EQUALS(static_cast<int>(s.get_src().y), 0);
    }

    void testSpritegroupSetCurrentAndContextsDriveContainedSprites() {
        Texture2D texture = make_texture(96, 64);
        auto first = std::make_shared<sprite>(&texture, 2, 3);
        auto second = std::make_shared<sprite>(&texture, 2, 3);

        spritegroup group(4);
        group.add(first);
        group.add(second);

        first->set_currentframe(2);
        first->set_num_loops(4);
        TS_ASSERT(group.set_current(0));
        TS_ASSERT_EQUALS(first->get_currentframe(), 0);
        TS_ASSERT_EQUALS(first->get_num_loops(), 0);

        group.setcontexts(1);
        TS_ASSERT_EQUALS(first->get_currentcontext(), 1);
        TS_ASSERT_EQUALS(second->get_currentcontext(), 1);
        TS_ASSERT_EQUALS(static_cast<int>(first->get_src().y), 32);
        TS_ASSERT_EQUALS(static_cast<int>(second->get_src().y), 32);
    }

    void testDamagePopupFontSizeTracksCameraZoomToStayReadable() {
        gamestate g;
        damage_popup_t popup = {};
        popup.lifetime_seconds = 0.7f;
        popup.age_seconds = 0.0f;

        g.cam2d.zoom = 2.0f;
        const float zoomed_out_size = damage_popup_font_size_world(g, popup);

        g.cam2d.zoom = 8.0f;
        const float zoomed_in_size = damage_popup_font_size_world(g, popup);

        TS_ASSERT(zoomed_out_size > zoomed_in_size);
        TS_ASSERT(zoomed_in_size >= 3.0f);
    }
};
