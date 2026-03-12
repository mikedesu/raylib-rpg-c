#pragma once

#include "../ComponentTable.h"
#include "../ComponentTraits.h"
#include "../entitytype.h"
#include "../vec2.h"
#include "../vec3.h"
#include <cxxtest/TestSuite.h>
#include <memory>
#include <vector>

class ComponentTableTestSuite : public CxxTest::TestSuite {
public:
    void testComponentTableStoresIndependentTypes() {
        ComponentTable table;
        table.set<name>(1, "hero");
        table.set<hp>(1, vec2{12, 14});
        table.set<entitytype>(1, ENTITY_PLAYER);
        table.set<location>(1, vec3{3, 4, 0});

        TS_ASSERT(table.has<name>(1));
        TS_ASSERT(table.has<hp>(1));
        TS_ASSERT(table.has<entitytype>(1));
        TS_ASSERT(table.has<location>(1));
        TS_ASSERT_EQUALS(table.get<name>(1).value_or("missing"), "hero");
        const vec2 hp_value = table.get<hp>(1).value_or(vec2{-1, -1});
        TS_ASSERT_EQUALS(hp_value.x, 12);
        TS_ASSERT_EQUALS(hp_value.y, 14);
        TS_ASSERT_EQUALS(table.get<entitytype>(1).value_or(ENTITY_NONE), ENTITY_PLAYER);

        const vec3 loc = table.get<location>(1).value_or(vec3{-1, -1, -1});
        TS_ASSERT_EQUALS(loc.x, 3);
        TS_ASSERT_EQUALS(loc.y, 4);
        TS_ASSERT_EQUALS(loc.z, 0);

    }

    void testComponentTableRemoveAndClear() {
        ComponentTable table;
        table.set<hp>(7, vec2{21, 21});
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

    void testComponentTableSetOverwritesExistingValue() {
        ComponentTable table;

        table.set<hp>(5, vec2{12, 15});
        table.set<hp>(5, vec2{19, 24});
        table.set<name>(5, "orc");
        table.set<name>(5, "ogre");

        TS_ASSERT(table.has<hp>(5));
        TS_ASSERT(table.has<name>(5));
        const vec2 hp_value = table.get<hp>(5).value_or(vec2{-1, -1});
        TS_ASSERT_EQUALS(hp_value.x, 19);
        TS_ASSERT_EQUALS(hp_value.y, 24);
        TS_ASSERT_EQUALS(table.get<name>(5).value_or("missing"), "ogre");

        auto hpStore = table.getStore<hp>();
        auto nameStore = table.getStore<name>();
        TS_ASSERT_EQUALS(hpStore->size(), 1U);
        TS_ASSERT_EQUALS(nameStore->size(), 1U);
    }

    void testComponentTableHighChurnInsertRemoveAcrossKinds() {
        ComponentTable table;

        for (entityid id = 1; id <= 64; ++id) {
            table.set<hp>(id, vec2{static_cast<int>(id * 3), static_cast<int>(id * 3 + 2)});
            table.set<location>(id, vec3{static_cast<int>(id), static_cast<int>(id + 1), static_cast<int>(id % 2)});
        }

        for (entityid id = 1; id <= 64; ++id) {
            TS_ASSERT(table.has<hp>(id));
            const vec2 hp_value = table.get<hp>(id).value_or(vec2{-1, -1});
            TS_ASSERT_EQUALS(hp_value.x, static_cast<int>(id * 3));
            TS_ASSERT_EQUALS(hp_value.y, static_cast<int>(id * 3 + 2));
        }

        for (entityid id = 2; id <= 64; id += 2) {
            TS_ASSERT(table.remove<hp>(id));
            TS_ASSERT(table.remove<location>(id));
        }

        for (entityid id = 1; id <= 64; ++id) {
            const bool removed = (id % 2) == 0;
            TS_ASSERT_EQUALS(table.has<hp>(id), !removed);
            TS_ASSERT_EQUALS(table.has<location>(id), !removed);
        }

        for (entityid id = 2; id <= 64; id += 2) {
            table.set<hp>(id, vec2{static_cast<int>(id * 5), static_cast<int>(id * 3 + 2)});
            table.set<location>(id, vec3{99, static_cast<int>(id), 1});
        }

        for (entityid id = 1; id <= 64; ++id) {
            TS_ASSERT(table.has<hp>(id));
            TS_ASSERT(table.has<location>(id));

            const int expectedHp = (id % 2) == 0 ? static_cast<int>(id * 5) : static_cast<int>(id * 3);
            const vec3 loc = table.get<location>(id).value_or(vec3{-1, -1, -1});
            const vec2 hp_value = table.get<hp>(id).value_or(vec2{-1, -1});

            TS_ASSERT_EQUALS(hp_value.x, expectedHp);
            TS_ASSERT_EQUALS(hp_value.y, static_cast<int>(id * 3 + 2));
            if ((id % 2) == 0) {
                TS_ASSERT_EQUALS(loc.x, 99);
                TS_ASSERT_EQUALS(loc.y, static_cast<int>(id));
                TS_ASSERT_EQUALS(loc.z, 1);
            } else {
                TS_ASSERT_EQUALS(loc.x, static_cast<int>(id));
                TS_ASSERT_EQUALS(loc.y, static_cast<int>(id + 1));
                TS_ASSERT_EQUALS(loc.z, static_cast<int>(id % 2));
            }
        }
    }
};
