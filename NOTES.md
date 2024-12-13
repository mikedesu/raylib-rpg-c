# Notes 12-12-2024

- animation stacking
    - i added an array that allows me to add animations per spritegroup 
    - so, if a sprite is to have multiple animations in one turn, we can add them and then iterate the animations
    - the animations themselves are just integers, so it should be straight-forward
    - but first, i probably need to re-do how we process turns right now:
        - currently, we process each action one at a time
        - what we want is to process ALL actions, and then animate them via the actionresults table in the gamestate
            - by iterating the action results table, we should be able to add animations to all the sprites, and then animate them all at once in a way that 
                - any pending animations get processed in sequence, and then the sprite returns to a default state

