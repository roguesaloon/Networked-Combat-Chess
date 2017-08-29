#pragma once

#include "Fighter.h"
#include <SFML\Network.hpp>

class Battlefield
{
public:
	Battlefield(const sf::Font &font);
	void draw(sf::RenderWindow &window);
	void setPlayer(const sf::Color &color, Piece* piece, const bool &attacker);
	void reset();
	bool update(const sf::Color &color, const sf::Int64 &deltaTime, const sf::Int64 &elaspedTime, sf::TcpSocket* socket, const sf::Window &window);

private:
	sf::Font* m_font;
	sf::Text m_titleText;
	sf::Texture m_backdrop_texture;

	sf::Sprite m_backdrop_sprite;
	sf::Int64 m_lastRecieveTime;

	Fighter* m_whitePlayer;
	Fighter* m_blackPlayer;

	sf::Packet m_packet;

	static sf::Color m_color;

	void drawPlayer(Fighter* player, sf::RenderWindow &window);

};

