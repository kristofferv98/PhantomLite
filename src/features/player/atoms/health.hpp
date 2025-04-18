// health.hpp
#pragma once
namespace player::atoms {
  struct Health {
    int current;
    int max;
  };
  Health make_health(int max_pips);
  bool apply_damage(Health& h, int pips);
  bool apply_heal(Health& h, int pips);
  bool is_alive(const Health& h);
} // namespace player::atoms 