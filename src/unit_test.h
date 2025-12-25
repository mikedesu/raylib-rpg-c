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
};
