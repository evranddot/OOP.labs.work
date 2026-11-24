#include "GameFacade.h"
#include "GameMediator.h"
#include "Handler.h"
#include "Command.h"
#include <iostream>

GameFacade::GameFacade(GameField& field, std::shared_ptr<Base> player_base)
    : mediator_(std::make_shared<GameMediator>(field, std::move(player_base))),
    history_(std::make_unique<CommandHistory>()) {
    init_handler_chain();
}

GameFacade::~GameFacade() = default;

void GameFacade::init_handler_chain() {
    auto create_h = std::make_shared<CreateHandler>(this);
    auto move_h = std::make_shared<MoveHandler>(this);
    auto attack_h = std::make_shared<AttackHandler>(this);
    auto collect_h = std::make_shared<CollectHandler>(this);
    auto heal_h = std::make_shared<HealHandler>(this);
    auto build_h = std::make_shared<BuildHandler>(this);
    auto research_h = std::make_shared<ResearchHandler>(this);
    auto distribute_h = std::make_shared<DistributeHandler>(this);
    auto produce_h = std::make_shared<ProduceHandler>(this);
    auto stats_h = std::make_shared<StatsHandler>(this);

    create_h->setNext(move_h);
    move_h->setNext(attack_h);
    attack_h->setNext(collect_h);
    collect_h->setNext(heal_h);
    heal_h->setNext(build_h);
    build_h->setNext(research_h);
    research_h->setNext(distribute_h);
    distribute_h->setNext(produce_h);
    produce_h->setNext(stats_h);

    chain_head_ = create_h;
}

bool GameFacade::getUnitPosition(int unit_id, int& out_x, int& out_y) const {
    return mediator_ ? mediator_->getUnitPosition(unit_id, out_x, out_y) : false;
}


void GameFacade::executeCommand(std::shared_ptr<Command> cmd) {
    if (!cmd) return;
    std::cout << "[Лог | Фасад] Выполнение команды: " << cmd->getName() << "\n";
    cmd->execute();
    if (history_) {
        history_->push(std::move(cmd));
    }
}

bool GameFacade::processInput(const std::string& input) {
    std::cout << "[Лог | Ввод пользователя] > " << input << "\n";
    if (!chain_head_) return false;
    if (!chain_head_->handle(input)) {
        std::cout << "[Ошибка] Неизвестная команда: '" << input << "'. Введите 'help' для справки.\n";
        return false;
    }
    return true;
}

bool GameFacade::undo() {
    return history_ ? history_->undo() : false;
}

void GameFacade::createUnit(const std::string& unit_type, int x, int y) {
    executeCommand(std::make_shared<CreateUnitCommand>(mediator_, unit_type, x, y));
}

void GameFacade::moveUnit(int unit_id, int x, int y) {
    executeCommand(std::make_shared<MoveUnitCommand>(mediator_, unit_id, x, y));
}

void GameFacade::attack(int attacker_id, int target_id) {
    executeCommand(std::make_shared<AttackCommand>(mediator_, attacker_id, target_id));
}

void GameFacade::collectResource(int unit_id, int resource_point_id) {
    executeCommand(std::make_shared<CollectResourceCommand>(mediator_, unit_id, resource_point_id));
}

void GameFacade::heal(int unit_id, int target_id) {
    executeCommand(std::make_shared<HealCommand>(mediator_, unit_id, target_id));
}

void GameFacade::buildBuilding(const std::string& building_type, int x, int y) {
    executeCommand(std::make_shared<BuildBuildingCommand>(mediator_, building_type, x, y));
}

void GameFacade::research(const std::string& tech_id) {
    executeCommand(std::make_shared<ResearchCommand>(mediator_, tech_id));
}

void GameFacade::distributeResources(const std::string& from, const std::string& to, int amount) {
    executeCommand(std::make_shared<DistributeResourcesCommand>(mediator_, from, to, amount));
}

void GameFacade::produceUnit(int building_id, const std::string& unit_type) {
    executeCommand(std::make_shared<ProduceUnitCommand>(mediator_, building_id, unit_type));
}

void GameFacade::getStats(const std::string& filter) const {
    if (mediator_) mediator_->getStats(filter);
}

void GameFacade::inspectUnit(int unit_id) const {
    if (mediator_) mediator_->inspect_unit(unit_id);
}

void GameFacade::nextTurn() {
    if (mediator_) mediator_->end_turn();
}

void GameFacade::renderField() const {
    if (mediator_) mediator_->render_field();
}

bool GameFacade::getUnitState(int unit_id, int& out_x, int& out_y, int& out_moves_left) const {
    return mediator_ ? mediator_->getUnitState(unit_id, out_x, out_y, out_moves_left) : false;
}

