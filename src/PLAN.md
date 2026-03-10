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

## Current State

- `gamestate.h` was reduced significantly and now acts more like a top-level coordinator.
- The project still uses implementation headers included at the bottom of `gamestate.h`.
- This is intentional for now because the build is still effectively single-translation-unit oriented (`main.cpp` includes `gamestate.h` directly).
- Dead NPCs now live on tiles via `dead_npc_cache` in `dungeon_tile.h`, so pull/interact behavior has to account for that storage path explicitly.

## Files Added

- `gamestate_world_impl.h`
- `gamestate_entity_factory_impl.h`
- `gamestate_lifecycle_impl.h`
- `gamestate_scene_impl.h`
- `gamestate_input_impl.h`
- `gamestate_npc_combat_impl.h`
- `gamestate_world_interaction_impl.h`

## Verified

- Build command used:

```sh
make clean && CXXFLAGS="-DDEBUG_ASSERT=1 -DNPCS_ALL_AT_ONCE -DDEBUG=1 -DMASTER_VOLUME=1.0f -DMUSIC_OFF " make game
```

- Code compiles and links successfully.
- The final `make game` step still reports failure because `log_build_stats.sh` tries to write `/home/darkmage/current_loc.txt`, which is outside the sandbox. The `game` binary is still produced.

## Next Refactor Targets

These are the best remaining cleanup seams for the next session:

1. Inventory management helpers
   - equip/unequip
   - item use
   - inventory mutations and related utility functions

2. Debug and diagnostics
   - `update_debug_panel_buffer`
   - possibly related profiling/debug helpers

## Notes For Next Session

- Re-check the dead-body interaction path before doing more world-interaction cleanup.
- `try_entity_pull()` was manually adjusted after the extraction because corpse pulling broke when dead NPCs stopped being represented only through the old tile caches.
- If world interaction is revisited again, compare behavior across:
  - boxes
  - dead NPC bodies
  - any logic that still assumes `get_cached_live_npc()` or `get_cached_box()` is the only source of truth

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

`Refactor the remaining inventory management helpers out of gamestate.h, using PLAN.md as the handoff note.`
