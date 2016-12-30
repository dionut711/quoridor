#include <SFML\Graphics.hpp>
#include <time.h>
#include <cmath>
#include <iostream>

int wallMatrix[17][9];
int nrOfPlayers = 4;
//// DON'T LET WALLS TO COLIDE EACHOTHER ////
int crossWalls[8][8];
/////// DISPLAY PLAYER'S TURN IMAGES ////////
sf::Texture tTurn[4];
sf::Sprite sTurn;



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
void nextTurn(int &currentTurn)
{
	currentTurn = (currentTurn + 1) % nrOfPlayers;
	sTurn.setTexture(tTurn[currentTurn]);
	//Check if next turn is AI
}
bool funWallCanBePlaced(int rotate,sf::Vector2i posWall)
{
	if (crossWalls[posWall.x][posWall.y] == 1)
		return 0;
	if (rotate == 0)
	{
		if (wallMatrix[posWall.y * 2][posWall.x] != 0 || wallMatrix[posWall.y * 2 + 2][posWall.x] != 0)
			return 0;
	}
	else
		if (wallMatrix[posWall.y * 2 + 1][posWall.x] != 0 || wallMatrix[posWall.y * 2 + 1][posWall.x + 1] != 0)
			return 0;
	return 1;
}

int main()
{
	int turn = 0;
	int maxWallsPerPlayer[4];
	int rotate = 0;

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
	bool isWallPlaceable = false;
	bool wallOrientation = false;
	bool JustOneWall = false;
	sButtonWall1.setPosition(19, 25);

	//exit button
	sf::Texture tExit1, tExit2;
	tExit1.loadFromFile("images/Exit1.png");
	tExit2.loadFromFile("images/Exit2.png");
	sf::Sprite sExit1;
	sExit1.setTexture(tExit1);
	sExit1.setPosition(850, 25);
	bool isExit = false;

	//turn indicator GUI
	tTurn[0].loadFromFile("images/Player1Turn.png");
	tTurn[1].loadFromFile("images/Player2Turn.png");
	tTurn[2].loadFromFile("images/Player3Turn.png");
	tTurn[3].loadFromFile("images/Player4Turn.png");
	sTurn.setTexture(tTurn[0]);
	sTurn.setPosition(19, 90);


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

				/////////////// CHECK FOR THE WIN
				if (pawn[0].x != 8 && pawn[1].x != 0 && pawn[2].y != 8 && pawn[3].y != 0) {

					if (e.type == sf::Event::MouseButtonPressed)
						if (e.key.code == sf::Mouse::Left)
							if (sPawn[turn].getGlobalBounds().contains(posMouse.x, posMouse.y))
							{
								isMove = true;
							}

					if (e.type == sf::Event::MouseButtonReleased)
						if (e.key.code == sf::Mouse::Left)
							sButtonWall1.setTexture(tButtonWall1);

					////////// PLACE THE WALL ////////  
					posWall.x = (posMouse.x - leftMarginForPlacingWalls) / wallActiveZone;
					posWall.y = (posMouse.y - topMarginForPlacingWalls) / wallActiveZone;
					fixedPosWall.x = 219 + wallActiveZone*posWall.x;
					fixedPosWall.y = 81 + wallActiveZone*posWall.y;
					if (e.type == sf::Event::MouseButtonPressed)
						if (e.key.code == sf::Mouse::Left)
							if (JustOneWall == true && funWallCanBePlaced(rotate, posWall))
							{
								crossWalls[posWall.x][posWall.y] = 1;
								sWalls[nrOfPlacedWalls + 1].setTexture(tWall);
								JustOneWall = false;
								maxWallsPerPlayer[turn] -= 1;
								nextTurn(turn);
								canWallBePlaced = true;

								//Mark walls in matrix
								if (rotate == 0) {
									wallMatrix[posWall.y * 2][posWall.x] = 1;
									wallMatrix[posWall.y * 2 + 2][posWall.x] = 1;
								}
								else {
									wallMatrix[posWall.y * 2 + 1][posWall.x] = 1;
									wallMatrix[posWall.y * 2 + 1][posWall.x + 1] = 1;
								}
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
								JustOneWall = true;
								rotate = 0;
							}

					if (e.type == sf::Event::MouseButtonReleased)
						if (e.key.code == sf::Mouse::Right && isMove == false && !isWallPlaceable && JustOneWall == true)
						{
							nrOfPlacedWalls -= 1;
							JustOneWall = false;
						}
					if (e.type == sf::Event::MouseWheelMoved)
						if (JustOneWall)
						{
							sWalls[nrOfPlacedWalls - 1].rotate(90 * e.mouseWheel.delta);
							rotate = (rotate + 1) % 2;
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

							sf::Vector2i deltaPawn = sf::Vector2i(abs(pawn[turn].x - nextPawn[turn].x), abs(pawn[turn].y - nextPawn[turn].y));
							//diagonal move
							if (abs(deltaPawn.x - deltaPawn.y) != 1)
								nextPawn[turn] = pawn[turn];
							//more than one position
							else if (deltaPawn.x > 1 || deltaPawn.y > 1)
								nextPawn[turn] = pawn[turn];

							

							//check for wall
							if (nextPawn[turn] != pawn[turn])
								if (funCheckforWall(pawn[turn], nextPawn[turn]))
									nextPawn[turn] = pawn[turn];



							if (pawn[turn] != nextPawn[turn])
							{
								pawn[turn] = nextPawn[turn];
								posPawn[turn] = sf::Vector2i(marginWidth + sizeTotal*pawn[turn].y, widthWall + sizeTotal*pawn[turn].x);
								sPawn[turn].setPosition(posPawn[turn].x, posPawn[turn].y);
								std::cout << "pawn.y = " << pawn[turn].y << " pawn.x = " << pawn[turn].x << " ";
								nextTurn(turn);
							}
							else
							{
								posPawn[turn] = sf::Vector2i(marginWidth + sizeTotal*pawn[turn].y, widthWall + sizeTotal*pawn[turn].x);
								sPawn[turn].setPosition(posPawn[turn].x, posPawn[turn].y);
							}

						}
				}
			}

			if (JustOneWall && posWall.x >= 0 && posWall.y >= 0 && posWall.x <= 7 && posWall.y <= 7)
				sWalls[nrOfPlacedWalls - 1].setPosition(fixedPosWall.x, fixedPosWall.y);

			if (isMove)
				sPawn[turn].setPosition(posMouse.x - sPawn[turn].getTextureRect().width / 2, posMouse.y - sPawn[turn].getTextureRect().height / 2);

			window.clear(sf::Color::White);
			window.draw(sBoard);

			if (nrOfPlacedWalls > WallsPlaceableLimit)
				nrOfPlacedWalls = WallsPlaceableLimit;
			for (int i = 0; i <= nrOfPlacedWalls - 1; i++)
				window.draw(sWalls[i]);

			//Draw Buttons
			window.draw(sTurn);
			window.draw(sButtonWall1);
			window.draw(sExit1);
			for (int i = 0; i <= nrOfPlayers - 1; i++)
				window.draw(sPawn[i]);
			if (pawn[0].x == 8 || pawn[1].x == 0 || pawn[2].y == 8 || pawn[3].y == 0) {
				window.clear(sf::Color::Black);
			}

			window.display();
	}
	return 0;
}