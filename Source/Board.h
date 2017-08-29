#pragma once
#include "Piece.h"

class Board : private std::vector<sf::RectangleShape> // Thin Layer Around (Vector) Collection, 
													  // extended to act as chess board
{
public:
	Board();
	sf::RectangleShape getSquareOnGrid(const char column, const int row) const;
	sf::Vector2i getHighlightedSquare(const sf::Vector2i &mousePos) const;
	bool isSquareClicked(const sf::Vector2i &square, const sf::Vector2i &mousePos) const;
	bool isSelectedPiece(const Piece &otherPiece) const;
	Piece* getSelecetdPiece();
	void setSelectedPiece(const Piece &selectedPiece);
	void unSelectPiece();
	void draw(sf::RenderWindow &window);
private:
	bool isSquareHighlighted(const sf::Vector2i &square, const sf::Vector2i &mousePos) const;
	bool isSquareHighlighted(const sf::RectangleShape &square, const sf::Vector2i &mousePos) const;
	std::vector<sf::Int8> getRowsInColumn(const char column) const;
	sf::Vector2i Board::getGridPositionFromIndex(const int index) const;
	Piece* m_selected;
};