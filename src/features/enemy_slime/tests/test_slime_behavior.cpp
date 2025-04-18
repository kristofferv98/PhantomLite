/// test_slime_behavior.cpp — Tests for slime enemy behavior

#include <catch2/catch_all.hpp>
#include "../atoms/behavior_atoms.hpp"
#include "../enemy_slime.hpp"

// Mock the player position for testing
Vector2 mock_player_position = {0, 0};

Vector2 get_player_position() {
    return mock_player_position;
}

TEST_CASE("Slime wandering behavior", "[enemy][slime][behavior][wander]") {
    const enemy::EnemySpec& slime_spec = enemy::get_slime_spec();
    Vector2 start_pos = {100.0f, 100.0f};
    enemy::EnemyInstance slime(slime_spec, start_pos);
    
    // Override wander parameters for testing
    slime.wander.radius = 50.0f;
    slime.wander.idle_time = 0.5f;
    slime.wander.has_target = false;
    slime.wander.current_timer = 0.0f;
    
    // First call should set a target
    enemy::BehaviorResult result = enemy::atoms::wander_random(slime, 0.1f);
    
    SECTION("Wander sets a target when needed") {
        REQUIRE(slime.wander.has_target);
        REQUIRE(result == enemy::BehaviorResult::Running);
    }
    
    SECTION("Wander moves toward target") {
        Vector2 initial_pos = slime.position;
        
        // Force a specific target
        slime.wander.target = {initial_pos.x + 20.0f, initial_pos.y + 20.0f};
        slime.wander.has_target = true;
        
        // Update a few times to simulate movement
        for (int i = 0; i < 10; i++) {
            enemy::atoms::wander_random(slime, 0.1f);
        }
        
        // Should have moved toward target
        REQUIRE(slime.position.x > initial_pos.x);
        REQUIRE(slime.position.y > initial_pos.y);
    }
    
    SECTION("Wander completes when target reached") {
        // Set target very close to current position to ensure it's reached quickly
        slime.wander.target = {slime.position.x + 1.0f, slime.position.y + 1.0f};
        slime.wander.has_target = true;
        
        // Update until completed
        enemy::BehaviorResult result;
        for (int i = 0; i < 10; i++) {
            result = enemy::atoms::wander_random(slime, 0.1f);
            if (result == enemy::BehaviorResult::Completed) break;
        }
        
        REQUIRE(result == enemy::BehaviorResult::Completed);
        REQUIRE_FALSE(slime.wander.has_target);
        REQUIRE(slime.wander.current_timer > 0.0f); // Should be idling
    }
}

TEST_CASE("Slime chasing behavior", "[enemy][slime][behavior][chase]") {
    const enemy::EnemySpec& slime_spec = enemy::get_slime_spec();
    Vector2 start_pos = {100.0f, 100.0f};
    enemy::EnemyInstance slime(slime_spec, start_pos);
    
    // Set chase parameters
    slime.chase.detection_radius = 100.0f;
    slime.chase.chasing = false;
    
    SECTION("Chase activates when player in range") {
        // Place player in detection range
        mock_player_position = {150.0f, 150.0f};
        
        enemy::BehaviorResult result = enemy::atoms::chase_player(slime, 0.1f);
        
        REQUIRE(slime.chase.chasing);
        REQUIRE(result == enemy::BehaviorResult::Running);
    }
    
    SECTION("Chase moves toward player") {
        // Place player in detection range
        mock_player_position = {150.0f, 150.0f};
        slime.chase.chasing = true;
        
        Vector2 initial_pos = slime.position;
        
        // Update a few times to simulate movement
        for (int i = 0; i < 10; i++) {
            enemy::atoms::chase_player(slime, 0.1f);
        }
        
        // Should have moved toward player
        REQUIRE(slime.position.x > initial_pos.x);
        REQUIRE(slime.position.y > initial_pos.y);
    }
    
    SECTION("Chase deactivates when player out of range") {
        // Start chasing
        slime.chase.chasing = true;
        
        // Place player far away
        mock_player_position = {500.0f, 500.0f};
        
        // Update until chase stops
        enemy::BehaviorResult result;
        for (int i = 0; i < 10; i++) {
            result = enemy::atoms::chase_player(slime, 0.1f);
            if (result == enemy::BehaviorResult::Failed) break;
        }
        
        REQUIRE(result == enemy::BehaviorResult::Failed);
        REQUIRE_FALSE(slime.chase.chasing);
    }
}

TEST_CASE("Slime behavior test", "[slime]") {
    // Add tests here
    REQUIRE(true);
} 