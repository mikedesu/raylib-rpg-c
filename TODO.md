# TODO

- [ ] create infrastructure for A/B testing of a specific game logic
    - [ ] individual NPC turn rendering (individual NPC turns render one-at-a-time)
    - [ ] mass NPC turn rendering (all NPCs' turns render simultaneously)

- [ ] enemies are currently attacking each other randomly (shouldn't happen)

- [ ] wolf shield animation bugfix
- [ ] elf shield animation bugfix
- [ ] dwarf shield animation bugfix
- [ ] slime shield animation bugfix
- [ ] bat walk animation bugfix
- [ ] bat shield animation bugfix (CRASHING)
- [ ] warg shield animation bugfix (CRASHING)

- [x] drawing order needs changing so that NPCs (warg etc) do not render behind door sprites when adjacent
- [x] drawing order needs changing so that player target box renders beneath if player is a warg etc

- [x] when handling light radius and visibility distance, we need to check line-of-sight so we don't render
things that might be behind doors, walls, etc.

- [x] Shader so that the target box glows

- [ ] fix web shaders
