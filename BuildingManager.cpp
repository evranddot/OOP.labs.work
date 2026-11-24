#include "BuildingManager.h"
#include <iostream>

Building* BuildingManager::buildBuilding(std::unique_ptr<Building> building) {
    if (!building) return nullptr;
    Building* ptr = building.get();
    std::cout << "[Строительство] Заложено здание '" << ptr->get_type()
        << "' (ID:" << ptr->get_id() << ") на (" << ptr->get_x() << ", " << ptr->get_y() << ").\n";
    buildings_.push_back(std::move(building));
    return ptr;
}

Building* BuildingManager::getBuildingById(int id) {
    for (auto& b : buildings_) {
        if (b && !b->is_destroyed() && b->get_id() == id) {
            return b.get();
        }
    }
    return nullptr;
}

bool BuildingManager::has_building(const std::string& name, bool must_be_ready) const {
    for (const auto& b : buildings_) {
        if (b && !b->is_destroyed() && b->get_type() == name) {
            if (!must_be_ready || b->is_constructed()) return true;
        }
    }
    return false;
}

Building* BuildingManager::find_building_at(int x, int y) {
    for (auto& b : buildings_) {
        if (b && !b->is_destroyed() && b->get_x() == x && b->get_y() == y) {
            return b.get();
        }
    }
    return nullptr;
}

Hospital* BuildingManager::find_ready_hospital() {
    for (auto& b : buildings_) {
        if (b && !b->is_destroyed() && b->is_constructed()) {
            if (auto* hosp = dynamic_cast<Hospital*>(b.get())) {
                return hosp;
            }
        }
    }
    return nullptr;
}

bool BuildingManager::startResearch(const std::string& tech_name) {
    if (!has_building("Laboratory", true)) {
        std::cout << "[Исследования] Требуется построенная Лаборатория (Laboratory)!\n";
        return false;
    }

    for (const auto& r : researches_) {
        if (r.name == tech_name) {
            std::cout << "[Исследования] Технология '" << tech_name << "' уже исследуется или изучена.\n";
            return false;
        }
    }

    if (tech_name == "weapon" || tech_name == "attack_upgrade" || tech_name == "оружие") {
        researches_.push_back({ tech_name, 2, false, 10, 0 });
    }
    else if (tech_name == "armor" || tech_name == "броня") {
        researches_.push_back({ "armor", 2, false, 0, 10 });
    }
    else if (tech_name == "heavy" || tech_name == "техника") {
        researches_.push_back({ "heavy", 3, false, 5, 5 });
    }
    else {
        std::cout << "[Исследования] Неизвестная технология. Доступно: weapon, armor, heavy.\n";
        return false;
    }


    std::cout << "[Исследования] Запущено исследование: " << researches_.back().name << "\n";
    return true;
}

std::unique_ptr<Unit> BuildingManager::produceUnit(const std::string& building_type, const std::string& unit_type, int spawn_x, int spawn_y) {
    for (auto& b : buildings_) {
        if (b && !b->is_destroyed() && b->get_type() == building_type && b->is_constructed()) {
            return b->produce(unit_type, spawn_x, spawn_y);
        }
    }
    std::cout << "[Производство] Готовое здание '" << building_type << "' не найдено!\n";
    return nullptr;
}

std::vector<std::string> BuildingManager::tick_turn(ResourceSystem& resources) {
    std::vector<std::string> events;

    for (auto& b : buildings_) {
        if (!b || b->is_destroyed()) continue;
        bool was_ready = b->is_constructed();
        b->update();
        if (!was_ready && b->is_constructed()) {
            events.push_back("building:" + b->get_type());
        }
        else if (was_ready && b->get_type() == "PowerPlant") {
            resources.addResources(0, 0, 25);
        }
    }

    for (auto& tech : researches_) {
        if (!tech.completed && tech.remaining_turns > 0) {
            tech.remaining_turns--;
            if (tech.remaining_turns == 0) {
                tech.completed = true;
                events.push_back("research:" + tech.name);
            }
        }
    }
    return events;
}

int BuildingManager::get_total_attack_bonus() const noexcept {
    int sum = 0;
    for (const auto& t : researches_) {
        if (t.completed) sum += t.attack_bonus;
    }
    return sum;
}

int BuildingManager::get_total_armor_bonus() const noexcept {
    int sum = 0;
    for (const auto& t : researches_) {
        if (t.completed) sum += t.armor_bonus;
    }
    return sum;
}

void BuildingManager::print_status() const {
    std::cout << "=== СПИСОК ЗДАНИЙ (" << buildings_.size() << ") ===\n";
    if (buildings_.empty()) {
        std::cout << "  Зданий пока нет.\n";
    }
    else {
        for (const auto& b : buildings_) {
            if (b && !b->is_destroyed()) b->print_info();
        }
    }

    std::cout << "=== АКТИВНЫЕ И ИЗУЧЕННЫЕ ИССЛЕДОВАНИЯ ===\n";
    if (researches_.empty()) {
        std::cout << "  Исследований нет.\n";
    }
    else {
        for (const auto& t : researches_) {
            std::cout << "  - " << t.name << ": "
                << (t.completed ? "Изучено" : "Осталось ходов: " + std::to_string(t.remaining_turns)) << "\n";
        }
    }
}
