#pragma once

#include "../ComponentTable.h"
#include "../ComponentTraits.h"
#include "../entitytype.h"
#include "../vec3.h"
#include <cxxtest/TestSuite.h>
#include <memory>
#include <vector>

class ComponentTableTestSuite : public CxxTest::TestSuite {
public:
    void testComponentTableStoresIndependentTypes() {
        ComponentTable table;
        table.set<name>(1, "hero");
        table.set<hp>(1, 12);
        table.set<entitytype>(1, ENTITY_PLAYER);
        table.set<location>(1, vec3{3, 4, 0});

        TS_ASSERT(table.has<name>(1));
        TS_ASSERT(table.has<hp>(1));
        TS_ASSERT(table.has<entitytype>(1));
        TS_ASSERT(table.has<location>(1));
        TS_ASSERT_EQUALS(table.get<name>(1).value_or("missing"), "hero");
        TS_ASSERT_EQUALS(table.get<hp>(1).value_or(-1), 12);
        TS_ASSERT_EQUALS(table.get<entitytype>(1).value_or(ENTITY_NONE), ENTITY_PLAYER);

        const vec3 loc = table.get<location>(1).value_or(vec3{-1, -1, -1});
        TS_ASSERT_EQUALS(loc.x, 3);
        TS_ASSERT_EQUALS(loc.y, 4);
        TS_ASSERT_EQUALS(loc.z, 0);

        TS_ASSERT(!table.has<maxhp>(1));
        TS_ASSERT(!table.get<maxhp>(1).has_value());
    }

    void testComponentTableRemoveAndClear() {
        ComponentTable table;
        table.set<hp>(7, 21);
        table.set<name>(7, "orc");
        table.set<inventory>(7, std::make_shared<std::vector<entityid>>(std::vector<entityid>{3, 4}));

        TS_ASSERT(table.remove<hp>(7));
        TS_ASSERT(!table.has<hp>(7));
        TS_ASSERT(!table.remove<hp>(7));

        auto inv = table.get<inventory>(7);
        TS_ASSERT(inv.has_value());
        TS_ASSERT_EQUALS(inv.value()->size(), 2U);
        TS_ASSERT_EQUALS(inv.value()->at(0), 3);

        table.clear();
        TS_ASSERT(!table.has<name>(7));
        TS_ASSERT(!table.has<inventory>(7));
        TS_ASSERT(!table.get<name>(7).has_value());
    }
};
