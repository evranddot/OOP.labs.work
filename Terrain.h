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
        return 0;
    }

    std::string getSymbol() const override { return "\033[33m.\033[0m"; }
    std::string getName() const override { return "Земля"; }
};


class Mountains : public Terrain {
public:
    bool isPassable(UnitType type) const override {
        return type != UnitType::Machine;
    }

    double getDefenseBonus(UnitType type) const override {
        return 0;
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
        return 0;
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
        return 10;
    }

    std::string getSymbol() const override { return "\033[32m*\033[0m"; }
    std::string getName() const override { return "Лес"; }
};



