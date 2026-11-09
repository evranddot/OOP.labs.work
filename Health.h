#include<cassert>
#include <algorithm>
class Health {

private:
	int current_health;

public:
	Health(int start_health) {
		assert(start_health >= 0 && "Критическая ошибка: здоровье не может быть отрицательной!");
		current_health = (std::max)(0, start_health);
	}

	int get_health() const {
		return current_health;
	}

	void damage(int damage_amount) {
		if (damage_amount > 0) {
			if (current_health - damage_amount < 0) {
				current_health = 0;
			}
			else {
				current_health = current_health - damage_amount;
			}
		}
	}

	void heal(int heal_amount) {
		if (heal_amount > 0) {
			current_health += heal_amount;
		}
	}

	bool is_dead() const {
		return current_health == 0;
	}
}; 
