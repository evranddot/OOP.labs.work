#include "Handler.h"
#include "GameFacade.h"
#include <sstream>
#include <iostream>
#include <algorithm>

namespace {
    std::string normalize_coords(std::string s) {
        std::replace(s.begin(), s.end(), ',', ' ');
        return s;
    }
}

// Сценарий 1: create unit <type> at <x>,<y>  ИЛИ  create <type> at <x>,<y>
bool CreateHandler::handle(const std::string& cmd) {
    std::istringstream iss(normalize_coords(cmd));
    std::string action;
    iss >> action;
    if (action != "create") return passToNext(cmd);

    std::string token, unit_type, kw_at;
    int x = 0, y = 0;
    if (!(iss >> token)) {
        std::cout << "[Синтаксис] Используйте: create unit <infantry|scout|tank|worker|medic> at <x>,<y>\n";
        return true;
    }

    if (token == "unit") {
        if (!(iss >> unit_type >> kw_at >> x >> y) || kw_at != "at") {
            std::cout << "[Синтаксис] Используйте: create unit <infantry|scout|tank|worker|medic> at <x>,<y>\n";
            return true;
        }
    }
    else {
        unit_type = token;
        if (!(iss >> kw_at >> x >> y) || kw_at != "at") {
            std::cout << "[Синтаксис] Используйте: create unit <infantry|scout|tank|worker|medic> at <x>,<y>\n";
            return true;
        }
    }

    if (facade_) facade_->createUnit(unit_type, x, y);
    return true;
}

// Сценарий 2: move unit <id> to <x>,<y>  ИЛИ  move <id> to <x>,<y>
bool MoveHandler::handle(const std::string& cmd) {
    std::istringstream iss(normalize_coords(cmd));
    std::string action;
    iss >> action;
    if (action != "move") return passToNext(cmd);

    std::string token, kw_to;
    int unit_id = 0, x = 0, y = 0;
    if (!(iss >> token)) {
        std::cout << "[Синтаксис] Используйте: move unit <unitId> to <x>,<y>\n";
        return true;
    }

    if (token == "unit") {
        if (!(iss >> unit_id >> kw_to >> x >> y) || kw_to != "to") {
            std::cout << "[Синтаксис] Используйте: move unit <unitId> to <x>,<y>\n";
            return true;
        }
    }
    else {
        try { unit_id = std::stoi(token); }
        catch (...) { std::cout << "[Синтаксис] Некорректный ID юнита.\n"; return true; }
        if (!(iss >> kw_to >> x >> y) || kw_to != "to") {
            std::cout << "[Синтаксис] Используйте: move unit <unitId> to <x>,<y>\n";
            return true;
        }
    }

    if (facade_) facade_->moveUnit(unit_id, x, y);
    return true;
}

// Сценарий 2: attack unit <id> target <targetId>  ИЛИ  attack <id> <targetId>
bool AttackHandler::handle(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string action;
    iss >> action;
    if (action != "attack") return passToNext(cmd);

    std::string token;
    int attacker_id = 0, target_id = 0;
    if (!(iss >> token)) {
        std::cout << "[Синтаксис] Используйте: attack unit <unitId> target <targetId>\n";
        return true;
    }

    if (token == "unit") {
        std::string kw_target;
        if (!(iss >> attacker_id >> kw_target >> target_id) || kw_target != "target") {
            std::cout << "[Синтаксис] Используйте: attack unit <unitId> target <targetId>\n";
            return true;
        }
    }
    else {
        try { attacker_id = std::stoi(token); }
        catch (...) { std::cout << "[Синтаксис] Некорректный ID атакующего.\n"; return true; }
        if (!(iss >> target_id)) {
            std::cout << "[Синтаксис] Используйте: attack unit <unitId> target <targetId>\n";
            return true;
        }
    }

    if (facade_) facade_->attack(attacker_id, target_id);
    return true;
}

// Сценарий 4: collect unit <id> from <pointId>  ИЛИ  collect <id> from <pointId>
bool CollectHandler::handle(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string action;
    iss >> action;
    if (action != "collect") return passToNext(cmd);

    std::string token, kw_from;
    int unit_id = 0, point_id = 0;
    if (!(iss >> token)) {
        std::cout << "[Синтаксис] Используйте: collect unit <unitId> from <pointId>\n";
        return true;
    }

    if (token == "unit") {
        if (!(iss >> unit_id >> kw_from >> point_id) || kw_from != "from") {
            std::cout << "[Синтаксис] Используйте: collect unit <unitId> from <pointId>\n";
            return true;
        }
    }
    else {
        try { unit_id = std::stoi(token); }
        catch (...) { std::cout << "[Синтаксис] Некорректный ID юнита.\n"; return true; }
        if (!(iss >> kw_from >> point_id) || kw_from != "from") {
            std::cout << "[Синтаксис] Используйте: collect unit <unitId> from <pointId>\n";
            return true;
        }
    }

    if (facade_) facade_->collectResource(unit_id, point_id);
    return true;
}

// heal unit <id> target <targetId>  ИЛИ  heal <id> <targetId>
bool HealHandler::handle(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string action;
    iss >> action;
    if (action != "heal") return passToNext(cmd);

    std::string token;
    int unit_id = 0, target_id = 0;
    if (!(iss >> token)) {
        std::cout << "[Синтаксис] Используйте: heal <unitId> <targetId>\n";
        return true;
    }

    if (token == "unit") {
        std::string kw_target;
        if (!(iss >> unit_id >> kw_target >> target_id) || kw_target != "target") {
            std::cout << "[Синтаксис] Используйте: heal unit <unitId> target <targetId>\n";
            return true;
        }
    }
    else {
        try { unit_id = std::stoi(token); }
        catch (...) { std::cout << "[Синтаксис] Некорректный ID лекаря.\n"; return true; }
        if (!(iss >> target_id)) {
            std::cout << "[Синтаксис] Используйте: heal <unitId> <targetId>\n";
            return true;
        }
    }

    if (facade_) facade_->heal(unit_id, target_id);
    return true;
}

// Сценарий 3: build <building> at <x>,<y>
bool BuildHandler::handle(const std::string& cmd) {
    std::istringstream iss(normalize_coords(cmd));
    std::string action;
    iss >> action;
    if (action != "build") return passToNext(cmd);

    std::string building_type, kw_at;
    int x = 0, y = 0;
    if (!(iss >> building_type >> kw_at >> x >> y) || kw_at != "at") {
        std::cout << "[Синтаксис] Используйте: build <barracks|powerplant|laboratory|warehouse|hospital> at <x>,<y>\n";
        return true;
    }

    if (facade_) facade_->buildBuilding(building_type, x, y);
    return true;
}

// Сценарий 3: research <tech>
bool ResearchHandler::handle(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string action;
    iss >> action;
    if (action != "research") return passToNext(cmd);

    std::string tech;
    if (!(iss >> tech)) {
        std::cout << "[Синтаксис] Используйте: research <attack_upgrade|weapon|armor|heavy>\n";
        return true;
    }

    if (facade_) facade_->research(tech);
    return true;
}

// Сценарий 4: distribute <resource> to <target> <amount>
bool DistributeHandler::handle(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string action;
    iss >> action;
    if (action != "distribute") return passToNext(cmd);

    std::string from, kw_to, to;
    int amount = 0;
    if (!(iss >> from >> kw_to >> to >> amount) || kw_to != "to") {
        std::cout << "[Синтаксис] Используйте: distribute <wood|metal|energy> to <сектор> <amount>\n";
        return true;
    }

    if (facade_) facade_->distributeResources(from, to, amount);
    return true;
}

// Сценарий 3: produce <unit> in <buildingId>
bool ProduceHandler::handle(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string action;
    iss >> action;
    if (action != "produce") return passToNext(cmd);

    std::string unit_type, kw_in;
    int building_id = 0;
    if (!(iss >> unit_type >> kw_in >> building_id) || kw_in != "in") {
        std::cout << "[Синтаксис] Используйте: produce <infantry|scout|tank|worker|medic> in <buildingId>\n";
        return true;
    }

    if (facade_) facade_->produceUnit(building_id, unit_type);
    return true;
}

// stats | stats units | stats buildings | stats resources
bool StatsHandler::handle(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string action;
    iss >> action;
    if (action != "stats") return passToNext(cmd);

    std::string sub;
    iss >> sub;
    if (!sub.empty() && sub != "units" && sub != "buildings" && sub != "resources") {
        std::cout << "[Синтаксис] Используйте: stats [units|buildings|resources]\n";
        return true;
    }

    if (facade_) facade_->getStats(sub);
    return true;
}
