#pragma once
#include <iostream>
#include "Unit.h"
#include "Building.h"
#include "Base.h"

class Combat {
public:
    virtual ~Combat() = default;


    virtual int calculateDamage(Unit& attacker, Unit& target, int tech_attack_bonus = 0) const = 0;

    virtual bool attack_unit(Unit& attacker, Unit& target, int tech_attack_bonus = 0) = 0;
    virtual bool attack_building(Unit& attacker, Building& target, int tech_attack_bonus = 0) = 0;
    virtual bool attack_base(Unit& attacker, Base& target, int tech_attack_bonus = 0) = 0;
};

class CombatSystem : public Combat {
public:
    CombatSystem() = default;

    int calculateDamage(Unit& attacker, Unit& target, int tech_attack_bonus = 0) const override;
    bool attack_unit(Unit& attacker, Unit& target, int tech_attack_bonus = 0) override;
    bool attack_building(Unit& attacker, Building& target, int tech_attack_bonus = 0) override;
    bool attack_base(Unit& attacker, Base& target, int tech_attack_bonus = 0) override;
};
