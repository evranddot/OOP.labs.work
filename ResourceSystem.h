#pragma once
#include <string>
#include <map>
#include <iostream>

struct ResourcePack {
    int wood = 0;
    int metal = 0;
    int energy = 0;
};

class ResourceSystem {
private:
    int wood;
    int metal;
    int energy;
    std::map<std::string, ResourcePack> allocations_;

public:
    ResourceSystem(int start_wood = 300, int start_metal = 300, int start_energy = 200);

    int get_wood() const noexcept { return wood; }
    int get_metal() const noexcept { return metal; }
    int get_energy() const noexcept { return energy; }


    void addResources(int add_wood, int add_metal, int add_energy);
    bool spendResources(int cost_wood, int cost_metal, int cost_energy);
    bool distribute(const std::string& target, const std::string& res_type, int amount);


    void add_resource(const std::string& type, int amount);
    bool has_enough(const ResourcePack& cost) const noexcept;
    bool spend(const ResourcePack& cost);

    void print_status() const;
};
