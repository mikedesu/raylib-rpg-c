# GameState Refactor Plan

Compact handoff for active refactor and test work.

## Immediate Old Code Cleanup

- [ ] Remove ALL references to spells, magic-use, etc
  - [x] Goal
    - [x] This will return eventually, but to consolidate the existing codebase, I want to purge magic immediately
    - [x] Remove runtime spellcasting behavior, spell entities, spell rendering, spell ECS state, spell-specific tests, and obsolete spell assets/metadata references
    - [x] Keep non-spell combat and the rest of the entity/inventory/world-interaction work stable throughout
  - [x] Step 1: remove gameplay entry points first
    - [x] Delete the spell-cast action path in `gamestate_world_interaction_impl.h`
    - [x] Remove the `KEY_M` spell test/debug input hook
    - [x] Verify there is no remaining player or NPC path that can spawn or manipulate spells during gameplay
    - [x] Verification: `rg -n "try_entity_cast_spell|handle_test_cast_spell|KEY_M" .`
  - [x] Step 2: remove spell entity creation and lifecycle handling
    - [x] Remove `create_spell_with`, `create_spell_at_with`, and their declarations from `gamestate.h` and `gamestate_entity_factory_impl.h`
    - [x] Remove `update_spells_state()` and its call site from the main lifecycle update path
    - [x] Remove `ENTITY_SPELL` from shared entity-type plumbing once no callers remain
    - [x] Verification: `rg -n "create_spell|update_spells_state|ENTITY_SPELL" .`
  - [x] Step 3: remove spell-specific ECS component state
    - [x] Remove `spell.h`
    - [x] Remove spell-only component tags from `ComponentTraits.h`
    - [x] Remove spell-only includes from shared headers
    - [x] Decide whether generic `casting` survives; current expectation is to remove it too because it is only used by spellcasting right now
    - [x] Verification: `rg -n "spelltype|spellstate|spell_casting|spell_persisting|spell_ending|spell_complete|struct casting" .`
  - [x] Step 4: remove rendering and sprite wiring
    - [x] Remove spell spritegroup creation from `create_sg_byid.h`
    - [x] Remove spell animation state transitions from `libdraw_update_sprites.h`
    - [x] Remove spell texture-key includes from `tx_keys.h`
    - [x] Delete `tx_keys_spells.h` once no includes remain
    - [x] Verification: `rg -n "create_spell_sg_byid|tx_keys_spells|TX_SPELL|ENTITY_SPELL" .`
  - [ ] Step 5: clean shared enums, helper text, and tests
    - [x] Remove spell event enum values from `event_type.h` if they are otherwise unused
    - [x] Remove spell expectations from `test_suites/test_utility_helpers.h`
    - [ ] Remove or update any legacy notes/docs that describe spellcasting as active behavior
    - [ ] Verification: `rg -n "spell|magic" test_suites/ unit_test.h unit_test_old.h PLAN.md MESSAGE.md`
  - [ ] Step 6: clean asset/config leftovers
    - [ ] Remove spell texture references from `textures.txt` if they are no longer loaded
    - [ ] Remove spell texture ids if they become dead after the previous steps
    - [ ] Review `music.txt` entries such as `dungeon-magic.mp3` and `dungeon-spells.mp3`; only remove or rename them if they are specifically part of the obsolete spell system rather than general soundtrack naming
    - [ ] Verification: `rg -n "spell|magic" textures.txt texture_ids.h tx_keys*.h music.txt`
  - [ ] Step 7: final verification pass
    - [ ] Build and run tests after each stage, not only at the end
    - [ ] Final grep sweep should show no active spellcasting code paths
    - [ ] Verification:

```sh
make clean && make tests && ./tests
rg -n "spell|magic|cast" .
```

  - [ ] Known current touchpoints to retire
    - [x] `spell.h`
    - [x] `tx_keys_spells.h`
    - [x] `create_sg_byid.h`
    - [x] `libdraw_update_sprites.h`
    - [x] `gamestate_world_interaction_impl.h`
    - [x] `gamestate_entity_factory_impl.h`
    - [x] `gamestate_lifecycle_impl.h`
    - [x] `gamestate.h`
    - [x] `entitytype.h`
    - [x] `event_type.h`
    - [x] `ComponentTraits.h`
    - [x] `test_suites/test_utility_helpers.h`

## Immediate New Feature Development

- [ ] Leveling up
  - [ ] instead of an orc spawning on floor 2, lets make it spawn a `green_slime`.
    - [ ] green slimes are not-aggressive and will not attack the player until they are attacked
  - [ ] create a floor 3
  - [ ] floor 3 should be larger than floor 2
  - [ ] floor 3 should have 9 green slimes on it
  - [ ] each green slime spawned is level 1
  - [ ] green slimes have racial attribute modifiers like ELF, DWARF, etc.
  - [ ] killing each slime should reward 1 xp
    - [ ] green slimes have -2 to strength
    - [ ] green slimes have -2 to constitution
    - [ ] green slimes have -2 to dexterity
    - [ ] green slimes have -2 to charisma
    - [ ] green slimes have -2 to wisdom
    - [ ] green slimes have -2 to intelligence
  - [ ] create a floor 4
  - [ ] floor 4 should be approximately the same size as floor 2
  - [ ] floor 4 should have 1 orc spawn on it
  - [ ] the default orc spawns in with a randomly-selected weapon out of the existing weapon types (for now), and a small healing potion

- Interactions with non-aggressive entities
  - [x] Talk to NPCs
    - [x] first-pass `KEY_E` interaction opens a centered dialogue modal using one `dialogue_text` statement per NPC
    - [ ] dialog boxes
      - [x] portray of which character is talking displayed in the message box
      - [x] can contain multiple lines of text
      - [ ] must be centered horizontally
      - [ ] must be centered around 1/4 from the top of the screen

  - [x] Prop Descriptions
    - [x] first-pass `KEY_E` interaction opens a centered description modal using one text description per prop, box, chest, and door
    - [ ] Some descriptor / dialog box with information about the prop 
      - [ ] "This is a dirty mop!" etc
  - [x] Decide on a primary "interact" keypress
    - [x] first-pass generic interact/examine uses `KEY_E`
    - KEY_O is for "open"
    - KEY_PERIOD is for up/downstairs
    - KEY_A is for "attack"
    - KEY_S is for pickup item
    - etc
- [ ] Update the help menu text to reflect the new key press(es)


## Current State

- `gamestate.h` is now a top-level coordinator with implementation headers split by responsibility:
  - world/entity factory/lifecycle/scene/input/combat/world interaction/inventory
- `libdraw.h` was split into smaller renderer-focused headers, but rendering is still implementation-header based and still has global-state seams.
- Tile/entity storage is cache-based:
  - single-slot: player/live NPC, prop, box, door
  - fixed-capacity: dead NPCs, items
  - presentation/pickup still intentionally use the top cached item only
- Dungeon generation now uses connected room-and-corridor growth instead of the old one-big-room bootstrap.
- Small maps use dedicated compact-map rules; `8x8` now supports `2x2` rooms and zero-gap adjacency.
- Gameplay bootstrap currently builds 2 floors:
  - floor `0`: `8x8`
  - floor `1`: `16x16`
- Props are back in the active gameplay path and flow through tile caches, placement, rendering, and solidity checks.
- Doors are back in the active gameplay path:
  - generated candidate marking
  - entity placement
  - rendering
  - `KEY_O` open/close interaction
  - closed-door tile discovery without revealing tiles beyond the door
- Treasure chests are now in the active gameplay path:
  - dedicated `ENTITY_CHEST` tile caching and rendering
  - first-floor placement during gameplay bootstrap
  - `KEY_O` open/close interaction when facing the chest
  - open/closed sprite swapping
  - chest inventory UI with `ENTER` take and `TAB` deposit-mode toggle
  - inventory transfer between hero and chest
- Non-aggressive interaction modal is now in the active gameplay path:
  - `KEY_E` interact/examine when facing an entity
  - NPC dialogue uses one `dialogue_text` string per NPC for now
  - props, boxes, chests, and doors expose first-pass description text
  - modal shows the speaker/object name and supports multiline text
- Doxygen is the active documentation standard; `make docs` works.
- `hp` component is now a `vec2`.
- Entities now have `alignment`
- NPCs now have `aggression`
- Restarting the game after being killed now no longer restarts raylib and closes the window -- instead, it keeps it open and simply returns to the TitleScreen
- Treasure Chest entities now exist and are handled
- `dungeon_floor::get_random_loc()` now excludes occupied chest/player tiles so gameplay spawn selection matches its "empty placement tile" contract
- Player creation now retries on a fresh valid tile if the initially requested spawn location has become invalid

## Recent Completed Work

- Refactored `gamestate.h` and first-pass `libdraw.h` structure.
- Reintroduced and stabilized props, boxes, multi-item tiles, dead-body pulling, and quit confirmation.
- Added docs infrastructure plus first-pass docs for core headers.
- Built a real test suite under `test_suites/` while keeping `make tests` as the single fast runner.
- Re-enabled first-pass door generation/rendering/interaction and tightened tile-level single-door caching.
- Tightened prop placement so generated props avoid chokepoints and doorway approaches that can soft-block room ingress.
- Added first-pass non-aggressive interaction support for NPCs, props, boxes, chests, and doors via a dedicated modal on `KEY_E`.
- Removed active spellcasting runtime code, spell entities, spell ECS state, spell rendering hooks, and spell-specific utility/test coverage.
- Fixed gameplay-start player spawn selection so occupied chest tiles are no longer considered valid random spawn locations.
- Expanded unit coverage to 84 passing tests across:
  - gamestate lifecycle
  - dungeon/bootstrap
  - placement
  - entity factories
  - inventory/equipment
  - tile/cache helpers
  - component table
  - world interaction
  - combat/bootstrap smoke tests
- Recent bugs found and fixed by tests:
  - `init_dungeon()` skipped `assign_random_stairs()` in non-`DEBUG_ASSERT` builds
  - `logic_init()` had the same skipped stairs-assignment bug
  - `tile_has_door(vec3)` incorrectly used `current_floor` instead of `loc.z`
  - non-`NPCS_ALL_AT_ONCE` turn handling could route non-NPC ids into `handle_npc()` and throw `std::bad_optional_access`
  - generated closed doors were not rendered/discovered correctly on first sight because tile discovery treated the door tile itself as fully blocked
  - prop placement could soft-block room ingress by placing solid props on chokepoints or immediate doorway approaches
  - `tile_t` could accept a second `ENTITY_DOOR` cache entry on the same tile
  - lethal combat left the killed NPC in the tile's live-NPC cache while also adding it to the dead-body cache, which blocked movement onto corpse tiles until the live cache entry was cleared
  - `dungeon_floor::get_random_loc()` could return an occupied chest tile, which made gameplay player creation assert in `create_player_at_with()`

## Verification

- Desktop build has been repeatedly verified with:

```sh
make clean && CXXFLAGS="-DDEBUG_ASSERT=1 -DNPCS_ALL_AT_ONCE -DDEBUG=1 -DMASTER_VOLUME=1.0f -DMUSIC_OFF " make game
```

- Current unit-test verification:

```sh
make clean && make tests && ./tests
```

## High-Value Backlog

- [x] Re-enable door entity creation and placement
  - [x] implemented as a separate pass from dungeon floor creation, similar to how props are placed
  - [x] simple door placement at first
  - [ ] more intelligent door placement later
- [x] re-enable `try_entity_open_door` method when `KEY_O` is pressed


- [ ] Continue top-down `libdraw.h` cleanup
  - reduce remaining global-state coupling
  - tighten target/render ownership
  - revisit whether `libdraw_scene_dispatch.h` stays as compatibility glue
  - preserve all `loc.z`-sensitive renderer behavior

- [ ] Continue documentation pass
  - next likely headers:
    - [ ] `libdraw.h`
    - [ ] `libdraw_context.h`
    - [ ] `inputstate.h`
    - [ ] `gamestate_inventory.h`
    - [ ] `libgame_defines.h`
    - [ ] `character_creation.h`
    - [ ] `debugpanel.h`
    - [ ] `scene.h`
    - [ ] `controlmode.h`
    - [ ] `entity_actions.h`
    - [ ] `entitytype.h`
    - [ ] `entityid.h`
    - [ ] `item.h`
    - [ ] `weapon.h`
    - [ ] `shield.h`
    - [ ] `potion.h`
    - [ ] `spell.h`
    - [ ] `proptype.h`
    - [ ] `race.h`
    - [ ] `rarity.h`

- [ ] Tighten current `PROP` system
  - solid vs passable props needs more intentional rules
  - [x] first-pass chokepoint/doorway soft-lock avoidance
  - placement quality still needs broader layout-awareness beyond the first-pass chokepoint rule
  - keep banner textures out of the active floor-prop pool for now

- [ ] Preserve recent UI/layout fixes
  - help menu box sizing now measures real multiline content
  - inventory right-side detail text is `20`

## Unit Test Backlog

`unit_test_old.h` remains the resurrection source for larger gameplay-simulation tests. Active fast tests live under `test_suites/`.

- [ ] Legacy audit
  - [ ] classify each old test as migrate/update/split/retire

- [ ] Gamestate lifecycle
  - [x] reset/default invariants
  - [x] entity id allocation and dirty range
  - [x] hero assignment/reset
  - [x] message queue/history behavior
  - [x] camera reset behavior
  - [x] music reset/default behavior

- [ ] Dungeon/bootstrap
  - [x] floor-count semantics
  - [x] dimensions and spawn sanity
  - [x] compact-map `8x8` sanity
  - [x] multi-floor correctness
  - [x] upstairs/downstairs validity

- [ ] Placement
  - [x] `place_doors()` zero-on-empty
  - [x] deterministic positive door placement
  - [x] `place_props()` zero-on-empty
  - [x] deterministic positive prop placement
  - [x] `get_random_loc()` skips chest-occupied spawn tiles
  - [x] closed door tile is discoverable while sight beyond the door stays blocked
  - [x] tile cache rejects a second door on the same tile
  - [x] richer mixed occupancy behavior: boxes/props/doors/items/live NPCs/dead NPCs/player

- [ ] Entity factory shape
  - [x] dagger
  - [x] shield
  - [x] potion
  - [x] prop
  - [x] box defaults
  - [x] door defaults
  - [x] orc / monster init shape

- [ ] Inventory/equipment
  - [x] top-item pickup from stacked tiles
  - [x] inventory insert/remove bookkeeping
  - [x] weapon equip/unequip
  - [x] shield equip/unequip
  - [x] potion consumption
  - [x] equipped item drop behavior

- [ ] Gameplay interaction
  - [x] movement blocked by walls/doors/solid props
  - [x] push/pull behavior
  - [x] dead-body pull behavior via `dead_npc_cache`
  - [x] `loc.z`-sensitive interaction logic
  - [x] `KEY_O` door open/close behavior
  - [x] `KEY_E` NPC dialogue interaction
  - [x] `KEY_E` prop/box/chest/door description interaction
  - [x] interaction modal close/restore-control behavior
  - [x] moving onto a tile containing a freshly killed dead NPC

- [ ] Combat / simulation
  - [x] single-monster spawn sanity
  - [x] multi-monster uniqueness/count constraints
  - [x] `logic_init()` bootstrap sanity
  - [x] bounded tick/turn smoke test
  - [x] max-monster / too-many-monster boundary behavior
  - [ ] bounded 1v1 combat simulation with stable invariants
  - [ ] longer simulation tests over hundreds/thousands of turns

- [ ] Path/layout
  - [ ] corridor/connectivity checks
  - [ ] no disconnected player start
  - [x] first-pass door/prop placement no longer creates the obvious doorway soft-lock found this session
  - [ ] broader door/prop placement should not create obvious soft-locks

- [ ] ComponentTable
  - [x] set/get/has/remove/clear across multiple kinds
  - [x] shared-pointer storage behavior
  - [x] overwrite semantics
  - [x] high-churn multi-entity insert/remove coverage

- [ ] Renderer-adjacent deterministic seams
  - [ ] lifecycle init/teardown invariants
  - [ ] spritegroup creation honors type and `loc.z`
  - [ ] scene dispatch routing
  - [ ] frame-stat bookkeeping
  - [ ] render-target/bootstrap ownership assumptions

- [ ] Test policy
  - [x] fast deterministic tests stay in `make tests`
  - [x] suites are split by domain under `test_suites/`
  - [ ] decide whether heavy simulations need a separate target or flag

## Design Notes

- Dungeon generation should scale by room count/connectivity density, not by room footprint.
- `8x8` needs dedicated geometry rules; larger-map assumptions do not scale down cleanly.
- Future dungeon quality gains should focus on loops/interconnection, not just more density.
- World-interaction cleanup must preserve explicit dead-body cache handling and bounds checks in `try_entity_pull()`.
- NPC death transitions must move the entity from the tile's live-NPC slot into `dead_npc_cache`; corpse tiles remain walkable unless another solid/live blocker is present.
- Longer-term: move from implementation headers to real `.cpp` files once the build is ready for multiple translation units.

## Nice-To-Have Later

- Loop-making pass for dungeon generation
- L-corridor support
- explicit room/corridor region metadata
- smarter spawn-selection rules
- debug counters/visualization for generation quality
- safer web asset packaging for browser distribution
