#pragma once
#include <string>
#include <memory>
#include <iostream>
#include "Unit.h"
#include "Strategy.h"


class NeutralObject {
protected:
    int x_coord;
    int y_coord;
    std::string symbol;
    std::string name;
    bool is_consumed = false;

    virtual std::unique_ptr<IInteractionStrategy> select_strategy(Role role) const = 0;

public:
    NeutralObject(int x, int y, std::string sym, std::string obj_name)
        : x_coord(x), y_coord(y), symbol(std::move(sym)), name(std::move(obj_name)) {
    }

    virtual ~NeutralObject() = default;

    void interact(Unit& unit) {
        if (is_consumed) return;
        std::cout << "[Объект: " << name << "] Взаимодействие с юнитом " << unit.get_type() << ":\n";
        auto strategy = select_strategy(unit.get_role());
        if (strategy) {
            strategy->execute(unit);
        }
        is_consumed = true;
    }

    int get_x() const noexcept { return x_coord; }
    int get_y() const noexcept { return y_coord; }
    const std::string& get_symbol() const noexcept { return symbol; }
    const std::string& get_name() const noexcept { return name; }
    bool consumed() const noexcept { return is_consumed; }
};

class Hangar : public NeutralObject {
protected:
    std::unique_ptr<IInteractionStrategy> select_strategy(Role role) const override {
        switch (role) {
        case Role::Machine:  return std::make_unique<HealStrategy>(20);
        case Role::Heavy:    return std::make_unique<HealStrategy>(10);
        case Role::Standard:
        default:             return std::make_unique<HealStrategy>(5);
        }
    }

public:
    Hangar(int x, int y)
        : NeutralObject(x, y, "\033[91m#\033[0m", "Ангар") {
    }
};

class Trap : public NeutralObject {
protected:
    std::unique_ptr<IInteractionStrategy> select_strategy(Role role) const override {
        switch (role) {
        case Role::Standard: return std::make_unique<DamageStrategy>(25);
        case Role::Heavy:    return std::make_unique<DamageStrategy>(10);
        case Role::Machine:
        default:             return std::make_unique<DamageStrategy>(5);
        }
    }

public:
    Trap(int x, int y)
        : NeutralObject(x, y, "\033[91m!\033[0m", "Ловушка") {
    }
};

class Warehouse : public NeutralObject {
protected:
    std::unique_ptr<IInteractionStrategy> select_strategy(Role role) const override {
        switch (role) {
        case Role::Standard: return std::make_unique<ArmorBonusStrategy>(5);
        case Role::Heavy:    return std::make_unique<ArmorBonusStrategy>(10);
        case Role::Machine:
        default:             return std::make_unique<ArmorBonusStrategy>(7);
        }
    }

public:
    Warehouse(int x, int y)
        : NeutralObject(x, y, "\033[91m&\033[0m", "Склад") {
    }
};

class Food : public NeutralObject {
protected:
    std::unique_ptr<IInteractionStrategy> select_strategy(Role role) const override {
        switch (role) {
        case Role::Standard: return std::make_unique<AttackBonusStrategy>(10);
        case Role::Heavy:    return std::make_unique<AttackBonusStrategy>(15);
        case Role::Machine:
        default:             return std::make_unique<AttackBonusStrategy>(5);
        }
    }

public:
    Food(int x, int y)
        : NeutralObject(x, y, "\033[91m@\033[0m", "Еда") {
    }
};

inline Unit& operator+(Unit& unit, Hangar& hangar) {
    hangar.interact(unit);
    return unit;
}

inline Unit& operator-(Unit& unit, Trap& trap) {
    trap.interact(unit);
    return unit;
}

inline Unit& operator*(Unit& unit, Warehouse& warehouse) {
    warehouse.interact(unit);
    return unit;
}

inline Unit& operator/(Unit& unit, Food& food) {
    food.interact(unit);
    return unit;
}

