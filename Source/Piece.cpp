#include "Piece.h"
#include "Board.h"
#include <thread>

Piece::Piece(wchar_t _piece, const sf::Color &_color, const sf::Font &_font)
{
	m_piece = _piece;

	if (_color == sf::Color::Black)
		_piece += 6;

	this->setString(_piece);
	this->setFont(_font);
	this->setFillColor(_color);

	m_health = 100;

	// Sets Appropiate Helath Depending on Piece
	switch (m_piece)
	{
	case L'♙':
		m_health += 10; break;
	case L'♗':
	case L'♘':
		m_health += 30; break;
	case L'♖':
		m_health += 50; break;
	case L'♕':
		m_health += 90; break;
	case L'♔':
		m_health += 100; break;
	}

	m_taken = false;

}

void Piece::setGridPosition(const char _column, const int _row, const std::vector<sf::RectangleShape>* _board)
{
	if (m_taken)
	{
		m_gridPosition = sf::Vector2i(0, 0);
		return;
	}

	m_gridPosition = sf::Vector2i(_column, _row);

	//Piece Pos = Square Pos + ((Size Difference Between Square And Piece)/2)

	m_square = ((Board*)_board)->getSquareOnGrid(_column, _row);

	auto &sizeDifference = m_square.getSize() - getSize();

	m_screenPosition = sf::Vector2i(m_square.getPosition() + sf::Vector2f(sizeDifference.x / 2.00f, sizeDifference.y / 2.00f));

	this->setPosition((float)m_screenPosition.x, (float)m_screenPosition.y);
}

sf::Vector2i Piece::getGridPosition() const
{
	return m_gridPosition;
}

// Gets where a piece can move to
// Based on the rules of chess
std::vector<sf::Vector2i> Piece::getAvailableMoves(const std::vector<std::vector<Piece>> &_playerSet, const std::vector<std::vector<Piece>> &_otherSet) const
{
	std::vector<sf::Vector2i> moves;

	if (m_taken)
	{
		moves.push_back(sf::Vector2i(0, 0));
		return moves;
	}

	sf::Vector2i square;
		

	// Possible Moves For Each Piece

	switch (m_piece)
	{
	case L'♙':	// Pawn
	{
				sf::Uint8 startRow;
				sf::Int8 forward;

				// Adjust's Starting Position, and Forward Direction Based On Color

				if (this->getFillColor() == sf::Color::Black)
				{
					startRow = 7;
					forward = -1;
				}

				if (this->getFillColor() == sf::Color::White)
				{
					startRow = 2;
					forward = 1;
				}
					

				 // One Square In Front
				 square = sf::Vector2i(m_gridPosition.x, m_gridPosition.y + forward);

				 // Can Move If One Square In Front Has No Piece On it
				 if (!isBlocked(square, _playerSet) && !isBlocked(square, _otherSet))
				 {
					moves.push_back(square);

					// Two Squares In Front
					square = sf::Vector2i(m_gridPosition.x, m_gridPosition.y + 2 * forward);

					 // Can Move If Pawn Has Not Moved, and Square Has No Piece On it
					 if (m_gridPosition.y == startRow)
					 {
						 if (!isBlocked(square, _playerSet) && !isBlocked(square, _otherSet))
							moves.push_back(square);
					 }
				 }

				 // Diagonal Right Forward
				 square = sf::Vector2i(m_gridPosition.x+1, m_gridPosition.y + forward);

				 // Can Move (To Take) If It Has An Enemy Piece On It
				 if (isBlocked(square, _otherSet))
					 moves.push_back(square);

				 // Diagonal Left Forward
				 square = sf::Vector2i(m_gridPosition.x - 1, m_gridPosition.y + forward);

				 // Can Move (To Take) If Has Enemy Piece On It
				 if (isBlocked(square, _otherSet))
				 {
					 moves.push_back(square);
				 }

				 // Promotion (Pawn Upgrade) Disregarded Due to High Complexity

				 break;
	}
	case L'♗': // Bishop
	{
				 // Loops for each Direction Bishop Can Move In
				 for (int l = 0, j = 0, k = 0; l < 4; ++l)
				 {
					 // Determines Direction (All Diagonals)
					 switch (l)
					 {
					 case 0:
						 j = +1; k = +1; break;
					 case 1:
						 j = +1; k = -1; break;
					 case 2:
						 j = -1; k = +1; break;
					 case 3:
						 j = -1; k = -1; break;
					 }

					 // Loops Through Each (Possible) Square Bishop Can Move To In Direction
					 for (int i = 1; i < 8; ++i)
					 {
						 square = sf::Vector2i(m_gridPosition.x + (i*j), m_gridPosition.y + (i*k));
						 if (isBlocked(square, _playerSet))
							 break; // Cannot Move There If Occupied By Friendly Piece, Also Blocks Path for Moving Further In Direction
						 if (!isBlocked(square, _playerSet))
							 moves.push_back(square); // Otherwise Can Move There
						 if (isBlocked(square, _otherSet))
						 {
							 break; // If Occupied By Enemy Piece, Cannot Move Further In Direction (Take Piece)
						 }
					 }
				 }

				 break;
	}
	case L'♘': // Knight
	{
				 // Loops For Each Square Knight Can Move To
				 for (int i = 0, j = 0, k = 0; i < 8; ++i)
				 {
					 // Determines Position Of (Possible) Squares To Move To (L Shapes)
					 switch (i)
					 {
					 case 0:
						 j = -1; k = +2; break;
					 case 1:
						 j = +1; k = +2; break;
					 case 2:
						 j = +2; k = +1; break;
					 case 3:
						 j = +2; k = -1; break;
					 case 4:
						 j = +1; k = -2; break;
					 case 5:
						 j = -1; k = -2; break;
					 case 6:
						 j = -2; k = -1; break;
					 case 7:
						 j = -2; k = +1; break;
					 }

					 square = sf::Vector2i(m_gridPosition.x + j, m_gridPosition.y + k);
					 if (!isBlocked(square, _playerSet))
					 {
						 moves.push_back(square); // Can Move To Square If Not Occupied By Friendly, (Will Take If Occupied By Enemy)
					 }
				 }

				 break;
	}
	case L'♖': // Rook
	{
				 // Similar To Bishop, Loops Through (Possible) Directions
				 for (int l = 0, j = 0, k = 0; l < 4; ++l)
				 {
					 // All Straights
					 switch (l)
					 {
					 case 0:
						 j = 1; k = 0; break;
					 case 1:
						 j = -1; k = 0; break;
					 case 2:
						 j = 0; k = 1; break;
					 case 3:
						 j = 0; k = -1; break;
					 }

					 // Loops Through Every Square In Direction
					 // Can Move There When Not Occupied By Friendly
					 // Taking Piece Blocks Further Moevment In Direction
					 for (int i = 1; i < 8; ++i)
					 {
						 square = sf::Vector2i(m_gridPosition.x + (i*j), m_gridPosition.y + (i*k));
						 if (isBlocked(square, _playerSet))
							 break;
						 if (!isBlocked(square, _playerSet))
							 moves.push_back(square);
						 if (isBlocked(square, _otherSet))
						 {
							 break;
						 }
					 }
				 }

				 break;
	}
	case L'♕': //Queen
	{
				 // Follows (Mostly) Combined Rules For Bishop And Rook
				 // So Gets All Directions (Straight And Diagonal)
				 for (int l = 0, j = 0, k = 0; l < 8; ++l)
				 {
					 switch (l)
					 {
					 case 0:
						 j = +1; k = +1; break;
					 case 1:
						 j = +1; k = -1; break;
					 case 2:
						 j = -1; k = +1; break;
					 case 3:
						 j = -1; k = -1; break;
					 case 4:
						 j = 1; k = 0; break;
					 case 5:
						 j = -1; k = 0; break;
					 case 6:
						 j = 0; k = 1; break;
					 case 7:
						 j = 0; k = -1; break;
					 }

					 // Loops Through Poissibles
					 // Cannot Move To Friendly
					 // Taking Will Block Further Movement
					 for (int i = 1; i < 8; ++i)
					 {
						 square = sf::Vector2i(m_gridPosition.x + (i*j), m_gridPosition.y + (i*k));
						 if (isBlocked(square, _playerSet))
							 break;
						 if (!isBlocked(square, _playerSet))
							 moves.push_back(square);
						 if (isBlocked(square, _otherSet))
						 {
							 break;
						 }
							 
					 }
				 }

				 break;
	}
	case L'♔':
	{
				// Similar Rules To Knight
				// Can Move To Any Directly Adjacent Square
				// Assuming It Is Not Occupied By Friendly
				for (int i = 0, j = 0, k = 0; i < 8; ++i)
				{
					switch (i)
					{
					case 0:
						j = +1; k = -1; break;
					case 1:
						j = +1; k = 0; break;
					case 2:
						j = +1; k = +1; break;
					case 3:
						j = -1; k = -1; break;
					case 4:
						j = -1; k = 0; break;
					case 5:
						j = -1; k = +1; break;
					case 6:
						j = 0; k = -1; break;
					case 7:
						j = 0; k = +1; break;
					}

					square = sf::Vector2i(m_gridPosition.x + j, m_gridPosition.y + k);
					if (!isBlocked(square, _playerSet))
					{
						moves.push_back(square);
					}
				}

				// Castling Disregarded Due To Complexity of Moving Two Pieces (Goes against design)
				// Check Also Disregarded, as less relevent in this game
				
				break;
	}
	}

	// Returns Possible Moves A Piece Can Perfrom
	return moves;
}

sf::RectangleShape Piece::getSquare() const
{
	return m_square;
}

bool Piece::isClicked(const sf::Vector2i &_mousePos) const
{
	if (m_taken) return false;

	return (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) &&
		getGlobalBounds().contains((sf::Vector2f)_mousePos));
}

sf::Vector2f Piece::getSize() const
{
	return sf::Vector2f(this->getGlobalBounds().width, this->getGlobalBounds().height);
}

// If a Square is Blocked by a Piece in Specified Set
bool Piece::isBlocked(const sf::Vector2i &_square, const std::vector<std::vector<Piece>> &_pieces) const
{

	for (const auto &collection : _pieces)
	{
		for (const auto &piece : collection)
		{
			if (piece.m_gridPosition == _square && !piece.isTaken())
				return true;
		}
	}

	return false;
}

wchar_t Piece::getPiece() const
{
	return m_piece;
}

void Piece::adjustHealth(sf::Int16 _value)
{
	m_health += _value;
}

sf::Int16 Piece::getHealth() const
{
	return m_health;
}

bool Piece::isTaken() const
{
	return m_taken;
}

// Takes A Piece
// Very Simple, But Concept Is Bigger
// Piece Is Inactive After taken flag is true
// This is checked for in multiple places
// Soft Take was chosen (as opposed to deletion)
// This helps maintain structure
void Piece::take()
{
	m_taken = true;
}
