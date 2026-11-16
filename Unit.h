#pragma once
#include <string>
#include <memory>
#include "Health.h"
#include "Armor.h"
#include "Attack.h"
#include "Terrain.h"

class Unit;

class IUnitObserver {
public:
	virtual ~IUnitObserver() = default;
	virtual void on_unit_destroyed(Unit* dead_unit) = 0;
};

class Unit {
protected:
	Health health;
	Armor armor;
	Attack attack;
	int x_coord;
	int y_coord;
	char map_symbol;

	IUnitObserver* observer = nullptr;
	bool death_notified = false;

public:
	Unit(Health h, Armor a, Attack att, int start_x, int start_y, char symbol)
		: health(h), armor(a), attack(att), x_coord(start_x), y_coord(start_y), map_symbol(symbol) {
	}

	virtual ~Unit() = default;

	virtual std::unique_ptr<Unit> clone() const = 0;
	virtual std::string get_type() const = 0;


	virtual Role get_role() const noexcept = 0;

	int get_x() const noexcept { return x_coord; }
	int get_y() const noexcept { return y_coord; }
	char get_symbol() const noexcept { return map_symbol; }

	const Health& get_health_obj() const noexcept { return health; }
	const Armor& get_armor_obj() const noexcept { return armor; }
	const Attack& get_attack_obj() const noexcept { return attack; }

	void set_coordinates(int new_x, int new_y) noexcept {
		x_coord = new_x;
		y_coord = new_y;
	}
	void set_observer(IUnitObserver* obs) noexcept {
		observer = obs;
	}

	void take_damage(int damage_amount) {
		int reduced_damage = armor.reduce(damage_amount);
		health.damage(reduced_damage);
		if (health.is_dead() && !death_notified) {
			death_notified = true;
			if (observer) {
				observer->on_unit_destroyed(this);
			}
		}
	}

	bool can_attack(const Unit& target) const noexcept {
		if (this == &target) return false;
		if (health.is_dead() || target.get_health_obj().is_dead()) return false;
		return attack.is_in_range(x_coord, y_coord, target.get_x(), target.get_y());
	}

	void change_armor(int delta) {
		if (delta > 0) armor.add_armor(delta);
		else if (delta < 0) armor.remove_armor(-delta);
	}

	bool attack_unit(Unit& target, int target_terrain_bonus = 0) {
		if (!can_attack(target)) {
			std::cout << "[Атака невозможна] Цель вне радиуса (" << attack.get_attack_range() << ") или недоступна.\n";
			return false;
		}
		target.take_damage(attack.get_damage());
		return true;
	}
	void heal_unit(int amount) {
		health.heal(amount);
	}

	void boost_damage(int bonus) {
		attack.add_damage(bonus);
	}

};
