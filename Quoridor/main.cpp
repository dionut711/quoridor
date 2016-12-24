#include <SFML\Graphics.hpp>
#include <iostream>

	int main()
{
	sf::RenderWindow window(sf::VideoMode(969, 696), "Quoridor");
	sf::Texture img_board;
	img_board.loadFromFile("images/QuoridorBoard.png");
	sf::Sprite spr_board(img_board);

		while (window.isOpen())
		{

			sf::Event evnt;
				while (window.pollEvent(evnt))
				{
					if (evnt.type == sf::Event::Closed)
						window.close();
				}

			window.clear();
			window.draw(spr_board);
			window.display();
		}
	return 0;
}