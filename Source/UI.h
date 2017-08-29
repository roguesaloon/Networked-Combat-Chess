#pragma once
#include <SFML\Graphics.hpp>
#include "Board.h"
#include "Battlefield.h"

class UI : private sf::RectangleShape
{
public:
	UI(const Board &board);
	void draw(sf::RenderWindow &window, const sf::Vector2i &mousePos);
	void takePiece(Piece &pieceToTake, Piece &pieceTaking, std::string &mode) const;
	void updateBattlefield(std::string &mode, const sf::Int64 &deltaTime, const sf::Int64 &elaspedTime, sf::TcpSocket* socket, const sf::Window &window);
private:
	sf::Font m_font;
	sf::Text m_squareHighlighted;
	sf::Text m_selectedPiece;
	Battlefield* m_battlefield;
	Board* m_board;


	void setText(const sf::Vector2i &_mousePos);
};

