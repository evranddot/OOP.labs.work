#pragma once

#include "Unit.h"


class BountyUnit : public Unit {
public:
	using Unit::Unit;
	std::string get_type() const override { return "Bounty"; }
};

class Hunter : public BountyUnit {
public:
	Hunter(int x, int y) : BountyUnit(Health(150), Armor(20), Attack(20, 3), x, y, 'H') {}

	std::unique_ptr<Unit> clone() const override { return std::make_unique<Hunter>(*this); }
	std::string get_type() const override { return "Hunter (Bounty)"; }
	Role get_role() const noexcept override { return Role::Standard; }

};
class Mandalorian : public BountyUnit {
public:
	Mandalorian(int x, int y) : BountyUnit(Health(200), Armor(100), Attack(45, 6), x, y, 'M') {}

	std::unique_ptr<Unit> clone() const override { return std::make_unique<Mandalorian>(*this); }
	std::string get_type() const override { return "Mandalorian (Bounty)"; }
	Role get_role() const noexcept override { return Role::Heavy; }
};
class Crawler : public BountyUnit {
public:
	Crawler(int x, int y) : BountyUnit(Health(450), Armor(80), Attack(50, 10), x, y, 'C') {}

	std::unique_ptr<Unit> clone() const override { return std::make_unique<Crawler>(*this); }
	std::string get_type() const override { return "Crawler (Bounty)"; }
	Role get_role() const noexcept override { return Role::Machine; }
};



class RebelUnit : public Unit {
public:
	using Unit::Unit;
	std::string get_type() const override { return "Rebel"; }
};

class Rebel: public RebelUnit {
public:
	Rebel(int x, int y) : RebelUnit(Health(130), Armor(25), Attack(20, 4), x, y, 'R') {}

	std::unique_ptr<Unit> clone() const override { return std::make_unique<Rebel>(*this); }
	std::string get_type() const override { return "Rebel (Rebel)"; }
	Role get_role() const noexcept override { return Role::Standard; }

};
class Wookiee : public RebelUnit {
public:
	Wookiee(int x, int y) : RebelUnit(Health(250), Armor(10), Attack(40, 7), x, y, 'W') {}

	std::unique_ptr<Unit> clone() const override { return std::make_unique<Wookiee>(*this); }
	std::string get_type() const override { return "Wookiee (Rebel)"; }
	Role get_role() const noexcept override { return Role::Heavy; }
};
class Xwyng : public RebelUnit {
public:
	Xwyng(int x, int y) : RebelUnit(Health(500), Armor(150), Attack(55, 14), x, y, 'X') {}

	std::unique_ptr<Unit> clone() const override { return std::make_unique<Xwyng>(*this); }
	std::string get_type() const override { return "X-wyng (Rebel)"; }
	Role get_role() const noexcept override { return Role::Machine; }
};


class ImperialUnit : public Unit {
public:
	using Unit::Unit;
	std::string get_type() const override { return "Imperial"; }
};

class Stormtrooper : public ImperialUnit {
public:
	Stormtrooper(int x, int y) : ImperialUnit(Health(120), Armor(40), Attack(23, 6), x, y, 'S') {}

	std::unique_ptr<Unit> clone() const override { return std::make_unique<Stormtrooper>(*this); }
	std::string get_type() const override { return "Stormtrooper (Imperial)"; }
	Role get_role() const noexcept override { return Role::Standard; }

};
class Deathtrooper : public ImperialUnit {
public:
	Deathtrooper(int x, int y) : ImperialUnit(Health(150), Armor(105), Attack(40, 7), x, y, 'D') {}

	std::unique_ptr<Unit> clone() const override { return std::make_unique<Deathtrooper>(*this); }
	std::string get_type() const override { return "Deathtrooper (Imperial)"; }
	Role get_role() const noexcept override { return Role::Heavy; }
};
class ATAT : public ImperialUnit {
public:
	ATAT(int x, int y) : ImperialUnit(Health(500), Armor(350), Attack(70, 14), x, y, 'A') {}

	std::unique_ptr<Unit> clone() const override { return std::make_unique<ATAT>(*this); }
	std::string get_type() const override { return "AT-AT (Imperial)"; }
	Role get_role() const noexcept override { return Role::Machine; }
};

