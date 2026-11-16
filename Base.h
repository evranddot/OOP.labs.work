#pragma once
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include "Unit.h"
#include "UnitFactory.h"
#include "Health.h"
#include "Armor.h"

class Base : public IUnitObserver {
private:
    std::string faction_name;
    int x_coord;
    int y_coord;
    char map_symbol;

    Health health;
    Armor defense_level;
    size_t max_units;

    std::unique_ptr<UnitFactory> factory;
    std::vector<Unit*> active_units;
    std::vector<std::string> destroyed_history;

public:
    Base(std::string name, int x, int y, int start_health, size_t max_u, int def_lvl,
        std::unique_ptr<UnitFactory> unit_factory, char symbol = 'B')
        : faction_name(std::move(name)), x_coord(x), y_coord(y), map_symbol(symbol),
        health(start_health), defense_level(def_lvl), max_units(max_u),
        factory(std::move(unit_factory)) {
    }

    std::unique_ptr<Unit> createUnit(Role type, int x, int y) {
        if (is_destroyed()) {
            std::cout << "[База " << faction_name << "] Уничтожена и не может создавать юнитов!\n";
            return nullptr;
        }
        if (active_units.size() >= max_units) {
            std::cout << "[База " << faction_name << "] Достигнут лимит юнитов (" << max_units << ")!\n";
            return nullptr;
        }

        if (!factory) return nullptr;

        auto unit = factory->create_unit(type, x, y);
        if (unit) {
            unit->change_armor(defense_level.get_armor());
            unit->set_observer(this);
            active_units.push_back(unit.get());
            std::cout << "[База " << faction_name << "] Создан юнит " << unit->get_type()
                << " на (" << x << ", " << y << "). Активно: " << active_units.size() << "/" << max_units << "\n";
        }
        return unit;
    }

    void on_unit_destroyed(Unit* dead_unit) override {
        if (!dead_unit) return;
        auto it = std::find(active_units.begin(), active_units.end(), dead_unit);
        if (it != active_units.end()) {
            destroyed_history.push_back(dead_unit->get_type());
            active_units.erase(it);
            std::cout << "[Наблюдатель: База " << faction_name << "] Зафиксирована гибель "
                << dead_unit->get_type() << "! Свободно слотов: "
                << (max_units - active_units.size()) << "/" << max_units << "\n";
        }
    }

    void take_damage(int damage_amount) {
        int reduced = defense_level.reduce(damage_amount);
        health.damage(reduced);
        if (is_destroyed()) {
            std::cout << "[КРИТИЧНО] База фракции " << faction_name << " на (" << x_coord << ", " << y_coord << ") УНИЧТОЖЕНА!\n";
        }
    }

    int get_x() const noexcept { return x_coord; }
    int get_y() const noexcept { return y_coord; }
    char get_symbol() const noexcept { return map_symbol; }
    const std::string& get_name() const noexcept { return faction_name; }
    int get_health() const { return health.get_health(); }
    int get_defense_level() const { return defense_level.get_armor(); }
    size_t get_active_count() const noexcept { return active_units.size(); }
    size_t get_max_units() const noexcept { return max_units; }
    const std::vector<std::string>& get_destroyed_history() const noexcept { return destroyed_history; }
    bool is_destroyed() const { return health.is_dead(); }

    void print_status() const {
        std::cout << "=== База [" << faction_name << "] (" << x_coord << ", " << y_coord << ") ===\n"
            << "  Здоровье: " << health.get_health()
            << " | Защита: " << defense_level.get_armor()
            << " | Юниты: " << active_units.size() << "/" << max_units
            << " | Потерь: " << destroyed_history.size() << "\n";
    }
};
