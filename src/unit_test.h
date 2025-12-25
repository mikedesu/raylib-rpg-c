#include "gamestate.h"
#include "mprint.h"
#include <cxxtest/TestSuite.h>


#define as TS_ASSERT
#define as_equals TS_ASSERT_EQUALS

class MyTestSuite : public CxxTest::TestSuite {
public:
    void testGamestateBasic() {
        minfo("test gamestate basic");
        gamestate g;

        as(g.cam_lockon == true);
        as(g.msg_system.size() == 0);
        as(g.msg_history.size() == 0);

        g.set_hero_id(1);
        as(g.hero_id == 1);

        g.reset();

        as(g.hero_id == ENTITYID_INVALID);
        as(g.cam_lockon == true);
        as(g.msg_system.size() == 0);
        as(g.msg_history.size() == 0);

        g.reset();
    }

    void testGamestateThrowaway() {
        minfo("test gamestate throwaway");
        for (int i = 0; i < 10; i++) {
            gamestate g;
        }
    }

    void testEntityManagement() {
        minfo("test entity management");
        gamestate g;
        
        // Verify initial state
        as(g.next_entityid == 1);
        as(g.new_entityid_begin == ENTITYID_INVALID);
        as(g.new_entityid_end == ENTITYID_INVALID);
        as(!g.dirty_entities);
        
        // Add first entity
        entityid first = g.add_entity();
        as(first == 1);
        as(g.next_entityid == 2);
        as(g.new_entityid_begin == 1);
        as(g.new_entityid_end == 2);
        as(g.dirty_entities);
        
        // Add second entity
        entityid second = g.add_entity();
        as(second == 2);
        as(g.next_entityid == 3);
        as(g.new_entityid_begin == 1);  // Should stay at first ID
        as(g.new_entityid_end == 3);    // Should update to new end
        as(g.dirty_entities);
        
        // Reset and verify clean state
        g.reset();
        as(g.next_entityid == 1);
        as(g.new_entityid_begin == ENTITYID_INVALID);
        as(g.new_entityid_end == ENTITYID_INVALID);
        as(!g.dirty_entities);
    }

    void testMessageSystem() {
        minfo("Starting testMessageSystem");
        gamestate g;
        
        // Verify initial state
        minfo("Checking initial state");
        as(g.msg_system.size() == 0);
        as(g.msg_history.size() == 0);
        as(!g.msg_system_is_active);
        
        // Add messages
        minfo("Adding test messages");
        g.msg_system.push_back("Test message 1");
        g.msg_system.push_back("Test message 2");
        as(g.msg_system.size() == 2);
        minfo("Message system size: %zu", g.msg_system.size());
        as(g.msg_system[0] == "Test message 1");
        as(g.msg_system[1] == "Test message 2");
        
        // Verify history
        minfo("Checking message history");
        g.msg_history = g.msg_system;
        as(g.msg_history.size() == 2);
        minfo("History size: %zu", g.msg_history.size());
        as(g.msg_history[0] == "Test message 1");
        as(g.msg_history[1] == "Test message 2");
        
        // Test reset
        minfo("Testing reset behavior");
        g.reset();
        as(g.msg_system.size() == 0);
        as(g.msg_history.size() == 0);
        as(!g.msg_system_is_active);
        minfo("Reset complete");
    }

    void testCameraSystem() {
        minfo("Starting testCameraSystem");
        gamestate g;
        
        // Verify initial state
        minfo("Checking initial camera state");
        as(g.cam2d.zoom == 4.0f);
        as(g.cam2d.rotation == 0.0);
        as(g.cam_lockon == true);
        as(g.cam_changed == false);
        
        // Test camera movement
        minfo("Testing camera movement");
        g.cam2d.target = (Vector2){100, 100};
        as(g.cam2d.target.x == 100);
        as(g.cam2d.target.y == 100);
        minfo("Camera target set to (%.1f, %.1f)", g.cam2d.target.x, g.cam2d.target.y);
        
        // Test zoom
        minfo("Testing camera zoom");
        g.cam2d.zoom = 2.0f;
        as(g.cam2d.zoom == 2.0f);
        minfo("Camera zoom set to %.1f", g.cam2d.zoom);
        
        // Test reset
        minfo("Testing camera reset");
        g.reset();
        as(g.cam2d.zoom == 4.0f);
        as(g.cam2d.rotation == 0.0);
        as(g.cam_lockon == true);
        as(g.cam_changed == false);
        minfo("Reset complete - zoom: %.1f, changed: %d", g.cam2d.zoom, g.cam_changed);
    }
};
