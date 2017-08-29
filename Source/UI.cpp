#include "UI.h"
#include <sstream>

UI::UI(const Board &_board) :
	m_board((Board*)&_board)
{

	// Initializes UI

	this->setSize(sf::Vector2f(470, 600));
	this->setPosition(730, 0);
	this->setFillColor(sf::Color(192, 192, 192));

	m_font.loadFromFile("Assets/FreeSerif.ttf");

	m_squareHighlighted.setPosition(960, 100);
	m_squareHighlighted.setFillColor(sf::Color::Red);
	m_squareHighlighted.setFont(m_font);

	m_selectedPiece.setPosition(m_squareHighlighted.getPosition().x-50, 85);
	m_selectedPiece.setFillColor(sf::Color::Red);
	m_selectedPiece.setFont(m_font);
	m_selectedPiece.setCharacterSize(50);

	// Calls Battlefield Constructor
	m_battlefield = new Battlefield(m_font);
}

void UI::draw(sf::RenderWindow &_window, const sf::Vector2i &_mousePos)
{
	
	setText(_mousePos);	
	_window.draw(*this);
	_window.draw(m_squareHighlighted);
	_window.draw(m_selectedPiece);

	m_battlefield->draw(_window);
}

// Sets Text To Display To Screen (Square Number)
void UI::setText(const sf::Vector2i &_mousePos)
{
	if (m_board->getSelecetdPiece() == NULL)
		m_selectedPiece.setString("");
	else
		m_selectedPiece.setString(m_board->getSelecetdPiece()->getPiece());

	std::stringstream string;
	if (m_board->getHighlightedSquare(_mousePos).x == 0)
		string << 0;
	else
		string << (char)m_board->getHighlightedSquare(_mousePos).x;
	string << "," << m_board->getHighlightedSquare(_mousePos).y;
	m_squareHighlighted.setString(string.str());
}

// Calls The Battlefield Update
// Executed (Framely) When In Fight Sequence

void UI::updateBattlefield(std::string &_mode, const sf::Int64 &_deltaTime, const sf::Int64 &_elaspedTime, sf::TcpSocket* _socket, const sf::Window &_window)
{
	sf::Color color;

	if (_mode.find("black") != std::string::npos)
		color = sf::Color::Black;

	if (_mode.find("white") != std::string::npos)
		color = sf::Color::White;

	// Success Indicates Fight Is Over
	// Return False By Default
	if (m_battlefield->update(color, _deltaTime, _elaspedTime, _socket, _window))
	{
		_mode = _mode.erase(_mode.size() - 5); // Removes the fight from Mode Varaible
		m_battlefield->reset(); // Cleans Up Battlefield
	}
}

// Take A Piece
// Slightly Misleading Name
// Instigates Fight Sequence
void UI::takePiece(Piece &_pieceToTake, Piece &_pieceTaking, std::string &_mode) const
{
	// Checks For Taken, Important
	// Taken Pieces Can't Fight
	if (!_pieceToTake.isTaken() || !_pieceTaking.isTaken())
	{
		_mode += "fight"; // Adds Fight To Mode String

		// Sets Each Player, In Battlefield
		m_battlefield->setPlayer(_pieceToTake.getFillColor(), (Piece*)&_pieceToTake, false);
		m_battlefield->setPlayer(_pieceTaking.getFillColor(), (Piece*)&_pieceTaking, true);
	}
}

