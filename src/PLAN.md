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
- The current dungeon floor generation still effectively produces one large rectangular playable area inside a `16x16` grid.
- That generation approach is now the main design problem to solve next, ahead of more refactor-only cleanup.
- Doors and boxes are available again, so the map-generation pass can now start producing layouts that actually justify them.

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

1. Upgrade dungeon floor generation so floors stop being one big rectangle and instead produce a more maze-like layout.
2. Introduce adjacent rooms connected by narrower passageways/corridors.
3. Make the passageways and room transitions deliberate enough that door placement becomes a natural part of the generated layout.
4. Review `gamestate_world_impl.h` and `dungeon_floor.h` first, since that is where the current floor-shape assumptions likely live.
5. Keep the first pass focused on generation shape and connectivity, not decoration.

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
