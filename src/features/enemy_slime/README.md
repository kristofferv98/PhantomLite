# Enemy Slime Slice

The enemy_slime slice implements the Forest Slime enemy type with its behaviors, animations, and combat interactions.

## Context Primer

This slice contains everything related to the Forest Slime enemies:

- Behavior atoms (wander, chase, attack)
- Animation system for slime movement
- Collision detection with player
- Health and damage handling
- Drops system (hearts, coins)
- Debug visualization tools

The slime uses a behavior-based AI system where different atomic behaviors (wander, chase, attack) are composed to create more complex enemy interactions. Slimes detect the player within a certain radius, chase when detected, and attack when in range. 

