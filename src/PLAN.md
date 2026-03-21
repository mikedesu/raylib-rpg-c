# Project Plan

## New Task

- [x] `gamestate` now maintains an `std::unordered_map<int, std::set>` `floor_npcs` such that:
  - [x] Each `dungeon_floor`'s NPCs are stored in unique `set` objects
  - [x] Whenever we create an NPC on a certain `dungeon_floor`, we add their `entityid` to that `set`
  - [x] If an NPC (or the player) moves between floors, the floor-membership bookkeeping path is updated accordingly
  - [x] We changed how `update_npcs` and `handle_npcs` work:
    - [x] Instead of updating every NPC across every floor every turn, they now only process the NPCs on the current dungeon floor
      - [x] The initial end-goal of this process is because we can hear the orc on floor 3 opening and closing the door
      - [x] The major benefit to this is that in the future as the gamestate entity count grows, this should maintain a performant experience

## Working Notes

- **Do not ever tamper with `texture_ids.h` or `textures.txt`**
- `generate_texture_files.py` regenerates both files and they are used at game load to map each spritesheet/filepath to:
  - generated texture id
  - number of rows / contexts
  - number of columns / frames per context
  - Spell texture entries may appear again in those generated files in the future when the script is rerun.
  - Do not remove spell entries from those generated files in the future, even if active spell gameplay is currently absent.
- Build command for now: 
```
make clean && CXXFLAGS="-DDEBUG_ASSERT=1 -DNPCS_ALL_AT_ONCE -DDEBUG=1 -DMASTER_VOLUME=1.0f " make game
```

- As you accomplish individual task items, you will mark them off:
```
  - [ ] unaccomplished
  - [x] accomplished
```

- If you have to break a task down or leave a task incomplete:
```
  - [ ] unaccomplished
    - [ ] incomplete task current status
  - [x] accomplished
```

- When a UI text block looks garbled or illegible, check for rows being stacked too tightly with `DrawText`; the Raylib default font needs line heights larger than font size so descenders do not overdraw adjacent lines.
- Whenever we implement a new feature, we need to implement corresponding unit test functions that properly check for individual scenarios, including most common inputs and edge cases, and verify that the unit tests build and pass
- If a new feature requires simulating a full instance of a game, it goes in `test_suites/test_heavy_simulation.h`


## Event-Queue Migration

Current status:
- The queue path is already active for movement, push, pull, manual door toggles, chest toggles, interaction/talk, pickup, hero item use, hero inventory equip/unequip, hero inventory drop, chest transfer, stairs traversal, and attack intent.
- Queued follow-up events already exist for pressure-plate refresh, linked-door state changes, combat block/damage/death/XP/drop/player-death, NPC aggro/provoke, and weapon/shield durability loss.
- Human-verified queue slices so far: push, pull, pressure-plate door follow-up, open door, stairs traversal, and chest toggles.
- Current automated verification baseline: `make tests && ./tests` with `170` passing tests at the last recorded handoff.
- The migration is no longer a hard blocker, but there is still cleanup and final evaluation to do before calling it intentionally paused or complete.

Active items:
- [ ] Event-queue migration
  - [ ] MUST BE DONE INCREMENTALLY SO AS TO NOT BREAK ANYTHING DURING DEVELOPMENT
  - [ ] Each working piece will be tested by a real human
  - [ ] Upon completion, all new actions and events shall be built using the new events system
  - [ ] All new files, functions, classes, methods, etc. shall be properly documented conforming to existing standards
  - [ ] Continue converting remaining hardcoded gameplay mutations/events into the event-queue / action-resolution pipeline where it still makes sense
  - [ ] evaluate whether queued inventory/item/chest helper-backed intents should keep their legacy direct helper implementations or move to queue-native resolvers
  - [ ] evaluate whether remaining legacy direct movement/world helpers (`handle_box_push`, `try_entity_move`, `update_pressure_plates_for_floor`, etc.) should stay as compatibility seams or be further collapsed behind queue-owned entrypoints

## Bugs To Fix

- [x] Upstairs/downstairs tiles should not spawn where a door should go
  - [x] When arriving on Floor 2 from Floor 1, I am sometimes appearing on a tile that joins 2 rooms
    - [x] This tile would be a perfect place for a door, but instead, there is a upstairs tile
    - [x] Doors should obviously not be created on top of upstairs/downstairs tiles
    - [x] Tiles that contain doors should not have their types changed to upstairs or downstairs
    - [x] Doors should be created before assignment of upstairs or downstairs or any other exit tiles (yet to be created...)

## Backlog Of Tasks

- [ ] Renderer / architecture
  - [ ] continue top-down `libdraw` cleanup
  - [ ] reduce remaining global-state coupling in rendering
  - [ ] eventually move away from implementation headers toward real `.cpp` files
- [ ] Continue top-down `libdraw` cleanup and reduce remaining rendering global-state coupling.
- [ ] Dungeon / world quality
  - [ ] make sure that upstairs/downstairs tiles do not spawn where doors should go
  - [ ] improve door placement quality beyond the first-pass implementation
  - [ ] keep tightening prop placement so it cannot create broader layout soft-locks
  - [ ] consider smarter spawn-selection rules and better dungeon interconnection / loops
- [ ] Leveling / progression
  - [ ] choose a proper level-up sound or music cue
  - [ ] decide what future non-player (or player) level-up rules should be beyond HP gain
  - [ ] add future progression systems such as skills / feats when ready
- [ ] save game
  - [ ] a lot of stuff goes here
- [ ] load game
  - [ ] a lot of stuff goes here
- [ ] Documentation

## Future Work

- [ ] Multi-dimensional dungeons
  - [ ] Dungeons are currently a vector of dungeon_floor objects
  - [ ] To facilitate the addition of multiple "exits" to a dungeon_floor, we need to consider how best to map each exit
    - [ ] At the moment, our dungeon floors have a correlation between floor number and "what floor/zone is this?"...what if one floor has two exits: a downstairs, and a door into a small shack?
    - [ ] How do we best represent such a `zone`? "world_zone"?
      - [ ] world: collection of `world_zone`s
      - [ ] world_zone: previously `dungeon`
      - [ ] world_zone_area: previously `dungeon_floor`
