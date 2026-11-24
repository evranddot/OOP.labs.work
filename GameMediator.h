#pragma once
#include <string>
#include <memory>

class GameField;
class Base;
class Unit;
class Building;
class UnitManager;
class ResourceSystem;
class Combat;
class BuildingManager;

class GameMediator {
private:
    GameField& field_;
    std::unique_ptr<GameField> prev_field_;
    std::shared_ptr<Base> player_base_;

    std::unique_ptr<UnitManager> unit_manager_;
    std::unique_ptr<ResourceSystem> resource_system_;
    std::unique_ptr<Combat> combat_system_;
    std::unique_ptr<BuildingManager> building_manager_;

    int turn_number_ = 1;

    void save_snapshot();

public:
    GameMediator(GameField& field, std::shared_ptr<Base> player_base);
    ~GameMediator();

    GameMediator(const GameMediator&) = delete;
    GameMediator& operator=(const GameMediator&) = delete;
    bool getUnitPosition(int unitId, int& out_x, int& out_y) const;


    // 5 методов уведомлений Посредника (Этап 2, пункт 10)
    void notifyUnitCreated(const Unit& unit);
    void notifyResourceCollected(const std::string& resource_type, int amount);
    void notifyCombatResolved(const Unit& attacker, const std::string& target_name, int damage_dealt);
    void notifyBuildingCompleted(const std::string& building_type);
    void notifyResearchCompleted(const std::string& tech_name);

    // Игровые операции, вызываемые из Command (Этап 3, пункты 3–12)
    void createUnit(const std::string& type, int x, int y);
    void moveUnit(int unitId, int targetX, int targetY);
    void resolveCombat(int attackerId, int targetId);
    void collectResource(int unitId, int resourcePointId = 0);
    void healUnit(int unitId, int targetId);
    void buildBuilding(const std::string& buildingType, int x, int y);
    void startResearch(const std::string& technologyId);
    void distributeResources(const std::string& from, const std::string& to, int amount);
    void produceUnit(int buildingId, const std::string& unitType);
    void getStats(const std::string& filter = "") const;

    // Сервисные методы (откат хода, инспекция, смена хода, отрисовка поля)
    void undoLastAction();
    void inspect_unit(int unitId) const;
    void end_turn();
    void render_field() const;


    bool getUnitState(int unitId, int& out_x, int& out_y, int& out_moves_left) const;

};
