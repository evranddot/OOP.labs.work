#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Building.h"
#include "ResourceSystem.h"

struct Technology {
    std::string name;
    int remaining_turns;
    bool completed = false;
    int attack_bonus = 0;
    int armor_bonus = 0;
};

class BuildingManager {
private:
    std::vector<std::unique_ptr<Building>> buildings_;
    std::vector<Technology> researches_;

public:
    BuildingManager() = default;


    Building* buildBuilding(std::unique_ptr<Building> building);
    bool startResearch(const std::string& tech_name);
    std::unique_ptr<Unit> produceUnit(const std::string& building_type, const std::string& unit_type, int spawn_x, int spawn_y);
    Building* getBuildingById(int id);

    bool has_building(const std::string& name, bool must_be_ready = true) const;
    Building* find_building_at(int x, int y);
    Hospital* find_ready_hospital();


    std::vector<std::string> tick_turn(ResourceSystem& resources);

    int get_total_attack_bonus() const noexcept;
    int get_total_armor_bonus() const noexcept;

    const std::vector<std::unique_ptr<Building>>& get_buildings() const noexcept { return buildings_; }
    void print_status() const;
};
