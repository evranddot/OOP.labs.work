#include "Building.h"
#include "Units.h"

Building::Building(std::string b_type, int x, int y, char symbol, int hp, int arm, int turns_to_build)
    : id_(next_building_id++), type(std::move(b_type)), health(hp), armor_(arm),
    position{ x, y }, symbol_(symbol), buildProgress(0), buildRequired(turns_to_build) {
}

void Building::update() {
    if (!is_constructed()) {
        advance_construction(1);
    }
}

void Building::advance_construction(int steps) {
    if (is_constructed() || steps <= 0) return;
    buildProgress += steps;
    if (buildProgress >= buildRequired) {
        buildProgress = buildRequired;
        std::cout << "[Строительство] Здание '" << type << "' (ID:" << id_
            << ") на (" << position.x << ", " << position.y << ") ПОСТРОЕНО!\n";
    }
}

void Building::take_damage(int dmg) {
    int reduced = armor_.reduce(dmg);
    health.damage(reduced);
    std::cout << "[Здание] '" << type << "' (ID:" << id_ << ") получило " << reduced
        << " урона (ХП: " << health.get_health() << ").\n";
}

void Building::print_info() const {
    std::cout << "  ID:" << id_ << " [" << symbol_ << "] " << type
        << " на (" << position.x << ", " << position.y << ")"
        << " | ХП: " << health.get_health()
        << " | Стадия: " << (is_constructed() ? "Готово" : "Строится (" + std::to_string(buildProgress) + "/" + std::to_string(buildRequired) + ")")
        << "\n";
}

std::unique_ptr<Unit> Barracks::produce(const std::string& unit_type, int spawn_x, int spawn_y) {
    if (!is_constructed()) {
        std::cout << "[Barracks] Здание ещё строится!\n";
        return nullptr;
    }
    if (unit_type == "infantry" || unit_type == "standard") return std::make_unique<Infantry>(spawn_x, spawn_y);
    if (unit_type == "scout")    return std::make_unique<Scout>(spawn_x, spawn_y);
    if (unit_type == "worker")   return std::make_unique<Worker>(spawn_x, spawn_y);
    if (unit_type == "medic" || unit_type == "heavy") return std::make_unique<Medic>(spawn_x, spawn_y);
    if (unit_type == "tank" || unit_type == "machine") return std::make_unique<Tank>(spawn_x, spawn_y);

    std::cout << "[Barracks] Неизвестный тип юнита: " << unit_type << "\n";
    return nullptr;
}

std::unique_ptr<Unit> PowerPlant::produce(const std::string&, int, int) {
    std::cout << "[PowerPlant] Электростанция вырабатывает энергию, а не юнитов.\n";
    return nullptr;
}

std::unique_ptr<Unit> Laboratory::produce(const std::string&, int, int) {
    std::cout << "[Laboratory] Лаборатория проводит исследования, а не наём юнитов.\n";
    return nullptr;
}

std::unique_ptr<Unit> StorageBuilding::produce(const std::string&, int, int) {
    std::cout << "[Warehouse] Склад хранит ресурсы.\n";
    return nullptr;
}

std::unique_ptr<Unit> Hospital::produce(const std::string&, int, int) {
    std::cout << "[Hospital] Госпиталь лечит юнитов.\n";
    return nullptr;
}
