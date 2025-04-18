/// test_health.cpp — Unit tests for the player health atom

#include <catch2/catch_all.hpp>
#include "../atoms/health.hpp"

TEST_CASE("Health atom initialization", "[player][atom][health]") {
    player::atoms::Health health = player::atoms::make_health(10);
    REQUIRE(health.max == 10);
    REQUIRE(health.current == 10);
    REQUIRE(player::atoms::is_alive(health));
}

TEST_CASE("Health atom applying damage", "[player][atom][health]") {
    player::atoms::Health health = player::atoms::make_health(10);
    
    SECTION("Applying partial damage") {
        bool was_alive = player::atoms::apply_damage(health, 3);
        REQUIRE(was_alive); // Should have been alive before damage
        REQUIRE(health.current == 7);
        REQUIRE(player::atoms::is_alive(health));
    }
    
    SECTION("Applying exact lethal damage") {
        bool was_alive = player::atoms::apply_damage(health, 10);
        REQUIRE(was_alive);
        REQUIRE(health.current == 0);
        REQUIRE_FALSE(player::atoms::is_alive(health));
    }
    
    SECTION("Applying excessive damage") {
        bool was_alive = player::atoms::apply_damage(health, 15);
        REQUIRE(was_alive);
        REQUIRE(health.current == 0);
        REQUIRE_FALSE(player::atoms::is_alive(health));
    }
    
    SECTION("Applying damage when already dead") {
        player::atoms::apply_damage(health, 10); // Kill the player
        bool was_alive = player::atoms::apply_damage(health, 5); // Apply more damage
        REQUIRE_FALSE(was_alive); // Should have been dead
        REQUIRE(health.current == 0);
        REQUIRE_FALSE(player::atoms::is_alive(health));
    }
}

TEST_CASE("Health atom applying healing", "[player][atom][health]") {
    player::atoms::Health health = player::atoms::make_health(10);
    player::atoms::apply_damage(health, 5); // Take some damage first
    REQUIRE(health.current == 5);
    
    SECTION("Applying partial healing") {
        bool did_heal = player::atoms::apply_heal(health, 3);
        REQUIRE(did_heal);
        REQUIRE(health.current == 8);
        REQUIRE(player::atoms::is_alive(health));
    }
    
    SECTION("Applying exact full healing") {
        bool did_heal = player::atoms::apply_heal(health, 5);
        REQUIRE(did_heal);
        REQUIRE(health.current == 10);
        REQUIRE(player::atoms::is_alive(health));
    }
    
    SECTION("Applying excessive healing") {
        bool did_heal = player::atoms::apply_heal(health, 10);
        REQUIRE(did_heal);
        REQUIRE(health.current == 10); // Should cap at max
        REQUIRE(player::atoms::is_alive(health));
    }
    
    SECTION("Applying healing when already full") {
        player::atoms::apply_heal(health, 10); // Heal to full
        bool did_heal = player::atoms::apply_heal(health, 2); // Try to heal more
        REQUIRE_FALSE(did_heal);
        REQUIRE(health.current == 10);
        REQUIRE(player::atoms::is_alive(health));
    }
} 