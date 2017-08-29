#pragma once
#include "Piece.h"

class Set
{

public:
	Set(const sf::Color &color, const std::vector<sf::RectangleShape> &board);
	std::vector<std::vector<Piece>> &getPieces();
	void draw(sf::RenderWindow &window);
	
private:
	enum Pieces // Used To Simply Refer To Piece In Array
	{
		Q, K, R, B, N, P
	};

	std::vector<std::vector<Piece>> m_pieces;
	const sf::Color m_color;
	const std::vector<sf::RectangleShape>* m_board;
	sf::Font m_font;
	void initPositions();
};