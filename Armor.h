#pragma once
#include <cassert>
#include <algorithm>


class Armor {

private:
	int armor_value;

public:
	Armor(int start_armor) {
		assert(start_armor >= 0 && "Критическая ошибка: броня не может быть отрицательной!");
		armor_value = (std::max)(0, start_armor);
	}

	int get_armor() const {
		return armor_value;
	}

	int reduce(int damage_amount) const {
		if (damage_amount <= 0) {
			return 0;
		}
		int effective_damage = damage_amount - armor_value;

		if (effective_damage < 0) {
			return 0;
		}
		else {
			return effective_damage;
		}
	}

	void add_armor(int bonus) {
		if (bonus > 0) armor_value += bonus;
	}

	void remove_armor(int bonus) {
		if (bonus > 0) armor_value = (std::max)(0, armor_value - bonus);
	}
};