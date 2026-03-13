# Project Plan

## Immediate New Changes To Make


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

- Cleanup and testing
  - Active spellcasting runtime code was removed.
  - A real fast unit-test suite now lives under `test_suites/`.
  - Current fast test coverage includes lifecycle, bootstrap, placement, factories, inventory, tile/cache behavior, combat smoke tests, and world interaction.
  - A separate heavy test target now exists for longer deterministic simulations.
  - Recent added coverage includes dungeon connectivity, bounded combat invariants, renderer-adjacent seam tests, and heavy pathfinding/chase plus combat soak tests.

- open door/open chest input change from KEY_O to KEY_D

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

## Active Backlog

- Leveling / progression
  - choose a proper level-up sound or music cue
  - decide what future non-player (or player) level-up rules should be beyond HP gain
  - add future progression systems such as skills / feats when ready

- Interaction polish
  - tighten interaction/dialog box layout so it is intentionally centered and positioned
  - improve description/dialog content beyond first-pass placeholder text

- Dungeon / world quality
  - improve door placement quality beyond the first-pass implementation
  - keep tightening prop placement so it cannot create broader layout soft-locks
  - consider smarter spawn-selection rules and better dungeon interconnection / loops

- Renderer / architecture
  - continue top-down `libdraw` cleanup
  - reduce remaining global-state coupling in rendering
  - eventually move away from implementation headers toward real `.cpp` files

- Documentation
  - continue Doxygen coverage on remaining core headers

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
