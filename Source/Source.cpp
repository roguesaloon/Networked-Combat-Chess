#include <SFML/Network.hpp>
#include <iostream>
#include <chrono>
#include "Set.h"
#include "Board.h"
#include "UI.h"


int main(int argc, char* argv[])
{
	std::string mode;
	sf::TcpSocket socket;

	void makeConnection(std::string &mode, sf::TcpSocket &socket, int argc, char* argv[]);
	makeConnection(mode, socket, argc, argv);

	void playGame(std::string &mode, sf::TcpSocket &socket);
	playGame(mode, socket);
	

	return EXIT_SUCCESS;
}

// Makes A Connection Between The Players
void makeConnection(std::string &mode, sf::TcpSocket &socket, int argc, char* argv[])
{
	// Takes in argument from command line (if present)
	if (argc > 1)
	{
		mode = argv[1];
	}
	else // Otherwise explicity ask's for argument
	{
		std::cout << "Mode or IP Address To Connect To: ";
		std::cin >> mode;
		std::cout << std::endl;
	}

	// Port to use for game
	const sf::Uint16 &port = 53000;

	// Create Game Function
	// Will listen for a connection (On given port)
	// And display PC's IP address details
	// By design the host is always white
	auto createGame = [&port, &socket, &mode]()
	{  
		std::cout << "Creating Game\n" << std::endl;

		sf::TcpListener listener;

		listener.listen(port);

		

		std::cout << "This PC Has Local  IP " << sf::IpAddress::getLocalAddress() << std::endl;
		std::cout << "            Public IP " << sf::IpAddress::getPublicAddress(sf::milliseconds(200)) << std::endl;


		std::cout << "\nWaiting For Other Player..." << std::endl;

		listener.accept(socket);

		mode = "white";
	};

	// Look For Game Function
	// Will try to connect to a given ip address (from intial argument)
	// This should be the host computer
	// Will return false on failure to connect
	// By design guest is always black
	auto lookForGame = [&port, &socket, &mode]()
	{
		sf::IpAddress IP_ADDRESS;

		if (mode == "local") // Use local address if "local" is typed
		{
			IP_ADDRESS = sf::IpAddress::getLocalAddress();
		}
		else
		{
			IP_ADDRESS = mode;
		}
 
		int count = 1;
		const int MAX_RETRY = 3; // Times to attempt to connect, before returning failure

		while (socket.connect(IP_ADDRESS, port) != sf::Socket::Done)
		{
			std::cout << "Looking For Game..." << count << std::endl;

			if (count >= MAX_RETRY)
			{
				std::cout << "\nNo Game Found\n" << std::endl;
				return false;
			}

			++count;
		}

		mode = "black";

		return true;
	};


	// Using Initial argument
	// On create, or a failure to find game
	// Create a game
	if (mode == "create" || !lookForGame())
		createGame();

	std::cout << "Connected" << std::endl;
}



void playGame(std::string &mode, sf::TcpSocket &socket)
{

#pragma region GameInit

	// Create Window
	sf::RenderWindow window(sf::VideoMode(1200, 600), "I am " + mode);	
	window.setFramerateLimit(144);
	window.setVerticalSyncEnabled(true);

	// Game Objects
	Board board;
	UI ui(board);	
	Set blackSet(sf::Color::Black, *((std::vector<sf::RectangleShape>*)&board));
	Set whiteSet(sf::Color::White, *((std::vector<sf::RectangleShape>*)&board));
	
	// Game Wide Variables
	bool endGame = false;
	sf::Vector2i mousePos; 
	sf::Int16 shotsTaken = 0;

	// Time Initialization
	std::chrono::time_point<std::chrono::system_clock> timeStart, timeNow, timeLastFrame;
	timeStart = std::chrono::system_clock::now();
	sf::Int64 timeElasped, timeDelta;
	sf::Int64 checkLatency(const std::string &mode, sf::TcpSocket &socket, const std::chrono::time_point<std::chrono::system_clock> &startPoint);
	sf::Int64 timeLatency = checkLatency(mode, socket, timeStart);

	// Network Initialization
	sf::Packet packet;
	socket.setBlocking(false);

	// Determines which set is the player
	// and which is the opponent
	// Based on initial mode string
	Set *playerSet, *otherSet;
	(mode == "white") ? (playerSet = &whiteSet) : (playerSet = &blackSet);
	(mode == "black") ? (otherSet = &whiteSet) : (otherSet = &blackSet);

	// Function Prototypes
	void takeShot(std::string &mode, sf::Int16 &shotsTaken, sf::Vector2i const &mousePos, Board &board, UI const &ui, sf::Packet &packet, sf::TcpSocket &socket, Set *playerSet, Set *otherSet);
	void recievePacket(sf::Int64 const &timeElasped, sf::TcpSocket &socket, sf::Packet &packet, sf::Int16 &shotsTaken, UI const &ui, Board const &board, std::string &mode, Set *playerSet, Set *otherSet);

#pragma endregion

	while (window.isOpen())
	{
		/*updateTime*/ [&]()
		{		
			timeLastFrame = timeNow;
			timeNow = std::chrono::system_clock::now();

			timeDelta = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - timeLastFrame).count();

			timeElasped = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - timeStart).count() + timeLatency;

		}();

		/*handleEvents*/ [&window]()
		{		
			sf::Event event;  

			while (window.pollEvent(event))
			{
				// Close window: exit
				if (event.type == sf::Event::Closed)
					window.close();
			}
		}();

		mousePos = (sf::Mouse::getPosition(window));

		takeShot(mode, shotsTaken, mousePos, board, ui, packet, socket, playerSet, otherSet);

		if (!(mode.find("fight") != std::string::npos))
		{
			recievePacket(timeElasped, socket, packet, shotsTaken, ui, board, mode, playerSet, otherSet);
		}
		else
		{
			ui.updateBattlefield(mode, timeDelta, timeElasped, &socket, window);
		}

		if (playerSet->getPieces()[0][0].isTaken() || otherSet->getPieces()[0][0].isTaken())
			endGame = true;


		/*draw*/ [&]()
		{
			window.clear(sf::Color(0, 0, 255));

			board.draw(window);
			if (!endGame)
			{
				blackSet.draw(window);
				whiteSet.draw(window);
			}
			ui.draw(window, mousePos);

			window.display();
		}();

	}
}

// Recieves a packet containing information about the other players shot
// And updates on players view appropiately
// This packet is received once every second
// It is recieved regardless whether a packet is sent
// If there is no packet to recieve, it will just move on
void receivePacket(sf::Int64 const &timeElasped, sf::TcpSocket &socket, sf::Packet &packet, sf::Int16 &shotsTaken, Board &board, UI const &ui, std::string &mode, Set *playerSet, Set *otherSet)
{
	static sf::Int64 timeLastRecieve = 0;

	if (timeElasped - timeLastRecieve > 1000)
	{
		timeLastRecieve = timeElasped;

		sf::Int32 piece, num, letter, number, enemyPiece, enemyNum;
		bool pieceTaken;;
		sf::Int16 shotsNum;

		if (socket.receive(packet) == socket.Done)
		{
			packet >> shotsNum >> piece >> num >> letter >> number >> pieceTaken >> enemyPiece >> enemyNum;



			if (piece >= 0 && piece < (sf::Int32)playerSet->getPieces().size() && number >= 0 && number <= 8)
			{
				shotsTaken = shotsNum;

				if (!otherSet->getPieces()[piece][num].isTaken())
					otherSet->getPieces()[piece][num].setGridPosition(letter, number, (std::vector<sf::RectangleShape>*)&board);

				if (pieceTaken)
					//playerSet->getPieces()[enemyPiece][enemyNum].take();
					ui.takePiece(playerSet->getPieces()[enemyPiece][enemyNum], otherSet->getPieces()[piece][num], mode);
			}
		}
	}
}

// Take Shot
// Called every frame
// Proceses logic for a player taking their shot
// Sends packet when a shot is taken
void takeShot(std::string &mode, sf::Int16 &shotsTaken, sf::Vector2i const &mousePos, Board &board, UI const &ui, sf::Packet &packet, sf::TcpSocket &socket, Set *playerSet, Set *otherSet)
{
		// Checks if it is the players shot
		// If not return
		if (!(mode == "white" && shotsTaken % 2 == 0) && !(mode == "black" && shotsTaken % 2 == 1))
			return;

		// Loops through all of the players pieces
		for (sf::Uint32 i = 0; i < playerSet->getPieces().size(); ++i)
		{
			for (sf::Uint32 j = 0; j < playerSet->getPieces()[i].size(); ++j)
			{
				auto &piece = playerSet->getPieces()[i][j];

				// If The piece is taken
				// Move onto next piece
				if (piece.isTaken())
					continue;

				// If the piece is clicked on
				// Then make that the selected piece
				if (piece.isClicked(mousePos))
					board.setSelectedPiece(piece);

				// If the piece is not the selected piece
				// Move onto next piece
				if (!board.isSelectedPiece(piece))
					continue;

				// Checks available moves
				// Move to square on click

				// Loops through all possible moves
				for (auto &move : piece.getAvailableMoves(playerSet->getPieces(), otherSet->getPieces()))
				{
					// Move onto next move
					// If the square to move to isn't clicked
					// Hence only execute rest of code when it is
					if (!board.isSquareClicked(move, mousePos))
						continue;

					// Move the piece to desired square
					piece.setGridPosition(move.x, move.y, (std::vector<sf::RectangleShape>*)&board);

					// Checks if the move made takes an enemy piece
					auto isTaken = [&](sf::Uint32 &k, sf::Uint32 &l)
					{
						for (k = 0; k < otherSet->getPieces().size(); ++k)
						{
							for (l = 0; l < otherSet->getPieces()[k].size(); ++l)
							{
								if (otherSet->getPieces()[k][l].getGridPosition() == move)
								{
									ui.takePiece(otherSet->getPieces()[k][l], piece, mode);
									//otherSet->getPieces()[k][l].take();
									return true;
								}
							}
						}

						return false;
					};

					// Temp variables to store index of opponent piece (to be taken)
					sf::Uint32 k = 0, l = 0;

					// Loads up packet and sends it across to other player
					//board.unSelectPiece();
					++shotsTaken;
					packet.clear();
					packet << shotsTaken << i << j << move.x << move.y << isTaken(k, l);
					packet << k << l;
					socket.send(packet);
				}
			}
		}
}

// Recieves a packet containing information about the other players shot
// And updates on players view appropiately
// This packet is received once every second
// It is recieved regardless whether a packet is sent
// If there is no packet to recieve, it will just move on
void recievePacket(sf::Int64 const &timeElasped, sf::TcpSocket &socket, sf::Packet &packet, sf::Int16 &shotsTaken, UI const &ui, Board const &board, std::string &mode, Set *playerSet, Set *otherSet)
{
	static sf::Int64 timeLastRecieve = 0;

	//Recieve Once A Second
	if (timeElasped - timeLastRecieve > 1000)
	{
		timeLastRecieve = timeElasped;

		sf::Int32 piece, num, letter, number, enemyPiece, enemyNum;
		bool pieceTaken;
		sf::Int16 shotsNum;

		if (socket.receive(packet) == socket.Done)
		{
			packet >> shotsNum >> piece >> num >> letter >> number >> pieceTaken >> enemyPiece >> enemyNum;

			if (piece >= 0 && piece < (sf::Int32)playerSet->getPieces().size() && number >= 0 && number <= 8)
			{
				shotsTaken = shotsNum;

				if (!otherSet->getPieces()[piece][num].isTaken())
					otherSet->getPieces()[piece][num].setGridPosition(letter, number, (std::vector<sf::RectangleShape>*)&board);

				if (pieceTaken)
					//playerSet->getPieces()[enemyPiece][enemyNum].take();
					ui.takePiece(playerSet->getPieces()[enemyPiece][enemyNum], otherSet->getPieces()[piece][num], mode);
			}
		}
	}
}

sf::Int64 checkLatency(const std::string &mode, sf::TcpSocket &socket, const std::chrono::time_point<std::chrono::system_clock> &startPoint)
{
	sf::Packet testPacket;
	sf::Int64 latency;

	if (!(mode.find("white") != std::string::npos))
	{

		testPacket << "Test";

		socket.send(testPacket);

		socket.receive(testPacket);

		testPacket >> latency;

	}
	else
	{
		socket.receive(testPacket);

		latency = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startPoint).count();

		testPacket.clear();

		testPacket << latency;

		socket.send(testPacket);
	}

	return latency;
}