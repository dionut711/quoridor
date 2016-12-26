#include <SFML\Graphics.hpp>
#include <iostream>

	int main()
{
	sf::RenderWindow window(sf::VideoMode(969, 696), "Quoridor");
	sf::Texture img_board;
	sf::Texture img_pawns;
	img_board.loadFromFile("images/QuoridorBoard.png");
	img_pawns.loadFromFile("images/QuoridorPawns.png");
	sf::Sprite spr_board(img_board);
	sf::Sprite spr_pawns(img_pawns);
	spr_pawns.setTextureRect(sf::IntRect(0, 0, 58, 58));

	float dx, dy;
	bool isMove = false;
		while (window.isOpen())
		{
			sf::Vector2i mousePos = sf::Mouse::getPosition(window);
			sf::Event evnt;
				while (window.pollEvent(evnt))
				{
					if (evnt.type == sf::Event::Closed)
						window.close();

					if(evnt.type == sf::Event::MouseButtonPressed)
						if(evnt.key.code == sf::Mouse::Left)
							if (spr_pawns.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
								isMove = true;
								dx = mousePos.x - spr_pawns.getPosition().x;
								dy = mousePos.y - spr_pawns.getPosition().y;
							}
					if (evnt.type == sf::Event::MouseButtonReleased)
						if (evnt.key.code == sf::Mouse::Left) {
							isMove = false;
							sf::Vector2f p = spr_pawns.getPosition() + sf::Vector2f(32, 32);
							sf::Vector2f newPos = sf::Vector2f(76 * int(p.x / 76), 76 * int(p.y / 76) + 13);
							spr_pawns.setPosition(newPos);
						
						}
					if (isMove)
						spr_pawns.setPosition(mousePos.x - dx, mousePos.y - dy);
				}

			window.clear();
			window.draw(spr_board);
			window.draw(spr_pawns);
			window.display();
		}
	return 0;
}