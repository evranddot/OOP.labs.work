#include <iostream>
#include <windows.h>
#include "GameField.h"
#include "UnitFactory.h"
#include "Base.h"

const int CONST_SLEEP = 500;


void print_units_info(const GameField& field) {
    size_t counter = 1;
    for (const auto& creature : field) {
        std::cout << counter++ << " : [" << creature.get_symbol() << "] " << creature.get_type()
            << " на позиции (" << creature.get_x() << ", " << creature.get_y() << ")"
            << " ХП: " << creature.get_health_obj().get_health()
            << " Броня: " << creature.get_armor_obj().get_armor() << '\n';
    }
    std::cout << '\n';
}





int main() {
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    BountyFactory bounty_fab;
    RebelFactory rebel_fab;
    ImperialFactory imperial_fab;


    std::cout << "Шаг 1: Создание поля и наполнение юнитами\n";
    GameField field1(20, 20, 12);
    field1.generate_random_terrain();

    auto rebel_base = std::make_shared<Base>("Rebel Alliance", 0, 0, 1200, 10, 15, std::make_unique<RebelFactory>(), 'V');
    auto imperial_base = std::make_shared<Base>("Galactic Empire", 19, 19, 1200, 10, 20, std::make_unique<ImperialFactory>(), 'I');
    auto bounty_base = std::make_shared<Base>("Bounty Hunters", 15, 3, 1200, 10, 25, std::make_unique<BountyFactory>(), 'B');

    field1.add_base(rebel_base);
    field1.add_base(imperial_base);
    field1.add_base(bounty_base);

    auto hangar = std::make_shared<Hangar>(5, 12);
    auto trap = std::make_shared<Trap>(13, 2);
    auto warehouse = std::make_shared<Warehouse>(7, 7);
    auto food = std::make_shared<Food>(4, 16);

    field1.add_neutral_object(hangar);
    field1.add_neutral_object(trap);
    field1.add_neutral_object(warehouse);
    field1.add_neutral_object(food);



    field1.add_unit(bounty_base->createUnit(Role::Machine, 11, 11));
    field1.add_unit(rebel_base->createUnit(Role::Standard, 3, 2));
    field1.add_unit(rebel_base->createUnit(Role::Heavy, 1, 1));
    field1.add_unit(rebel_base->createUnit(Role::Standard, 1, 3));
    field1.add_unit(imperial_base->createUnit(Role::Heavy, 10, 2));
    field1.add_unit(imperial_base->createUnit(Role::Standard, 10, 4));
    field1.add_unit(imperial_base->createUnit(Role::Standard, 11, 5));
    field1.add_unit(imperial_base->createUnit(Role::Machine, 11, 7));

    std::cout << "Карта мира:\n\n";
    field1.display();

    Sleep(CONST_SLEEP);

    std::cout << "Шаг 2: Список юнитов на поле:\n";
    print_units_info(field1);

    Sleep(CONST_SLEEP);

    Unit* atat = nullptr;
    for (auto& creature : field1) {
        if (creature.get_type().find("AT-AT") != std::string::npos) {
            atat = &creature;
            break;
        }
    }

    if (atat != nullptr) {
        for (int volley = 1; volley <= 5; ++volley) {
            for (auto& target : field1) {
                if (target.get_type().find("Rebel") != std::string::npos && !target.get_health_obj().is_dead()) {
                    if (atat->attack_unit(target)) {
                        break;
                    }
                }
            }
        }
    }

    field1.remove_dead_units();

    Sleep(CONST_SLEEP);

    for (auto& creature : field1) {
        if (creature.get_type().find("Rebel") != std::string::npos && !creature.get_health_obj().is_dead()) {
            int old_x = creature.get_x();
            int old_y = creature.get_y();

            if (!field1.move_unit(creature, old_x, old_y + 2)) {
                field1.move_unit(creature, old_x + 2, old_y);
            }
        }
    }


    std::cout << "\nШаг 3: Поле боя после атаки:\n\n";
    field1.display();
    
    std::cout << "Информация об юнитах после атаки\n";
    print_units_info(field1);

    Sleep(CONST_SLEEP);

    return 0;
}
