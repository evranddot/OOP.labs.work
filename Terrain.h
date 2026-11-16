#pragma once
#include <string>
#include <memory>
#include <iostream>


enum class Role { Standard, Heavy, Machine };
using UnitType = Role;


class Terrain {
public:
    virtual ~Terrain() = default;

    virtual bool isPassable(UnitType type) const = 0;
    virtual double getDefenseBonus(UnitType type) const = 0;

    virtual std::string getSymbol() const = 0;
    virtual std::string getName() const = 0;
};

class Ground : public Terrain {
public:
    bool isPassable(UnitType type) const override {
        return true;
    }

    double getDefenseBonus(UnitType type) const override {
        return 0.0;
    }

    std::string getSymbol() const override { return "\033[33m*\033[0m"; }
    std::string getName() const override { return "Земля"; }
};


class Mountains : public Terrain {
public:
    bool isPassable(UnitType type) const override {
        return type != UnitType::Machine;
    }

    double getDefenseBonus(UnitType type) const override {
        return 0.0;
    }

    std::string getSymbol() const override { return "\033[38;5;172m^\033[0m"; }
    std::string getName() const override { return "Горы"; }
};


class Ravines : public Terrain {
public:
    bool isPassable(UnitType type) const override {
        return type == UnitType::Machine;
    }

    double getDefenseBonus(UnitType type) const override {
        return 0.0;
    }

    std::string getSymbol() const override { return "\033[33m_\033[0m"; }
    std::string getName() const override { return "Овраги"; }
};

class Forest : public Terrain {
public:
    bool isPassable(UnitType type) const override {
        return true;
    }

    double getDefenseBonus(UnitType type) const override {
        return 10.0;
    }

    std::string getSymbol() const override { return "\033[32m*\033[0m"; }
    std::string getName() const override { return "Лес"; }
};


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
