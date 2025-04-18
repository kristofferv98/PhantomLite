// health.cpp
#include "health.hpp"
#include <algorithm>

namespace player::atoms {

Health make_health(int max_pips) {
  return {max_pips, max_pips};
}

bool apply_damage(Health& h, int p) {
  int previous_health = h.current;
  h.current = std::max(0, h.current - p);
  return previous_health > 0; // Return true if damage was applied (was alive)
}

bool apply_heal(Health& h, int p) {
  int previous_health = h.current;
  h.current = std::min(h.max, h.current + p);
  return previous_health < h.max; // Return true if healing occurred
}

bool is_alive(const Health& h) {
  return h.current > 0;
}

} // namespace player::atoms 