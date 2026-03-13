# GameState Refactor Plan

Compact handoff for active refactor and test work.

## Immediate New Feature Development

- Treasure Chest entities
  - [ ] Have texture keys 
    - TX_CHEST_OPEN_00
    - TX_CHEST_CLOSED_00
  - [ ] Function like an inventory that doesn't belong to an NPC
    - [ ] Is still a real physical entity
      - ENTITY_CHEST or ENTITY_TREASURE_CHEST is fine
      - [ ] Has a location
      - [ ] Has HP/MaxHP
      - [ ] Can be pushed
      - [ ] Can be pulled
      - [ ] Can be opened with KEY_O like a door
        - [ ] Upon open, the sprite context should change from TX_CHEST_CLOSED_00 to TX_CHEST_OPEN_00, and a display similar to "display inventory" will appear.
          - [ ] On the left hand side is a grid of items inside the chest, if any
          - [ ] On the right hand side is an information panel containing details about the currently selected item in the chest
          - [ ] Items cannot be dropped out of the chest
          - [ ] Items can be taken from the chest by pressing KEY_ENTER when they are selected 
            - [ ] Currently there is no bound on how many items a player or any entity can have in their inventory
          - [ ] A key press (undecided) from this display can switch us to our own inventory so that we may deposit an item from our own inventory into the treasure chest
      - [ ] there may be other as-yet undecided qualities to a chest but this is good for now
- [ ] once Treasure Chest and its associated functions/methods are implemented, we want to create a new, empty treasure chest on the 1st floor.
  - [ ] it should NOT occupy an upstairs or downstairs tile
  - [ ] it should not overlap with any other entity
  - [ ] if we need to, we need to update `dungeon_tile` to accomodate a treasure chest's entity id
  - [ ] draw_dungeon_floor should accomodate the new entitytype


- Interactions with non-aggressive entities
  - NPCs
    - Talking / dialog boxes
  - PROPS
    - Some descriptor / dialog box with information about the prop 
      - "This is a dirty mop!" etc



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
- Doxygen is the active documentation standard; `make docs` works.
- `hp` component is now a `vec2`.
- Entities now have `alignment`
- NPCs now have `aggression`
- Restarting the game after being killed now no longer restarts raylib and closes the window -- instead, it keeps it open and simply returns to the TitleScreen

## Recent Completed Work

- Refactored `gamestate.h` and first-pass `libdraw.h` structure.
- Reintroduced and stabilized props, boxes, multi-item tiles, dead-body pulling, and quit confirmation.
- Added docs infrastructure plus first-pass docs for core headers.
- Built a real test suite under `test_suites/` while keeping `make tests` as the single fast runner.
- Re-enabled first-pass door generation/rendering/interaction and tightened tile-level single-door caching.
- Tightened prop placement so generated props avoid chokepoints and doorway approaches that can soft-block room ingress.
- Expanded unit coverage to 57 passing tests across:
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
  - [x] closed door tile is discoverable while sight beyond the door stays blocked
  - [x] tile cache rejects a second door on the same tile
  - [x] richer mixed occupancy behavior: boxes/props/doors/items/live NPCs/dead NPCs/player

- [ ] Entity factory shape
  - [x] dagger
  - [x] shield
  - [x] potion
  - [x] prop
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
