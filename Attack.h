#pragma once
#include "Armor.h"
#include <cmath>
#include <algorithm>

class Attack {

private:
	int damage_value;
	int attack_range;

public:

	Attack(int start_damage, int start_range = 1) {
		if (start_damage < 0) {
			damage_value = 0;
		}
		else {
			damage_value = start_damage;
		}

		if (start_range < 1) {
			attack_range = 1;
		}
		else {
			attack_range = start_range;
		}
	}

	void add_damage(int bonus) {
		if (bonus > 0) damage_value += bonus;
	}

	int get_damage() const {
		return damage_value;
	}

	int calculate_damage(const Armor& target_armor) const {
		return target_armor.reduce(damage_value);
	}

	int get_attack_range() const {
		return attack_range;
	}

	bool is_in_range(int x1, int y1, int x2, int y2) const {
		int dist = (std::max)(std::abs(x1 - x2), std::abs(y1 - y2));
		return dist <= attack_range;
	}
};
