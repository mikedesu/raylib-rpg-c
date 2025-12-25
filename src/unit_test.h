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
};
