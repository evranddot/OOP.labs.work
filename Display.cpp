#include "Display.h"
#include "GameField.h"
#include "Terrain.h"
#include "Building.h"
#include "Base.h"
#include "Units.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <windows.h>

namespace {
    std::string get_unit_color(const Unit& u) {
        if (dynamic_cast<const RebelUnit*>(&u))    return "\033[94m"; // Синий: Повстанцы
        if (dynamic_cast<const ImperialUnit*>(&u)) return "\033[91m"; // Красный: Империя
        if (dynamic_cast<const BountyUnit*>(&u))   return "\033[93m"; // Жёлтый: Наёмники
        return "\033[92m";                                            // Зелёный: Юниты игрока
    }

    std::string format_unit_symbol(const Unit& u) {
        return get_unit_color(u) + std::string(1, u.get_symbol()) + "\033[0m";
    }

    std::vector<std::string> build_grid_lines(
        const GameField& field,
        const std::vector<std::unique_ptr<Building>>* buildings = nullptr)
    {
        int width = field.get_width();
        int height = field.get_height();

        std::vector<std::vector<std::string>> grid(height, std::vector<std::string>(width, "\033[33m.\033[0m"));

        // 1. Ландшафт
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (const Terrain* t = field.get_terrain(x, y)) {
                    grid[y][x] = t->getSymbol();
                }
            }
        }

        // 2. Нейтральные объекты (#, !, &, @)
        for (const auto& obj : field.get_neutral_objects()) {
            if (obj && !obj->consumed()) {
                int x = obj->get_x();
                int y = obj->get_y();
                if (x >= 0 && x < width && y >= 0 && y < height) {
                    grid[y][x] = obj->get_symbol();
                }
            }
        }

        // 3. Здания
        if (buildings) {
            for (const auto& b : *buildings) {
                if (b && !b->is_destroyed()) {
                    int x = b->get_x();
                    int y = b->get_y();
                    if (x >= 0 && x < width && y >= 0 && y < height) {
                        grid[y][x] = std::string("\033[96m") + b->get_symbol() + "\033[0m";
                    }
                }
            }
        }

        // 4. Базы
        for (const auto& b : field.get_bases()) {
            if (b && !b->is_destroyed()) {
                int x = b->get_x();
                int y = b->get_y();
                if (x >= 0 && x < width && y >= 0 && y < height) {
                    grid[y][x] = std::string("\033[95m") + b->get_symbol() + "\033[0m";
                }
            }
        }

        // 5. Юниты
        for (const auto& u : field) {
            int x = u.get_x();
            int y = u.get_y();
            if (x >= 0 && x < width && y >= 0 && y < height) {
                grid[y][x] = format_unit_symbol(u);
            }
        }

        std::vector<std::string> lines;
        lines.reserve(height + 2);

        std::string border;
        for (int x = 0; x < width + 2; ++x) border += "X ";
        lines.push_back(border);

        for (int y = 0; y < height; ++y) {
            std::string row = "X ";
            for (int x = 0; x < width; ++x) {
                row += grid[y][x];
                row += ' ';
            }
            row += "X ";
            lines.push_back(std::move(row));
        }

        lines.push_back(border);
        return lines;
    }

    std::vector<std::string> build_right_panel(const GameField& field) {
        std::vector<std::string> info;

        // 1. Информация о базах
        const auto& bases = field.get_bases();
        info.push_back("=== БАЗЫ НА КАРТЕ (" + std::to_string(bases.size()) + ") ===");
        if (bases.empty()) {
            info.push_back("  Базы отсутствуют.");
        }
        else {
            for (const auto& b : bases) {
                if (!b) continue;
                std::string status = b->is_destroyed() ? "УНИЧТОЖЕНА" : ("ХП:" + std::to_string(b->get_health()));
                std::string line =
                    "[\033[95m" + std::string(1, b->get_symbol()) + "\033[0m] " + b->get_name() +
                    " (" + std::to_string(b->get_x()) + "," + std::to_string(b->get_y()) + ")" +
                    " | " + status +
                    " | Защ:" + std::to_string(b->get_defense_level()) +
                    " | Юниты:" + std::to_string(b->get_active_count()) + "/" + std::to_string(b->get_max_units()) +
                    " | Здания:" + std::to_string(b->get_buildings().size()) +
                    " | Потерь:" + std::to_string(b->get_destroyed_history().size());
                info.push_back(std::move(line));
            }
        }

        info.push_back("");

        // 2. Информация о юнитах
        info.push_back("=== АКТУАЛЬНЫЕ ЮНИТЫ (" + std::to_string(field.get_units_count()) + ") ===");
        if (field.get_units_count() == 0) {
            info.push_back("  На карте нет активных юнитов.");
        }
        else {
            for (const auto& u : field) {
                std::string color = get_unit_color(u);
                std::string line =
                    "ID:" + std::to_string(u.get_id()) + " [" + color + u.get_symbol() + "\033[0m] " +
                    u.get_type() +
                    " | (" + std::to_string(u.get_x()) + "," + std::to_string(u.get_y()) + ")" +
                    " | ХП:" + std::to_string(u.get_health_obj().get_health()) +
                    " Бр:" + std::to_string(u.get_armor_obj().get_armor()) +
                    " Ур:" + std::to_string(u.get_attack_obj().get_damage()) +
                    "(R" + std::to_string(u.get_attack_obj().get_attack_range()) + ")" +
                    " | Ход:" + std::to_string(u.get_move_points()) + "/" + std::to_string(u.get_max_move_points()) +
                    " | " + u.get_order();
                info.push_back(std::move(line));
            }
        }

        info.push_back("");

        // 3. Список возможных команд и легенда (под юнитами)
        info.push_back("=== ВОЗМОЖНЫЕ КОМАНДЫ ===");
        info.push_back("  create unit <тип> at <x>,<y>          - нанять юнита (infantry|scout|tank|worker|medic)");
        info.push_back("  move unit <id> to <x>,<y>             - переместить юнита в клетку (x, y)");
        info.push_back("  wasd <id>                             - пошаговое управление юнитом с клавиатуры");
        info.push_back("  attack unit <id> target <targetId>    - атаковать вражеского юнита или здание");
        info.push_back("  heal <id> <targetId>                  - вылечить союзного юнита");
        info.push_back("  collect unit <id> from <pointId>      - собрать ресурсы с клетки местности");
        info.push_back("  build <здание> at <x>,<y>             - построить (barracks|powerplant|laboratory|warehouse|hospital)");
        info.push_back("  research <технология>                 - изучить улучшение (attack_upgrade|armor|heavy)");
        info.push_back("  produce <тип> in <buildingId>         - произвести юнита в готовом здании");
        info.push_back("  distribute <ресурс> to <цель> <кол>   - перевести ресурсы (wood|metal|energy)");
        info.push_back("  stats [units|buildings|resources]     - показать подробную статистику");
        info.push_back("  turn | undo | demo | exit             - след. ход | отмена хода | демо | выход");
        info.push_back("  Объекты: \033[91m#\033[0m Ангар | \033[91m&\033[0m Склад | \033[91m@\033[0m Еда | \033[91m!\033[0m Ловушка");

        return info;
    }
}

void Display::render(
    const GameField& current,
    const std::vector<std::unique_ptr<Building>>* buildings,
    bool overwrite) const
{
    std::cout.flush();
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi{};
    COORD saved_pos{ 0, 0 };

    if (overwrite && GetConsoleScreenBufferInfo(hOut, &csbi)) {
        saved_pos = csbi.dwCursorPosition;
        SetConsoleCursorPosition(hOut, { 0, 0 });
    }

    int width = current.get_width();
    int field_visual_width = (width + 2) * 2;

    auto field_lines = build_grid_lines(current, buildings);
    auto right_lines = build_right_panel(current);

    std::string header_left = "=== ИГРОВОЕ ПОЛЕ ===";
    if (static_cast<int>(header_left.size()) < field_visual_width) {
        header_left.append(field_visual_width - header_left.size(), ' ');
    }

    std::string frame;
    frame += header_left + " | " + (right_lines.empty() ? "" : right_lines[0]) + "\033[K\n";

    size_t right_body_count = right_lines.empty() ? 0 : right_lines.size() - 1;
    // Резервируем минимум 28 строк под поле + правую панель (базы + юниты + команды)
    size_t total_rows = (std::max)({ field_lines.size(), right_body_count, static_cast<size_t>(27) });
    std::string empty_left(field_visual_width, ' ');

    for (size_t i = 0; i < total_rows; ++i) {
        std::string left = (i < field_lines.size()) ? field_lines[i] : empty_left;
        std::string right = (i + 1 < right_lines.size()) ? right_lines[i + 1] : "";
        frame += left + " | " + right + "\033[K\n";
    }

    std::cout << frame << std::flush;

    if (overwrite) {
        SHORT min_log_y = static_cast<SHORT>(total_rows + 1);
        if (saved_pos.Y < min_log_y) {
            saved_pos.Y = min_log_y;
            saved_pos.X = 0;
        }
        SetConsoleCursorPosition(hOut, saved_pos);
    }
}

void Display::load_map(GameField& field, const std::vector<std::string>& map_layout) {
    int width = field.get_width();
    int height = field.get_height();
    int rows = static_cast<int>(map_layout.size());

    for (int y = 0; y < (std::min)(height, rows); ++y) {
        int cols = static_cast<int>(map_layout[y].size());
        for (int x = 0; x < (std::min)(width, cols); ++x) {
            char tile = map_layout[y][x];
            switch (tile) {
            case '*':
            case 'F':
                field.set_terrain(x, y, std::make_unique<Forest>());
                break;
            case '^':
            case 'M':
                field.set_terrain(x, y, std::make_unique<Mountains>());
                break;
            case '_':
            case 'R':
                field.set_terrain(x, y, std::make_unique<Ravines>());
                break;
            case '.':
            default:
                field.set_terrain(x, y, std::make_unique<Ground>());
                break;
            }
        }
    }
}

void Display::load_default_map(GameField& field) {
    std::vector<std::string> default_layout = {
        "....**.......^^.....",
        "....**.......^^.....",
        "..****..............",
        ".........____.......",
        ".........____.......",
        "....................",
        ".....^^........**...",
        ".....^^........**...",
        "...............**...",
        "....................",
        "....____............",
        "....____.......^^...",
        "...............^^...",
        "....................",
        "..**................",
        "..**.......____.....",
        "...........____.....",
        "....................",
        ".....^^........**...",
        ".....^^........**..."
    };

    load_map(field, default_layout);
}