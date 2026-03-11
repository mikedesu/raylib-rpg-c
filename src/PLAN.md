# GameState Refactor Plan

This file is the handoff note for the `gamestate.h` cleanup work.

## Completed This Session

- Refactored `gamestate.h` into responsibility-specific implementation headers for world generation, lifecycle, scenes, input, combat/AI, world interaction, and inventory.
- Split `libdraw.h` into smaller renderer-focused headers and kept `libdraw.h` as the stable composition include.
- Stabilized several gameplay systems:
  - restored/fixed dead-body pulling, including edge-of-map bounds validation
  - changed tile item storage to a fixed-capacity cache while keeping floor rendering/pickup on the top item
  - re-enabled boxes and reset the hero light radius back down to `3`
- Reworked dungeon generation away from the single-room bootstrap:
  - added a first-pass floor-painting API
  - switched to connected room-and-corridor generation
  - tuned scaling toward more rooms/density instead of oversized rooms
- Added a gameplay-only quit confirmation flow:
  - `Escape` in gameplay now opens a simple `Y/N` confirm prompt
  - non-gameplay scenes still instant-quit
  - web builds return to title/reset instead of closing the app
- Ran the planned floor-size test pass and recorded the results:
  - `8x8` underuses space and tends to collapse to tiny single-room layouts
  - `16x16`, `32x32`, `8x16`, and `16x8` all produced acceptable-to-good interconnected layouts in manual testing
- Improved the small-map generator for `8x8`:
  - added a compact-map branch with `2x2` minimum/maximum room sizing for `8x8`
  - allowed zero-gap adjacency and removed extra overlap padding for compact maps
  - tuned compact-map room-count targeting down to a realistic `4-5` rooms
  - manual verification after the change showed `8x8` feels noticeably better and uses space more effectively
- Updated gameplay bootstrap to generate 2 floors:
  - floor `0`: `8x8`
  - floor `1`: `16x16`
- Added random staircase assignment per floor and bound stairs traversal to `.`
- Added a per-floor full-light debug toggle on `l`, with debug/help text support

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
- Very small maps now use a dedicated compact-map generation path:
  - `8x8` floors allow `2x2` rooms
  - `8x8` floors allow direct adjacency without a mandatory one-tile gap
  - compact-map spacing rules are intentionally looser than the general generator so the tiny interior can still support multiple regions

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
- Additional verification this session:
  - repeated `make clean && make game` rebuilds succeeded through compile and link after each dungeon-generation change
  - after updating `log_build_stats.sh`, `make clean && make game` now completes successfully end-to-end

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

- World-interaction caution:
  - re-check the dead-body interaction path before more cleanup in that area
  - `try_entity_pull()` now has explicit source/destination bounds checks and should stay that way
  - if interaction logic changes again, compare behavior across boxes, dead bodies, and any code that still assumes single-source tile caches
- Item-stack reminder:
  - storage now supports multiple items per tile
  - presentation is still intentionally minimal: draw and pick up only the top cached item
- Manual floor-size takeaways:
  - `8x8`: improved after the compact-map tuning pass; no longer feels stuck in the previous tiny-single-room failure mode
  - `16x16`: good, 4 runs all produced 6 interconnected rooms
  - `32x32`: good, 2 runs produced about 21 interconnected rooms with no obvious disconnected spaces
  - `8x16` and `16x8`: acceptable, both produced 3 interconnected rooms
- Dungeon-generation state:
  - the old one-big-room path is no longer the default
  - the current generator is in a solid first-pass state and produces connected room/corridor layouts
  - it still reads as tree-like because growth anchors from existing rooms
  - the next quality jump is more interconnection, not just more density
- Gameplay-layout implication:
  - doors and boxes are available again, so future generation passes can start leaning into chokepoints and room purpose more intentionally

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
- Additional small-map lesson:
  - `8x8` needs its own geometry rules
  - applying the same minimum room size, gap, and spacing assumptions used by larger maps underutilizes the tiny playable interior

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

1. Improve interconnection after the initial room-growth pass.
  - the generator now handles `8x8` more credibly, so the next quality step is less about fit and more about topology
  - add loop-making or nearby-room connection passes so layouts read less tree-like

2. Add more corridor flexibility.
  - consider L-corridors or similar options so room attachment is not limited to straight-axis overlap
  - keep the existing connected-room feel, but reduce the number of rejected placements caused by alignment constraints

3. Preserve the small-map floor rules explicitly.
  - compact maps should keep the `2x2` minimum room size
  - compact maps should keep zero-gap adjacency available where needed
  - avoid regressing `8x8` back toward single-room collapse while improving the broader generator
