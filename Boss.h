/*
Boss.h
Author: John Templonuevo
Date: 06/18/2026

Description:
Defines the Boss enemy tank in the Tank Battle game.

The Boss class extends the Enemy class and introduces
stronger enemy behaviour for a dedicated boss encounter.

Boss Features:
	- Larger tank size
	- Increased health
	- Custom movement behaviour
	- Triple-shot attack pattern
	- Stronger combat stats

Health System:
	Unlike standard enemies, the boss is not destroyed by a
	single projectile hit.

	The boss starts with 20 HP and takes damage when hit.

	The boss is defeated only when its health reaches zero.

Attack Pattern:
	The boss fires three bullets at once in cone pattern

	All bullets use the existing Bullet class and participate
	in the normal collision system.

Inheritance:

	Tank
	  ↓
	Enemy
	  ↓
	Boss

*/

#ifndef BOSS_H
#define BOSS_H

#include "Enemy.h"
#include "Bullet.h"

#include <vector>

class Boss : public Enemy
{
private:
	int hp;
	int maxHp;

public:
	Boss();
	void Update(float deltaTime) override;
	void TakeDamage(int damage);
	int GetHP() const;
	int GetMaxHP() const;
	bool isDefeated() const;

	void FireTripleShot(std::vector<Bullet>& bullets);

	~Boss();
};

#endif
