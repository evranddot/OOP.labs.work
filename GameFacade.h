#pragma once
#include <string>
#include <memory>

class GameMediator;
class Handler;
class Command;
class CommandHistory;
class GameField;
class Base;

class GameFacade {
private:
    std::shared_ptr<GameMediator> mediator_;
    std::unique_ptr<CommandHistory> history_;
    std::shared_ptr<Handler> chain_head_;

    void init_handler_chain();

public:
    GameFacade(GameField& field, std::shared_ptr<Base> player_base);
    ~GameFacade();

    GameFacade(const GameFacade&) = delete;
    GameFacade& operator=(const GameFacade&) = delete;

    bool getUnitPosition(int unit_id, int& out_x, int& out_y) const;
    bool getUnitState(int unit_id, int& out_x, int& out_y, int& out_moves_left) const;


    // Этап 4, пункт 2: выполнение команды с записью в историю
    void executeCommand(std::shared_ptr<Command> cmd);

    // Этап 4, пункт 14: передача строки в первый обработчик цепочки
    bool processInput(const std::string& input);

    bool undo();

    // Этап 4, пункт 1: 10 методов фасада
    void createUnit(const std::string& unit_type, int x, int y);
    void moveUnit(int unit_id, int x, int y);
    void attack(int attacker_id, int target_id);
    void collectResource(int unit_id, int resource_point_id = 0);
    void heal(int unit_id, int target_id);
    void buildBuilding(const std::string& building_type, int x, int y);
    void research(const std::string& tech_id);
    void distributeResources(const std::string& from, const std::string& to, int amount);
    void produceUnit(int building_id, const std::string& unit_type);
    void getStats(const std::string& filter = "") const;

    // Сервисные методы для игрового цикла
    void inspectUnit(int unit_id) const;
    void nextTurn();
    void renderField() const;
};