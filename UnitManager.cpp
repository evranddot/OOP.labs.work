#include "UnitManager.h"
#include "Units.h"
#include <iostream>
#include <cmath>
#include <algorithm>

Unit* UnitManager::getUnitById(GameField& field, int id) const {
    for (Unit& u : field) {
        if (u.get_id() == id) return &u;
    }
    // Если по ID не нашли, проверяем как порядковый индекс на поле
    if (id >= 0 && static_cast<size_t>(id) < field.get_units_count()) {
        return field.get_unit(static_cast<size_t>(id));
    }
    return nullptr;
}

Unit* UnitManager::createUnit(Base& base, GameField& field, const std::string& unit_type,
    int x, int y, int tech_atk_bonus, int tech_arm_bonus) {
    std::unique_ptr<Unit> unit;

    if (unit_type == "infantry" || unit_type == "пехотинец") unit = std::make_unique<Infantry>(x, y);
    else if (unit_type == "scout" || unit_type == "разведчик") unit = std::make_unique<Scout>(x, y);
    else if (unit_type == "tank" || unit_type == "танк") unit = std::make_unique<Tank>(x, y);
    else if (unit_type == "worker" || unit_type == "рабочий") unit = std::make_unique<Worker>(x, y);
    else if (unit_type == "medic" || unit_type == "медик") unit = std::make_unique<Medic>(x, y);
    else if (unit_type == "standard") unit = base.createUnit(Role::Standard, x, y);
    else if (unit_type == "heavy")    unit = base.createUnit(Role::Heavy, x, y);
    else if (unit_type == "machine")  unit = base.createUnit(Role::Machine, x, y);
    else {
        std::cout << "[UnitManager] Неизвестный тип юнита '" << unit_type << "'.\n";
        return nullptr;
    }

    if (!unit) return nullptr;

    // Если юнит создан напрямую (Infantry/Scout/Tank/Worker/Medic), регистрируем его в базе
    if (unit_type != "standard" && unit_type != "heavy" && unit_type != "machine") {
        unit->change_armor(base.get_defense_level());
        base.registerUnit(std::shared_ptr<Unit>(unit.get(), [](Unit*) {}));
    }

    if (tech_atk_bonus > 0) unit->boost_damage(tech_atk_bonus);
    if (tech_arm_bonus > 0) unit->change_armor(tech_arm_bonus);
    unit->set_order("Развёрнут у базы");

    Unit* raw_ptr = unit.get();
    if (!field.add_unit(std::move(unit))) {
        base.unregisterUnit(raw_ptr->get_id());
        return nullptr;
    }
    return raw_ptr;
}

bool UnitManager::moveUnit(GameField& field, int unit_id_or_index, int new_x, int new_y) {
    Unit* unit = getUnitById(field, unit_id_or_index);
    if (!unit) {
        std::cout << "[UnitManager] Юнит (ID/индекс " << unit_id_or_index << ") не найден!\n";
        return false;
    }

    int dist = (std::max)(std::abs(unit->get_x() - new_x), std::abs(unit->get_y() - new_y));
    if (dist == 0) return false;

    if (unit->get_move_points() < dist) {
        std::cout << "[UnitManager] Недостаточно очков хода! Требуется: " << dist
            << ", доступно: " << unit->get_move_points() << ".\n";
        return false;
    }

    if (field.move_unit(*unit, new_x, new_y)) {
        unit->spend_move_points(dist);
        unit->set_order("Перемещение в (" + std::to_string(new_x) + ", " + std::to_string(new_y) + ")");
        return true;
    }
    return false;
}

bool UnitManager::removeUnit(GameField& field, Base& base, int unit_id) {
    size_t idx = 0;
    for (Unit& u : field) {
        if (u.get_id() == unit_id) {
            base.unregisterUnit(unit_id);
            return field.remove_unit_by_index(idx);
        }
        ++idx;
    }
    std::cout << "[UnitManager] Юнит с ID " << unit_id << " не найден для удаления.\n";
    return false;
}

bool UnitManager::collect_resource(GameField& field, int unit_id_or_index, ResourceSystem& resources) {
    Unit* unit = getUnitById(field, unit_id_or_index);
    if (!unit) {
        std::cout << "[UnitManager] Юнит не найден!\n";
        return false;
    }
    if (!unit->spend_move_points(1)) {
        std::cout << "[UnitManager] Нет очков хода для добычи!\n";
        return false;
    }

    const Terrain* terrain = field.get_terrain(unit->get_x(), unit->get_y());
    std::string t_name = terrain ? terrain->getName() : "Земля";
    unit->set_order("Добыча ресурсов (" + t_name + ")");

    int bonus_mult = (dynamic_cast<Worker*>(unit) != nullptr) ? 2 : 1;

    if (t_name == "Лес") resources.add_resource("wood", 30 * bonus_mult);
    else if (t_name == "Горы") resources.add_resource("metal", 30 * bonus_mult);
    else if (t_name == "Овраги") resources.add_resource("energy", 25 * bonus_mult);
    else {
        resources.add_resource("wood", 10 * bonus_mult);
        resources.add_resource("metal", 10 * bonus_mult);
    }
    return true;
}

bool UnitManager::heal_unit(GameField& field, int unit_id_or_index, BuildingManager& buildings, ResourceSystem& resources) {
    Unit* unit = getUnitById(field, unit_id_or_index);
    if (!unit) {
        std::cout << "[UnitManager] Юнит не найден!\n";
        return false;
    }

    for (Unit& candidate : field) {
        if (auto* medic = dynamic_cast<Medic*>(&candidate)) {
            int dist = (std::max)(std::abs(unit->get_x() - medic->get_x()),
                std::abs(unit->get_y() - medic->get_y()));
            if (dist <= 2) {
                medic->heal_ally(*unit, 40);
                unit->set_order("Лечение медиком");
                std::cout << "[Лечение] Медик (ID:" << medic->get_id() << ") вылечил "
                    << unit->get_type() << " (+40 ХП).\n";
                return true;
            }
        }
    }


    if (Hospital* hospital = buildings.find_ready_hospital()) {
        int dist = (std::max)(std::abs(unit->get_x() - hospital->get_x()),
            std::abs(unit->get_y() - hospital->get_y()));
        if (dist <= 2) {
            unit->heal_unit(50);
            unit->set_order("Лечение в Госпитале");
            std::cout << "[Лечение] " << unit->get_type() << " восстановлен в Госпитале (+50 ХП).\n";
            return true;
        }
    }


    if (!resources.spendResources(0, 10, 15)) return false;
    unit->heal_unit(25);
    unit->set_order("Полевое лечение");
    std::cout << "[Лечение] " << unit->get_type() << " использовал полевой медпакет (+25 ХП).\n";
    return true;
}

void UnitManager::inspect_unit(const GameField& field, int unit_id_or_index) const {
    size_t idx = 0;
    for (const Unit& u : field) {
        if (u.get_id() == unit_id_or_index || static_cast<int>(idx) == unit_id_or_index) {
            std::cout << "=== КАРТОЧКА ЮНИТА ===\n  ";
            u.print_status();
            return;
        }
        ++idx;
    }
    std::cout << "[Инспекция] Юнит не найден!\n";
}

void UnitManager::reset_all_moves(GameField& field) {
    for (Unit& u : field) {
        u.reset_move_points();
    }
}
