#include "Fighter.h"


Fighter::Fighter(Piece* _piece, const sf::Color &_color, const bool _attacker)
{
	// Initializes Fighter

	m_texture.loadFromFile("Assets/Sprites.png");

	m_piece = _piece;

	// Sets Size Of HitBox (Where Collisions Are Detected)
	m_hitBox = sf::FloatRect(0, 0, 12, 4);

	this->setTexture(m_texture);

	sf::Vector2i size = sf::Vector2i(50, 98);
	sf::Vector2i position = sf::Vector2i(0, 0);

	// Set's Position in Sprite Sheet To load From
	if (_color == sf::Color::Black)
		position.y += 98;

	
	switch (m_piece->getPiece())
	{
	case L'♙':
		position.x = 20; break;
	case L'♖':
		position.x = 82; break;
	case L'♘':
		position.x = 155; break;
	case L'♗':
		position.x = 218; break;
	case L'♕':
		position.x = 285; break;
	case L'♔':
		position.x = 348; break;
	}

	this->setTextureRect(sf::IntRect(position, size));
	
	this->setOrigin(getLocalBounds().width * 0.5f, getLocalBounds().height * 0.5f);

	// Sets Position And Direction Based On Color
	if (_color == sf::Color::Black)
	{
		faceLeft();
		this->setPosition(1140, 540);
	}
	else
	{
		faceRight();
		this->setPosition(790, 540);
	}

	// Sets Critical Health Appropiate To Peice
	switch (m_piece->getPiece())
	{
	case L'♙':
		m_healthCritical = 10; break;
	case L'♗':
	case L'♘':
		m_healthCritical = 30; break;
	case L'♖':
		m_healthCritical = 50; break;
	case L'♕':
		m_healthCritical = 90; break;
	case L'♔':
		m_healthCritical = 100; break;
	}

	// Appropiates Health

	// System Is Slightly Confusing
	// Explanation Is

	// All Pieces Have Base Health (100)
	// All Pieces Have Critical Health (Last Piece of Health, Set Above)
	// Players Accumulate Extra Health For Successfully Taking A Piece On There Turn
	// An Attacker Always Starts A Battle With Extra Health
	// All Health Is Accumulative, And Not For Each In Fights

	m_healthBase = m_piece->getHealth() - m_healthCritical;

	if (m_healthBase > 100)
		m_healthBase = 100;

	m_healthExtra = m_piece->getHealth() - m_healthBase - m_healthCritical;

	if (m_healthExtra < 0)
		m_healthExtra = 0;

	if (_attacker)
	{
		m_piece->adjustHealth(20);
		m_healthExtra += 20;
	}
		

	m_attacker = _attacker;

	m_attackingState = "No";
}

void Fighter::faceLeft()
{
	if (!m_facingLeft)
		this->scale(-1, 1);

	m_facingLeft = true;
}

void Fighter::faceRight()
{
	if (m_facingLeft)
		this->scale(1, 1);

	m_facingLeft = false;
}

// Gets Health Into 3 Passed In Values
void Fighter::getHealth(sf::Int16 &extra, sf::Int16 &base, sf::Int16 &critical) const
{
	extra = m_healthExtra;
	base = m_healthBase;
	critical = m_healthCritical;
}

// Sets Health From 3 Passed In Values
void Fighter::setHealth(sf::Int16 _health[3])
{
	m_healthExtra = _health[0];
	m_healthBase = _health[1];
	m_healthCritical = _health[2];
}

bool Fighter::isFacingLeft() const
{
	return m_facingLeft;
}

void Fighter::attack()
{
	m_attackingState = "AttackingP1";

}

bool Fighter::isAttacking() const
{
	return  (m_attackingState.find("Attacking") != std::string::npos);
}

// Status Update
// Called Every Frame For Each Fighter
// Processes Information
void Fighter::updateStatus(Fighter* _enemy, const sf::Int64 &_elaspedTime)
{
	// Sets Hit Box Position, Based On Player
	m_hitBox.left = this->getPosition().x - 6;
	m_hitBox.top = this->getPosition().y + 22;

	// Attack Logic
	// String Used Here For Simplicity
	// Creates Smooth-Ish Kick Forward-Back Animation
	// After Attack Triggered
	// No Delta Time Here, Funky With Unlocked Framerate
	if (isAttacking())
	{
		if (m_attackingState == "AttackingP1")
		{
			if (!isFacingLeft())
			{
				this->rotate(-4);

				if (this->getRotation() < 315)
					m_attackingState = "AttackingP2";
			}
			else
			{
				this->rotate(4);

				if (this->getRotation() > 45)
					m_attackingState = "AttackingP2";
			}
		}

		if (m_attackingState == "AttackingP2")
		{
			if (this->getRotation() > 0)
			{
				if (!isFacingLeft())
					this->rotate(2);
				else
					this->rotate(-2);
			}
			else
			{
				this->setRotation(0);
				m_attackingState = "AttackingCooldown";
				m_attackCooldown = _elaspedTime;
			}
		}

		if (m_attackingState == "AttackingCooldown")
		{
			if (_elaspedTime - m_attackCooldown > 300)
				m_attackingState = "No";
		}

	}

	// Update Health

	m_healthBar.setExtra(m_healthExtra);
	m_healthBar.setBase(m_healthBase);
	m_healthBar.setCritical(m_healthCritical);

	updateCollisions(_enemy, _elaspedTime);

	// Sets Health Bar poistion
	// Attacker Is Higher Up, To Stop Them Overlapping
	int offset = -10;

	if (m_attacker)
		offset += 10;

	m_healthBar.setPosition(this->getPosition().x, this->getPosition().y - this->getLocalBounds().height*0.5f - offset);
}

void Fighter::updateCollisions(Fighter* _enemy, const sf::Int64 &_elaspedTime)
{

	static sf::Int64 cooldown = 0;

	// Detects A Collision
	// Waits For A Cooldown Timer Before Collision
	// Can Do Damage Again
	if (_enemy->m_attackingState == "AttackingP1" && _enemy->getGlobalBounds().intersects(m_hitBox) && _elaspedTime - cooldown > 50)
	{
		m_piece->adjustHealth(-4);

		if (m_healthExtra > 0)
			m_healthExtra -= 4;
		else if (m_healthBase > 0)
			m_healthBase -= 4;
		else
			m_healthExtra -= 4;

		cooldown = _elaspedTime;
	}
}

// Return true
// If No Health Is Left
bool Fighter::isDead() const
{
	if (m_piece->getHealth() < 0)
	{
		return true;
	}

	return false;
}

// Take The Piece From The Board
void Fighter::finish()
{
	m_piece->take();
}

void Fighter::draw(sf::RenderWindow &_window)
{
	m_healthBar.draw(_window);

	drawHitBox(_window);

	_window.draw(*this);
}

void Fighter::drawHitBox(sf::RenderWindow &_window)
{
	sf::RectangleShape hitBox;

	hitBox.setSize(sf::Vector2f(m_hitBox.width, m_hitBox.height));
	hitBox.setPosition(m_hitBox.left, m_hitBox.top);
	hitBox.setFillColor(sf::Color::Red);
}

Fighter::HealthBar::HealthBar()
{

	//Initializes Healtn Bar

	this->setFillColor(sf::Color::Black);
	this->setSize(sf::Vector2f(0, 9));

	m_extra.setFillColor(sf::Color::Green);
	m_extra.setSize(sf::Vector2f(0, 7));

	m_base.setFillColor(sf::Color::Yellow);
	m_base.setSize(sf::Vector2f(0, 7));

	m_critical.setFillColor(sf::Color::Red);
	m_critical.setSize(sf::Vector2f(0, 7));
}

// Function To Set Different part Of Health
// Split Into Multiple Fucntions For Convienience
void Fighter::HealthBar::setExtra(int _extra)
{
	if (_extra > 0)
		m_extra.setSize(sf::Vector2f((_extra / 4) + 0.8f, m_extra.getSize().y));
	else
		m_extra.setSize(sf::Vector2f(0, m_extra.getSize().y));
}

void Fighter::HealthBar::setBase(int _base)
{
	if (_base > 0)
		m_base.setSize(sf::Vector2f((_base / 4) + 0.8f, m_base.getSize().y));
	else
		m_base.setSize(sf::Vector2f(0, m_base.getSize().y));
}

void Fighter::HealthBar::setCritical(int _critical)
{
	if (_critical > 0)
		m_critical.setSize(sf::Vector2f((_critical / 4) + 0.8f, m_critical.getSize().y));
	else
		m_critical.setSize(sf::Vector2f(0, m_critical.getSize().y));
}

// Darws The Health Bar To Screen
// Figures Out It's Size Based On Remaining Health
void Fighter::HealthBar::draw(sf::RenderWindow &_window)
{
	this->setSize(sf::Vector2f(m_extra.getSize().x + m_base.getSize().x + m_critical.getSize().x + 2, this->getSize().y));
	this->setOrigin(this->getSize().x*0.5f, this->getOrigin().y);
	m_extra.setPosition(this->getPosition().x - this->getSize().x*0.5f + 1, this->getPosition().y + 1);
	m_base.setPosition(m_extra.getPosition().x + m_extra.getSize().x, this->getPosition().y + 1);
	m_critical.setPosition(m_base.getPosition().x + m_base.getSize().x, this->getPosition().y + 1);
	_window.draw(*this);
	_window.draw(m_extra);
	_window.draw(m_base);
	_window.draw(m_critical);
}
