Great job with the recent analysis on my game's attack logic!

We are moving on to cleaning up `try_entity_move.h` next.

What's the biggest one thing you can see that I need to work on fixing or cleaning up next?

Focus your criticism on the file itself, and do not base your own criticism based on the notes below, except to sift out what NOT to report on. 
We are looking to clean up the file, fix potential bugs, and prepare for the future. We will clean up other files like this as well.

I have already done the following:

- [x] **FIXED** skeleton idle animation appears odd, flashes in-and-out
- [x] Shields now have durability and have durability reduced when successfully blocking
- [x] **FIXED** There is now a bug where taking lethal damage with a shield does not kill the player
- [x] magic values were created for MAX BLOCK CHANCE and MINIMUM DAMAGE so they arent hardcoded into try attack entity
- [x] There are three identical calls to process_attack_results() in process_attack_entity(), each marked with // <===== ############. 
  - [x] fixed and cleaned up
- [x] re-ordered the attack roll and shield block checks and verified that they work as intended
- [x] unify attack sound handling in `try_entity_attack.h`
  - [x] created an `attack_result_t` to separate attack result types: HIT, MISS, BLOCK (to start with...DODGE, DEFLECT, etc may come later...)
  - [x] sound effect selected based on `attack_result_t`
- [x] textures for zombie, rat, and skeleton
- [x] **FIXED** some NPC races' unarmed attack animations are incorrect
- [x] **FIXED** The bat's walk animation shouldn't be the attack animation

Previous analysis suggestions you've brought up that I will eventually implement, or ideas that I've realized I need to handle (**DO NOT BASED ANALYSIS ON THESE**)

- [ ] NPCs moving into tiles too far away to see kind of "disappear" instead of moving into the tile
- [ ] The way we handle race2str and str2race can be updated
  - [ ] we should be using a `map` instead of hard-coding strings into vectors
- [ ] **BUG** i just saw an NPC orc push a box under the player
- [ ] on game restart after dying, we need to clear the message history, as well as reset the gamestate in general
- [x] to prepare for unit testing and automation, we need to re-factor gamestate into a proper object
  - [x] initial gamestate class created
  - [ ] code from `liblogic` will be used to manage initializing the dungeon, etc
  - [ ] we will be able to set a flag indicating the gamestate is in a unit-test environment (so we can skip rendering and simply simulate the dungeon)
- [ ] break `process_attack_entity` and `process_attack_results` up into further functions
- [ ] Currently in `process_attack_results`, the damage calculation doesn't account for
  - [ ] Target's equipped armor **WONT FIX: armor is currently un-implemented**
  - [ ] Armor durability loss (similar to weapon durability logic) **WONT FIX: armor is currently un-implemented**
  - [ ] Damage reduction from armor (you have a damage_reduction component defined but it's not used) **WONT FIX: damage reduction currently un-implemented**
  - [ ] magic damage
    - [ ] fire
    - [ ] ice 
    - [ ] water
    - [ ] wind
    - [ ] electric
    - [ ] earth
    - [ ] holy
    - [ ] dark
  - [ ] damage types 
    - [ ] blunt
    - [ ] slashing
    - [ ] piercing
    - [ ] poison
  - [ ] damage to attributes and stats

