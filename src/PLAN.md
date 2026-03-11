# GameState Refactor Plan

This file is the handoff note for the `gamestate.h` cleanup work.

## Completed This Session

- Extracted world generation and placement methods out of `gamestate.h` into `gamestate_world_impl.h`.
- Extracted entity factory and setup methods out of `gamestate.h` into `gamestate_entity_factory_impl.h`.
- Extracted lifecycle/bootstrap/update flow out of `gamestate.h` into `gamestate_lifecycle_impl.h`.
- Extracted scene/bootstrap handlers out of `gamestate.h` into `gamestate_scene_impl.h`.
- Extracted player input and menu handling out of `gamestate.h` into `gamestate_input_impl.h`.
- Extracted NPC AI, combat resolution, and pathfinding out of `gamestate.h` into `gamestate_npc_combat_impl.h`.
- Extracted world interaction helpers out of `gamestate.h` into `gamestate_world_interaction_impl.h`.
- Restored dead-body pulling after the dead NPC tile-cache refactor by updating `try_entity_pull()` in `gamestate_world_interaction_impl.h`.
- Extracted inventory management helpers out of `gamestate.h` into `gamestate_inventory_impl.h`.
- Fixed an edge-of-map crash in `try_entity_pull()` by validating both the pull destination and the pull source before accessing floor tiles.
- Reworked tile item storage from a single cached item to a fixed-capacity `item_cache`, preserving the cache-oriented tile model while allowing multiple items per tile.
- Updated floor rendering so item stacks currently draw a single representative top item instead of overlapping every cached item sprite.
- Split `libdraw.h` into smaller renderer-focused headers:
  - `libdraw_context.h` for shared renderer globals
  - `libdraw_scene_dispatch.h` for scene-to/from-texture dispatch
  - `libdraw_frame.h` for per-frame orchestration
  - `libdraw_lifecycle.h` for init/shutdown and render-target bootstrap
- Reduced `libdraw.h` to a thin composition header so the public include stays stable while ownership is clearer.
- Re-enabled test box creation during `gamestate` init.
- Re-enabled box drawing in `draw_dungeon_floor.h`.
- Reduced the hero default light radius from the temporary testing value down to `3`.
- Introduced a first-pass dungeon floor painting API in `dungeon_floor.h`:
  - clamped rectangle painting
  - adjacent-region placement
  - straight connector/corridor carving
  - floor upgrade pass
  - door-candidate refresh pass
- Replaced the one-big-room dungeon bootstrap with connected room-and-corridor generation in `gamestate_world_impl.h`.
- Tuned the new generator so larger floors scale by room count and density rather than by making each room much larger.
- Increased the default gameplay dungeon bootstrap from `16x16` to `64x64` in `gamestate_lifecycle_impl.h`.
- Made `room` geometry/name/description getters `const` so generation code can safely inspect room metadata through const references.

## Current State

- `gamestate.h` was reduced significantly and now acts more like a top-level coordinator.
- The project still uses implementation headers included at the bottom of `gamestate.h`.
- This is intentional for now because the build is still effectively single-translation-unit oriented (`main.cpp` includes `gamestate.h` directly).
- Dead NPCs now live on tiles via `dead_npc_cache` in `dungeon_tile.h`, so pull/interact behavior has to account for that storage path explicitly.
- Inventory mutations, equip/unequip handling, and potion use now live in `gamestate_inventory_impl.h`.
- Tile entity storage is now more explicitly cache-based:
  - single-slot caches for player/live NPC, box, and door
  - fixed-capacity caches for dead NPCs and items
- Multi-item tiles are now supported at the storage layer, but rendering and pickup still intentionally treat the top cached item as the visible/interactive representative.
- Renderer code is still implementation-header based, but `libdraw.h` now reads as a composition root instead of a monolith.
- Scene draw dispatch, render-target lifecycle, and per-frame orchestration are now separated enough to refactor further without changing runtime behavior first.
- Dungeon generation no longer defaults to a single open rectangle.
- The current dungeon bootstrap now creates a `64x64` floor for gameplay testing.
- The current generator produces:
  - many small connected rooms
  - short corridors between adjacent regions
  - viable door candidate chokepoints inferred from tile neighborhoods
- The current scaling direction is intentionally density-first:
  - keep rooms relatively small
  - add more rooms as floor area increases
  - keep gap/corridor lengths short so bigger maps read as denser, not emptier

## Files Added

- `gamestate_world_impl.h`
- `gamestate_entity_factory_impl.h`
- `gamestate_lifecycle_impl.h`
- `gamestate_scene_impl.h`
- `gamestate_input_impl.h`
- `gamestate_npc_combat_impl.h`
- `gamestate_world_interaction_impl.h`
- `gamestate_inventory_impl.h`
- `item_cache.h`
- `libdraw_context.h`
- `libdraw_scene_dispatch.h`
- `libdraw_frame.h`
- `libdraw_lifecycle.h`

## Verified

- Build command used:

```sh
make clean && CXXFLAGS="-DDEBUG_ASSERT=1 -DNPCS_ALL_AT_ONCE -DDEBUG=1 -DMASTER_VOLUME=1.0f -DMUSIC_OFF " make game
```

- Code compiles and links successfully.
- The final `make game` step still reports failure because `log_build_stats.sh` tries to write `/home/darkmage/current_loc.txt`, which is outside the sandbox. The `game` binary is still produced.
- Additional verification this session:
  - repeated `make clean && make game` rebuilds succeeded through compile and link after each dungeon-generation change
  - the same post-build `log_build_stats.sh` permission failure still causes `make game` to return nonzero even though the binary is produced

## Next Refactor Targets

These are the best remaining cleanup seams for the next session:

1. `libdraw.h` / renderer organization
   - split render lifecycle/bootstrap from per-frame scene dispatch
   - identify a cleaner top-down structure around:
     - render init / shutdown
     - render target orchestration
     - scene-specific draw dispatch
     - sprite update pre/post passes
   - use `draw_frame_2d.h` as a likely composition root for gameplay rendering and look for similar seams in title/character-creation paths

2. Debug and diagnostics
   - `update_debug_panel_buffer`
   - possibly related profiling/debug helpers

3. Remaining `gamestate.h` stragglers
   - small inline helpers that still fit better in extracted implementation headers
   - reassess whether any world-interaction or input-adjacent helpers should be grouped further

## Notes For Next Session

- Re-check the dead-body interaction path before doing more world-interaction cleanup.
- `try_entity_pull()` now has explicit bounds checks for both the actor destination and the pull source tile.
- Item stacking now exists at the tile-cache layer; if interaction/render behavior changes again, keep storage concerns separate from presentation concerns.
- Current item-stack presentation is intentionally minimal:
  - draw only the top cached item on the floor
  - pickup still operates on the top cached item
- If world interaction is revisited again, compare behavior across:
  - boxes
  - dead NPC bodies
  - any logic that still assumes `get_cached_live_npc()` or `get_cached_box()` is the only source of truth
- The old one-big-room `16x16` generation path is no longer the default gameplay path.
- The new dungeon generation work is now in a good first-pass state rather than being the main glaring gameplay problem.
- Doors and boxes are available again, so the map-generation pass can now start producing layouts that actually justify them.
- The current generator is still tree-like in spirit because each new room is attached from an anchor room.
- The next quality jump is likely more interconnection, not simply more density.

## Libdraw Review Notes

- `libdraw.h` is now the best next refactor target.
- Highest-value signal:
  - it mixes renderer lifecycle (`libdraw_init`, `libdraw_close`)
  - frame orchestration (`drawframe`)
  - scene dispatch
  - render target ownership/bootstrap
  - and sprite-update sequencing
- `draw_frame_2d.h` already behaves like a gameplay rendering composition unit and is a good anchor for extracting cleaner responsibilities upward.
- Likely first-pass extraction seams:
  - render initialization / shutdown helpers
  - scene draw dispatch helpers
  - target-texture orchestration helpers
  - sprite-update pre/post pipeline coordination
- The current system appears to have a coherent feel already; the refactor goal should be structural clarity and smaller ownership surfaces, not changing rendering behavior.
- First-pass structural split has been applied; the next pass should focus on deeper cleanup inside the extracted units rather than moving more code around blindly.

## Longer-Term Cleanup Ideas

- Move from implementation headers to real `.cpp` files once the build system is ready for multiple translation units.
- Re-evaluate whether `gamestate` should keep owning all behavior directly, or whether some of these areas should become subsystems/helpers.
- Group related methods by responsibility more explicitly:
  - world/bootstrap
  - entities/factories
  - player input
  - combat/AI
  - inventory/items
  - rendering/debug-facing state preparation

## Suggested Starting Point Next Time

Continue with:

1. Add secondary cross-connections between already-placed rooms so larger floors stop reading as a simple branch/tree structure.
2. Extend the floor painting API from rectangles plus straight connectors into richer primitives:
   - L-shaped corridors
   - variable-width hallways
   - explicit region objects or paint operations
3. Revisit door placement after more interconnection exists so doors appear at intentional chokepoints instead of every merely valid candidate.
4. Review `gamestate_world_impl.h` and `dungeon_floor.h` first, since that is now the active gameplay iteration area.
5. Keep the next pass focused on structure/connectivity/readability before moving on to decoration.

## Dungeon Generation Intent

- The next major gameplay-facing improvement is dungeon floor creation, not more header extraction.
- The target feel is:
  - multiple adjacent rooms
  - corridors/passageways between them
  - more maze-like traversal
  - chokepoints that make door placement meaningful
- Avoid generating a single open rectangular floor as the default layout.
- First-pass success criteria:
  - the player spawns into a connected dungeon
  - the floor reads as rooms plus hallways rather than one open chamber
  - there are obvious doorway candidates between spaces
  - existing entity placement (`player`, `NPC`, `items`, `boxes`) still works on valid walkable tiles
- After the shape generation is improved, revisit door-placement rules so doors are placed because of the map layout rather than as a cosmetic afterthought.

## Dungeon Generation Progress

- First-pass dungeon floor generation is now in place and working:
  - connected room growth from an initial seed room
  - adjacent region placement
  - straight corridor carving between regions
  - automatic door candidate refresh based on local tile topology
- The current `64x64` gameplay bootstrap was useful because it exposed the difference between:
  - bad scaling by enlarging rooms
  - better scaling by increasing room count and density
- Important design lesson from this session:
  - scaling dungeon generation should primarily increase structure count/connectivity density
  - it should not primarily increase room footprint

## Possible Next Things

1. Add loop-making passes that connect nearby existing rooms after the initial growth pass.
2. Add L-corridor support so room adjacency is not limited to perfect overlap on one axis.
3. Introduce region metadata on `dungeon_floor` so rooms/corridors are tracked explicitly instead of only implied by painted tiles.
4. Split door candidacy into:
   - geometrically valid
   - gameplay desirable
   so later passes can be selective.
5. Add dedicated spawn-selection rules so player/NPC/item placement can prefer rooms over corridors when useful.
6. Add lightweight debug visualization or counters for:
   - room count
   - corridor count
   - door candidate count
   - walkable tile count
7. Revisit `parts` so it becomes a real density/style control instead of a mostly indirect heuristic input.
8. Add alternate generation styles later using the same painting API rather than replacing the API:
   - sparse stronghold
   - dense catacombs
   - chamber-and-hub layouts

## Future Things

1. The web build that uses `emcc` in the `Makefile` packages the game assets in a way that I can push the game build to itch.io so that people may play the game in their browser. I am interested in finding a way for us to package assets in a way that cannot be easily reverse-engineered and stolen as that would result in a violation of the licensing the assets were sold under (makes sense right?). I'd love to allow people to play a compiled binary or build and run the game themselves but the assets need to be delivered in a way that is safe to distribute without reasonable threat of theft.

## Definite Next Things

1. When in the primary gameplay scene, when pressing escape, currently it instantly exits the game. I want to incrementally modify this behavior. This was a fine start but I need to present the player with a window that asks if they really want to exit, with a yes or no menu toggle in the window that pops up. Current message window behavior does not offer things like Yes/No or any toggleable optionality when presenting a message to the player. "Do You Really Want To Quit? Y/N", "Are You Sure You Want To Quit? Press Y or N" could be acceptable instead of a whole real "toggle" like I have in the Title Screen or in the Character Creation Screen. The simpler the better. So, to summarize, we need:
  - Message Windows that can offer a selection that returns a true/false or etc. depending on what the user inputs
  - Use the new Message Window to implement a "quit" safeguard before exiting...
    - "Do You Want To Exit? Y/N" or "Are You Sure You Want To Exit? Press Y or N"
    - We still press Escape once to bring up the "HandleQuit" Message Window
    - Outside of the Gameplay Scene, Escape will instant-quit the game still

2. Reduce the dungeon floor size to 8x8 to begin with. We tested 64x64 last time and it feels really good with the new maze generation algorithm. I want to verify that it works at 8x8, then 16x16, then 32x32. This should give us a lot of variety when generating floors.
  - [ ] 8x8 tested (human must verify before proceeding)
  - [ ] 16x16 tested (human must verify before proceeding)
  - [ ] 32x32 tested (human must verify before proceeding)
  - [ ] 8x16 tested (human must verify before proceeding)
  - [ ] 16x8 tested (human must verify before proceeding)

3. Prepare to generate a 2nd dungeon floor and add it to the gamestate dungeon's list of floors.
  - 1st floor: 8x8
  - 2nd floor: 16x16

4. Update the floor generation algorithms so that we assign 2 randomly-selected but different tiles (x, y) per floor (z) to be the `UPSTAIRS` and `DOWNSTAIRS` tiles. 

5. Implement the `try_entity_go_upstairs` and `try_entity_go_downstairs` or similar functions necessary to handle any entity attempting to ascend or descend a staircase
  - Method fails and produces a message history log stating the entity tried to go upstairs but there was no stairs, or some other error (yet to be decided) occurs
  - Same goes for downstairs
  - On success, gamestate dungeon's `current_floor` will point to the proper floor...the "top" of the dungeon begins at floor 0, and descending means the `current_floor` increases. 
  - Attempting to ascend floors on floor 0 will result in a message box stating an error if the player/hero attempts to do so, and a message history log for other entities if they attempt to do so
  - Attempting to descend floors on the last floor in gamestate.d.floors will result in a similar popup if the hero attempts it, and a message history log if attempted by another entity (NPC, etc)

