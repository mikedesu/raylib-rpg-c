#include "ComponentTraits.h"
#include "entityid.h"
#include "entitytype.h"
#include "gamestate.h"
#include "get_nanoseconds.h"
#include "mprint.h"
#include "potion.h"
#include "scene.h"
#include "weapon.h"
#include <chrono>
#include <cxxtest/TestSuite.h>
#include <raylib.h>
#include <set>

using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::system_clock;
using std::chrono::time_point;

class MyTestSuite : public CxxTest::TestSuite {
public:
    void testGamestateEmpty() {
    }

    void testDeadNpcCacheBasic() {
        dead_npc_cache cache;
        TS_ASSERT_EQUALS(cache.get_count(), 0);
        
        // Test basic add
        cache.add_id(100);
        TS_ASSERT_EQUALS(cache.get_count(), 1);
        TS_ASSERT(cache.contains(100) != -1);
        
        // Test remove
        cache.remove_id(100);
        TS_ASSERT_EQUALS(cache.get_count(), 0);
        TS_ASSERT_EQUALS(cache.contains(100), -1);
    }

    void testDeadNpcCacheBoundaries() {
        dead_npc_cache cache;
        
        // Fill the cache
        for (int i = 0; i < 8; i++) {
            cache.add_id(i);
            TS_ASSERT_EQUALS(cache.get_count(), i+1);
        }
        
        // Verify all present
        for (int i = 0; i < 8; i++) {
            TS_ASSERT(cache.contains(i) != -1);
        }
        
        // Test wraparound when adding 9th item
        cache.add_id(9);
        TS_ASSERT_EQUALS(cache.get_count(), 8);
        TS_ASSERT_EQUALS(cache.contains(0), -1); // First item should be pushed out
        TS_ASSERT(cache.contains(9) != -1);      // New item should be last
    }

    void testDeadNpcCacheRemove() {
        dead_npc_cache cache;
        cache.add_id(1);
        cache.add_id(2);
        cache.add_id(3);
        
        // Remove middle item
        TS_ASSERT(cache.remove_id(2));
        TS_ASSERT_EQUALS(cache.get_count(), 2);
        TS_ASSERT_EQUALS(cache.contains(2), -1);
        
        // Remove non-existent
        TS_ASSERT(!cache.remove_id(999));
        TS_ASSERT_EQUALS(cache.get_count(), 2);
    }
};
