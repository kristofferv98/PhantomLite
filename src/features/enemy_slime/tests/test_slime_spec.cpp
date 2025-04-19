/// test_slime_spec.cpp — Unit tests for slime enemy specifications

#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "../enemy_slime.hpp"

TEST_CASE("Slime specification matches WORLDBUILDING.MD", "[enemy][slime][spec]") {
    const enemy::EnemySpec& slime = enemy::get_slime_spec();
    
    SECTION("Basic properties are correct") {
        REQUIRE(slime.id == enemy::EnemyID::FOR_SLIME);
        REQUIRE(slime.hp == 2);
        REQUIRE(slime.dmg == 1);
        REQUIRE(slime.speed == 60.0f);
    }
    
    SECTION("Size is correct for a 1x1 tile") {
        REQUIRE(slime.size.x == 32.0f);
        REQUIRE(slime.size.y == 32.0f);
    }
    
    SECTION("Behaviors are correct") {
        REQUIRE(slime.behaviors[0] == enemy::BehaviorAtom::wander_random);
        REQUIRE(slime.behaviors[1] == enemy::BehaviorAtom::chase_player);
    }
    
    SECTION("Drop chances are correct") {
        // Check heart drop (30%)
        auto heart_drop = std::find_if(slime.drops.begin(), slime.drops.end(),
            [](const enemy::DropChance& drop) { return drop.type == enemy::DropType::Heart; });
        REQUIRE(heart_drop != slime.drops.end());
        REQUIRE(heart_drop->chance == 30);
        
        // Check coin drop (70%)
        auto coin_drop = std::find_if(slime.drops.begin(), slime.drops.end(),
            [](const enemy::DropChance& drop) { return drop.type == enemy::DropType::Coin; });
        REQUIRE(coin_drop != slime.drops.end());
        REQUIRE(coin_drop->chance == 70);
    }
}

TEST_CASE("Enemy instance initialization", "[enemy][slime][instance]") {
    const enemy::EnemySpec& slime_spec = enemy::get_slime_spec();
    Vector2 test_pos = {100.0f, 100.0f};
    
    enemy::EnemyInstance slime(slime_spec, test_pos);
    
    SECTION("Instance properties are correctly initialized") {
        REQUIRE(slime.hp == slime_spec.hp);
        REQUIRE(slime.position.x == test_pos.x);
        REQUIRE(slime.position.y == test_pos.y);
        REQUIRE(slime.active);
    }
    
    SECTION("Collision rectangle is centered on position") {
        REQUIRE(slime.collision_rect.x == Approx(test_pos.x - slime_spec.size.x/2));
        REQUIRE(slime.collision_rect.y == Approx(test_pos.y - slime_spec.size.y/2));
        REQUIRE(slime.collision_rect.width == slime_spec.size.x);
        REQUIRE(slime.collision_rect.height == slime_spec.size.y);
    }
}

TEST_CASE("Enemy damage and death", "[enemy][slime][hit]") {
    enemy::init_enemies();
    
    Vector2 test_pos = {100.0f, 100.0f};
    enemy::spawn_slime(test_pos);
    
    // Create a hit in the same position
    Rectangle hit_rect = {test_pos.x - 16, test_pos.y - 16, 32, 32};
    enemy::Hit hit = {
        .dmg = 1,
        .knockback = {10.0f, 0.0f},
        .type = enemy::Hit::Type::Melee
    };
    
    SECTION("Hit detection works") {
        REQUIRE(enemy::hit_enemy_at(hit_rect, hit));
    }
    
    SECTION("Hit applies damage") {
        enemy::hit_enemy_at(hit_rect, hit);
        
        // Need second hit to kill
        REQUIRE(enemy::hit_enemy_at(hit_rect, hit));
        
        // Update to process removal of dead enemies
        enemy::update_enemies(0.016f);
        
        // Enemy should be gone after the second hit
        REQUIRE_FALSE(enemy::hit_enemy_at(hit_rect, hit));
    }
    
    enemy::cleanup_enemies();
}

TEST_CASE("Slime spec test", "[slime]") {
    // Add tests here
    REQUIRE(true);
} 

