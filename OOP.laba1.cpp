#include <iostream>
#include <sstream>
#include <string>
#include <functional>
#include <windows.h>
#include <conio.h>
#include "GameField.h"
#include "UnitFactory.h"
#include "Base.h"
#include "Objects.h"
#include "GameFacade.h"

void init_console() {
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    std::cout << "\033[2J\033[H";
}

// Выполняет действие (печатая логи внизу), а затем точечно обновляет единственное поле в {0, 0}
void execute_and_update_screen(GameFacade& game, const std::function<void()>& action) {
    action();
    game.renderField();
}

// Интерактивное управление юнитом с помощью WASD
void control_unit_wasd(GameFacade& game, int unit_id) {
    int x = 0, y = 0, moves_left = 0;
    if (!game.getUnitState(unit_id, x, y, moves_left)) {
        std::cout << "[Управление] Юнит с ID:" << unit_id << " не найден!\n";
        return;
    }

    std::cout << "\n==================== РЕЖИМ УПРАВЛЕНИЯ WASD ====================\n"
        << "  Юнит ID: " << unit_id << " | Клавиши: [W/A/S/D] Шаг | [Q / Esc] Выход\n"
        << "===============================================================\n";

    while (true) {
        game.getUnitState(unit_id, x, y, moves_left);
        game.renderField();

        if (moves_left <= 0) {
            std::cout << "[Лимит хода] Очки хода закончились! Нажмите любую клавишу для выхода...\n";
            _getch();
            break;
        }

        int ch = _getch();
        if (ch == 0 || ch == 224) {
            int arrow = _getch();
            if (arrow == 72) ch = 'w';
            else if (arrow == 80) ch = 's';
            else if (arrow == 75) ch = 'a';
            else if (arrow == 77) ch = 'd';
        }

        if (ch == 27 || ch == 'q' || ch == 'Q') {
            break;
        }

        int nx = x, ny = y;
        if (ch == 'w' || ch == 'W') ny -= 1;
        else if (ch == 's' || ch == 'S') ny += 1;
        else if (ch == 'a' || ch == 'A') nx -= 1;
        else if (ch == 'd' || ch == 'D') nx += 1;
        else continue;

        game.moveUnit(unit_id, nx, ny);
    }
}

void run_demo_scenarios(GameFacade& game) {
    std::cout << "=== ДЕМОНСТРАЦИОННЫЕ СЦЕНАРИИ ===\n";
    game.processInput("create unit infantry at 5,5");
    game.processInput("create unit infantry at 6,5");
    game.processInput("move unit 7 to 7,7");
    game.processInput("move unit 8 to 7,8");
    game.processInput("attack unit 7 target 8");
    game.processInput("build barracks at 3,3");
    game.processInput("build laboratory at 3,4");
    game.nextTurn();
    game.nextTurn();
    game.processInput("research attack_upgrade");
    game.processInput("produce infantry in 1");
    game.processInput("collect unit 7 from 1");
    game.processInput("distribute wood to barracks 50");
    game.processInput("stats");
}

int main() {
    init_console();

    GameField field(20, 20, 25);
    field.init_default_map();

    auto player_base = std::make_shared<Base>("Player Base", 0, 0, 1200, 20, 15, std::make_unique<RebelFactory>(), 'V');
    auto enemy_base = std::make_shared<Base>("Empire Base", 19, 19, 1200, 20, 20, std::make_unique<ImperialFactory>(), 'I');
    field.add_base(player_base);
    field.add_base(enemy_base);

    // --- Инициализация нейтральных объектов на карте ---
    field.add_neutral_object(std::make_shared<Hangar>(3, 2));      // # Ремонт/лечение
    field.add_neutral_object(std::make_shared<Warehouse>(2, 4));   // & Бонус брони
    field.add_neutral_object(std::make_shared<Food>(6, 3));        // @ Бонус атаки
    field.add_neutral_object(std::make_shared<Trap>(5, 6));        // ! Ловушка

    field.add_neutral_object(std::make_shared<Hangar>(16, 17));    // # Ремонт/лечение
    field.add_neutral_object(std::make_shared<Warehouse>(17, 15)); // & Бонус брони
    field.add_neutral_object(std::make_shared<Food>(13, 16));      // @ Бонус атаки
    field.add_neutral_object(std::make_shared<Trap>(14, 13));      // ! Ловушка
    // ---------------------------------------------------

    // --- Инициализация 6 стартовых юнитов разных фракций (по 3 на каждую базу) ---
    std::ostringstream startup_log;
    std::streambuf* old_buf = std::cout.rdbuf(startup_log.rdbuf());

    // Фракция Повстанцев (привязаны к player_base)
    field.spawn_faction_unit(*player_base, Role::Standard, 1, 0, "Патруль базы Повстанцев");  // ID: 1
    field.spawn_faction_unit(*player_base, Role::Heavy, 0, 1, "Охрана базы Повстанцев");   // ID: 2
    field.spawn_faction_unit(*player_base, Role::Machine, 1, 1, "Поддержка Повстанцев");     // ID: 3

    // Фракция Империи (привязаны к enemy_base)
    field.spawn_faction_unit(*enemy_base, Role::Standard, 18, 19, "Патруль базы Империи");    // ID: 4
    field.spawn_faction_unit(*enemy_base, Role::Heavy, 19, 18, "Охрана базы Империи");     // ID: 5
    field.spawn_faction_unit(*enemy_base, Role::Machine, 18, 18, "Тяжёлая техника Империи"); // ID: 6

    std::cout.rdbuf(old_buf);
    // -----------------------------------------------------------------------------

    GameFacade game(field, player_base);

    // Очищаем экран один раз при старте, рисуем поле в {0, 0} и выводим стартовый лог строго под полем
    std::cout << "\033[2J\033[H";
    game.renderField();
    std::cout << startup_log.str();

    std::string input;
    while (true) {
        std::cout << "\nВведите команду > ";
        if (!std::getline(std::cin, input)) break;
        if (input.empty()) continue;

        if (input == "exit" || input == "quit") {
            break;
        }
        else if (input.rfind("wasd ", 0) == 0) {
            try {
                int id = std::stoi(input.substr(5));
                control_unit_wasd(game, id);
                execute_and_update_screen(game, [&]() {
                    std::cout << "[Управление] Выход из режима WASD для юнита ID:" << id << ".\n";
                    });
            }
            catch (...) {
                std::cout << "[Синтаксис] Используйте: wasd <unitId>\n";
            }
        }
        else if (input == "demo") {
            execute_and_update_screen(game, [&]() {
                run_demo_scenarios(game);
                });
        }
        else if (input == "turn") {
            execute_and_update_screen(game, [&]() {
                game.nextTurn();
                });
        }
        else if (input == "undo") {
            execute_and_update_screen(game, [&]() {
                game.undo();
                });
        }
        else {
            execute_and_update_screen(game, [&]() {
                game.processInput(input);
                });
        }
    }

    return 0;
}
