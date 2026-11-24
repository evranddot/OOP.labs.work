#include "GameMediator.h"
#include "GameField.h"
#include "Base.h"
#include "UnitManager.h"
#include "ResourceSystem.h"
#include "CombatSystem.h"
#include "BuildingManager.h"
#include "Building.h"
#include <iostream>

GameMediator::GameMediator(GameField& field, std::shared_ptr<Base> player_base)
    : field_(field),
    player_base_(std::move(player_base)),
    unit_manager_(std::make_unique<UnitManager>()),
    resource_system_(std::make_unique<ResourceSystem>(300, 300, 200)),
    combat_system_(std::make_unique<CombatSystem>()),
    building_manager_(std::make_unique<BuildingManager>()) {
}

GameMediator::~GameMediator() = default;

void GameMediator::save_snapshot() {
    prev_field_ = std::make_unique<GameField>(field_);
}

void GameMediator::undoLastAction() {
    if (!prev_field_) {
        std::cout << "[Посредник] Нет сохранённого состояния для отката!\n";
        return;
    }
    field_ = *prev_field_;
    prev_field_.reset();

    // Восстанавливаем привязку юнитов к их базам после отката поля
    for (const auto& base : field_.get_bases()) {
        if (base) base->clear_units();
    }
    for (Unit& u : field_) {
        for (IUnitObserver* obs : u.get_observers()) {
            if (auto* base = dynamic_cast<Base*>(obs)) {
                base->registerUnit(std::shared_ptr<Unit>(&u, [](Unit*) {}));
            }
        }
    }

    std::cout << "[Посредник] Состояние игрового поля восстановлено на ход назад!\n";
}

// --- Методы уведомлений Посредника (Этап 2) ---

void GameMediator::notifyUnitCreated(const Unit& unit) {
    std::cout << "[Посредник] Зарегистрирован новый юнит: " << unit.get_type()
        << " (ID:" << unit.get_id() << ") на (" << unit.get_x() << ", " << unit.get_y() << ").\n";
}

void GameMediator::notifyResourceCollected(const std::string& resource_type, int amount) {
    std::cout << "[Посредник] Собраны ресурсы (" << resource_type << ": +" << amount << ").\n";
}

void GameMediator::notifyCombatResolved(const Unit& attacker, const std::string& target_name, int damage_dealt) {
    std::cout << "[Посредник] Бой завершён: " << attacker.get_type()
        << " нанёс " << damage_dealt << " урона цели '" << target_name << "'.\n";
}

void GameMediator::notifyBuildingCompleted(const std::string& building_type) {
    std::cout << "[Посредник] Строительство здания '" << building_type << "' завершено!\n";
}

void GameMediator::notifyResearchCompleted(const std::string& tech_name) {
    std::cout << "[Посредник] Исследование '" << tech_name << "' завершено!\n";
}

// --- Методы команд (Этап 3) ---

void GameMediator::createUnit(const std::string& type, int x, int y) {
    if (!player_base_) return;
    if (!resource_system_->spendResources(50, 50, 20)) return;

    save_snapshot();
    Unit* created = unit_manager_->createUnit(*player_base_, field_, type, x, y,
        building_manager_->get_total_attack_bonus(),
        building_manager_->get_total_armor_bonus());

    if (created) {
        notifyUnitCreated(*created);
    }
    else {
        resource_system_->addResources(50, 50, 20);
    }
}

void GameMediator::moveUnit(int unitId, int targetX, int targetY) {
    save_snapshot();
    unit_manager_->moveUnit(field_, unitId, targetX, targetY);
}

void GameMediator::resolveCombat(int attackerId, int targetId) {
    Unit* attacker = unit_manager_->getUnitById(field_, attackerId);
    if (!attacker) {
        std::cout << "[Посредник] Атакующий юнит ID:" << attackerId << " не найден!\n";
        return;
    }

    int tech_bonus = building_manager_->get_total_attack_bonus();
    save_snapshot();

    // 1. Поиск цели среди юнитов по ID
    if (Unit* target_unit = unit_manager_->getUnitById(field_, targetId)) {
        int dmg = combat_system_->calculateDamage(*attacker, *target_unit, tech_bonus);
        if (combat_system_->attack_unit(*attacker, *target_unit, tech_bonus)) {
            notifyCombatResolved(*attacker, target_unit->get_type(), dmg);
            field_.remove_dead_units();
        }
        return;
    }

    // 2. Поиск цели среди зданий по ID
    if (Building* target_building = building_manager_->getBuildingById(targetId)) {
        if (combat_system_->attack_building(*attacker, *target_building, tech_bonus)) {
            notifyCombatResolved(*attacker, target_building->get_type(),
                attacker->get_attack_obj().get_damage() + tech_bonus);
        }
        return;
    }

    std::cout << "[Бой] Цель с ID:" << targetId << " не найдена на поле!\n";
}

void GameMediator::collectResource(int unitId, int resourcePointId) {
    if (resourcePointId > 0) {
        std::cout << "[Добыча] Юнит ID:" << unitId << " направлен к ресурсной точке #" << resourcePointId << ".\n";
    }
    int before = resource_system_->get_wood() + resource_system_->get_metal() + resource_system_->get_energy();
    if (unit_manager_->collect_resource(field_, unitId, *resource_system_)) {
        int after = resource_system_->get_wood() + resource_system_->get_metal() + resource_system_->get_energy();
        notifyResourceCollected("ресурсы местности", after - before);
    }
}

void GameMediator::healUnit(int unitId, int targetId) {
    Unit* healer = unit_manager_->getUnitById(field_, unitId);
    Unit* target = unit_manager_->getUnitById(field_, targetId);
    if (!target) {
        std::cout << "[Лечение] Целевой юнит ID:" << targetId << " не найден!\n";
        return;
    }
    if (healer) {
        healer->set_order("Лечение юнита ID:" + std::to_string(target->get_id()));
    }
    unit_manager_->heal_unit(field_, targetId, *building_manager_, *resource_system_);
}

bool GameMediator::getUnitPosition(int unitId, int& out_x, int& out_y) const {
    const Unit* u = unit_manager_->getUnitById(field_, unitId);
    if (!u) return false;
    out_x = u->get_x();
    out_y = u->get_y();
    return true;
}

void GameMediator::buildBuilding(const std::string& buildingType, int x, int y) {
    if (!field_.is_cell_free(x, y) || building_manager_->find_building_at(x, y) != nullptr) {
        std::cout << "[Посредник] Клетка (" << x << ", " << y << ") занята или вне границ поля!\n";
        return;
    }

    std::unique_ptr<Building> new_building;
    if (buildingType == "barracks" || buildingType == "Barracks") new_building = std::make_unique<Barracks>(x, y);
    else if (buildingType == "powerplant" || buildingType == "PowerPlant") new_building = std::make_unique<PowerPlant>(x, y);
    else if (buildingType == "laboratory" || buildingType == "Laboratory" || buildingType == "lab") new_building = std::make_unique<Laboratory>(x, y);
    else if (buildingType == "warehouse" || buildingType == "Warehouse") new_building = std::make_unique<StorageBuilding>(x, y);
    else if (buildingType == "hospital" || buildingType == "Hospital") new_building = std::make_unique<Hospital>(x, y);
    else {
        std::cout << "[Посредник] Неизвестный тип здания. Доступно: barracks, powerplant, laboratory, warehouse, hospital.\n";
        return;
    }

    if (!resource_system_->spendResources(80, 60, 30)) return;

    save_snapshot();
    Building* ptr = building_manager_->buildBuilding(std::move(new_building));
    if (ptr && player_base_) {
        player_base_->registerBuilding(std::shared_ptr<Building>(ptr, [](Building*) {}));
    }
}

void GameMediator::startResearch(const std::string& technologyId) {
    if (!resource_system_->spendResources(40, 60, 80)) return;
    if (!building_manager_->startResearch(technologyId)) {
        resource_system_->addResources(40, 60, 80);
    }
}

void GameMediator::distributeResources(const std::string& from, const std::string& to, int amount) {
    std::cout << "[Посредник] Запрос на перевод " << amount << " ед. (" << from << ") в сектор '" << to << "':\n";
    resource_system_->distribute(to, from, amount);
}

void GameMediator::produceUnit(int buildingId, const std::string& unitType) {
    Building* building = building_manager_->getBuildingById(buildingId);
    if (!building) {
        std::cout << "[Производство] Здание с ID:" << buildingId << " не найдено!\n";
        return;
    }
    if (!building->is_constructed()) {
        std::cout << "[Производство] Здание '" << building->get_type() << "' ещё строится!\n";
        return;
    }

    int bx = building->get_x();
    int by = building->get_y();
    for (int dy = -2; dy <= 2; ++dy) {
        for (int dx = -2; dx <= 2; ++dx) {
            int sx = bx + dx;
            int sy = by + dy;
            if (field_.is_cell_free(sx, sy) && !building_manager_->find_building_at(sx, sy)) {
                auto produced = building->produce(unitType, sx, sy);
                if (produced && resource_system_->spendResources(40, 40, 15)) {
                    save_snapshot();
                    if (player_base_) {
                        produced->change_armor(player_base_->get_defense_level() + building_manager_->get_total_armor_bonus());
                        produced->boost_damage(building_manager_->get_total_attack_bonus());
                        player_base_->registerUnit(std::shared_ptr<Unit>(produced.get(), [](Unit*) {}));
                    }
                    Unit* raw = produced.get();
                    if (field_.add_unit(std::move(produced))) {
                        notifyUnitCreated(*raw);
                    }
                }
                return;
            }
        }
    }
    std::cout << "[Производство] Рядом со зданием ID:" << buildingId << " нет свободных клеток!\n";
}

void GameMediator::getStats(const std::string& filter) const {
    std::cout << "\n================ СОСТОЯНИЕ ИГРЫ (ХОД #" << turn_number_ << ") ================\n";
    if (filter.empty()) {
        for (const auto& base : field_.get_bases()) {
            if (base) base->print_status();
        }
        resource_system_->print_status();
        building_manager_->print_status();
    }
    else if (filter == "resources") {
        resource_system_->print_status();
    }
    else if (filter == "buildings") {
        building_manager_->print_status();
    }
    if (filter.empty() || filter == "units") {
        std::cout << "=== СПИСОК ЮНИТОВ НА ПОЛЕ (" << field_.get_units_count() << ") ===\n";
        if (field_.get_units_count() == 0) {
            std::cout << "  На поле нет активных юнитов.\n";
        }
        else {
            for (const auto& u : field_) {
                std::cout << "  ";
                u.print_status();
            }
        }
    }
    std::cout << "==========================================================\n\n";
}
void GameMediator::inspect_unit(int unitId) const {
    unit_manager_->inspect_unit(field_, unitId);
}
void GameMediator::end_turn() {
    ++turn_number_;
    std::cout << "\n--- Завершение хода. Начинается ход #" << turn_number_ << " ---\n";
    unit_manager_->reset_all_moves(field_);
    auto completed = building_manager_->tick_turn(*resource_system_);
    for (const auto& ev : completed) {
        if (ev.rfind("building:", 0) == 0) notifyBuildingCompleted(ev.substr(9));
        else if (ev.rfind("research:", 0) == 0) notifyResearchCompleted(ev.substr(9));
    }
}
void GameMediator::render_field() const {
    field_.display(&building_manager_->get_buildings(), true);
}
bool GameMediator::getUnitState(int unitId, int& out_x, int& out_y, int& out_moves_left) const {
    const Unit* u = unit_manager_->getUnitById(field_, unitId);
    if (!u) return false;
    out_x = u->get_x();
    out_y = u->get_y();
    out_moves_left = u->get_move_points();
    return true;
}