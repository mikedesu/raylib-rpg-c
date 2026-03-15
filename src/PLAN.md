# Project Plan

## START HERE

As a reminder, the proper way to build is: `make clean && CXXFLAGS="-DDEBUG_ASSERT=1 -DNPCS_ALL_AT_ONCE -DDEBUG=1 -DMASTER_VOLUME=1.0f " make game` for now...

**Do not ever tamper with `texture_ids.h` or `textures.txt`!**

## Top 5 Next Things

- [x] Remove all props from floor 4's creation so the pressure-plate tutorial setup is controlled and readable.
- [x] Implement the core `FLOOR_PRESSURE_PLATE` system with live activate/deactivate behavior tied to world state changes.
- [x] Add the floor 4 tutorial room with a pressure plate wired to a door that opens while occupied and closes when vacated.
- [x] Add the "open door" sound effect on the pressure plate on floor 4 if the door exists and is opened by the pressure plate being activated
- [ ] Continue top-down `libdraw` cleanup and reduce remaining rendering global-state coupling.
  - Recent passes centralized renderer-global declarations through `libdraw_context.h`, removed repeated ad hoc `extern` declarations across draw/update headers, and routed `libdraw.h` scene dispatch through the compatibility include.
  - Latest pass also collapsed libdraw-owned process-lifetime renderer state into a single `libdraw_ctx` object instead of many separate globals in `main.cpp`, so spritegroups, texture metadata, shaders, render targets, and presentation rectangles now move through one shared renderer context.
  - Latest follow-up also folded shared audio state and volume settings into `libdraw_ctx`, removing the remaining duplicated `gamestate` mirrors for renderer-owned music/settings state.
  - Latest follow-up also moved the top-level renderer entry points out of header-only `static inline` implementations into a real `libdraw.cpp`, so `libdraw.h`/`libdraw_*` API headers are now declaration-first at the entry-point layer.
  - Latest follow-up also moved sprite lifecycle/update helpers and spritegroup factory dispatch out of implementation headers into `libdraw.cpp`, shrinking a large part of the renderer-side implementation-header surface.
  - Latest follow-up also moved scene composition helpers, gameplay-scene composition, dungeon-floor rendering, and the player target box into `libdraw.cpp`.
  - Remaining cleanup still exists mostly in leaf scene/UI draw headers rather than libdraw orchestration itself.

- Keyboard customization is now live in the gameplay scene.
- Current shipped state:
  - full-keyboard and laptop profiles exist
  - post-character-creation keyboard profile prompt exists
  - controls menu supports profile switching and per-action rebinding
  - face-direction attack is bindable
  - stairs default is now `KEY_F`
  - sound controls are now live in the options menu
  - window box colors are now configurable in the options menu
  - default window/menu background color is now `Color(0, 0, 255, 128)`
  - Debug Panel background color now correctly uses `Color(0, 0, 255, 255)`

## Active Backlog

- [ ] Dungeon / world quality
  - [ ] improve door placement quality beyond the first-pass implementation
  - [ ] keep tightening prop placement so it cannot create broader layout soft-locks
  - [ ] consider smarter spawn-selection rules and better dungeon interconnection / loops

- [ ] Renderer / architecture
  - [ ] continue top-down `libdraw` cleanup
  - [ ] reduce remaining global-state coupling in rendering
  - Recent progress: removed a broad layer of duplicated renderer-global declarations from draw/update helpers by funneling them through `libdraw_context.h`, then consolidated libdraw-owned runtime renderer state behind a single `libdraw_ctx` object instead of many separate globals.
  - Recent progress: shared audio state and volume settings now also live in `libdraw_ctx`, with gameplay/UI paths using accessor-backed reads instead of redundant `gamestate` mirrors.
  - Recent progress: top-level renderer orchestration now lives in `libdraw.cpp` rather than header-only entry points, reducing implementation-header coupling at the public API boundary.
  - Recent progress: sprite update/animation dispatch and spritegroup creation dispatch now also live in `libdraw.cpp`, reducing renderer helper churn in public headers.
  - Recent progress: scene-to-texture composition and gameplay floor-rendering helpers now also live in `libdraw.cpp`, leaving mostly leaf draw widgets in implementation headers.
  - [ ] eventually move away from implementation headers toward real `.cpp` files
  - [ ] evaluate migrating direct `try_entity_*` gameplay mutations toward an event-queue / action-resolution pipeline
    - Current candidates include `try_entity_attack`, `try_entity_move`, push/pull handling, door/chest toggles, pressure-plate updates, and other world-state mutations that presently happen inline.
    - Estimated work: medium-to-large refactor because it would cut across input handling, NPC turns, combat resolution, movement, animation timing, message generation, and tests.
    - Main benefit: deterministic ordered resolution for chained effects, so one action can fan out into queued follow-up events without burying rules in nested direct calls.
    - Main gameplay payoff: much better orchestration for cascading systems such as pressure plates, traps, forced movement, on-hit reactions, death triggers, scripted room logic, and future multi-step interactions.
    - Main engineering payoff: easier debugging/logging/replay of world-state transitions, clearer separation between intent and resolution, and lower risk of fragile ordering bugs.
    - Likely migration path: introduce a narrow event queue for one domain first, such as movement plus world triggers, then expand to combat and other interaction systems after the pattern is stable.

- [ ] Documentation
  - [x] continue Doxygen coverage on remaining core headers
    - Completed a broad Doxygen/documentation sweep across the current non-generated header set, including core runtime, renderer, UI, world, helper, and test aggregate headers.
    - `texture_ids.h` was intentionally left untouched per project rules.

- [ ] Leveling / progression
  - [ ] choose a proper level-up sound or music cue
  - [ ] decide what future non-player (or player) level-up rules should be beyond HP gain
  - [ ] add future progression systems such as skills / feats when ready



## Immediate New Changes To Make

- [x] Remove all props from floor 4's creation
  - Floor 4 prop spawning is now skipped in `place_props()` so the tutorial floor stays clean for pressure-plate setup.
- [x] Develop a `FLOOR_PRESSURE_PLATE` system where
  - [x] A heavy object such as a `pushable` or `pullable` must be on the tile that has the pressure plate
  - [x] When triggered, can be tied to some state such as a `ENTITY_DOOR` opening and remaining opened so long as the plate is activated
  - [x] When a `pullable` or `pushable` or `ENTITY_PLAYER` or `ENTITY_NPC` removes from the tile, then the plate is no longer triggered, so an opened door would close, etc. - some state would revert to its pre-trigger state
- [x] In one of the rooms on floor 4, add a new pressure plate tile and connect it to a door
  - [x] this door can only open if the pressure plate is activated
  - [x] the player can walk on top of the pressure plate and watch the door open
  - [x] the player can walk off the pressure plate and watch the door close
  - [ ] we remove all props so that we can force the player into learning about dragging dead bodies onto pressure plates
  - Floor 4 now gets a deterministic arrival-room tutorial setup with a linked pressure plate and door.

- [ ] save game
  - [ ] a lot of stuff goes here
- [ ] load game
  - [ ] a lot of stuff goes here


## Critical Note

- Do not ever tamper with `texture_ids.h` or `textures.txt`.
- `generate_texture_files.py` regenerates both files and they are used at game load to map each spritesheet/filepath to:
  - generated texture id
  - number of rows / contexts
  - number of columns / frames per context
- Spell texture entries may appear again in those generated files in the future when the script is rerun.
- Do not remove spell entries from those generated files in the future, even if active spell gameplay is currently absent.

Compact status handoff for the current C++ / raylib dungeon project.

## Done So Far

- Core refactor
  - `gamestate.h` now acts as the top-level coordinator with implementation headers split by domain.
  - Rendering was partially split out of the old monolith into smaller renderer-focused headers.
  - `hp` is now `vec2`, where `x = current hp` and `y = max hp`.
  - Entities now carry `alignment` and NPCs now carry `aggro`.

- Dungeon and world systems
  - Dungeon generation now uses connected room-and-corridor growth instead of the old one-room bootstrap.
  - Compact-map rules were tightened so `8x8` maps generate valid geometry.
  - Doors, props, boxes, chests, and dead-body pulling are back in active gameplay.
  - `get_random_loc()` now rejects occupied tiles correctly for gameplay spawning.
  - Restarting after death now returns to the title screen without recreating the window.

- Gameplay bootstrap
  - Gameplay currently boots 4 floors:
    - floor `0`: `8x8`
    - floor `1`: `16x16`
    - floor `2`: `24x24`
    - floor `3`: `16x16`
  - Floor `1` now spawns a passive `green slime`.
  - Floor `2` now spawns 9 passive level-1 `green slimes`.
  - Floor `3` now spawns 1 orc with a random existing weapon and a small healing potion.
  - Green slime racial modifiers are implemented as `-2` to all six attributes.
  - Slime kills now award `1 xp`.

- Interaction and UI
  - `KEY_E` interact/examine modal works for NPCs, props, boxes, chests, and doors.
  - NPC dialogue uses `dialogue_text`.
  - Help text was updated for the active control scheme.
  - A level-up window now appears at `10 xp`, lets the player choose one attribute, and applies a permanent `+1`.
  - Level-up rewards now also add one hit die worth of max HP via reusable hooks.
  - Permanent attribute increases were separated from level-up rewards so future item-based stat gains can reuse the same path.
  - A mini inventory / chest menu mode now exists with an options-menu toggle between `full` and `mini`.
  - Mini inventory / chest display is world-adjacent to the player and supports compact scrolling selection plus item preview/details.

- Prop interaction polish
  - Candle props are now pullable.
  - Wooden table props are now both pushable and pullable.

- Cleanup and testing
  - Active spellcasting runtime code was removed.
  - A real fast unit-test suite now lives under `test_suites/`.
  - Current fast test coverage includes lifecycle, bootstrap, placement, factories, inventory, tile/cache behavior, combat smoke tests, and world interaction.
  - A separate heavy test target now exists for longer deterministic simulations.
  - Recent added coverage includes dungeon connectivity, bounded combat invariants, renderer-adjacent seam tests, and heavy pathfinding/chase plus combat soak tests.
  - Recent added coverage now also includes compact inventory selection behavior plus pull/push interactions for newly movable props.
  - Core header documentation was brought up to date across the current non-generated header set with Doxygen file/type/helper coverage.
  - Recent renderer cleanup removed repeated renderer-global `extern` declarations across many `libdraw`-adjacent headers, consolidated them behind `libdraw_context.h`, and grouped libdraw-owned runtime renderer state into the shared `libdraw_ctx` object.

- open door/open chest input change from KEY_O to KEY_D

- keyboard layouts fully configurable
- window fg/bg colors fully configurable
- sound volume fully configurable
- mini damage numbers
- mini inventory display
- text interaction polish
- Player light radius does NOT render behind closed doors


## Current State

- Stable gameplay features in the active path
  - multi-floor dungeon generation
  - player and NPC spawning
  - melee combat
  - inventory / equipment
  - chest inventory transfer
  - doors and props
  - interaction modal
  - level-up modal

- Known recent verification baseline
  - `make clean && make tests && ./tests`
  - `make clean && make tests_heavy && ./tests_heavy`
  - `make clean && make game`


## Remaining Cleanup

- Spell / magic leftovers
  - active runtime spell paths are removed
  - dead spell texture assets and generated texture metadata were removed
  - remaining matches are intentional:
    - `magic_values.h` as a generic constants header
    - future-design notes in `MESSAGE.md`
    - soundtrack filenames such as `dungeon-magic.mp3` / `dungeon-spells.mp3`
    - archived `.old` snapshot files

## Test Backlog

- [x] Add longer bounded combat simulations with stable invariants.
  - Added a deterministic bounded melee duel test covering death/tile-cache/xp invariants.
- [x] Add stronger path/connectivity assertions for generated floors.
  - Added deterministic connectivity tests asserting each generated floor is a single connected walkable region and that compact-map stairs remain reachable.
- [x] Add more renderer-adjacent deterministic seam tests where practical.
  - Added deterministic seam tests for tile-to-texture mapping and sprite/spritegroup animation/context transitions.
- [x] Decide whether heavy simulation tests should live in a separate target from `make tests`.
  - Added a separate `tests_heavy` target that includes the fast suite plus long-running combat and pathfinding/chase simulation coverage.

## Notes

- `unit_test_old.h` remains the resurrection source for older large simulation tests.
- Future dungeon-quality gains should focus on connectivity and loops, not just more rooms.
- Recent gameplay work has been landing with tests first or tests alongside implementation; keep that pattern.
