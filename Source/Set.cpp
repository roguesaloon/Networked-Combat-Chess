#include "Set.h"


Set::Set(const sf::Color &_color, const std::vector<sf::RectangleShape> &_board)
	: m_color(_color), m_board(&_board)
{
	// Constructs Set Of Pieces

	// If Not Balck or White, Don't Continue
	if (m_color != sf::Color::Black && m_color != sf::Color::White)
		return;

	m_font.loadFromFile("Assets/Symbola.ttf");

	// Creates Arrays For each type of piece
	std::vector<Piece> rooks(2);
	std::vector<Piece> knights(2);
	std::vector<Piece> bishops(2);
	std::vector<Piece> queen(1); // Arrays of 1 seem stupid, 
	std::vector<Piece> king(1);  // but fundamental to program design
	std::vector<Piece> pawns(8);

	// Puts Those Arrays in 2D Array
	// Order Is Important Here
	// So That Unicode Characters Match Up
	m_pieces.push_back(king);
	m_pieces.push_back(queen);
	m_pieces.push_back(rooks);
	m_pieces.push_back(bishops);
	m_pieces.push_back(knights);
	m_pieces.push_back(pawns);
	
	// Start at King
	wchar_t i = L'♔';

	// Loop Through Collection
	for (auto &collection : m_pieces)
	{
		for (auto &piece : collection)
		{
			// Initialize Each Piece In Array
			piece = Piece(i, m_color, m_font);
		}
		++i; // Increase Counter, To Next Piece
	}


	initPositions();

}

// Sets Starting Position For Each Piece
void Set::initPositions()
{
	sf::Int8 row;

	if (m_color == sf::Color::White)
		row = 1;

	if (m_color == sf::Color::Black)
		row = 8;

	m_pieces[R][0].setGridPosition('A', row, m_board);
	m_pieces[N][0].setGridPosition('B', row, m_board);
	m_pieces[B][0].setGridPosition('C', row, m_board);
	m_pieces[K][0].setGridPosition('D', row, m_board);
	m_pieces[Q][0].setGridPosition('E', row, m_board);
	m_pieces[B][1].setGridPosition('F', row, m_board);
	m_pieces[N][1].setGridPosition('G', row, m_board);
	m_pieces[R][1].setGridPosition('H', row, m_board);

	if (m_color == sf::Color::White)
		row = 2;

	if (m_color == sf::Color::Black)
		row = 7;

	for (int column = 'A', i = 0; column < 'I'; ++column, ++i)
		m_pieces[P][i].setGridPosition(column, row, m_board);
}

std::vector<std::vector<Piece>> &Set::getPieces()
{
	return m_pieces;
}

void Set::draw(sf::RenderWindow &_window)
{
	for (auto &collection : m_pieces)
	{
		for (auto &piece : collection)
		{
			if (!piece.isTaken())
			_window.draw(*((sf::Text*)&piece));
		}
			
	}
}