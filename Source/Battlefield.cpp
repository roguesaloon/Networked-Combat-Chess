#include "Battlefield.h"
#include <chrono>
#include <iostream>

Battlefield::Battlefield(const sf::Font &_font)
{
	// Initializes the Battlefield, Basically on Startup

	m_font = (sf::Font*)&_font;

	m_titleText.setPosition(815, 220);
	m_titleText.setFillColor(sf::Color::Red);
	m_titleText.setFont(*m_font);
	m_titleText.setCharacterSize(50);

	m_titleText.setString("BATTLEFIELD");

	m_backdrop_texture.loadFromFile("Assets/Backdrop.png");

	m_backdrop_sprite = sf::Sprite(m_backdrop_texture);
	m_backdrop_sprite.setPosition(730, 300);

	m_blackPlayer = NULL;
	m_whitePlayer = NULL;

	m_lastRecieveTime = 0;
}

// Sets Players To Fight
// Created Dynamically
// Called On Start of Fight
void Battlefield::setPlayer(const sf::Color &_color, Piece* _piece, const bool &_attacker)
{
	if (_color == sf::Color::Black)
		m_blackPlayer = new Fighter(_piece, _color, _attacker);
	else
		m_whitePlayer = new Fighter(_piece, _color, _attacker);
}

// Reset The Battlefield, back to original state
void Battlefield::reset()
{
	delete m_blackPlayer;
	delete m_whitePlayer;
	m_blackPlayer = NULL;
	m_whitePlayer = NULL;
	m_lastRecieveTime = 0;
}

sf::Color Battlefield::m_color;

// Called Once per Frame When Fight is In Progress
bool Battlefield::update(const sf::Color &_color, const sf::Int64 &_deltaTime, const sf::Int64 &_elaspedTime, sf::TcpSocket* _socket, const sf::Window &_window)
{
	m_color = _color;

	// Determines which enemy is which
	Fighter* player;
	Fighter* enemy;

	if (_color == sf::Color::Black)
	{
		player = m_blackPlayer;
		enemy = m_whitePlayer;
	}
	else
	{
		player = m_whitePlayer;
		enemy = m_blackPlayer;
	}
	
	// Set Speed
	const float speed = (float)_deltaTime / 4;
	float velocity = 0; 

	// Keyboard Controls, Changes Velocity For Direction
	// Attack on Space
	if (_window.hasFocus())
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			velocity = -speed;
			if(!player->isAttacking())
				player->faceLeft();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			velocity = speed;
			if (!player->isAttacking())
				player->faceRight();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !player->isAttacking())
		{
			player->attack();
		}
	}	
	
	player->move(velocity, 0);

	// Checks on Both Players
	player->updateStatus(enemy, _elaspedTime);
	enemy->updateStatus(player, _elaspedTime);	
	
	bool isEnemyDead = enemy->isDead();
	bool isPlayerDead = player->isDead();

	// Parameters for Enemy (Other Player)
	static float enemyVelocity = 0;
	sf::Vector2f enemyPosition;
	sf::Int16 enemyHealth[3];


	// Gets Current Health Of You (to Become Enemy, After Sending Packet)
	player->getHealth(enemyHealth[0],enemyHealth[1],enemyHealth[2]);

	// If Either Player is Dead, Go To Send Packet Stage Immediately
	if (isEnemyDead || isPlayerDead)
		m_lastRecieveTime = 1;

	// Set Position of Other Player
	// X Position Calculated Extremely Crudely
	// Based On Last Velocity, (could have changed since last update)
	// Not really taken into account
	// Updates are frequent, so not often noticeable
	enemyPosition.x = enemy->getPosition().x + enemyVelocity;
	enemyPosition.y = enemy->getPosition().y;

	// 3 Times A Second
	if (_elaspedTime - m_lastRecieveTime > 333)
	{
		m_lastRecieveTime = _elaspedTime;

		// Load Up Packet
		m_packet.clear();
		m_packet << player->getPosition().x << velocity << player->isFacingLeft() << player->isAttacking() << isEnemyDead << isPlayerDead << enemyHealth[0] << enemyHealth[1] << enemyHealth[2];
		// And Send It Across Network
		_socket->send(m_packet);

		// Expect To Recieve A Packet From The Other End
		if (_socket->receive(m_packet) == _socket->Done)
		{
			// Load Information In
			bool left, attacking;
			m_packet >> enemyPosition.x >> enemyVelocity >> left >> attacking >> isPlayerDead >> isEnemyDead >> enemyHealth[0] >> enemyHealth[1] >> enemyHealth[2];

			// Then updates enemy parameters, form sent parameters
			if (left)
				enemy->faceLeft();
			else
				enemy->faceRight();

			if (attacking && !enemy->isAttacking())
				enemy->attack();

			enemy->setHealth(enemyHealth);

		}
	}

	enemy->setPosition((sf::Vector2f)enemyPosition);

	// If Either Player Is Dead
	// Finish (Take) The Player
	// Then Return True, To Signal Sequence End
	if (isPlayerDead || isEnemyDead)
	{
		if (isPlayerDead)
			player->finish();

		if (isEnemyDead)
			enemy->finish();

		m_packet.clear();

		return true;

	}

	return false;

}

void Battlefield::draw(sf::RenderWindow &_window)
{
	_window.draw(m_titleText);
	_window.draw(m_backdrop_sprite);


	// On Black End Draw Black On Top
	if (m_color == sf::Color::Black)
	{	
		drawPlayer(m_whitePlayer, _window);
		drawPlayer(m_blackPlayer, _window);
	}
	else // On White End White On Top
	{
		drawPlayer(m_blackPlayer, _window);
		drawPlayer(m_whitePlayer, _window);
	}
}

void Battlefield::drawPlayer(Fighter* _player, sf::RenderWindow &_window)
{
	if (_player != NULL)
		_player->draw(_window);
}