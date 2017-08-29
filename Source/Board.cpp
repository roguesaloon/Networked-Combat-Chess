#include "Board.h"

Board::Board()
{
	//Initializes Board

	m_selected = NULL;

	// Sets Parameters For Board Squares
	const sf::Vector2f POSITION(120, 50);
	const sf::Vector2f SQUARE_SIZE(60, 60);
	const sf::Color BLACK = sf::Color(210, 140, 70, 240);
	const sf::Color WHITE = sf::Color(255, 205, 160, 240);

	// Adds Squares To Board
	for (int i = 0; i < 64; ++i)
	{
		this->push_back(sf::RectangleShape(SQUARE_SIZE));

		if (i > 0 && this->at(i - 1).getFillColor() == WHITE)
			(*this)[i].setFillColor(BLACK);
		else
			(*this)[i].setFillColor(WHITE);

		if (i > 0 && i % 8 == 0)
			(*this)[i].setFillColor(this->at(i - 1).getFillColor());

		(*this)[i].setPosition(POSITION.x + ((i % 8)*SQUARE_SIZE.x), POSITION.y + (SQUARE_SIZE.y*(i / 8)));
	}
}

// Returns The Raw Shape In Board, At Given Board Co-ordinate
sf::RectangleShape Board::getSquareOnGrid(const char _column, const int _row) const
{
	return (*this)[getRowsInColumn(_column)[_row]];
}

// Gets What Square Is Mouse Overed (as Board Co-ord Vector)
//Returns (0,0) if None
sf::Vector2i Board::getHighlightedSquare(const sf::Vector2i &_mousePos) const
{
	for (sf::Uint16 i = 0; i < this->size(); ++i)
	{
		if (isSquareHighlighted(this->at(i), _mousePos))
			return getGridPositionFromIndex(i);
	}

	return sf::Vector2i(0, 0);
}

bool Board::isSquareClicked(const sf::Vector2i &_square, const sf::Vector2i &_mousePos) const
{
	return sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && isSquareHighlighted(_square, _mousePos);
}

// Returns True, If Selected Piece Pointer Is The Given Piece
bool Board::isSelectedPiece(const Piece &_otherPiece) const
{
	return (m_selected == (Piece*)&_otherPiece);
}

Piece* Board::getSelecetdPiece()
{
	return m_selected;
}

void Board::setSelectedPiece(const Piece &_selectedPiece)
{
	m_selected = (Piece*)&_selectedPiece;
}

void Board::unSelectPiece()
{
	m_selected = NULL;
}

// Checks If A Specific Square (Board Co-ords) Is Highlighted
bool Board::isSquareHighlighted(const sf::Vector2i &_square, const sf::Vector2i &_mousePos) const
{
	if (_square.x >= 'A' && _square.x <= 'H' && _square.y >= 1 && _square.y <= 8)
		return isSquareHighlighted(getSquareOnGrid(_square.x, _square.y),_mousePos);

	return false;
}

// Called From Above
// Performs Check On Raw Shape
bool Board::isSquareHighlighted(const sf::RectangleShape &_square, const sf::Vector2i &_mousePos) const
{
	return _square.getGlobalBounds().contains((sf::Vector2f)_mousePos);
}

// Gets An Array, Representing All Row Numbers In a Given Column
// More Complex Than Obvious, As Single Dimensional Array Was Chosen
// To Represent Chess Board, To decrtease complexity elsehere
// Also Chess Board Counts Bottom Up, and obviously starts at 1 (not 0)
std::vector<sf::Int8> Board::getRowsInColumn(const char _column) const
{
	std::vector<sf::Int8> rowsInColumn;

	rowsInColumn.push_back(0);


	for (int l = 'A', r = 0; l < 'I'; ++l, ++r)
	{
		if (_column == l)
		{
			for (sf::Int8 i = this->size()-1; i > -1; i--)
			{
				if (i % 8 == r) // 8 Squares In Row
				{
					rowsInColumn.push_back(i);
				}
			}
		}
	}

	return rowsInColumn;
}

// Returns A Vector (Board Co-ord) based on a single dimensional index (0-63)
sf::Vector2i Board::getGridPositionFromIndex(const int _index) const
{
	sf::Vector2i gridPosition;

	for (int i = 0, j = 'A'; i < 8; ++i, ++j)
	{
		if (_index % 8 == i)
		{
			gridPosition.x = j;
			break;
		}
	}

	for (int i = 8, j = 8; i > 0; --i, j+=8)
	{
		if (_index < j)
		{
			gridPosition.y = i;
			break;
		}

	}

	return gridPosition;
}

void Board::draw(sf::RenderWindow &_window)
{
	for (const auto &square : *this)
		_window.draw(square);
}