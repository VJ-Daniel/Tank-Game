#include "Boss.h"

#include <cmath>
#include <glm.hpp>

// Constructor
// Initialize boss stats

Boss::Boss() : Enemy()
{
	maxHp = 20;
	hp = maxHp;

	// stronger stats than regular enemies but slower movement
	// regular values:
	//		detectionRange = 350
	//		shootingRange = 250
	//		fireCooldown = 1.2
	//		moveSpeed = 100

	detectionRange = 500.0f;
	shootingRange = 400.0f;
	fireCooldown = 1.5f;
	fireTimer = 0.0f;
	moveSpeed = 40.0f;
	rotationSpeed = 1.5f;

	// Custom behaviour init
	phase = BossPhase::Strafe;
	strafeAngle = 0.0f;
	strafeRadius = 350.0f;   // stays about shootingRange away during strafe

	// must be approximately 4x larger than player

	SetSize(128.0f, 128.0f);

}

void Boss::Update(float deltaTime)
{
	if (!alive)
		return;

	fireTimer += deltaTime;

	// --- Phase transition ---
	// Switch to aggressive when below half health
	if (hp <= maxHp / 2)
	{
		phase = BossPhase::Aggressive;
	}
	else
	{
		phase = BossPhase::Strafe;
	}

	glm::vec2 direction = DirectionToTarget();

	// Always face the player
	if (glm::dot(direction, direction) > 0.0001f)
	{
		rotation = std::atan2(direction.y, direction.x);
	}

	if (phase == BossPhase::Strafe)
	{
		// --- Phase 1: Strafe ---
		// Boss orbits around the player at shootingRange distance
		// by advancing a strafe angle each frame

		strafeAngle += 0.8f * deltaTime;   // orbit speed in radians/sec

		glm::vec2 target = targetPosition;

		// Desired position: orbit point around the player
		glm::vec2 orbitPos = {
			target.x + std::cos(strafeAngle) * strafeRadius,
			target.y + std::sin(strafeAngle) * strafeRadius
		};

		// Move toward that orbit point
		glm::vec2 toOrbit = orbitPos - position;
		float dist = glm::length(toOrbit);

		if (dist > 5.0f)
		{
			glm::vec2 moveDir = toOrbit / dist;  // normalize
			position += moveDir * moveSpeed * deltaTime;
		}
	}
	else
	{
		// --- Phase 2: Aggressive ---
		// Boss charges directly at player, faster
		if (glm::dot(direction, direction) > 0.0001f)
		{
			position += direction * (moveSpeed * 2.5f) * deltaTime;
		}

		// Tighten fire cooldown in aggressive phase
		fireCooldown = 0.8f;
	}
}

// boss takes damage instead of being destroyed instantly
void Boss::TakeDamage(int damage) {

	if (!alive) {
		return;
	}

	hp -= damage;

	if (hp <= 0) {
		hp = 0;
		Destroy();
	}
}

// get current hp
int Boss::GetHP() const {
	return hp;
}

// get maximum hp
int Boss::GetMaxHP() const {
	return maxHp;
}

// check if the boss is defeated
bool Boss::isDefeated() const {
	return hp <= 0;
}

// triple shot
// spawn three bullets at the same firing event
// forward left, forward, forward right
void Boss::FireTripleShot(std::vector<Bullet>& bullets) {

	if (!alive) {
		return;
	}

	// check cooldown
	if (!CanShoot()) {
		return;
	}

	// angle the side shots about 25 degrees or 0.45 radians
	float spreadAngle = 0.45f;

	// forward bullet
	glm::vec2 forward = {
		std::cos(rotation),
		std::sin(rotation)
	};

	// forward left bullet
	glm::vec2 forwardLeft = {
		std::cos(rotation + spreadAngle),
		std::sin(rotation + spreadAngle)
	};

	// forward right bullet
	glm::vec2 forwardRight = {
		std::cos(rotation - spreadAngle),
		std::sin(rotation - spreadAngle)
	};


	// create bullets
	Bullet middleBullet;
	middleBullet.SetPosition(position);
	middleBullet.SetDirection(forward);
	middleBullet.SetFromPlayer(false);

	Bullet leftBullet;
	leftBullet.SetPosition(position);
	leftBullet.SetDirection(forwardLeft);
	leftBullet.SetFromPlayer(false);

	Bullet rightBullet;
	rightBullet.SetPosition(position);
	rightBullet.SetDirection(forwardRight);
	rightBullet.SetFromPlayer(false);

	// add all three bullets during the same firing event
	bullets.push_back(middleBullet);
	bullets.push_back(leftBullet);
	bullets.push_back(rightBullet);

	// reset cooldown after firing
	ResetFireTimer();

}

Boss::~Boss() {}
