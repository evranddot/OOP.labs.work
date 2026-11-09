#pragma once

#include <memory>
#include "Unit.h"
#include "Units.h"

enum class Role { Standard, Heavy, Machine };

class UnitFactory {
public:
    virtual ~UnitFactory() = default;
    virtual std::unique_ptr<Unit> create_unit(Role role, int x, int y) const = 0;
};


class BountyFactory : public UnitFactory {
public:
    std::unique_ptr<Unit> create_unit(Role role, int x, int y) const override {
        if (role == Role::Standard) return std::make_unique<Hunter>(x, y);
        if (role == Role::Heavy)   return std::make_unique<Mandalorian>(x, y);
        if (role == Role::Machine) return std::make_unique<Crawler>(x, y);
        return nullptr;
    }
};



class RebelFactory : public UnitFactory {
public:
    std::unique_ptr<Unit> create_unit(Role role, int x, int y) const override {
        if (role == Role::Standard) return std::make_unique<Rebel>(x, y);
        if (role == Role::Heavy)   return std::make_unique<Wookiee>(x, y);
        if (role == Role::Machine) return std::make_unique<Xwyng>(x, y);
        return nullptr;
    }
};



class ImperialFactory : public UnitFactory {
public:
    std::unique_ptr<Unit> create_unit(Role role, int x, int y) const override {
        if (role == Role::Standard) return std::make_unique<Stormtrooper>(x, y);
        if (role == Role::Heavy)   return std::make_unique<Deathtrooper>(x, y);
        if (role == Role::Machine) return std::make_unique<ATAT>(x, y);
        return nullptr;
    }
};


