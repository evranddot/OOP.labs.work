#pragma once
#include <iostream>
#include "Unit.h"

class IInteractionStrategy {
public:
    virtual ~IInteractionStrategy() = default;
    virtual void execute(Unit& unit) const = 0;
};

class HealStrategy : public IInteractionStrategy {
private:
    int heal_amount;
public:
    explicit HealStrategy(int amount) : heal_amount(amount) {}
    void execute(Unit& unit) const override {
        unit.heal_unit(heal_amount);
        std::cout << "[Стратегия: Ремонт/Лечение] " << unit.get_type()
            << " восстановил +" << heal_amount << " ХП (теперь: "
            << unit.get_health_obj().get_health() << ")!\n";
    }
};

class DamageStrategy : public IInteractionStrategy {
private:
    int damage_amount;
public:
    explicit DamageStrategy(int amount) : damage_amount(amount) {}
    void execute(Unit& unit) const override {
        std::cout << "[Стратегия: Ловушка] " << unit.get_type()
            << " получает " << damage_amount << " ед. урона!\n";
        unit.take_damage(damage_amount);
    }
};

class ArmorBonusStrategy : public IInteractionStrategy {
private:
    int armor_bonus;
public:
    explicit ArmorBonusStrategy(int bonus) : armor_bonus(bonus) {}
    void execute(Unit& unit) const override {
        unit.change_armor(armor_bonus);
        std::cout << "[Стратегия: Снаряжение] " << unit.get_type()
            << " получает +" << armor_bonus << " к броне (теперь: "
            << unit.get_armor_obj().get_armor() << ")!\n";
    }
};

class AttackBonusStrategy : public IInteractionStrategy {
private:
    int attack_bonus;
public:
    explicit AttackBonusStrategy(int bonus) : attack_bonus(bonus) {}
    void execute(Unit& unit) const override {
        unit.boost_damage(attack_bonus);
        std::cout << "[Стратегия: Боевой дух] " << unit.get_type()
            << " получает +" << attack_bonus << " к атаке (теперь: "
            << unit.get_attack_obj().get_damage() << ")!\n";
    }
};
