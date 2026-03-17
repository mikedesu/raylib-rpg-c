# Project Plan

## Session Handoff

- Event-queue migration status:
  - queued and working in the current tree: movement, implicit push resolution, pull, manual door toggles, chest toggles, interaction/talk intents, pickup intents, hero item-use intents, hero inventory equip/unequip intents, hero inventory drop intents, chest transfer intents, stairs traversal, and attack intent
  - explicit queued follow-up events currently exist for pressure-plate refresh after movement, push, pull, and stairs traversal
  - attack intent now enters through the queue for both player and NPC adjacent attacks, and queued combat now fans out into explicit block, damage, death, XP reward, inventory drop, and player-death follow-up events
  - queued combat now also fans out into explicit NPC aggro/provoke and weapon/shield durability-loss follow-up events instead of resolving those side effects inline under attack resolution
  - queued pressure-plate refresh now also fans out into explicit linked-door open/close follow-up events instead of mutating those door states inline during queued processing
  - the old direct combat compatibility wrappers have now been removed; combat enters through queued attack intent and queued combat follow-up events only
  - legacy direct `handle_box_push` / `try_entity_move` / `update_pressure_plates_for_floor` and direct inventory/combat helpers still work for compatibility outside the queued pipeline
- Human-verified slices so far:
  - pull
  - open door
  - stairs traversal
  - chest toggles
- Current automated baseline:
  - `make tests && ./tests`
  - latest passing count before handoff: `167` tests
- Next session target:
  - human-verify the queued push and pressure-plate door-follow-up slices in live gameplay if that has not happened yet
  - continue combat migration with queued combat message-generation cleanup and any remaining downstream simplification now that the old direct combat wrappers are gone
  - likely next contained combat-only slices: tighten queued combat message-generation ownership and remove any stale combat-era notes/docs that still describe the deleted direct path
  - preserve the current gameplay contract while migrating, the same way earlier slices kept legacy `try_entity_*` helpers working during the transition
  - Event-queue test hardening:
    - [x] add queued pressure-plate close-path test after the last occupant leaves a linked plate
    - [x] add queued stairs traversal test that refreshes source and destination floors and updates both linked doors correctly
    - [x] add queued attack-miss test that proves no damage/death/durability side effects fire on a miss
    - [x] add queued lethal-hit-on-hero test that proves the queued player-death follow-up resolves cleanly
- Files most relevant for the next step:
  - `event_type.h`
  - `gamestate.h`
  - `gamestate_world_interaction_impl.h`
  - `gamestate_npc_combat_impl.h`
  - `test_suites/test_combat_bootstrap.h`

## START HERE

As a reminder, the proper way to build is: `make clean && CXXFLAGS="-DDEBUG_ASSERT=1 -DNPCS_ALL_AT_ONCE -DDEBUG=1 -DMASTER_VOLUME=1.0f " make game` for now...

**Do not ever tamper with `texture_ids.h` or `textures.txt`!**

## Top 5 Next Things

- [ ] Event-queue migration
  - [ ] MUST BE DONE INCREMENTALLY SO AS TO NOT BREAK ANYTHING DURING DEVELOPMENT
  - [ ] Each working piece will be tested by a real human
  - [ ] Upon completion, all new actions and events shall be built using the new events system
  - [ ] All new files, functions, classes, methods, etc. shall be properly documented conforming to existing standards
  - Recent progress:
    - Added a narrow turn-scoped gameplay event queue with documented payload/result structs.
    - Player movement input and NPC movement now queue `EVENT_MOVE_INTENT`, and successful queued movement fans out into `EVENT_REFRESH_PRESSURE_PLATES`.
    - Implicit push resolution that occurs during movement now queues `EVENT_PUSH_INTENT`, with queued push likewise fanning out into `EVENT_REFRESH_PRESSURE_PLATES`.
    - Player pull input now also queues `EVENT_PULL_INTENT`, with queued pull likewise fanning out into `EVENT_REFRESH_PRESSURE_PLATES`.
    - Player manual door toggles now queue `EVENT_OPEN_DOOR_INTENT`.
    - Player stairs traversal now queues `EVENT_TRAVERSE_STAIRS_INTENT`, with queued floor changes scheduling ordered pressure-plate refresh follow-ups for source and destination floors.
    - Player chest toggles now queue `EVENT_OPEN_CHEST_INTENT`.
    - Player interact/talk input now queues `EVENT_INTERACT_INTENT`.
    - Player pickup input now queues `EVENT_PICKUP_INTENT`.
    - Hero inventory item use now queues `EVENT_USE_ITEM_INTENT`, while the actual potion effect still resolves through the existing inventory helper behind that queued entrypoint.
    - Hero inventory equip/unequip now queues `EVENT_EQUIP_ITEM_INTENT`, while the actual weapon/shield toggle still resolves through the existing inventory helper behind that queued entrypoint.
    - Hero inventory drop now queues `EVENT_DROP_ITEM_INTENT`, while the actual inventory-to-floor transfer still resolves through the existing drop helper behind that queued entrypoint.
    - Chest deposit/withdraw confirm now queues `EVENT_CHEST_TRANSFER_INTENT`, while the actual inventory transfer still resolves through the existing chest/inventory helper behind that queued entrypoint.
    - Player and NPC adjacent attacks now queue `EVENT_ATTACK_INTENT`.
    - Queued attack intent now schedules explicit ordered `EVENT_PROVOKE_NPC`, `EVENT_ATTACK_BLOCK`, `EVENT_ATTACK_DAMAGE`, `EVENT_ATTACK_DEATH`, `EVENT_ATTACK_AWARD_XP`, `EVENT_ATTACK_DROP_INVENTORY`, `EVENT_ATTACK_PLAYER_DEATH`, `EVENT_ATTACK_WEAPON_DURABILITY`, and `EVENT_ATTACK_SHIELD_DURABILITY` follow-up events so more combat side effects no longer need to resolve inline under the queued entrypoint.
    - Legacy direct `try_entity_attack`, `process_attack_entity`, and `process_attack_results` combat compatibility helpers have now been deleted after the queued combat path absorbed their behavior.
    - Queued pressure-plate refresh now schedules explicit ordered door-state follow-up events for linked doors instead of toggling those doors inline during queued processing.
    - Queued push intent and queued pressure-plate door follow-up resolution now also have automated coverage through the new queue path.
    - Pull was verified by a real human after the queue migration slice landed.
    - Manual door toggles were verified by a real human after the queue migration slice landed.
    - Stairs traversal was verified by a real human after the queue migration slice landed.
    - Chest toggles were verified by a real human after the queue migration slice landed.
    - Interaction/talk intents have automated coverage through the queued path and still preserve the legacy direct helper.
    - Pickup intents now also have automated coverage through the queued path and still preserve the legacy direct helper.
    - Hero item-use intents now also have automated coverage through the queued path and still preserve the legacy direct potion helper.
    - Hero inventory equip/unequip intents now also have automated coverage through the queued path and still preserve the legacy direct equip/unequip helpers.
    - Hero inventory drop intents now also have automated coverage through the queued path and still preserve the legacy direct drop helpers.
    - Chest transfer intents now also have automated coverage through the queued path and still preserve the legacy direct transfer helper.
    - Deeper queued combat coverage now also exists for provoke/aggro follow-ups, queued weapon/shield durability break handling, and queued NPC inventory drops after death.
    - Legacy direct `try_entity_move` / `try_entity_pull` / `try_entity_open_door` / `try_entity_stairs` / `try_entity_open_chest` paths still work during the migration so existing helpers and tests are not forced over all at once.
    - Current automated verification for the migrated slice: `make tests && ./tests`.
  - [ ] Below are some of the example type of events that shall be converted over from their existing hardcoded forms:
  - [x] `try_entity_attack`
  - [x] `try_entity_push`
  - [x] open door
  - [x] pressure plate triggered
  - [x] talked with NPC
  - [x] went upstairs/downstairs
  - [ ] etc...
  - [ ] evaluate migrating direct `try_entity_*` gameplay mutations toward an event-queue / action-resolution pipeline
    - Current candidates include `try_entity_attack`, being damaged v.s. blocking, dying, `try_entity_move`, push/pull handling, door/chest toggles, pressure-plate updates, and other world-state mutations that presently happen inline.
    - Estimated work: medium-to-large refactor because it would cut across input handling, NPC turns, combat resolution, movement, animation timing, message generation, and tests.
    - Main benefit: deterministic ordered resolution for chained effects, so one action can fan out into queued follow-up events without burying rules in nested direct calls.
    - Main gameplay payoff: much better orchestration for cascading systems such as pressure plates, traps, forced movement, on-hit reactions, death triggers, scripted room logic, and future multi-step interactions.
    - Main engineering payoff: easier debugging/logging/replay of world-state transitions, clearer separation between intent and resolution, and lower risk of fragile ordering bugs.
    - Likely migration path: introduce a narrow event queue for one domain first, such as movement plus world triggers, then expand to combat and other interaction systems after the pattern is stable.
    - Current next suggested slice after movement + push/pull + manual door toggles + stairs traversal + chest toggles + combat intent + deeper combat follow-ups: human-verify queued push / pressure-plate door behavior in gameplay, then continue queued combat cleanup now that the direct combat wrappers are gone.

- [ ] Continue top-down `libdraw` cleanup and reduce remaining rendering global-state coupling.
  - Recent passes centralized renderer-global declarations through `libdraw_context.h`, removed repeated ad hoc `extern` declarations across draw/update headers, and routed `libdraw.h` scene dispatch through the compatibility include.
  - Latest pass also collapsed libdraw-owned process-lifetime renderer state into a single `libdraw_ctx` object instead of many separate globals in `main.cpp`, so spritegroups, texture metadata, shaders, render targets, and presentation rectangles now move through one shared renderer context.
  - Latest follow-up also folded shared audio state and volume settings into `libdraw_ctx`, removing the remaining duplicated `gamestate` mirrors for renderer-owned music/settings state.
  - Latest follow-up also moved the top-level renderer entry points out of header-only `static inline` implementations into a real `libdraw.cpp`, so `libdraw.h`/`libdraw_*` API headers are now declaration-first at the entry-point layer.
  - Latest follow-up also moved sprite lifecycle/update helpers and spritegroup factory dispatch out of implementation headers into `libdraw.cpp`, shrinking a large part of the renderer-side implementation-header surface.
  - Latest follow-up also moved scene composition helpers, gameplay-scene composition, dungeon-floor rendering, and the player target box into `libdraw.cpp`.
  - Latest follow-up also moved a broad set of gameplay-scene leaf draw helpers into `libdraw.cpp`, including HUD/info overlays, gameplay menus/prompts, and sprite/equipment draw layering.
  - Remaining cleanup now exists mostly in the larger scene/widget headers such as inventory/chest/title/character-creation and a few small utility draw helpers.

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
  - [ ] make sure that upstairs/downstairs tiles do not spawn where doors should go
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
  - Recent progress: many gameplay-scene leaf draw widgets now also live in `libdraw.cpp`, so the remaining implementation-header surface is concentrated in a smaller number of larger draw modules.
  - [ ] eventually move away from implementation headers toward real `.cpp` files
  - [ ] evaluate migrating direct `try_entity_*` gameplay mutations toward an event-queue / action-resolution pipeline
    - Current candidates include `try_entity_attack`, `try_entity_move`, push/pull handling, door/chest toggles, pressure-plate updates, and other world-state mutations that presently happen inline.
    - Estimated work: medium-to-large refactor because it would cut across input handling, NPC turns, combat resolution, movement, animation timing, message generation, and tests.
    - Main benefit: deterministic ordered resolution for chained effects, so one action can fan out into queued follow-up events without burying rules in nested direct calls.
    - Main gameplay payoff: much better orchestration for cascading systems such as pressure plates, traps, forced movement, on-hit reactions, death triggers, scripted room logic, and future multi-step interactions.
    - Main engineering payoff: easier debugging/logging/replay of world-state transitions, clearer separation between intent and resolution, and lower risk of fragile ordering bugs.
    - Likely migration path: movement + pressure-plate follow-ups, push/pull, manual door toggles, stairs traversal, chest toggles, attack intent, and deeper queued combat follow-ups are now queued; the next contained slice should be pruning or deleting the remaining direct-combat compatibility wrappers.

- [ ] Documentation
  - [x] continue Doxygen coverage on remaining core headers
    - Completed a broad Doxygen/documentation sweep across the current non-generated header set, including core runtime, renderer, UI, world, helper, and test aggregate headers.
    - `texture_ids.h` was intentionally left untouched per project rules.

## Immediate New Changes To Make

- [ ] Leveling / progression
  - [ ] choose a proper level-up sound or music cue
  - [ ] decide what future non-player (or player) level-up rules should be beyond HP gain
  - [ ] add future progression systems such as skills / feats when ready

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
    - floor `1`: `24x24`
    - floor `2`: `16x16`
    - floor `3`: `16x16`
  - Floor `1` now spawns a passive `green slime`.
  - Floor `2` now spawns 9 passive level-1 `green slimes`.
  - Floor `2` now hosts the pullables / pressure-plate tutorial room with two linked plates, a wooden sign, pullable props in both rooms, and a deterministic downstairs to Floor `3`.
  - Floor `2` now spawns 1 orc with a random existing weapon and a small healing potion.
  - Floor `3` is currently a placeholder `16x16` stone floor with props and boxes.
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
  - Turn action migration progress:
    - queued movement intent now resolves through the new gameplay-event queue
    - queued push intent now resolves through the same queue
    - queued pull intent now resolves through the same queue
    - queued manual door-toggle intent now resolves through the same queue
    - queued stairs-traversal intent now resolves through the same queue
    - queued chest-toggle intent now resolves through the same queue
    - queued interact/talk, pickup, hero item-use, hero equip/unequip, hero drop, and chest transfer intents now also resolve through the same queue
    - queued attack intent now also fans out into explicit block, damage, death, XP reward, inventory drop, player-death, provoke, and durability-loss follow-up events
    - legacy direct combat compatibility entrypoints now also delegate into the queued combat pipeline
    - queued movement, push, pull, and stairs traversal schedule pressure-plate refresh as explicit ordered follow-up events
    - queued pressure-plate refresh now fans out into explicit linked-door open/close follow-up events
    - queued pull migration has now been human-verified in gameplay
    - queued manual door-toggle migration has now been human-verified in gameplay
    - queued stairs-traversal migration has now been human-verified in gameplay
    - queued chest-toggle migration has now been human-verified in gameplay

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

- Pressure plates
- Floor 3 pressure plate + pullables tutorial

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
- Libdraw cleanup hand-off:
  - `libdraw.cpp` now also owns the inventory/chest/mini-inventory renderer cluster, shared item-detail panel/grid helpers, title screen, character-creation scene, debug-panel plumbing, gameplay-scene damage-popup drawing, and renderer-side gamestate-flag advancement.
  - One small renderer seam helper intentionally remains header-inline: `damage_popup_font_size_world()` in `draw_damage_numbers.h`, because `make tests` links deterministic seam tests without `libdraw.o`.
  - The main renderer-only implementation headers left are now mostly tiny leaf/support helpers such as `libdraw_update_weapon_for_entity.h`, `libdraw_update_shield_for_entity.h`, and `draw_version.h`, plus the existing debug-only frame-stats inline helpers.
  - Next pass should either finish those tiny renderer helpers in `libdraw.cpp` or split a small renderer-support `.cpp` if `libdraw.cpp` starts becoming awkward, but keep the work renderer-local and out of gameplay code.
  - Verification baseline after each renderer move remains: `make game`, `make tests`, `./tests`.
