#pragma once

#include <SFML\Graphics.hpp>

class Piece : private sf::Text
{
public:
	Piece() { };
	Piece(wchar_t piece, const sf::Color &color, const sf::Font &font);
	void setGridPosition(const char column, const int row, const std::vector<sf::RectangleShape>* board);
	sf::Vector2i getGridPosition() const;
	std::vector<sf::Vector2i> getAvailableMoves(const std::vector<std::vector<Piece>> &playerSet, const std::vector<std::vector<Piece>> &otherSet) const;
	sf::RectangleShape getSquare() const;
	bool isClicked(const sf::Vector2i &mousePos) const;
	wchar_t getPiece() const;
	void adjustHealth(sf::Int16 value);
	sf::Int16 getHealth() const;

	bool isTaken() const;
	void take();

	using sf::Text::getFillColor;

private:
	sf::Vector2i m_gridPosition;
	sf::Vector2i m_screenPosition;
	sf::RectangleShape m_square;
	sf::Int16 m_health;
	wchar_t m_piece;
	bool m_taken;

	sf::Vector2f getSize() const;
	bool isBlocked(const sf::Vector2i &square, const std::vector<std::vector<Piece>> &pieces) const;

	
};




