#include "CombatSystem.h"

int CombatSystem::calculateDamage(Unit& attacker, Unit& target, int tech_attack_bonus) const {
    int raw_damage = attacker.get_attack_obj().get_damage() + (tech_attack_bonus > 0 ? tech_attack_bonus : 0);
    return target.get_armor_obj().reduce(raw_damage);
}

bool CombatSystem::attack_unit(Unit& attacker, Unit& target, int tech_attack_bonus) {
    if (&attacker == &target) {
        std::cout << "[Бой] Юнит не может атаковать сам себя!\n";
        return false;
    }
    if (attacker.get_health_obj().is_dead() || target.get_health_obj().is_dead()) {
        std::cout << "[Бой] Атакующий или цель уже уничтожены!\n";
        return false;
    }
    if (!attacker.can_attack(target)) {
        std::cout << "[Бой] Цель вне радиуса атаки (" << attacker.get_attack_obj().get_attack_range() << ")!\n";
        return false;
    }

    int raw_damage = attacker.get_attack_obj().get_damage() + (tech_attack_bonus > 0 ? tech_attack_bonus : 0);
    int expected_dmg = calculateDamage(attacker, target, tech_attack_bonus);

    attacker.set_order("Атака цели ID:" + std::to_string(target.get_id()));
    std::cout << "[Бой] " << attacker.get_type() << " (ID:" << attacker.get_id() << ") атакует "
        << target.get_type() << " (ID:" << target.get_id() << "). Итоговый урон по броне: " << expected_dmg << "\n";

    target.takeDamage(raw_damage);
    return true;
}

bool CombatSystem::attack_building(Unit& attacker, Building& target, int tech_attack_bonus) {
    if (attacker.get_health_obj().is_dead() || target.is_destroyed()) return false;

    if (!attacker.get_attack_obj().is_in_range(attacker.get_x(), attacker.get_y(), target.get_x(), target.get_y())) {
        std::cout << "[Бой] Здание '" << target.get_name() << "' вне радиуса атаки!\n";
        return false;
    }

    int raw_damage = attacker.get_attack_obj().get_damage() + (tech_attack_bonus > 0 ? tech_attack_bonus : 0);
    attacker.set_order("Штурм здания: " + target.get_name());
    target.take_damage(raw_damage);
    return true;
}

bool CombatSystem::attack_base(Unit& attacker, Base& target, int tech_attack_bonus) {
    if (attacker.get_health_obj().is_dead() || target.is_destroyed()) return false;

    if (!attacker.get_attack_obj().is_in_range(attacker.get_x(), attacker.get_y(), target.get_x(), target.get_y())) {
        std::cout << "[Бой] База '" << target.get_name() << "' вне радиуса атаки!\n";
        return false;
    }

    int raw_damage = attacker.get_attack_obj().get_damage() + (tech_attack_bonus > 0 ? tech_attack_bonus : 0);
    attacker.set_order("Атака базы: " + target.get_name());
    target.take_damage(raw_damage);
    return true;
}
