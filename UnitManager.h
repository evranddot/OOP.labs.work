#pragma once
#include <string>
#include <cstddef>
#include "Unit.h"
#include "Base.h"
#include "GameField.h"
#include "ResourceSystem.h"
#include "BuildingManager.h"

class UnitManager {
public:
    UnitManager() = default;

    Unit* createUnit(Base& base, GameField& field, const std::string& unit_type,
        int x, int y, int tech_atk_bonus = 0, int tech_arm_bonus = 0);
    bool moveUnit(GameField& field, int unit_id_or_index, int new_x, int new_y);
    bool removeUnit(GameField& field, Base& base, int unit_id);
    Unit* getUnitById(GameField& field, int id) const;


    bool collect_resource(GameField& field, int unit_id_or_index, ResourceSystem& resources);
    bool heal_unit(GameField& field, int unit_id_or_index, BuildingManager& buildings, ResourceSystem& resources);
    void inspect_unit(const GameField& field, int unit_id_or_index) const;
    void reset_all_moves(GameField& field);
};
