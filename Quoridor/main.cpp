#include <SFML\Graphics.hpp>
#include <time.h>
#include <cmath>
#include <iostream>
int main()
{
	int nrOfPlayers = 4;
	int turn = 0;

	int sizeSquare = 61, widthWall = 15, marginWidth = 155;
	int sizeTotal = sizeSquare + widthWall;
	sf::RenderWindow window(sf::VideoMode(969, 696), "Game");

	sf::Texture tBoard,tWall;
	tBoard.loadFromFile("images/QuoridorBoard1.png");
	tWall.loadFromFile("images/QuoridorWall.jpg");
	sf::Sprite sBoard(tBoard),sWall(tWall);
	sf::Texture tPawn[4];
	sWall.setPosition(-100, -100);
	tPawn[0].loadFromFile("images/QuoridorPawn0.png");
	tPawn[1].loadFromFile("images/QuoridorPawn1.png");
	tPawn[2].loadFromFile("images/QuoridorPawn2.png");
	tPawn[3].loadFromFile("images/QuoridorPawn3.png");
	sf::Sprite sPawn[4];
	for (int i = 0;i <= nrOfPlayers - 1;i++)
		sPawn[i].setTexture(tPawn[i]);

	bool isMove = false;


	sf::Vector2i pawn[4];
	pawn[0] = sf::Vector2i(4, 0);
	pawn[1] = sf::Vector2i(4, 8);
	pawn[2] = sf::Vector2i(0, 4);
	pawn[3] = sf::Vector2i(8, 4);
	sf::Vector2i nextPawn[4];
	sf::Vector2i posPawn[4];

	for (int i = 0; i <= nrOfPlayers - 1; i++)
	{
		sPawn[i].setPosition(marginWidth + sizeTotal*pawn[i].x, widthWall + sizeTotal*pawn[i].y);
	}
	while (window.isOpen())
	{
		sf::Vector2i posMouse = sf::Mouse::getPosition(window);
		sf::Event e;
		while (window.pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
				window.close();

			if (e.type == sf::Event::MouseButtonPressed)
				if (e.key.code == sf::Mouse::Left)
					if (sPawn[turn].getGlobalBounds().contains(posMouse.x, posMouse.y))
					{
						isMove = true;
					}
			if (e.type == sf::Event::MouseButtonReleased)
				if (e.key.code == sf::Mouse::Left)
				{
					sWall.setPosition(posMouse.x, posMouse.y);
				}

			if (e.type == sf::Event::MouseButtonReleased)
				if (e.key.code == sf::Mouse::Left && isMove == true)
				{
					
					isMove = false;
					nextPawn[turn].x = (posMouse.x - marginWidth) / sizeTotal;
					nextPawn[turn].y = posMouse.y / sizeTotal;

					//Clamp
					if (nextPawn[turn].x < 0)
						nextPawn[turn].x = 0;
					else if (nextPawn[turn].x > 8)
						nextPawn[turn].x = 8;
					if (nextPawn[turn].y < 0)
						nextPawn[turn].y = 0;
					else if (nextPawn[turn].y > 8)
						nextPawn[turn].y = 8;

					//more than one position
					if (abs(pawn[turn].x - nextPawn[turn].x) > 1 || abs(pawn[turn].y - nextPawn[turn].y) > 1)
						nextPawn[turn] = pawn[turn];
					//diagonal move
					else if (abs(abs(pawn[turn].x - nextPawn[turn].x) - abs(pawn[turn].y - nextPawn[turn].y)) != 1)
						nextPawn[turn] = pawn[turn];

					if (pawn[turn] != nextPawn[turn])
					{
						pawn[turn] = nextPawn[turn];
						posPawn[turn] = sf::Vector2i(marginWidth + sizeTotal*pawn[turn].x, widthWall + sizeTotal*pawn[turn].y);
						sPawn[turn].setPosition(posPawn[turn].x, posPawn[turn].y);
						turn = (turn + 1) % nrOfPlayers;
					}
					else
					{
						posPawn[turn] = sf::Vector2i(marginWidth + sizeTotal*pawn[turn].x, widthWall + sizeTotal*pawn[turn].y);
						sPawn[turn].setPosition(posPawn[turn].x, posPawn[turn].y);
					}


					std::cout << turn << " ";
				}

		}

		if (isMove)
			sPawn[turn].setPosition(posMouse.x - sPawn[turn].getTextureRect().width / 2, posMouse.y - sPawn[turn].getTextureRect().height / 2);

		window.clear(sf::Color::White);
		window.draw(sBoard);
	 	window.draw(sWall);
		for (int i = 0;i <= nrOfPlayers - 1;i++)
			window.draw(sPawn[i]);
		window.display();
	}
	return 0;
}