#include "Command.h"
#include "GameMediator.h"
#include <iostream>
#include <utility>

// --- CommandHistory ---

void CommandHistory::push(std::shared_ptr<Command> cmd) {
    if (cmd) {
        history_.push_back(std::move(cmd));
    }
}


bool CommandHistory::undo() {
    if (history_.empty()) {
        std::cout << "[История команд] Нет команд для отмены.\n";
        return false;
    }
    std::cout << "[История команд] Отмена последней команды: " << history_.back()->getName() << "\n";
    history_.back()->undo();
    history_.pop_back();
    return true;
}

std::shared_ptr<Command> CommandHistory::getLastCommand() const noexcept {
    return history_.empty() ? nullptr : history_.back();
}


// --- Конкретные команды ---

CreateUnitCommand::CreateUnitCommand(std::shared_ptr<GameMediator> mediator, std::string unit_type, int pos_x, int pos_y)
    : Command(std::move(mediator)), type(std::move(unit_type)), x(pos_x), y(pos_y) {
}

void CreateUnitCommand::execute() {
    if (mediator_) mediator_->createUnit(type, x, y);
}

void CreateUnitCommand::undo() {
    if (mediator_) mediator_->undoLastAction();
}

MoveUnitCommand::MoveUnitCommand(std::shared_ptr<GameMediator> mediator, int unit_id, int target_x, int target_y)
    : Command(std::move(mediator)), unitId(unit_id), targetX(target_x), targetY(target_y) {
}

void MoveUnitCommand::execute() {
    if (mediator_) mediator_->moveUnit(unitId, targetX, targetY);
}

void MoveUnitCommand::undo() {
    if (mediator_) mediator_->undoLastAction();
}

AttackCommand::AttackCommand(std::shared_ptr<GameMediator> mediator, int attacker_id, int target_id)
    : Command(std::move(mediator)), attackerId(attacker_id), targetId(target_id) {
}

void AttackCommand::execute() {
    if (mediator_) mediator_->resolveCombat(attackerId, targetId);
}

void AttackCommand::undo() {
    if (mediator_) mediator_->undoLastAction();
}

CollectResourceCommand::CollectResourceCommand(std::shared_ptr<GameMediator> mediator, int unit_id, int resource_point_id)
    : Command(std::move(mediator)), unitId(unit_id), resourcePointId(resource_point_id) {
}

void CollectResourceCommand::execute() {
    if (mediator_) mediator_->collectResource(unitId, resourcePointId);
}

HealCommand::HealCommand(std::shared_ptr<GameMediator> mediator, int unit_id, int target_id)
    : Command(std::move(mediator)), unitId(unit_id), targetId(target_id) {
}

void HealCommand::execute() {
    if (mediator_) mediator_->healUnit(unitId, targetId);
}

BuildBuildingCommand::BuildBuildingCommand(std::shared_ptr<GameMediator> mediator, std::string building_type, int pos_x, int pos_y)
    : Command(std::move(mediator)), buildingType(std::move(building_type)), x(pos_x), y(pos_y) {
}

void BuildBuildingCommand::execute() {
    if (mediator_) mediator_->buildBuilding(buildingType, x, y);
}

void BuildBuildingCommand::undo() {
    if (mediator_) mediator_->undoLastAction();
}

ResearchCommand::ResearchCommand(std::shared_ptr<GameMediator> mediator, std::string tech_id)
    : Command(std::move(mediator)), technologyId(std::move(tech_id)) {
}

void ResearchCommand::execute() {
    if (mediator_) mediator_->startResearch(technologyId);
}

DistributeResourcesCommand::DistributeResourcesCommand(std::shared_ptr<GameMediator> mediator, std::string res_from, std::string res_to, int res_amount)
    : Command(std::move(mediator)), from(std::move(res_from)), to(std::move(res_to)), amount(res_amount) {
}

void DistributeResourcesCommand::execute() {
    if (mediator_) mediator_->distributeResources(from, to, amount);
}

ProduceUnitCommand::ProduceUnitCommand(std::shared_ptr<GameMediator> mediator, int building_id, std::string unit_type)
    : Command(std::move(mediator)), buildingId(building_id), unitType(std::move(unit_type)) {
}

void ProduceUnitCommand::execute() {
    if (mediator_) mediator_->produceUnit(buildingId, unitType);
}

void ProduceUnitCommand::undo() {
    if (mediator_) mediator_->undoLastAction();
}

StatsCommand::StatsCommand(std::shared_ptr<GameMediator> mediator)
    : Command(std::move(mediator)) {
}

void StatsCommand::execute() {
    if (mediator_) mediator_->getStats();
}
