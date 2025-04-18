#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../atoms/hearts.hpp"

TEST_CASE("Hearts atom basic functionality", "[ui][hearts]") {
    ui::Hearts hearts(12); // 3 hearts (12 pips)
    
    SECTION("Initial state") {
        REQUIRE(hearts.get_current_pips() == 12);
        REQUIRE(hearts.get_max_pips() == 12);
        REQUIRE(hearts.is_alive() == true);
    }
    
    SECTION("Taking damage") {
        hearts.take_damage(4); // Lose 1 heart
        REQUIRE(hearts.get_current_pips() == 8);
        REQUIRE(hearts.is_alive() == true);
        
        hearts.take_damage(7); // Lose almost 2 hearts
        REQUIRE(hearts.get_current_pips() == 1);
        REQUIRE(hearts.is_alive() == true);
        
        hearts.take_damage(1); // Just enough to die
        REQUIRE(hearts.get_current_pips() == 0);
        REQUIRE(hearts.is_alive() == false);
        
        // Can't go below 0
        hearts.take_damage(5);
        REQUIRE(hearts.get_current_pips() == 0);
    }
    
    SECTION("Healing") {
        hearts.take_damage(8); // Down to 4 pips
        REQUIRE(hearts.get_current_pips() == 4);
        
        hearts.heal(3); // Up to 7 pips
        REQUIRE(hearts.get_current_pips() == 7);
        
        hearts.heal(10); // Trying to heal beyond max
        REQUIRE(hearts.get_current_pips() == 12); // Capped at max_pips
    }
    
    SECTION("Edge cases") {
        // Test healing from 0
        hearts.take_damage(20); // Ensure at 0
        REQUIRE(hearts.get_current_pips() == 0);
        
        hearts.heal(1);
        REQUIRE(hearts.get_current_pips() == 1);
        REQUIRE(hearts.is_alive() == true);
    }
} 