#pragma once
#include <string>
#include <memory>
#include <vector>

class GameMediator;

// 1. Абстрактный класс Command
class Command {
protected:
    std::shared_ptr<GameMediator> mediator_;

public:
    explicit Command(std::shared_ptr<GameMediator> mediator) : mediator_(std::move(mediator)) {}
    virtual ~Command() = default;

    virtual void execute() = 0;
    virtual void undo() {}
    virtual std::string getName() const = 0;
};

// 2. История команд с поддержкой отката (Undo)
class CommandHistory {
private:
    std::vector<std::shared_ptr<Command>> history_;

public:
    void push(std::shared_ptr<Command> cmd);
    bool undo();
    std::shared_ptr<Command> getLastCommand() const noexcept;
    size_t size() const noexcept { return history_.size(); }
};


// 3. CreateUnitCommand
class CreateUnitCommand : public Command {
private:
    std::string type;
    int x;
    int y;

public:
    CreateUnitCommand(std::shared_ptr<GameMediator> mediator, std::string unit_type, int pos_x, int pos_y);
    void execute() override;
    void undo() override;
    std::string getName() const override { return "CreateUnitCommand"; }
};

// 4. MoveUnitCommand
class MoveUnitCommand : public Command {
private:
    int unitId;
    int targetX;
    int targetY;

public:
    MoveUnitCommand(std::shared_ptr<GameMediator> mediator, int unit_id, int target_x, int target_y);
    void execute() override;
    void undo() override;
    std::string getName() const override { return "MoveUnitCommand"; }
};

// 5. AttackCommand
class AttackCommand : public Command {
private:
    int attackerId;
    int targetId;

public:
    AttackCommand(std::shared_ptr<GameMediator> mediator, int attacker_id, int target_id);
    void execute() override;
    void undo() override;
    std::string getName() const override { return "AttackCommand"; }
};

// 6. CollectResourceCommand
class CollectResourceCommand : public Command {
private:
    int unitId;
    int resourcePointId;

public:
    CollectResourceCommand(std::shared_ptr<GameMediator> mediator, int unit_id, int resource_point_id = 0);
    void execute() override;
    std::string getName() const override { return "CollectResourceCommand"; }
};

// 7. HealCommand
class HealCommand : public Command {
private:
    int unitId;
    int targetId;

public:
    HealCommand(std::shared_ptr<GameMediator> mediator, int unit_id, int target_id);
    void execute() override;
    std::string getName() const override { return "HealCommand"; }
};

// 8. BuildBuildingCommand
class BuildBuildingCommand : public Command {
private:
    std::string buildingType;
    int x;
    int y;

public:
    BuildBuildingCommand(std::shared_ptr<GameMediator> mediator, std::string building_type, int pos_x, int pos_y);
    void execute() override;
    void undo() override;
    std::string getName() const override { return "BuildBuildingCommand"; }
};

// 9. ResearchCommand
class ResearchCommand : public Command {
private:
    std::string technologyId;

public:
    ResearchCommand(std::shared_ptr<GameMediator> mediator, std::string tech_id);
    void execute() override;
    std::string getName() const override { return "ResearchCommand"; }
};

// 10. DistributeResourcesCommand
class DistributeResourcesCommand : public Command {
private:
    std::string from;
    std::string to;
    int amount;

public:
    DistributeResourcesCommand(std::shared_ptr<GameMediator> mediator, std::string res_from, std::string res_to, int res_amount);
    void execute() override;
    std::string getName() const override { return "DistributeResourcesCommand"; }
};

// 11. ProduceUnitCommand
class ProduceUnitCommand : public Command {
private:
    int buildingId;
    std::string unitType;

public:
    ProduceUnitCommand(std::shared_ptr<GameMediator> mediator, int building_id, std::string unit_type);
    void execute() override;
    void undo() override;
    std::string getName() const override { return "ProduceUnitCommand"; }
};

// 12. StatsCommand
class StatsCommand : public Command {
public:
    explicit StatsCommand(std::shared_ptr<GameMediator> mediator);
    void execute() override;
    std::string getName() const override { return "StatsCommand"; }
};
