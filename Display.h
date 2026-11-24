#pragma once
#include <vector>
#include <string>
#include <memory>

class GameField;
class Building;

class Display {
public:
    void render(const GameField& current,
        const std::vector<std::unique_ptr<Building>>* buildings = nullptr,
        bool overwrite = true) const;

    static void load_map(GameField& field, const std::vector<std::string>& map_layout);
    static void load_default_map(GameField& field);
};
