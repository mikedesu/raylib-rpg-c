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
};
