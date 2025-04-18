/// types.cpp — Implementation of common enemy data structures

#include "types.hpp"

namespace enemies {

// Constructor implementation for EnemyRuntime
EnemyRuntime::EnemyRuntime(const EnemyStats& spec_ref, Vector2 pos)
    : spec(&spec_ref), 
      position(pos), 
      hp(spec_ref.hp), 
      collision_rect({pos.x - spec_ref.size.x/2, pos.y - spec_ref.size.y/2, spec_ref.size.x, spec_ref.size.y}),
      color(WHITE),
      facing(Facing::DOWN),
      wander(100.0f, 1.0f),  // Wander within 100px, idle for 1s
      active(true),
      anim_timer(0.0f),
      anim_frame(0),
      is_moving(false) {
    
    // Initialize chase parameters with values from spec
    chase.detection_radius = spec_ref.detection_radius;
    chase.chasing = false;
    
    // Initialize attack parameters with values from spec
    attack.attack_radius = spec_ref.attack_radius;
    attack.cooldown = spec_ref.attack_cooldown;
    attack.timer = 0.0f;
    attack.can_attack = true;
    attack.attacking = false;
}

} // namespace enemies 