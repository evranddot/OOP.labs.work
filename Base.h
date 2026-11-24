#pragma once
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include "Unit.h"
#include "UnitFactory.h"
#include "Building.h"
#include "ResourceSystem.h"
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

    ResourceSystem resources;
    std::unique_ptr<UnitFactory> factory;
    std::vector<std::shared_ptr<Unit>> units;
    std::vector<std::shared_ptr<Building>> buildings;
    std::vector<std::string> destroyed_history;

public:
    Base(std::string name, int x, int y, int start_health, size_t max_u, int def_lvl,
        std::unique_ptr<UnitFactory> unit_factory, char symbol = 'B')
        : faction_name(std::move(name)), x_coord(x), y_coord(y), map_symbol(symbol),
        health(start_health), defense_level(def_lvl), max_units(max_u),
        resources(300, 300, 200), factory(std::move(unit_factory)) {
    }

    ~Base() override {
        for (auto& u : units) {
            if (u) u->detach_observer(this);
        }
    }

    // Методы учёта юнитов и зданий по спецификации Этапа 2 (пункт 1)
    void registerUnit(std::shared_ptr<Unit> unit) {
        if (!unit) return;
        unit->attach_observer(this);
        units.push_back(std::move(unit));
    }

    void unregisterUnit(int unit_id) {
        units.erase(std::remove_if(units.begin(), units.end(),
            [unit_id](const std::shared_ptr<Unit>& u) { return !u || u->get_id() == unit_id; }),
            units.end());
    }

    void clear_units() noexcept {
        units.clear();
    }

    void registerBuilding(std::shared_ptr<Building> building) {
        if (building) {
            buildings.push_back(std::move(building));
        }
    }

    ResourceSystem& get_resources() noexcept { return resources; }
    const ResourceSystem& get_resources() const noexcept { return resources; }
    const std::vector<std::shared_ptr<Unit>>& get_units() const noexcept { return units; }
    const std::vector<std::shared_ptr<Building>>& get_buildings() const noexcept { return buildings; }

    std::unique_ptr<Unit> createUnit(Role type, int x, int y) {
        if (is_destroyed()) {
            std::cout << "[База " << faction_name << "] Уничтожена и не может создавать юнитов!\n";
            return nullptr;
        }
        if (units.size() >= max_units) {
            std::cout << "[База " << faction_name << "] Достигнут лимит юнитов (" << max_units << ")!\n";
            return nullptr;
        }
        if (!factory) return nullptr;

        auto unit = factory->create_unit(type, x, y);
        if (unit) {
            unit->change_armor(defense_level.get_armor());
            unit->attach_observer(this);
            // Регистрируем невладеющий shared_ptr для учёта в базе, пока владеет GameField
            units.emplace_back(unit.get(), [](Unit*) {});
            std::cout << "[База " << faction_name << "] Создан юнит " << unit->get_type()
                << " (ID:" << unit->get_id() << ") на (" << x << ", " << y
                << "). Активно: " << units.size() << "/" << max_units << "\n";
        }
        return unit;
    }

    void on_unit_event(const UnitEvent& event) override {
        if (!event.unit) return;

        switch (event.type) {
        case UnitEventType::Damaged:
            std::cout << "[Наблюдатель | База " << faction_name << "] Юнит " << event.unit->get_type()
                << " получил " << event.value << " урона. Осталось ХП: " << event.current_health << "\n";
            break;

        case UnitEventType::Moved:
            std::cout << "[Наблюдатель | База " << faction_name << "] Юнит " << event.unit->get_type()
                << " переместился на (" << event.x << ", " << event.y << ")\n";
            break;

        case UnitEventType::Destroyed: {
            auto it = std::find_if(units.begin(), units.end(),
                [&](const std::shared_ptr<Unit>& ptr) { return ptr.get() == event.unit; });
            if (it != units.end()) {
                destroyed_history.push_back(event.unit->get_type());
                units.erase(it);
                event.unit->detach_observer(this);
                std::cout << "[Наблюдатель | База " << faction_name << "] Зафиксирована гибель "
                    << event.unit->get_type() << "! Свободно слотов: " << (max_units - units.size()) << "/" << max_units << "\n";
            }
            break;
        }
        }
    }

    void take_damage(int damage_amount) {
        int reduced = defense_level.reduce(damage_amount);
        health.damage(reduced);
        if (is_destroyed()) {
            std::cout << "База фракции " << faction_name << " на (" << x_coord << ", " << y_coord << ") УНИЧТОЖЕНА!\n";
        }
    }

    int get_x() const noexcept { return x_coord; }
    int get_y() const noexcept { return y_coord; }
    char get_symbol() const noexcept { return map_symbol; }
    const std::string& get_name() const noexcept { return faction_name; }
    int get_health() const { return health.get_health(); }
    int get_defense_level() const { return defense_level.get_armor(); }
    size_t get_active_count() const noexcept { return units.size(); }
    size_t get_max_units() const noexcept { return max_units; }
    const std::vector<std::string>& get_destroyed_history() const noexcept { return destroyed_history; }
    bool is_destroyed() const { return health.is_dead(); }

    void print_status() const {
        std::cout << "База [" << faction_name << "] (" << x_coord << ", " << y_coord << ")\n"
            << "  Здоровье: " << health.get_health()
            << " | Защита: " << defense_level.get_armor()
            << " | Юниты: " << units.size() << "/" << max_units
            << " | Здания: " << buildings.size()
            << " | Потерь: " << destroyed_history.size() << "\n";
    }
};
