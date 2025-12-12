#include "draw_debug_panel.h"
#include "draw_update_debug_panel.h"
#include "libdraw_handle_debug_panel.h"

void handle_debug_panel(shared_ptr<gamestate> g) {
    if (g->debugpanelon) {
        update_debug_panel(g);
        draw_debug_panel(g);
    }
}
