#include "ResourceSystem.h"

ResourceSystem::ResourceSystem(int start_wood, int start_metal, int start_energy)
    : wood(start_wood), metal(start_metal), energy(start_energy) {
}

void ResourceSystem::addResources(int add_wood, int add_metal, int add_energy) {
    if (add_wood > 0) wood += add_wood;
    if (add_metal > 0) metal += add_metal;
    if (add_energy > 0) energy += add_energy;
}

bool ResourceSystem::spendResources(int cost_wood, int cost_metal, int cost_energy) {
    if (wood < cost_wood || metal < cost_metal || energy < cost_energy) {
        std::cout << "[Ресурсы] Недостаточно ресурсов! Требуется: дерево=" << cost_wood
            << ", металл=" << cost_metal << ", энергия=" << cost_energy << "\n";
        return false;
    }
    wood -= cost_wood;
    metal -= cost_metal;
    energy -= cost_energy;
    return true;
}

void ResourceSystem::add_resource(const std::string& type, int amount) {
    if (amount <= 0) return;
    if (type == "wood" || type == "дерево") wood += amount;
    else if (type == "metal" || type == "металл") metal += amount;
    else if (type == "energy" || type == "энергия") energy += amount;
    else {
        std::cout << "[Ресурсы] Неизвестный тип ресурса: " << type << "\n";
        return;
    }
    std::cout << "[Ресурсы] Получено +" << amount << " (" << type << ").\n";
}

bool ResourceSystem::has_enough(const ResourcePack& cost) const noexcept {
    return wood >= cost.wood && metal >= cost.metal && energy >= cost.energy;
}

bool ResourceSystem::spend(const ResourcePack& cost) {
    return spendResources(cost.wood, cost.metal, cost.energy);
}

bool ResourceSystem::distribute(const std::string& target, const std::string& res_type, int amount) {
    if (amount <= 0) {
        std::cout << "[Ресурсы] Количество должно быть больше 0.\n";
        return false;
    }

    ResourcePack req{};
    if (res_type == "wood" || res_type == "дерево") req.wood = amount;
    else if (res_type == "metal" || res_type == "металл") req.metal = amount;
    else if (res_type == "energy" || res_type == "энергия") req.energy = amount;
    else {
        std::cout << "[Ресурсы] Неизвестный ресурс: " << res_type << "\n";
        return false;
    }

    if (!spend(req)) return false;

    allocations_[target].wood += req.wood;
    allocations_[target].metal += req.metal;
    allocations_[target].energy += req.energy;

    std::cout << "[Ресурсы] Перераспределено " << amount << " ед. (" << res_type
        << ") в сектор '" << target << "'.\n";
    return true;
}

void ResourceSystem::print_status() const {
    std::cout << "=== ЗАПАСЫ РЕСУРСОВ ===\n"
        << "  Дерево: " << wood
        << " | Металл: " << metal
        << " | Энергия: " << energy << "\n";
    if (!allocations_.empty()) {
        std::cout << "  Распределено по секторам:\n";
        for (const auto& [sector, pack] : allocations_) {
            std::cout << "   - " << sector << ": дерево=" << pack.wood
                << ", металл=" << pack.metal << ", энергия=" << pack.energy << "\n";
        }
    }
}
