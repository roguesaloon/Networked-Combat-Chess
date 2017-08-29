#pragma once

#include <SFML\Graphics.hpp>
#include "Piece.h"

class Fighter : public sf::Sprite
{
public:
	Fighter(Piece* piece, const sf::Color &color, const bool attacker = false);

	void faceRight();
	void faceLeft();

	void attack();

	void updateStatus(Fighter* _enemy, const sf::Int64 &elaspedTime);
	

	void draw(sf::RenderWindow &window);
	

	void getHealth(sf::Int16 &extra, sf::Int16 &base, sf::Int16 &critical) const;
	void setHealth(sf::Int16 health[3]);

	bool isFacingLeft() const;
	bool isAttacking() const;
	bool isDead() const;
	void finish();

private:
	sf::Texture m_texture;

	bool m_facingLeft;
	bool m_attacker;
	std::string m_attackingState;
	sf::Int64 m_attackCooldown;
	Piece* m_piece;
	sf::FloatRect m_hitBox;
	sf::Int16 m_healthBase, m_healthExtra, m_healthCritical;
	
	void drawHitBox(sf::RenderWindow &window);
	void updateCollisions(Fighter* enemy, const sf::Int64 &elaspedTime);

	struct HealthBar : private sf::RectangleShape
	{
		sf::RectangleShape m_extra, m_base, m_critical;

		HealthBar();

		using sf::RectangleShape::setPosition;

		void setExtra(int extra);
		void setBase(int base);
		void setCritical(int critical);
		void draw(sf::RenderWindow &window);
	} m_healthBar;
};



