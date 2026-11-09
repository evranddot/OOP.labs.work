#include <iostream>
#include <windows.h>
#include "GameField.h"
#include "UnitFactory.h"

int main() {
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);




    BountyFactory bounty_fab;
    RebelFactory rebel_fab;
    ImperialFactory imperial_fab;


    std::cout << "СЦЕНАРИЙ 1: Создание поля и наполнение юнитами\n";
    GameField field1(20, 20, 12);

    field1.add_unit(bounty_fab.create_unit(Role::Machine, 11, 11));
    field1.add_unit(rebel_fab.create_unit(Role::Standard, 3, 2));
    field1.add_unit(rebel_fab.create_unit(Role::Heavy, 4, 4));
    field1.add_unit(rebel_fab.create_unit(Role::Standard, 1, 3));
    field1.add_unit(imperial_fab.create_unit(Role::Heavy, 10, 2));
    field1.add_unit(imperial_fab.create_unit(Role::Standard, 10, 4));
    field1.add_unit(imperial_fab.create_unit(Role::Standard, 11, 3));
    field1.add_unit(imperial_fab.create_unit(Role::Machine, 13, 3));

    std::cout << "Карта мира:\n\n";
    field1.display();

    std::cout << "СЦЕНАРИЙ 2: Тест Итератора и Боевых Характеристик\n";
    std::cout << "Список юнитов на поле:\n";

    size_t counter = 1;
    for (const auto& creature : field1) {
        std::cout << counter++ << " : [" << creature->get_symbol() << "] " << creature->get_type();
        std::cout << " на позиции (" << creature->get_x() << ", " << creature->get_y() << ")";
        std::cout << " ХП: " << creature->get_health_obj().get_health() << ' ';
        std::cout << " Броня: " << creature->get_armor_obj().get_armor() << '\n';
    }

    std::cout << "По повстанцам нанесена серия ударов AT-AT:\n";

    for (auto& creature : field1) {

        if (creature->get_type().find("Rebel") != std::string::npos) {
            creature->take_damage(210);
            std::cout << "Нанесен урон по: " << creature->get_type() << "\n";
        }
    }
    field1.remove_dead_units();

    field1.remove_dead_units();

    std::cout << "\nПоле боя после Армагеддона:\n";
    field1.display();

    return 0;
}