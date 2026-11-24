#pragma once
#include "Terrain.h"

class TerrainProxy : public Terrain {
private:
    mutable std::unique_ptr<Terrain> real_terrain;


    void ensure_initialized() const {
        if (!real_terrain) {
            real_terrain = std::make_unique<Ground>();
        }
    }

public:

    explicit TerrainProxy(std::unique_ptr<Terrain> terrain = nullptr)
        : real_terrain(std::move(terrain)) {
    }

    bool isPassable(UnitType type) const override {
        ensure_initialized();
        bool allowed = real_terrain->isPassable(type);
        if (!allowed) {
            std::cout << " Доступ на клетку '" << real_terrain->getName() << "' запрещён для данного типа юнита!\n";
        }
        return allowed;
    }

    double getDefenseBonus(UnitType type) const override {
        ensure_initialized();
        double bonus = real_terrain->getDefenseBonus(type);
        return bonus;
    }

    std::string getSymbol() const override {
        ensure_initialized();
        return real_terrain->getSymbol();
    }

    std::string getName() const override {
        ensure_initialized();
        return real_terrain->getName();
    }
};