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
        minfo("test message system");
        gamestate g;
        
        // Verify initial state
        as(g.msg_system.size() == 0);
        as(g.msg_history.size() == 0);
        as(!g.msg_system_is_active);
        
        // Add messages
        g.msg_system.push_back("Test message 1");
        g.msg_system.push_back("Test message 2");
        as(g.msg_system.size() == 2);
        as(g.msg_system[0] == "Test message 1");
        as(g.msg_system[1] == "Test message 2");
        
        // Verify history
        g.msg_history = g.msg_system;
        as(g.msg_history.size() == 2);
        as(g.msg_history[0] == "Test message 1");
        as(g.msg_history[1] == "Test message 2");
        
        // Test reset
        g.reset();
        as(g.msg_system.size() == 0);
        as(g.msg_history.size() == 0);
        as(!g.msg_system_is_active);
    }

    void testCameraSystem() {
        minfo("test camera system");
        gamestate g;
        
        // Verify initial state
        as(g.cam2d.zoom == 4.0f);
        as(g.cam2d.rotation == 0.0);
        as(g.cam_lockon == true);
        as(g.cam_changed == false);
        
        // Test camera movement
        g.cam2d.target = (Vector2){100, 100};
        as(g.cam2d.target.x == 100);
        as(g.cam2d.target.y == 100);
        
        // Test zoom
        g.cam2d.zoom = 2.0f;
        as(g.cam2d.zoom == 2.0f);
        as(g.cam_changed == true);
        
        // Test reset
        g.reset();
        as(g.cam2d.zoom == 4.0f);
        as(g.cam2d.rotation == 0.0);
        as(g.cam_lockon == true);
        as(g.cam_changed == false);
    }
};
