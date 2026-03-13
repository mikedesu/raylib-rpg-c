#pragma once

#include "../entitytype.h"
#include "../vec3.h"
#include <cxxtest/TestSuite.h>

class UtilityHelpersTestSuite : public CxxTest::TestSuite {
public:
    void testVec3Helpers() {
        const vec3 a{1, 2, 3};
        const vec3 b{4, 5, 6};
        const vec3 invalid{-1, -1, -1};

        const vec3 sum = vec3_add(a, b);
        const vec3 diff = vec3_sub(b, a);

        TS_ASSERT(vec3_equal(sum, vec3{5, 7, 9}));
        TS_ASSERT(vec3_equal(diff, vec3{3, 3, 3}));
        TS_ASSERT(vec3_invalid(invalid));
        TS_ASSERT(vec3_valid(a));
        TS_ASSERT((vec3{0, 0, 0} < vec3{0, 0, 1}));
        TS_ASSERT((vec3{0, 0, 1} < vec3{0, 1, 0}));
        TS_ASSERT((vec3{0, 1, 0} < vec3{1, 0, 0}));
    }

    void testEntityTypeToString() {
        TS_ASSERT_EQUALS(entitytype_to_str(ENTITY_NONE), "none");
        TS_ASSERT_EQUALS(entitytype_to_str(ENTITY_PLAYER), "player");
        TS_ASSERT_EQUALS(entitytype_to_str(ENTITY_NPC), "npc");
        TS_ASSERT_EQUALS(entitytype_to_str(ENTITY_ITEM), "item");
        TS_ASSERT_EQUALS(entitytype_to_str(ENTITY_BOX), "box");
        TS_ASSERT_EQUALS(entitytype_to_str(ENTITY_CHEST), "chest");
        TS_ASSERT_EQUALS(entitytype_to_str(ENTITY_DOOR), "door");
        TS_ASSERT_EQUALS(entitytype_to_str(ENTITY_SPELL), "spell");
        TS_ASSERT_EQUALS(entitytype_to_str(ENTITY_PROP), "prop");
    }
};
