#pragma once
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>
#include "Health.h"
#include "Armor.h"
#include "Attack.h"
#include "Terrain.h"

class Unit;

struct Position {
	int x = 0;
	int y = 0;
};

enum class UnitEventType {
	Damaged,
	Moved,
	Destroyed
};

struct UnitEvent {
	UnitEventType type;
	Unit* unit;
	int x;
	int y;
	int current_health;
	int value;
};

class IUnitObserver {
public:
	virtual ~IUnitObserver() = default;
	virtual void on_unit_event(const UnitEvent& event) = 0;
};

class Unit {
private:
	static inline int next_id_counter = 1;

protected:
	int id;
	Health health;
	Armor armor;
	Attack damage;
	Position position;
	char map_symbol;

	int max_move_points = 3;
	int move_points = 3;

	std::string currentOrder = "Ожидание приказа";

	std::vector<IUnitObserver*> observers;
	bool death_notified = false;

	void notify_observers(UnitEventType type, int extra_value = 0) {
		UnitEvent event{ type, this, position.x, position.y, health.get_health(), extra_value };
		auto observers_copy = observers;
		for (auto* obs : observers_copy) {
			if (obs) {
				obs->on_unit_event(event);
			}
		}
	}

public:
	Unit(Health h, Armor a, Attack att, int start_x, int start_y, char symbol, int max_mp = 3)
		: id(next_id_counter++), health(h), armor(a), damage(att),
		position{ start_x, start_y }, map_symbol(symbol),
		max_move_points(max_mp), move_points(max_mp) {
	}

	virtual ~Unit() = default;

	Unit(const Unit& other)
		: id(other.id), health(other.health), armor(other.armor), damage(other.damage),
		position(other.position), map_symbol(other.map_symbol),
		max_move_points(other.max_move_points), move_points(other.move_points),
		currentOrder(other.currentOrder), observers(other.observers), death_notified(other.death_notified) {
	}

	virtual std::unique_ptr<Unit> clone() const = 0;
	virtual std::string get_type() const = 0;
	virtual Role get_role() const noexcept = 0;

	void attach_observer(IUnitObserver* obs) {
		if (obs && std::find(observers.begin(), observers.end(), obs) == observers.end()) {
			observers.push_back(obs);
		}
	}

	void detach_observer(IUnitObserver* obs) {
		observers.erase(std::remove(observers.begin(), observers.end(), obs), observers.end());
	}

	const std::vector<IUnitObserver*>& get_observers() const noexcept { return observers; }

	int get_id() const noexcept { return id; }
	int get_x() const noexcept { return position.x; }
	int get_y() const noexcept { return position.y; }
	Position get_position() const noexcept { return position; }
	char get_symbol() const noexcept { return map_symbol; }

	int get_move_points() const noexcept { return move_points; }
	int get_max_move_points() const noexcept { return max_move_points; }
	const std::string& get_order() const noexcept { return currentOrder; }
	void set_order(const std::string& order) { currentOrder = order; }

	bool spend_move_points(int points) noexcept {
		if (points < 0 || move_points < points) return false;
		move_points -= points;
		return true;
	}

	void reset_move_points() noexcept { move_points = max_move_points; }

	const Health& get_health_obj() const noexcept { return health; }
	const Armor& get_armor_obj() const noexcept { return armor; }
	const Attack& get_attack_obj() const noexcept { return damage; }

	// Методы по спецификации Этапа 2 (пункт 2)
	virtual void move(int new_x, int new_y) {
		position.x = new_x;
		position.y = new_y;
		notify_observers(UnitEventType::Moved);
	}

	void set_coordinates(int new_x, int new_y) {
		move(new_x, new_y);
	}

	virtual void takeDamage(int damage_amount) {
		int reduced_damage = armor.reduce(damage_amount);
		health.damage(reduced_damage);
		notify_observers(UnitEventType::Damaged, reduced_damage);

		if (health.is_dead() && !death_notified) {
			death_notified = true;
			notify_observers(UnitEventType::Destroyed);
		}
	}

	void take_damage(int damage_amount) {
		takeDamage(damage_amount);
	}

	bool can_attack(const Unit& target) const noexcept {
		if (this == &target) return false;
		if (health.is_dead() || target.get_health_obj().is_dead()) return false;
		return damage.is_in_range(position.x, position.y, target.get_x(), target.get_y());
	}

	virtual bool attack(Unit& target) {
		if (!can_attack(target)) {
			std::cout << "[Атака невозможна] Цель вне радиуса (" << damage.get_attack_range() << ") или недоступна.\n";
			return false;
		}
		target.takeDamage(damage.get_damage());
		return true;
	}

	bool attack_unit(Unit& target) {
		return attack(target);
	}

	void change_armor(int delta) {
		if (delta > 0) armor.add_armor(delta);
		else if (delta < 0) armor.remove_armor(-delta);
	}

	void heal_unit(int amount) { health.heal(amount); }
	void boost_damage(int bonus) { damage.add_damage(bonus); }

	void print_status() const {
		std::cout << "ID:" << id << " [" << map_symbol << "] " << get_type()
			<< " | Позиция: (" << position.x << ", " << position.y << ")"
			<< " | ХП: " << health.get_health()
			<< " | Броня: " << armor.get_armor()
			<< " | Урон: " << damage.get_damage() << " (дальность " << damage.get_attack_range() << ")"
			<< " | Запас хода: " << move_points << "/" << max_move_points
			<< " | Приказ: " << currentOrder << "\n";
	}
};
