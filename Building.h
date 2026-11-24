#pragma once
#include <string>
#include <memory>
#include <iostream>
#include "Health.h"
#include "Armor.h"
#include "Unit.h"

class Building {
private:
    static inline int next_building_id = 1;

protected:
    int id_;
    std::string type;
    Health health;
    Armor armor_;
    Position position;
    char symbol_;
    int buildProgress;
    int buildRequired;

public:
    Building(std::string b_type, int x, int y, char symbol, int hp, int arm, int turns_to_build = 2);
    virtual ~Building() = default;

    virtual void update();
    virtual std::unique_ptr<Unit> produce(const std::string& unit_type, int spawn_x, int spawn_y) = 0;

    void advance_construction(int steps = 1);
    bool is_constructed() const noexcept { return buildProgress >= buildRequired; }

    void take_damage(int dmg);
    bool is_destroyed() const { return health.is_dead(); }

    int get_id() const noexcept { return id_; }
    const std::string& get_type() const noexcept { return type; }
    const std::string& get_name() const noexcept { return type; }
    int get_x() const noexcept { return position.x; }
    int get_y() const noexcept { return position.y; }
    Position get_position() const noexcept { return position; }
    char get_symbol() const noexcept { return symbol_; }
    int get_health() const { return health.get_health(); }
    int get_progress() const noexcept { return buildProgress; }
    int get_required() const noexcept { return buildRequired; }

    virtual void print_info() const;
};

class Barracks : public Building {
public:
    Barracks(int x, int y) : Building("Barracks", x, y, 'K', 300, 10, 2) {}
    std::unique_ptr<Unit> produce(const std::string& unit_type, int spawn_x, int spawn_y) override;
};

class PowerPlant : public Building {
public:
    PowerPlant(int x, int y) : Building("PowerPlant", x, y, 'E', 250, 5, 2) {}
    std::unique_ptr<Unit> produce(const std::string& unit_type, int spawn_x, int spawn_y) override;
};

class Laboratory : public Building {
public:
    Laboratory(int x, int y) : Building("Laboratory", x, y, 'L', 250, 5, 2) {}
    std::unique_ptr<Unit> produce(const std::string& unit_type, int spawn_x, int spawn_y) override;
};

class StorageBuilding : public Building {
public:
    StorageBuilding(int x, int y) : Building("Warehouse", x, y, 'W', 300, 10, 1) {}
    std::unique_ptr<Unit> produce(const std::string& unit_type, int spawn_x, int spawn_y) override;
};

class Hospital : public Building {
public:
    Hospital(int x, int y) : Building("Hospital", x, y, 'H', 250, 5, 2) {}
    std::unique_ptr<Unit> produce(const std::string& unit_type, int spawn_x, int spawn_y) override;
};
