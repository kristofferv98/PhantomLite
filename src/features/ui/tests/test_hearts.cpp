/// test_hearts.cpp — Unit tests for the hearts UI atom

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch_all.hpp>
#include "../atoms/hearts.hpp"

TEST_CASE("Hearts can be initialized with default values", "[hearts]") {
    ui::Hearts hearts;
    REQUIRE(hearts.get_max_pips() == 12);
    REQUIRE(hearts.get_current_pips() == 12);
}

TEST_CASE("Hearts can be initialized with custom values", "[hearts]") {
    ui::Hearts hearts(10, 5);
    REQUIRE(hearts.get_max_pips() == 10);
    REQUIRE(hearts.get_current_pips() == 5);
}

TEST_CASE("Hearts can take damage", "[hearts]") {
    ui::Hearts hearts(10, 10);
    hearts.take_damage(3);
    REQUIRE(hearts.get_current_pips() == 7);
}

TEST_CASE("Hearts cannot go below zero", "[hearts]") {
    ui::Hearts hearts(10, 5);
    hearts.take_damage(10);
    REQUIRE(hearts.get_current_pips() == 0);
}

TEST_CASE("Hearts can be healed", "[hearts]") {
    ui::Hearts hearts(10, 5);
    hearts.heal(3);
    REQUIRE(hearts.get_current_pips() == 8);
}

TEST_CASE("Hearts cannot exceed max health", "[hearts]") {
    ui::Hearts hearts(10, 5);
    hearts.heal(10);
    REQUIRE(hearts.get_current_pips() == 10);
}

TEST_CASE("Hearts health percentage is calculated correctly", "[hearts]") {
    ui::Hearts hearts(10, 5);
    REQUIRE(hearts.get_health_percent() == 0.5f);
    
    hearts.take_damage(5);
    REQUIRE(hearts.get_health_percent() == 0.0f);
    
    hearts.heal(10);
    REQUIRE(hearts.get_health_percent() == 1.0f);
} 