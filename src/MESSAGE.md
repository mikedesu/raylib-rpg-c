Great job with the recent analysis on my game's attack logic. What's the biggest one thing you can see that I need to work on fixing or cleaning up next?
Focus your criticism on the file itself, and do not base your own criticism based on the notes below. We are looking to clean up the file, fix potential 
bugs, and prepare for the future. We will clean up other files like this as well.

I have already done the following:

- [x] re-ordered the attack roll and shield block checks and verified that they work as intended
- [x] unify attack sound handling in `try_entity_attack.h`
  - [x] created an `attack_result_t` to separate attack result types: HIT, MISS, BLOCK (to start with...DODGE, DEFLECT, etc may come later...)
  - [x] sound effect selected based on `attack_result_t`

Previous analysis suggestions you've brought up that I will eventually implement.

- [ ] Currently in process_attack_results(), the damage calculation doesn't account for
  - [ ] Target's equipped armor **WONT FIX: armor is currently un-implemented**
  - [ ] Damage reduction from armor (you have a damage_reduction component defined but it's not used) **WONT FIX: damage reduction currently un-implemented**
  - [ ] Armor durability loss (similar to weapon durability logic) **WONT FIX: armor is currently un-implemented**

My own thoughts derived from your analysis:

- [ ] Shields will need durability soon as well as durability reduction and breaking handled

