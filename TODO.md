# TODO

- [ ] BUG death animation for player stuck on first frame
- [ ] BUG when going downstairs/upstairs, it is possible to be "on top" of another entity
    - [ ] when you walk off of the tile, the entity acts
    - [ ] unobserved what happens if you do not walk off
    - [ ] FIX NPCs should not be able to walk onto a stairs tile in general probably
- [ ] BUG NPCs appear to be able to walk through doors
- [ ] BUG shield direction context appears to be incorrect for the up-right direction
    - [ ] observed on a few races but not all
- [ ] wolf shield animation bugfix
- [ ] slime shield animation bugfix
- [ ] bat walk animation bugfix
- [ ] bat shield animation bugfix (CRASHING)
- [ ] warg shield animation bugfix (CRASHING)
- [ ] fix web shaders

- [ ] magic system
- [ ] more NPC races
- [ ] more accounting for stats etc in rolls
- [ ] armor
    - [ ] head
    - [ ] chest 
    - [ ] hands 
    - [ ] feet
    - [ ] rings
    - [ ] amulets
- [ ] critical hits
- [ ] humidity
- [ ] air quality
- [ ] poison
- [ ] trap tiles
- [ ] switches
- [ ] circuits
- [ ] stealing
- [ ] invisibility
- [ ] potions
- [ ] sneaking
- [ ] intelligent NPC action selection
- [ ] torches
- [ ] sokoban mechanics (re-introduction)
- [ ] item creation
- [ ] spell creation

- [ ] Automatic generation of texture_ids by filename similar to how sound effects works

--------------------------------------------------------------------------------

- [x] create infrastructure for A/B testing of a specific game logic
    - [x] individual NPC turn rendering (individual NPC turns render one-at-a-time)
    - [x] mass NPC turn rendering (all NPCs' turns render simultaneously)

- [x] enemies are currently attacking each other randomly (shouldn't happen)
  - [x] was adjacency check 

- [x] elf shield animation bugfix
- [x] dwarf shield animation bugfix
- [x] orc shield animation bugfix
- [x] goblin shield animation bugfix
- [x] halfling shield animation bugfix

- [x] drawing order needs changing so that NPCs (warg etc) do not render behind door sprites when adjacent
- [x] drawing order needs changing so that player target box renders beneath if player is a warg etc

- [x] when handling light radius and visibility distance, we need to check line-of-sight so we don't render
things that might be behind doors, walls, etc.

- [x] Shader so that the target box glows

