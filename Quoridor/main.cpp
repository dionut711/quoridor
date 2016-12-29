#include <SFML\Graphics.hpp>
#include <time.h>
#include <cmath>
#include <iostream>

int wallMatrix[17][9];

bool funCheckforWall(sf::Vector2i posStart, sf::Vector2i posEnd)
{
	if (posStart.x == posEnd.x)
	{
		if (posStart.y > posEnd.y)
		{
			int aux = posStart.y;
			posStart.y = posEnd.y;
			posEnd.y = aux;
		}
	}
	else if (posStart.y == posEnd.y)
	{
		if (posStart.x > posEnd.x)
		{
			int aux = posStart.x;
			posStart.x = posEnd.x;
			posEnd.x = aux;
		}
	}

	if (posStart.y == posEnd.y)
	{
		if (wallMatrix[posStart.x * 2 + 1][posStart.y] > 0)
			return 1;
	}
	else if (posStart.x == posEnd.x)
	{
		if (wallMatrix[posStart.x * 2][posStart.y] > 0)
			return 1;
	}
	return 0;
}

int main()
{
	int nrOfPlayers = 4;
	int turn = 0;
	int maxWallsPerPlayer[4];


	int sizeSquare = 61, widthWall = 15, marginWidth = 155;
	int sizeTotal = sizeSquare + widthWall;
	sf::RenderWindow window(sf::VideoMode(969, 696), "Game");

	sf::Texture tBoard;
	tBoard.loadFromFile("images/QuoridorBoard1.png");
	sf::Sprite sBoard;
	sBoard.setTexture(tBoard);

	sf::Texture tWall;
	tWall.loadFromFile("images/QuoridorWall.png");
	sf::Sprite sWalls[42];
	int nrOfPlacedWalls = 0;
	int WallsPlaceableLimit = 40;

	//////// PLACE THE WALL AT A PERFECT POSITION /////////
	int leftMarginForPlacingWalls = 181;
	int topMarginForPlacingWalls = 43;
	int wallActiveZone = 76;
	sf::Vector2i posWall;
	sf::Vector2i fixedPosWall;
	bool canWallBePlaced = false;

	//wall activate button
	sf::Texture tButtonWall1,tButtonWall2;
	tButtonWall1.loadFromFile("images/ButtonWall1.png");
	tButtonWall2.loadFromFile("images/ButtonWall2.png");
	sf::Sprite sButtonWall1;
	sButtonWall1.setTexture(tButtonWall1);
	bool isWallButton = false;
	bool isWallPlaceable = false;
	bool wallOrientation = false;
	bool JustOneWall = false;
	sButtonWall1.setPosition(19, 25);

	//wall variables
	
	for(int i=0;i<=16;i++)
		for (int j = 0; j <= 8; j++)
			wallMatrix[i][j] = 0;
		//temporary walls
	wallMatrix[1][4] = 1;
	wallMatrix[0][4] = 1;


	//exit button
	sf::Texture tExit1, tExit2;
	tExit1.loadFromFile("images/Exit1.png");
	tExit2.loadFromFile("images/Exit2.png");
	sf::Sprite sExit1;
	sExit1.setTexture(tExit1);
	sExit1.setPosition(850, 25);
	bool isExit = false;

	sf::Texture tPawn[4];
	tPawn[0].loadFromFile("images/QuoridorPawn0.png");
	tPawn[1].loadFromFile("images/QuoridorPawn1.png");
	tPawn[2].loadFromFile("images/QuoridorPawn2.png");
	tPawn[3].loadFromFile("images/QuoridorPawn3.png");
	sf::Sprite sPawn[4];
	for (int i = 0;i <= nrOfPlayers - 1;i++)
		sPawn[i].setTexture(tPawn[i]);

	bool isMove = false;


	sf::Vector2i pawn[4];
	pawn[0] = sf::Vector2i(0, 4);//blue
	pawn[1] = sf::Vector2i(8, 4);//green
	pawn[2] = sf::Vector2i(4, 0);//yellow
	pawn[3] = sf::Vector2i(4, 8);//red
	sf::Vector2i nextPawn[4];
	sf::Vector2i posPawn[4];
	for (int i = 0; i <= nrOfPlayers - 1; i++)
		sPawn[i].setPosition(marginWidth + sizeTotal*pawn[i].y, widthWall + sizeTotal*pawn[i].x);

	//// SET WALLS FOR PLAYERS ////
	for (int i = 0;i < nrOfPlayers;i++)
		maxWallsPerPlayer[i] = 10;

//////////////////// ACTION WHEN WINDOW IS OPEN //////////////////////////
	while (window.isOpen())
	{
		sf::Vector2i posMouse = sf::Mouse::getPosition(window);
		sf::Event e;
		while (window.pollEvent(e))
		{

			/////////// CLOSE THE WINDOW /////////////
			if (e.type == sf::Event::Closed)
				window.close();
			
			if (e.type == sf::Event::MouseButtonPressed)
				if (e.key.code == sf::Mouse::Left)
					if (sExit1.getGlobalBounds().contains(posMouse.x, posMouse.y)) {
						sExit1.setTexture(tExit2);
						isExit = true;
					}
			if (e.type == sf::Event::MouseButtonReleased)
				if (e.key.code == sf::Mouse::Left && isExit)
					window.close();

			if (e.type == sf::Event::MouseButtonPressed)
				if (e.key.code == sf::Mouse::Left)
					if (sPawn[turn].getGlobalBounds().contains(posMouse.x, posMouse.y))
					{
						isMove = true;
					}

			
			//Wall button
			if (e.type == sf::Event::MouseButtonPressed)
				if (e.key.code == sf::Mouse::Left)
					if (sButtonWall1.getGlobalBounds().contains(posMouse.x, posMouse.y) && JustOneWall == false && nrOfPlacedWalls < WallsPlaceableLimit && maxWallsPerPlayer[turn]>0)
					{
						sButtonWall1.setTexture(tButtonWall2);
						sWalls[nrOfPlacedWalls].setTexture(tWall);
						sWalls[nrOfPlacedWalls].setOrigin(7.5, 68.5);
						nrOfPlacedWalls += 1;
						std::cout << nrOfPlacedWalls << " ";
						isWallButton = true;
						JustOneWall = true;
					}
			if(e.type == sf::Event::MouseButtonReleased)
				if(e.key.code == sf::Mouse::Left)
					sButtonWall1.setTexture(tButtonWall1);
			////////// PLACE THE WALL ////////  
			posWall.x = (posMouse.x - leftMarginForPlacingWalls) / wallActiveZone;
			posWall.y = (posMouse.y - topMarginForPlacingWalls) / wallActiveZone;
			fixedPosWall.x = 219 + wallActiveZone*posWall.x;
			fixedPosWall.y = 81 + wallActiveZone*posWall.y;
			if(e.type == sf::Event::MouseButtonPressed)
				if(e.key.code == sf::Mouse::Left)
					if (!sButtonWall1.getGlobalBounds().contains(posMouse.x, posMouse.y) && JustOneWall == true)
					{
						// 219, 81
						//fixedPosWall.x = 219 + wallActiveZone*posWall.x;
						//fixedPosWall.y = 81 + wallActiveZone*posWall.y;
						//std::cout << "Wall.x = " << posWall.x << " Wall.y = " << posWall.y << " Mouse.x = "<<posMouse.x<<" Mouse.y = "<<posMouse.y<<" ";
						//std::cout << std::endl << "new.x = " << fixedPosWall.x << " new.y = " << fixedPosWall.y << std::endl;
						sWalls[nrOfPlacedWalls + 1].setTexture(tWall);
						//sWalls[nrOfPlacedWalls].setPosition(posMouse.x,posMouse.y);
						//nrOfPlacedWalls += 1;
						isWallButton = false;
						JustOneWall = false;
						maxWallsPerPlayer[turn] -= 1;
						turn = (turn + 1) % nrOfPlayers;
						canWallBePlaced = true;
					}

			if (e.type == sf::Event::MouseButtonReleased)
				if (e.key.code == sf::Mouse::Right && isMove == false && !isWallPlaceable && JustOneWall == true)
				{
					nrOfPlacedWalls -= 1;
					isWallButton = false;
					JustOneWall = false;
					std::cout << nrOfPlacedWalls << " ";
				}
			if (e.type == sf::Event::MouseWheelMoved)
				if (isWallButton)
				{
					sWalls[nrOfPlacedWalls - 1].rotate(90 * e.mouseWheel.delta);
				}

			if (e.type == sf::Event::MouseButtonReleased)
				if (e.key.code == sf::Mouse::Left && isMove == true)
				{

					isMove = false;
					nextPawn[turn].x = posMouse.y / sizeTotal;
					nextPawn[turn].y = (posMouse.x - marginWidth) / sizeTotal;

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
					//check for wall
					if(nextPawn[turn] != pawn[turn])
						if (funCheckforWall(pawn[turn], nextPawn[turn]))
							nextPawn[turn] = pawn[turn];
					


					if (pawn[turn] != nextPawn[turn])
					{
						pawn[turn] = nextPawn[turn];
						posPawn[turn] = sf::Vector2i(marginWidth + sizeTotal*pawn[turn].y, widthWall + sizeTotal*pawn[turn].x);
						sPawn[turn].setPosition(posPawn[turn].x, posPawn[turn].y);
						turn = (turn + 1) % nrOfPlayers;
					}
					else
					{
						posPawn[turn] = sf::Vector2i(marginWidth + sizeTotal*pawn[turn].y, widthWall + sizeTotal*pawn[turn].x);
						sPawn[turn].setPosition(posPawn[turn].x, posPawn[turn].y);
					}

				}

		}

		if (isWallButton)
		{
			//sWalls[nrOfPlacedWalls-1].setPosition(posMouse.x, posMouse.y);
			sWalls[nrOfPlacedWalls - 1].setPosition(fixedPosWall.x,fixedPosWall.y);
			//if (canWallBePlaced)
			//{
			//	sWalls[nrOfPlacedWalls - 1].setPosition(fixedPosWall.x, fixedPosWall.y);
			//	canWallBePlaced = false;
			//}
		}

		if (isMove)
			sPawn[turn].setPosition(posMouse.x - sPawn[turn].getTextureRect().width / 2, posMouse.y - sPawn[turn].getTextureRect().height / 2);

		window.clear(sf::Color::White);
	
		window.draw(sBoard);
		for (int i = 0;i <= nrOfPlayers - 1;i++)
			window.draw(sPawn[i]);
		if (nrOfPlacedWalls > WallsPlaceableLimit)
			nrOfPlacedWalls = WallsPlaceableLimit;
		for (int i = 0; i <= nrOfPlacedWalls - 1; i++)
			window.draw(sWalls[i]);
		// Draw Buttons
		window.draw(sButtonWall1);
		window.draw(sExit1);
		window.display();
	}
	return 0;
}