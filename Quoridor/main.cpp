#include <SFML\Graphics.hpp>
#include <time.h>
int main()
{
	int sizeSquare = 61, widthWall = 15, marginWidth = 155;
	int sizeTotal = sizeSquare + widthWall;
	sf::RenderWindow window(sf::VideoMode(969, 696), "Game");

	sf::Texture tPawn, tBoard;
	tPawn.loadFromFile("images/QuoridorPawn.png");
	tBoard.loadFromFile("images/QuoridorBoard.png");
	sf::Sprite sPawn(tPawn), sBoard(tBoard);

	bool isMove = false;

	sf::Vector2i pawn;
	sf::Vector2i posPawn;

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
					if (sPawn.getGlobalBounds().contains(posMouse.x, posMouse.y))
					{
						isMove = true;
					}
			if (e.type == sf::Event::MouseButtonReleased)
				if (e.key.code == sf::Mouse::Left)
				{
					isMove = false;
					pawn.x = (posMouse.x - marginWidth) / sizeTotal;
					pawn.y = posMouse.y / sizeTotal;
					if (pawn.x < 0)
						pawn.x = 0;
					else if (pawn.x > 8)
						pawn.x = 8;
					if (pawn.y < 0)
						pawn.y = 0;
					else if (pawn.y > 8)
						pawn.y = 8;


					posPawn = sf::Vector2i(marginWidth + sizeTotal*pawn.x, widthWall + sizeTotal*pawn.y);
					sPawn.setPosition(posPawn.x, posPawn.y);
				}

		}

		if (isMove)
			sPawn.setPosition(posMouse.x - sPawn.getTextureRect().width / 2, posMouse.y - sPawn.getTextureRect().height / 2);
		window.clear(sf::Color::White);
		window.draw(sBoard);
		window.draw(sPawn);
		window.display();
	}
	return 0;
}