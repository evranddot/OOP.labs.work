#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <iterator>
#include <cstddef>
#include "Unit.h"
#include "Terrain.h"
#include "Base.h"
#include "Objects.h"
#include <random>


class GameField {
private:
    int width, height;
    size_t max_units;
    std::vector<std::unique_ptr<Unit>> units;
    std::vector<std::vector<std::shared_ptr<Terrain>>> terrain_grid;
    std::vector<std::shared_ptr<NeutralObject>> neutral_objects;

    bool is_cell_occupied(int x, int y) const {
        for (const auto& u : units) {
            if (u && u->get_x() == x && u->get_y() == y) return true;
        }
        return false;
    }

    std::vector<std::shared_ptr<Base>> bases;

    bool is_cell_occupied_base(int x, int y) const {
        for (const auto& u : units) {
            if (u && u->get_x() == x && u->get_y() == y) return true;
        }
        for (const auto& b : bases) {
            if (b && !b->is_destroyed() && b->get_x() == x && b->get_y() == y) return true;
        }
        return false;
    }




public:
    template <bool IsConst>
    class BasicGameFieldIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Unit;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<IsConst, const Unit*, Unit*>;
        using reference = std::conditional_t<IsConst, const Unit&, Unit&>;

    private:
        using PtrType = std::conditional_t<IsConst, const std::unique_ptr<Unit>*, std::unique_ptr<Unit>*>;
        PtrType current;

    public:
        BasicGameFieldIterator() noexcept : current(nullptr) {}
        explicit BasicGameFieldIterator(PtrType ptr) noexcept : current(ptr) {}

        reference operator*() const noexcept { return **current; }
        pointer operator->() const noexcept { return current->get(); }

        BasicGameFieldIterator& operator++() noexcept { ++current; return *this; }
        BasicGameFieldIterator operator++(int) noexcept {
            BasicGameFieldIterator tmp = *this;
            ++current;
            return tmp;
        }

        bool operator==(const BasicGameFieldIterator& other) const noexcept { return current == other.current; }
        bool operator!=(const BasicGameFieldIterator& other) const noexcept { return current != other.current; }
    };

    using GameFieldIterator = BasicGameFieldIterator<false>;
    using ConstGameFieldIterator = BasicGameFieldIterator<true>;

    GameFieldIterator begin() noexcept { return GameFieldIterator(units.data()); }
    GameFieldIterator end() noexcept { return GameFieldIterator(units.data() + units.size()); }
    ConstGameFieldIterator begin() const noexcept { return ConstGameFieldIterator(units.data()); }
    ConstGameFieldIterator end() const noexcept { return ConstGameFieldIterator(units.data() + units.size()); }
    ConstGameFieldIterator cbegin() const noexcept { return begin(); }
    ConstGameFieldIterator cend() const noexcept { return end(); }

    GameField(int w, int h, size_t max_u) : width(w), height(h), max_units(max_u) {
        units.reserve(max_u);
        terrain_grid.resize(height, std::vector<std::shared_ptr<Terrain>>(width));
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                terrain_grid[y][x] = std::make_shared<TerrainProxy>();
            }
        }
    }

    GameField(const GameField& other)
        : width(other.width), height(other.height), max_units(other.max_units), terrain_grid(other.terrain_grid) {
        units.reserve(other.units.size());
        for (const auto& u : other.units) {
            if (u) units.push_back(u->clone());
        }
    }

    GameField& operator=(const GameField& other) {
        if (this != &other) {
            width = other.width;
            height = other.height;
            max_units = other.max_units;
            terrain_grid = other.terrain_grid;
            units.clear();
            units.reserve(other.units.size());
            for (const auto& u : other.units) {
                if (u) units.push_back(u->clone());
            }
        }
        return *this;
    }

    GameField(GameField&& other) noexcept
        : width(other.width), height(other.height), max_units(other.max_units),
        units(std::move(other.units)), terrain_grid(std::move(other.terrain_grid)) {
        other.width = 0; other.height = 0; other.max_units = 0;
    }

    GameField& operator=(GameField&& other) noexcept {
        if (this != &other) {
            width = other.width;
            height = other.height;
            max_units = other.max_units;
            units = std::move(other.units);
            terrain_grid = std::move(other.terrain_grid);
            other.width = 0; other.height = 0; other.max_units = 0;
        }
        return *this;
    }

    ~GameField() = default;

    void set_terrain(int x, int y, std::unique_ptr<Terrain> terrain) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            terrain_grid[y][x] = std::make_shared<TerrainProxy>(std::move(terrain));
        }
    }

    bool add_base(std::shared_ptr<Base> base) {
        if (!base) return false;
        int x = base->get_x();
        int y = base->get_y();
        if (x < 0 || x >= width || y < 0 || y >= height || is_cell_occupied(x, y)) {
            std::cout << "[Ошибка] Невозможно разместить базу на (" << x << ", " << y << ")!\n";
            return false;
        }
        bases.push_back(std::move(base));
        return true;
    }


    bool add_unit(std::unique_ptr<Unit> unit) {
        if (!unit) return false;
        int x = unit->get_x();
        int y = unit->get_y();

        if (x < 0 || x >= width || y < 0 || y >= height) {
            std::cout << "[Ошибка] Координаты юнита выходят за границы поля!\n";
            return false;
        }
        if (units.size() >= max_units) {
            std::cout << "[Ошибка] Превышен лимит юнитов на поле!\n";
            return false;
        }
        if (is_cell_occupied(x, y)) {
            std::cout << "[Ошибка] Клетка (" << x << ", " << y << ") уже занята!\n";
            return false;
        }
        if (!terrain_grid[y][x]->isPassable(unit->get_role())) {
            std::cout << "[Ошибка] Юнит не может быть размещён на этом типе ландшафта!\n";
            return false;
        }

        int bonus = static_cast<int>(terrain_grid[y][x]->getDefenseBonus(unit->get_role()));
        unit->change_armor(bonus);

        units.push_back(std::move(unit));
        return true;
    }

    bool move_unit(Unit& unit, int new_x, int new_y) {
        if (new_x < 0 || new_x >= width || new_y < 0 || new_y >= height) {
            std::cout << "[Предупреждение] Ход невозможен: координаты (" << new_x << ", " << new_y << ") за пределами поля!\n";
            return false;
        }
        if (is_cell_occupied(new_x, new_y)) {
            std::cout << "[Предупреждение] Ход невозможен: клетка (" << new_x << ", " << new_y << ") занята!\n";
            return false;
        }
        if (!terrain_grid[new_y][new_x]->isPassable(unit.get_role())) {
            std::cout << "[Предупреждение] Ход в (" << new_x << ", " << new_y << ") невозможен: непроходимый ландшафт!\n";
            return false;
        }

        int old_bonus = static_cast<int>(terrain_grid[unit.get_y()][unit.get_x()]->getDefenseBonus(unit.get_role()));
        int new_bonus = static_cast<int>(terrain_grid[new_y][new_x]->getDefenseBonus(unit.get_role()));
        unit.change_armor(new_bonus - old_bonus);

        unit.set_coordinates(new_x, new_y);

        for (auto& obj : neutral_objects) {
            if (obj && !obj->consumed() && obj->get_x() == new_x && obj->get_y() == new_y) {
                if (auto hangar = std::dynamic_pointer_cast<Hangar>(obj)) unit + *hangar;
                else if (auto trap = std::dynamic_pointer_cast<Trap>(obj)) unit - *trap;
                else if (auto warehouse = std::dynamic_pointer_cast<Warehouse>(obj)) unit** warehouse;
                else if (auto food = std::dynamic_pointer_cast<Food>(obj)) unit / *food;
            }
        }

        return true;
    }

    void move_unit(size_t index, int new_x, int new_y) {
        if (index < units.size() && units[index]) {
            move_unit(*units[index], new_x, new_y);
        }
    }

    std::vector<Unit*> find_targets_for(const Unit& attacker) {
        std::vector<Unit*> targets;
        for (Unit& candidate : *this) {
            if (attacker.can_attack(candidate)) {
                targets.push_back(&candidate);
            }
        }
        return targets;
    }

    void remove_dead_units() {
        for (auto it = units.begin(); it != units.end(); ) {
            if ((*it)->get_health_obj().is_dead()) {
                std::cout << "Юнит " << (*it)->get_type() << " погиб и удален.\n";
                it = units.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    bool remove_unit_by_index(size_t index) {
        if (index >= units.size()) {
            std::cout << "[Ошибка] Юнита с индексом " << index << " не существует!\n";
            return false;
        }
        std::cout << "Юнит " << units[index]->get_type() << " был принудительно удален игроком.\n";
        units.erase(units.begin() + index);
        return true;
    }

    void display() const {
        std::vector<std::vector<std::string>> display_grid(height, std::vector<std::string>(width, "\033[33m.\033[0m"));

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (terrain_grid[y][x]) {
                    display_grid[y][x] = terrain_grid[y][x]->getSymbol();
                }
            }
        }

        for (const auto& obj : neutral_objects) {
            if (obj && !obj->consumed() && obj->get_x() >= 0 && obj->get_x() < width && obj->get_y() >= 0 && obj->get_y() < height) {
                display_grid[obj->get_y()][obj->get_x()] = obj->get_symbol();
            }
        }

        for (const auto& b : bases) {
            if (b && !b->is_destroyed() && b->get_x() >= 0 && b->get_x() < width && b->get_y() >= 0 && b->get_y() < height) {
                display_grid[b->get_y()][b->get_x()] = std::string("\033[95m") + b->get_symbol() + "\033[0m";
            }
        }

        for (const auto& u : units) {
            if (u->get_x() >= 0 && u->get_x() < width && u->get_y() >= 0 && u->get_y() < height) {
                display_grid[u->get_y()][u->get_x()] = std::string(1,u->get_symbol());
            }
        }

        for (int x = 0; x < width + 2; ++x) std::cout << "X ";
        std::cout << '\n';

        for (int y = 0; y < height; ++y) {
            std::cout << "X ";
            for (int x = 0; x < width; ++x) std::cout << display_grid[y][x] << ' ';
            std::cout << "X \n";
        }

        for (int x = 0; x < width + 2; ++x) std::cout << "X ";
        std::cout << "\n\n";
    }

    void generate_random_terrain() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(1, 100);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int roll = dist(gen);
                if (roll <= 15) {
                    set_terrain(x, y, std::make_unique<Forest>());
                }
                else if (roll <= 25) {
                    set_terrain(x, y, std::make_unique<Mountains>());
                }
                else if (roll <= 35) {
                    set_terrain(x, y, std::make_unique<Ravines>());
                }
            }
        }
    }

    bool add_neutral_object(std::shared_ptr<NeutralObject> obj) {
        if (!obj) return false;
        int x = obj->get_x();
        int y = obj->get_y();
        if (x < 0 || x >= width || y < 0 || y >= height || is_cell_occupied(x, y)) {
            return false;
        }
        neutral_objects.push_back(std::move(obj));
        return true;
    }

    size_t get_units_count() const noexcept { return units.size(); }
};
