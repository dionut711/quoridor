#include <SFML\Graphics.hpp>
#include <time.h>
#include <cmath>
#include <iostream>
#include <random>
#include <Windows.h>
#include <SFML/Audio.hpp>

int waitingforAI = 0;
sf::Sprite sWin;
sf::Texture tWinPlayer, tWinComputer;

sf::RenderWindow window(sf::VideoMode(969, 696), "QUORIDOR",sf::Style::Close | sf::Style::Titlebar);
sf::Sprite sBoard,sWalls[100], sTurnDisplay;
sf::Sprite sButtonWall1;  
sf::Sprite sExit1, sBack;
sf::Sprite sPlayerWalls[11];


sf::Vector2i randPos;

int sizeSquare = 61;
int widthWall = 15, marginWidth = 155;
int sizeTotal = sizeSquare + widthWall;
int nrOfPlacedWalls;

bool musicIsPaused = false;
bool musicState = false;
bool musicOn = false;
bool goPrev = false, goUrm = false, helpMode = false, leaveHelp = false;
bool drawSclassic = false, drawSwild = false, drawShelp = false;
int helpStatus = 1, gameStatus = 0;




sf::Vector2i pawn[4];
sf::Sprite sPawn[4];

///// power ups
sf::Vector2i powerUpPos, powerUpPos2, powerUpPos3;
sf::Sprite spowerUp, spowerUp2, spowerUp3;
bool deleteWalls = false;

int maxWallsPerPlayer[4];
int maxNumber;
int turn;
int playerType[4];
int playerSkin[11];
int wallMatrix[17][9];
int nrOfPlayers = 2;
//// DON'T LET WALLS TO COLIDE EACHOTHER ////
int crossWalls[8][8];
///// no winner
int winner;

///// Change Button state
int buttonState;
bool addWindow = false;
bool removeWindow = false;
bool goBack = false;
bool goStart = false;
bool next[4];
bool previous[4];
bool nextSkin[4];
bool previousSkin[4];

//current turn displayer
sf::Sprite turnUI;


//////// Set The Mode : Classic is 0 , Wild is 1
bool setMode = false;
int countDown=4, countDown2=5, countDown3=6;
int threeMoves = 0;

sf::Vector2i posWall,fixedPosWall;
bool checkRotation[100];
int leftMarginForPlacingWalls = 181, topMarginForPlacingWalls = 43, wallActiveZone = 76;
void funDeleteWalls()
{
	for (int i = nrOfPlacedWalls - 1; i > nrOfPlacedWalls - nrOfPlayers - 1; i--)
	{
		if (i < 0)
			continue;
		sf::Vector2f deleteWallPos = sWalls[i].getPosition();
		posWall.x = (deleteWallPos.x - leftMarginForPlacingWalls) / wallActiveZone;
		posWall.y = (deleteWallPos.y - topMarginForPlacingWalls) / wallActiveZone;
		if (checkRotation[i] == 0)
		{
			wallMatrix[posWall.y * 2][posWall.x] = 0;
			wallMatrix[posWall.y * 2 + 2][posWall.x] = 0;
		}
		else
		{
			wallMatrix[posWall.y * 2 + 1][posWall.x] = 0;
			wallMatrix[posWall.y * 2 + 1][posWall.x + 1] = 0;
		}
		sWalls[i].setPosition(-100, -100);
	}
	nrOfPlacedWalls -= nrOfPlayers;
	if (nrOfPlacedWalls < 0)
		nrOfPlacedWalls = 0;
}
int powerMatrix[3][3];
sf::Vector2i powerUP() {
	sf::Vector2i randomUpCoord;
	randomUpCoord.x = (rand() % 3) + 3;
	randomUpCoord.y = (rand() % 3) + 3;
	std::cout << "PowerUp" << " x= " << randomUpCoord.x << " y= " << randomUpCoord.y << std::endl;
	return randomUpCoord;
}

void funCheckWinCondition()
{
	if (turn < 2)
	{
		if (pawn[turn].x == (1 - turn) * 8)
		{
			if (playerType[turn] == 0)
				sWin.setTexture(tWinPlayer);
			else
				sWin.setTexture(tWinComputer);

			turnUI.setTexture(*sPawn[turn].getTexture());
			turnUI.setPosition(460, 210);
			winner = 1;
		}
	}
	else if (turn >= 2)
	{
		if (pawn[turn].y == (3 - turn) * 8)
		{
			if (playerType[turn] == 0)
				sWin.setTexture(tWinPlayer);
			else
				sWin.setTexture(tWinComputer);

			turnUI.setTexture(*sPawn[turn].getTexture());
			turnUI.setPosition(460, 210);
			winner = 1;
		}
	}
}

void funDrawBoard()
{
	window.clear(sf::Color::White);
	window.draw(sBoard);
	
	for (int i = 0; i <= nrOfPlacedWalls - 1; i++)
		window.draw(sWalls[i]);
	for (int i = 0; i <= nrOfPlayers - 1; i++)
		window.draw(sPawn[i]);
	window.draw(sTurnDisplay);
	window.draw(sPlayerWalls[maxWallsPerPlayer[turn]]);
	if (setMode)
	{
		window.draw(spowerUp);
		window.draw(spowerUp2);
		window.draw(spowerUp3);
	}
	window.draw(sButtonWall1);
	window.draw(sExit1);
	window.draw(sBack);
	window.draw(turnUI);
	window.display();

}
bool isOccupiedByPawn(sf::Vector2i pos)
{
	for (int i = 0; i <= nrOfPlayers - 1; i++)
		if (pos.x == pawn[i].x && pos.y == pawn[i].y)
			return 1;
	return 0;
}
bool funCheckforWall(sf::Vector2i posStart, sf::Vector2i posEnd)
{
	if ((posStart.x - posEnd.x) != 0 && (posStart.y - posEnd.y) != 0)
	{
		std::cout << "called for" << std::endl;
		std::cout << posStart.x << "," << posStart.y << " and " << posEnd.x << "," << posEnd.y << std::endl;
		return 0;
	}
		
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

bool funWallCanBePlaced(int wallRotation, sf::Vector2i posWall)
{
	if (crossWalls[posWall.x][posWall.y] == 1)
		return 0;
	if (wallRotation == 0)
	{
		if (wallMatrix[posWall.y * 2][posWall.x] != 0 || wallMatrix[posWall.y * 2 + 2][posWall.x] != 0)
			return 0;
	}
	else
		if (wallMatrix[posWall.y * 2 + 1][posWall.x] != 0 || wallMatrix[posWall.y * 2 + 1][posWall.x + 1] != 0)
			return 0;
	return 1;
}
bool funBlocksPlayer(bool checkforplayer)
{
	int dl[4] = { -1, 0, 1, 0 };
	int dc[4] = { 0, 1, 0, -1 };
	int prim, ultim, p[2], v[2], c[81][2];
	bool foundPath;

	int board[9][9];
	for (int g = 0; g <= nrOfPlayers - 1; g++)
	{
		foundPath = false;
		c[0][0] = pawn[g].x;
		c[0][1] = pawn[g].y;
		prim = ultim = 0;

		for (int i = 0; i <= 8; i++)
			for (int j = 0; j <= 8; j++)
				board[i][j] = 0;
		for (int i = 0; i <= nrOfPlayers - 1; i++)
			board[pawn[i].x][pawn[i].y] = -1;
		board[pawn[turn].x][pawn[turn].y] = 1;


		while (prim <= ultim && !foundPath)
		{
			p[0] = c[prim][0];
			p[1] = c[prim][1];
			prim++;

			for (int k = 0; k <= 3; k++)
			{
				v[0] = p[0] + dl[k];
				v[1] = p[1] + dc[k];
				if (board[v[0]][v[1]] == -1)
				{
					v[0] = v[0] + dl[k];
					v[1] = v[1] + dc[k];
				}
				if(board)

				if (board[v[0]][v[1]] == 0 && v[0] >= 0 && v[0] <= 8 && v[1] >= 0 && v[1] <= 8 && !funCheckforWall(sf::Vector2i(p[0], p[1]), sf::Vector2i(v[0], v[1])))
				{
					/*
					sf::Vector2i tempPawn = pawn[g];
					pawn[g] = sf::Vector2i(v[0], v[1]);
					if (funBlocksPlayer(1))
					{
					pawn[g] = sf::Vector2i(tempPawn.x, tempPawn.y);
					}
					else
					{
					*/
					board[v[0]][v[1]] = board[p[0]][p[1]] + 1;

					ultim++;
					c[ultim][0] = v[0];
					c[ultim][1] = v[1];
					if (g == 0 || g == 1)
						if (v[0] == (1 - g) * 8)
							foundPath = true;

					if (g == 2 || g == 3)
						if (v[1] == (3 - g) * 8)
							foundPath = true;
					//}
				}
			}
		}

		if (foundPath == false)
			return true;

	}
	return false;
}
sf::Vector2i funReverseLee(int board[9][9], sf::Vector2i pos)
{
	bool foundPath = false;
	int dl[4] = { -1, 0, 1, 0 };
	int dc[4] = { 0, 1, 0, -1 };
	int v[2];
	if (board[pos.x][pos.y] == 2)
		return pos;

	sf::Vector2i mn(pos.x, pos.y);
	for (int i = 0; i <= 3; i++)
	{
		v[0] = pos.x + dl[i];
		v[1] = pos.y + dc[i];
		if (board[v[0]][v[1]] == -1 && !funCheckforWall(sf::Vector2i(pos.x, pos.y), sf::Vector2i(v[0], v[1])))
		{
			sf::Vector2i tempMin = mn;
			for (int k = 0; k <= 3; k++)
			{
				if ((k + 2) % 4 != i)
				{
					v[0] = pos.x + dl[i] + dl[k];
					v[1] = pos.y + dc[i] + dc[k];
					if (board[tempMin.x][tempMin.y] > board[v[0]][v[1]] && board[v[0]][v[1]] > 1)
					{
						tempMin.x = v[0];
						tempMin.y = v[1];
					}
					else
					{
						std::cout << "didnt find " << tempMin.x << "," << tempMin.y << "<" << v[0] << "," << v[1] << std::endl;
					}
				}
			}
			std::cout << "ill go here: " << tempMin.x << "," << tempMin.y << std::endl;
			v[0] = tempMin.x;
			v[1] = tempMin.y;
			//v[0] = v[0] + dl[i];
			//v[1] = v[1] + dc[i];
		}
		if (v[0] >= 0 && v[0] <= 8 && v[1] >= 0 && v[1] <= 8)//check if within boundaries
			if (!funCheckforWall(sf::Vector2i(pos.x, pos.y), sf::Vector2i(v[0], v[1])))//wall
				if (board[mn.x][mn.y] > board[v[0]][v[1]] && board[v[0]][v[1]] > 1)//minimal path
				{
					
					if (board[pos.x + dl[i]][pos.y + dc[i]] == 2)
					{

						sf::Vector2i tempPawn = pawn[turn];
						pawn[turn] = sf::Vector2i(pos.x + dl[i], pos.y + dc[i]);
						mn.x = v[0];
						mn.y = v[1];
						foundPath = true;
					}
					else
					{
						mn.x = v[0];
						mn.y = v[1];
						foundPath = true;
					}
		}
			
	}
	
	if (foundPath)
	{
		std::cout << mn.x << " " << mn.y << std::endl;
		return funReverseLee(board, mn);
	}
		
	else
		return sf::Vector2i(-1, -1);
}
sf::Vector2i funAImove(int turn)
{
	sf::Vector2i oldPawn = pawn[turn];
	sf::Vector2i nextPawn;
	int dl[4] = { -1, 0, 1, 0 };
	int dc[4] = { 0, 1, 0, -1 };
	int prim, ultim, p[2], v[2], c[81][2];

	int board[9][9];
	c[0][0] = pawn[turn].x;
	c[0][1] = pawn[turn].y;
	prim = ultim = 0;

	for (int i = 0; i <= 8; i++)
		for (int j = 0; j <= 8; j++)
			board[i][j] = 0;
	for (int i = 0; i <= nrOfPlayers - 1; i++)
		board[pawn[i].x][pawn[i].y] = -1;
	board[pawn[turn].x][pawn[turn].y] = 1;

	while (prim <= ultim)
	{
		p[0] = c[prim][0];
		p[1] = c[prim][1];
		prim++;

		for (int k = 0; k <= 3; k++)
		{
			v[0] = p[0] + dl[k];
			v[1] = p[1] + dc[k];

			if (board[v[0]][v[1]] == -1 && !funCheckforWall(sf::Vector2i(v[0] + dl[k], v[1] + dc[k]), sf::Vector2i(v[0], v[1])) && !funCheckforWall(sf::Vector2i(p[0], p[1]), sf::Vector2i(v[0], v[1])))
			{
				v[0] = v[0] + dl[k];
				v[1] = v[1] + dc[k];
			}
			if (board[v[0]][v[1]] == 0 && v[0] >= 0 && v[0] <= 8 && v[1] >= 0 && v[1] <= 8 && !funCheckforWall(sf::Vector2i(p[0], p[1]), sf::Vector2i(v[0], v[1])))
			{
				board[v[0]][v[1]] = board[p[0]][p[1]] + 1;

				ultim++;
				c[ultim][0] = v[0];
				c[ultim][1] = v[1];
				if (turn == 0 || turn == 1)
					if (v[0] == (1 - turn) * 8)
					{
						for (int qwe = 0; qwe <= 8; qwe++) {
							for (int rty = 0; rty <= 8; rty++) {
								std::cout << board[qwe][rty] << " ";
							}
							std::cout << std::endl;
						}
						sf::Vector2i tempPawn = funReverseLee(board, sf::Vector2i(v[0], v[1]));
						if (tempPawn.x >= 0)
							return funReverseLee(board, sf::Vector2i(v[0], v[1]));
						else
							return oldPawn;
					}

				if (turn == 2 || turn == 3)
					if (v[1] == (3 - turn) * 8)
					{
						for (int qwe = 0; qwe <= 8; qwe++) {
							for (int rty = 0; rty <= 8; rty++) {
								std::cout << board[qwe][rty] << " ";
							}
							std::cout << std::endl;
						}
						sf::Vector2i tempPawn = funReverseLee(board, sf::Vector2i(v[0], v[1]));
						if (tempPawn.x >= 0)
							return funReverseLee(board, sf::Vector2i(v[0], v[1]));
						else
							return oldPawn;
					}
			}
			//side jump
			else if (board[p[0] + dl[k]][p[1] + dc[k]] == -1 && funCheckforWall(sf::Vector2i(v[0] + dl[k], v[1] + dc[k]), sf::Vector2i(v[0], v[1])) && !funCheckforWall(sf::Vector2i(p[0], p[1]), sf::Vector2i(p[0] + dl[k], p[1] + dc[k])))
			{
				std::cout << "im doing a side jump from "<< p[0] <<","<< p[1]<<" above " << p[0] + dl[k] <<","<< p[1] + dc[k] << std::endl;
				for (int i = -1; i <= 1; i += 2)
				{
					v[0] = p[0] + i * (dl[k] == 0) + dl[k] * (dl[k] != 0);
					v[1] = p[1] + i * (dc[k] == 0) + dc[k] * (dc[k] != 0);

					if(board[v[0]][v[1]] == 0 && v[0] >= 0 && v[0] <= 8 && v[1] >= 0 && v[1] <= 8 && !funCheckforWall(sf::Vector2i(v[0], v[1]), sf::Vector2i(p[0] + dl[k], p[1] + dc[k])))
					{
						board[v[0]][v[1]] = board[p[0]][p[1]] + 1;

						ultim++;
						c[ultim][0] = v[0];
						c[ultim][1] = v[1];
						if (turn == 0 || turn == 1)
							if (v[0] == (1 - turn) * 8)
							{
								for (int qwe = 0; qwe <= 8; qwe++) {
									for (int rty = 0; rty <= 8; rty++) {
										std::cout << board[qwe][rty] << " ";
									}
									std::cout << std::endl;
								}
								sf::Vector2i tempPawn = funReverseLee(board, sf::Vector2i(v[0], v[1]));
								if (tempPawn.x >= 0)
									return funReverseLee(board, sf::Vector2i(v[0], v[1]));
								else
									return oldPawn;
							}

						if (turn == 2 || turn == 3)
							if (v[1] == (3 - turn) * 8)
							{
								for (int qwe = 0; qwe <= 8; qwe++) {
									for (int rty = 0; rty <= 8; rty++) {
										std::cout << board[qwe][rty] << " ";
									}
									std::cout << std::endl;
								}
								sf::Vector2i tempPawn = funReverseLee(board, sf::Vector2i(v[0], v[1]));
								if (tempPawn.x >= 0)
									return funReverseLee(board, sf::Vector2i(v[0], v[1]));
								else
									return oldPawn;
							}
					}
					
				}
			}
		}

	}
	return oldPawn;
}

///// NORMAL COMPUTER CALCULATES WITH LEE TO PUT AN WALL
/*int funAIcheckForPlacingWall(int turn)
{
	sf::Vector2i oldPawn = pawn[turn];
	sf::Vector2i nextPawn;
	int dl[4] = { -1, 0, 1, 0 };
	int dc[4] = { 0, 1, 0, -1 };
	int prim, ultim, p[2], v[2], c[81][2];

	int board[9][9];
	c[0][0] = pawn[turn].x;
	c[0][1] = pawn[turn].y;
	prim = ultim = 0;

	for (int i = 0; i <= 8; i++)
		for (int j = 0; j <= 8; j++)
			board[i][j] = 0;
	for (int i = 0; i <= nrOfPlayers - 1; i++)
		board[pawn[i].x][pawn[i].y] = 1;

	while (prim <= ultim)
	{
		p[0] = c[prim][0];
		p[1] = c[prim][1];
		prim++;

		for (int k = 0; k <= 3; k++)
		{
			v[0] = p[0] + dl[k];
			v[1] = p[1] + dc[k];

			if (board[v[0]][v[1]] == 0 && v[0] >= 0 && v[0] <= 8 && v[1] >= 0 && v[1] <= 8 && !funCheckforWall(sf::Vector2i(p[0], p[1]), sf::Vector2i(v[0], v[1])))
			{
				board[v[0]][v[1]] = board[p[0]][p[1]] + 1;

				ultim++;
				c[ultim][0] = v[0];
				c[ultim][1] = v[1];
				if (turn == 0 || turn == 1)
					if (v[0] == (1 - turn) * 8)
					{
						return board[v[0]][v[1]];
					}

				if (turn == 2 || turn == 3)
					if (v[1] == (3 - turn) * 8)
					{
						return board[v[0]][v[1]];
					}
			}

		}

	}
	return -1;
}
*/
void nextTurn()
{
	countDown--;
	countDown2--;
	countDown3--;
	if (countDown == 0) {
		randPos.y = (powerUpPos.x - marginWidth) / sizeTotal;
		randPos.x = (powerUpPos.y - widthWall) / sizeTotal;
		powerMatrix[randPos.x - 3][randPos.y - 3] = 0;
	}
	else
		if (countDown2 == 0) {
			randPos.y = (powerUpPos2.x - marginWidth) / sizeTotal;
			randPos.x = (powerUpPos2.y - widthWall) / sizeTotal;
			powerMatrix[randPos.x - 3][randPos.y - 3] = 0;
		}
		else
			if (countDown3 == 0) {
				randPos.y = (powerUpPos3.x - marginWidth) / sizeTotal;
				randPos.x = (powerUpPos3.y - widthWall) / sizeTotal;
				powerMatrix[randPos.x - 3][randPos.y - 3] = 0;
			}

	if (threeMoves > 0 && playerType[turn] > 0 && waitingforAI == 1)
	{
		threeMoves -= 1;
		std::cout << "doint another move" <<threeMoves<< std::endl;
	}
		

	if(threeMoves == 0)
		if (waitingforAI != 1)
		{
			turn = (turn + 1) % nrOfPlayers;
			turnUI.setTexture(*sPawn[turn].getTexture());
		}
		
		
	if (waitingforAI == 0 && playerType[turn] > 0)
	{
		waitingforAI = 500;
	}
	else if (waitingforAI == 1)
	{
		//Check if next turn is AI
		if (playerType[turn] > 0)
		{

			sf::Vector2i nextPawn = funAImove(turn);
			pawn[turn].x = nextPawn.x;
			pawn[turn].y = nextPawn.y;


			sf::Vector2i posPawn = sf::Vector2i(155 + 76 * pawn[turn].y, 15 + 76 * pawn[turn].x);
			sPawn[turn].setPosition(posPawn.x, posPawn.y);

			//stepped on power-up
			if (posPawn.x == powerUpPos.x && posPawn.y == powerUpPos.y)
			{
				threeMoves = 3;
				countDown = 3;
				powerUpPos.x = -100;
				powerUpPos.y = -100;
				spowerUp.setPosition(powerUpPos.x, powerUpPos.y);
			}
			if (posPawn.x == powerUpPos2.x && posPawn.y == powerUpPos2.y)
			{
				maxWallsPerPlayer[turn] = maxNumber;
				powerUpPos2.x = -100;
				powerUpPos2.y = -100;
				spowerUp2.setPosition(powerUpPos2.x, powerUpPos2.y);
				countDown2 = 0;
			}
			if (posPawn.x == powerUpPos3.x && posPawn.y == powerUpPos3.y)
			{
				funDeleteWalls();
				powerUpPos3.x = -100;
				powerUpPos3.y = -100;
				spowerUp3.setPosition(powerUpPos3.x, powerUpPos3.y);
				countDown3 = 0;
			}

			waitingforAI = 0;
			funCheckWinCondition();
			if (winner == 0)
				nextTurn();
		}
	}
		
}

int main()
{
	sf::Texture tMusicOn, tMusicOn1, tMusicOff, tMusicOff1;
	tMusicOn.loadFromFile("images/musicOn.png");
	tMusicOn1.loadFromFile("images/musicOn1.png");
	tMusicOff.loadFromFile("images/musicOff.png");
	tMusicOff1.loadFromFile("images/musicOff1.png");

	sf::Sprite sMusicButton;
	sMusicButton.setTexture(tMusicOn);
	sMusicButton.setPosition(800, 50);

	sf::Image icon;
	icon.loadFromFile("images/QuoridorPawn0.png");
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	sf::Clock clock;
	sf::Time timer;

	#pragma region FILE_LOADING
	// Add music
	sf::Music MenuMusic;
	if (!MenuMusic.openFromFile("music/MenuMusic.ogg"))
		return 0;
	MenuMusic.setLoop(true);

	
	////// POWER UPS
	sf::Texture tpowerUp, tpowerUp2, tpowerUp3;
	tpowerUp.loadFromFile("images/powerUp.png");
	tpowerUp2.loadFromFile("images/powerUp2.png");
	tpowerUp3.loadFromFile("images/powerUp3.png");
	spowerUp.setTexture(tpowerUp);
	spowerUp2.setTexture(tpowerUp2);
	spowerUp3.setTexture(tpowerUp3);
	powerUpPos.x = -100;
	powerUpPos.y = -100;
	spowerUp.setPosition(powerUpPos.x, powerUpPos.y);
	powerUpPos2.x = -100;
	powerUpPos2.y = -100;
	spowerUp2.setPosition(powerUpPos2.x, powerUpPos2.y);
	powerUpPos3.x = -100;
	powerUpPos3.y = -100;
	spowerUp3.setPosition(powerUpPos3.x, powerUpPos3.y);

	turnUI.setPosition(43, 80);

	for (int i = 0; i <= 3; i++)
		playerSkin[i] = i;

	sf::Texture tBoard;
	sf::Texture tPawn[11];
	sf::Texture tWall;

	///// Menu
	sf::Texture tMenuBackground, tButtonClassic, tButtonWild, tButtonQuit, tButtonClassic1, tButtonWild1, tButtonQuit1, tTurnDisplay,tTitle;
	tMenuBackground.loadFromFile("images/MenuBackgroundTest.png");
	tButtonClassic.loadFromFile("images/Classic1.png");
	tButtonWild.loadFromFile("images/Wild1.png");
	tButtonQuit.loadFromFile("images/Quit1.png");
	tButtonClassic1.loadFromFile("images/Classic2.png");
	tButtonWild1.loadFromFile("images/Wild2.png");
	tButtonQuit1.loadFromFile("images/Quit2.png");
	tTurnDisplay.loadFromFile("images/PlayerTurnDisplay.png");
	tTitle.loadFromFile("images/title.png");

	sf::Sprite sMenuBackground, sButtonClassic, sButtonWild, sButtonQuit, sTitle;
	sMenuBackground.setTexture(tMenuBackground);
	sMenuBackground.setTextureRect(sf::IntRect(0, 0, 969, 696));
	sButtonClassic.setTexture(tButtonClassic);
	sButtonQuit.setTexture(tButtonQuit);
	sButtonWild.setTexture(tButtonWild);
	sButtonClassic.setPosition(410, 300);
	sButtonWild.setPosition(410, 360);
	sButtonQuit.setPosition(410, 420);
	sTurnDisplay.setTexture(tTurnDisplay);
	sTurnDisplay.setPosition(19, 70);
	sTitle.setTexture(tTitle);
	sTitle.setPosition(140, 100);

	///// MENU TRICKS
	sf::Texture tStateClassic, tStateWild, tStateHelp;
	sf::Sprite sStateClassic, sStateWild, sStateHelp;
	tStateClassic.loadFromFile("images/stateClassic.png");
	tStateWild.loadFromFile("images/stateWild.png");
	tStateHelp.loadFromFile("images/stateHelp.png");
	sStateClassic.setTexture(tStateClassic);
	sStateWild.setTexture(tStateWild);
	sStateHelp.setTexture(tStateHelp);
	sStateClassic.setPosition(410, 180);
	sStateHelp.setPosition(410, 180);
	sStateWild.setPosition(410, 180);
	sf::Text textRef,textRef2,textRef3;
	sf::Font textFont;
	textFont.loadFromFile("textFont/Dalila Oblique.ttf");
	textRef.setFont(textFont);
	textRef.setFillColor(sf::Color::White);
	textRef.setPosition(50,650);
	textRef2.setFont(textFont);
	textRef2.setFillColor(sf::Color::Black);
	textRef2.setPosition(48, 652);
	textRef3.setFont(textFont);
	textRef3.setFillColor(sf::Color::Black);
	textRef3.setPosition(52, 648);
	std::string textChars = "Created by: Dominteanu Ionut - Lucian & Marian Alexandru - Daniel";
	textRef.setString(textChars);
	textRef2.setString(textChars);
	textRef3.setString(textChars);
	textRef.setCharacterSize(30);
	textRef2.setCharacterSize(30);
	textRef3.setCharacterSize(30);


	///// HELP MENU
	sf::Sprite sPrev, sUrm, sHelpButton, sHelpBackground,go_back;
	sf::Texture tPrev, tPrev1, tUrm, tUrm1, tHelpButton, tHelpButton1, tHB[8], tgo_back, tgo_back1;
	tgo_back.loadFromFile("images/close_help.png");
	tgo_back1.loadFromFile("images/close_help1.png");
	tPrev.loadFromFile("images/prev.png");
	tPrev1.loadFromFile("images/prev1.png");
	tUrm.loadFromFile("images/urm.png");
	tUrm1.loadFromFile("images/urm1.png");
	tHelpButton.loadFromFile("images/help.png");
	tHelpButton1.loadFromFile("images/help1.png");
	tHB[0].loadFromFile("images/helpBackground1.jpg");
	tHB[1].loadFromFile("images/helpBackground2.jpg");
	tHB[2].loadFromFile("images/helpBackground3.jpg");
	tHB[3].loadFromFile("images/helpBackground4.jpg");
	tHB[4].loadFromFile("images/helpBackground5.jpg");
	tHB[5].loadFromFile("images/helpBackground6.jpg");
	tHB[6].loadFromFile("images/helpBackground7.jpg");
	tHB[7].loadFromFile("images/helpBackground8.jpg");
	sPrev.setTexture(tPrev);
	sUrm.setTexture(tUrm);
	sHelpButton.setTexture(tHelpButton);
	sHelpButton.setPosition(869, 50);
	sPrev.setPosition(350, 570);
	sUrm.setPosition(570, 570);
	sHelpBackground.setTexture(tHB[0]);
	go_back.setTexture(tgo_back);
	go_back.setPosition(460, 570);
	
	///// SECOND MENU
	sf::Sprite sSetPlayerBackground[4], sAdd, sRemove, sStart, sNext[8], sPrevious[8], sState[4][3], sSkin[4][11];
	sf::Texture tSPB1, tSPB2, tSPB3, tSPB4, tAdd, tRemove, tAdd1, tRemove1, tBack, tBack1, tStart, tStart1, tNext, tPrevious, tState[3], tNext1, tPrevious1;
	tSPB1.loadFromFile("images/SetPlayer1.png");
	tSPB2.loadFromFile("images/SetPlayer2.png");
	tSPB3.loadFromFile("images/SetPlayer3.png");
	tSPB4.loadFromFile("images/SetPlayer4.png");
	tAdd.loadFromFile("images/add.png");
	tAdd1.loadFromFile("images/add1.png");
	tRemove.loadFromFile("images/remove.png");
	tRemove1.loadFromFile("images/remove1.png");
	tBack.loadFromFile("images/back.png");
	tBack1.loadFromFile("images/back1.png");
	tStart.loadFromFile("images/start.png");
	tStart1.loadFromFile("images/start1.png");
	tNext.loadFromFile("images/next.png");
	tPrevious.loadFromFile("images/previous.png");
	tNext1.loadFromFile("images/next1.png");
	tPrevious1.loadFromFile("images/previous1.png");
	tState[0].loadFromFile("images/PlayerState1.png");
	tState[1].loadFromFile("images/PlayerState2.png");
	tState[2].loadFromFile("images/PlayerState3.png");

	sSetPlayerBackground[0].setTexture(tSPB1);
	sSetPlayerBackground[0].setPosition(135, 50);
	sSetPlayerBackground[1].setTexture(tSPB2);
	sSetPlayerBackground[1].setPosition(135, 180);
	sSetPlayerBackground[2].setTexture(tSPB3);
	sSetPlayerBackground[2].setPosition(135, 310);
	sSetPlayerBackground[3].setTexture(tSPB4);
	sSetPlayerBackground[3].setPosition(135, 440);
	sBack.setTexture(tBack);
	sBack.setPosition(462, 570);
	sAdd.setTexture(tAdd);
	sAdd.setPosition(381, 570);
	sRemove.setTexture(tRemove);
	sRemove.setPosition(543, 570);
	sStart.setTexture(tStart);
	sStart.setPosition(425, 630);

	tPawn[0].loadFromFile("images/QuoridorPawn0.png");
	tPawn[1].loadFromFile("images/QuoridorPawn1.png");
	tPawn[2].loadFromFile("images/QuoridorPawn2.png");
	tPawn[3].loadFromFile("images/QuoridorPawn3.png");
	tPawn[4].loadFromFile("images/QuoridorPawn4.png");
	tPawn[5].loadFromFile("images/QuoridorPawn5.png");
	tPawn[6].loadFromFile("images/QuoridorPawn6.png");
	tPawn[7].loadFromFile("images/QuoridorPawn7.png");
	tPawn[8].loadFromFile("images/QuoridorPawn8.png");
	tPawn[9].loadFromFile("images/QuoridorPawn9.png");
	tPawn[10].loadFromFile("images/QuoridorPawn10.png");

	for (int i = 0; i <= 3; i++)
		for (int j = 0; j <= 10; j++)
		{
			sSkin[i][j].setTexture(tPawn[j]);
			sSkin[i][j].setPosition(696, 72 + i * 130);
		}

	for (int i = 0; i <= 3; i++)
		for (int j = 0; j <= 2; j++)
		{
			sState[i][j].setTexture(tState[j]);
			sState[i][j].setPosition(389, 50 + i * 130);
		}
	for (int i = 0; i <= 3; i++)
	{
		sNext[i].setTexture(tNext);
		sPrevious[i].setTexture(tPrevious);
		sNext[i].setPosition(583, 86 + i * 130);
		sPrevious[i].setPosition(395, 86 + i * 130);
	}
	for (int i = 4; i <= 7; i++)
	{
		sNext[i].setTexture(tNext);
		sPrevious[i].setTexture(tPrevious);
		sNext[i].setPosition(806, 86 + (i - 4) * 130);
		sPrevious[i].setPosition(620, 86 + (i - 4) * 130);
	}

	////// [images]Display number of walls for each player /////
	sf::Texture tPlayerWalls;
	tPlayerWalls.loadFromFile("images/Walls.png");

	int WallsPlaceableLimit;


	int wallRotation = 0;
	bool canWallBePlaced = false;
	

	//buttons
	sf::Texture tButtonWall1, tButtonWall2;
	sf::Texture tExit1, tExit2;
	sf::Vector2i nextPawn[4], posPawn[4];


	////////// WINNER ///////////
	tWinPlayer.loadFromFile("images/PlayerWin.png");
	tWinComputer.loadFromFile("images/ComputerWin.png");

	bool isExit = false, isMove = false;
	bool isWallPlaceable = false;
	bool wallOrientation = false;
	bool JustOneWall = false;

	tBoard.loadFromFile("images/QuoridorBoard1.png");
	sBoard.setTexture(tBoard);

	tWall.loadFromFile("images/QuoridorWall.png");

	nrOfPlacedWalls = 0;
	WallsPlaceableLimit = 40;

	for (int i = 0; i <= 10; i++) 
	{
		sPlayerWalls[i].setTexture(tPlayerWalls);
		sPlayerWalls[i].setTextureRect(sf::IntRect(i * 99, 0, 99, 83));
		sPlayerWalls[i].setPosition(20, 305);
	}

	//wall activate button

	tButtonWall1.loadFromFile("images/ButtonWall1.png");
	tButtonWall2.loadFromFile("images/ButtonWall2.png");

	sButtonWall1.setTexture(tButtonWall1);
	isWallPlaceable = false;
	wallOrientation = false;
	JustOneWall = false;
	sButtonWall1.setPosition(19, 25);

	//exit button
	isExit = false;
	tExit1.loadFromFile("images/Exit1.png");
	tExit2.loadFromFile("images/Exit2.png");

	sExit1.setTexture(tExit1);
	sExit1.setPosition(850, 25);

	isMove = false;

	#pragma endregion FILE_LOADING
	turnUI.setPosition(460, 210);
	int pos_x = 0,pos_y = 0,dir_x = 1,dir_y = 1;
	float showTime;
	while (window.isOpen())
	{
		timer = clock.getElapsedTime();
		showTime = timer.asSeconds();
		if (showTime > 0.02)
		{
			pos_x = pos_x + dir_x;
			pos_y = pos_y + dir_y;
			clock.restart();
			if (pos_x > 600 || pos_x < 0)
			{
				dir_x = -1 * dir_x;
				pos_x = pos_x + dir_x;
			}
			if (pos_y > 430 || pos_y < 0)
			{
				dir_y = -1 * dir_y;
				pos_y = pos_y + dir_y;
			}
		}
		
		sMenuBackground.setTextureRect(sf::IntRect(pos_x, pos_y, 969, 696));

		if (gameStatus == 0 && musicOn == false && !musicIsPaused)
		{
			MenuMusic.play();
			musicOn = true;
		}
		int AIsTime = 0;
		sf::Vector2i posMouse = sf::Mouse::getPosition(window);
		sf::Event e;

		if (gameStatus == 2)
		{
			turn = -1;
			threeMoves = 0;
			waitingforAI = 0;

			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
					powerMatrix[i][j] = 0;
			for (int owo = 0; owo < 3; owo++) {
				for (int wow = 0; wow < 3; wow++) {
					std::cout << powerMatrix[owo][wow] << " ";
				}
				std::cout << std::endl;
			}
			MenuMusic.stop();
			musicOn = false;
			powerUpPos.x = -100;
			powerUpPos.y = -100;
			spowerUp.setPosition(powerUpPos.x, powerUpPos.y);
			powerUpPos2.x = -100;
			powerUpPos2.y = -100;
			spowerUp2.setPosition(powerUpPos2.x, powerUpPos2.y);
			powerUpPos3.x = -100;
			powerUpPos3.y = -100;
			spowerUp3.setPosition(powerUpPos3.x, powerUpPos3.y);
			if (setMode) {
				countDown = 4;
				countDown2 = 5;
				countDown3 = 6;
				//Reset random numbers
				srand(time(NULL));
			}
			turnUI.setPosition(43, 80);
			for (int i = 0; i < nrOfPlayers; i++)
				sPawn[i].setTexture(tPawn[playerSkin[i]]);
			
			for (int i = 0; i < 17; i++)
				for (int j = 0; j < 9; j++)
					wallMatrix[i][j] = 0;
			for (int i = 0; i < 8; i++)
				for (int j = 0; j < 8; j++)
					crossWalls[i][j] = 0;
			winner = 0;
			isMove = false;
			isWallPlaceable = false;
			wallOrientation = false;
			JustOneWall = false;
			nrOfPlacedWalls = 0;
			if (!setMode)
				WallsPlaceableLimit = 40;
			else
				WallsPlaceableLimit = 80;
			pawn[0] = sf::Vector2i(0, 4);//blue
			pawn[1] = sf::Vector2i(8, 4);//green
			pawn[2] = sf::Vector2i(4, 0);//yellow
			pawn[3] = sf::Vector2i(4, 8);//red
			for (int i = 0; i <= nrOfPlayers - 1; i++)
				sPawn[i].setPosition(marginWidth + sizeTotal*pawn[i].y, widthWall + sizeTotal*pawn[i].x);
			//// SET WALLS FOR PLAYERS ////
			if (nrOfPlayers == 4)
				maxNumber = 5;
			else
				if (nrOfPlayers == 3)
					maxNumber = 7;
				else
					maxNumber = 10;
			for (int i = 0; i < nrOfPlayers; i++)
			{
				maxWallsPerPlayer[i] = maxNumber;
			}
			gameStatus = 3;
			sBack.setPosition(872, 90);
			nextTurn();
			
		}
		if (setMode)
		{
			if (countDown <= 0)
			{
				randPos = powerUP();
				while (powerMatrix[randPos.x - 3][randPos.y - 3] != 0)
					randPos = powerUP();
				powerMatrix[randPos.x - 3][randPos.y - 3] = 1;

				powerUpPos.x = marginWidth + sizeTotal*randPos.y;
				powerUpPos.y = widthWall + sizeTotal*randPos.x;
				spowerUp.setPosition(powerUpPos.x, powerUpPos.y);
				countDown = 20;
				for (int owo = 0; owo < 3; owo++) {
					for (int wow = 0; wow < 3; wow++) {
						std::cout << powerMatrix[owo][wow] << " ";
					}
					std::cout << std::endl;
				}
			}
			if (countDown2 <= 0)
			{
				randPos = powerUP();
				while (powerMatrix[randPos.x - 3][randPos.y - 3] != 0)
					randPos = powerUP();
				powerMatrix[randPos.x - 3][randPos.y - 3] = 1;

				powerUpPos2.x = marginWidth + sizeTotal*randPos.y;
				powerUpPos2.y = widthWall + sizeTotal*randPos.x;
				spowerUp2.setPosition(powerUpPos2.x, powerUpPos2.y);
				countDown2 = 15;
				for (int owo = 0; owo < 3; owo++) {
					for (int wow = 0; wow < 3; wow++) {
						std::cout << powerMatrix[owo][wow] << " ";
					}
					std::cout << std::endl;
				}
			}
			if (countDown3 <= 0)
			{
				randPos = powerUP();
				while (powerMatrix[randPos.x - 3][randPos.y - 3] != 0)
					randPos = powerUP();
				powerMatrix[randPos.x - 3][randPos.y - 3] = 1;

				powerUpPos3.x = marginWidth + sizeTotal*randPos.y;
				powerUpPos3.y = widthWall + sizeTotal*randPos.x;
				spowerUp3.setPosition(powerUpPos3.x, powerUpPos3.y);
				countDown3 = 18;
				for (int owo = 0; owo < 3; owo++) {
					for (int wow = 0; wow < 3; wow++) {
						std::cout << powerMatrix[owo][wow] << " ";
					}
					std::cout << std::endl;
				}
			}
		}


		while (window.pollEvent(e) && AIsTime == 0)
		{
			if (e.type == sf::Event::Closed)
				window.close();

			// MENU
			if (gameStatus == 0) 
			{
				///// CLOSE MENU
				if (sButtonClassic.getGlobalBounds().contains(posMouse.x, posMouse.y))
					drawSclassic = true;
				else
					drawSclassic = false;

				if (sButtonWild.getGlobalBounds().contains(posMouse.x, posMouse.y))
					drawSwild = true;
				else
					drawSwild = false;

				if (sHelpButton.getGlobalBounds().contains(posMouse.x, posMouse.y))
					drawShelp = true;
				else
					drawShelp = false;

				if (e.type == sf::Event::MouseButtonPressed)
					if (e.key.code == sf::Mouse::Left)
						if (sButtonQuit.getGlobalBounds().contains(posMouse.x, posMouse.y)) 
						{
							sButtonQuit.setTexture(tButtonQuit1);
							isExit = true;
						}
				if (e.type == sf::Event::MouseButtonReleased)
					if (e.key.code == sf::Mouse::Left && isExit)
						window.close();

				//// GO TO NEXT WINDOW FROM MENU
				if (e.type == sf::Event::MouseButtonPressed)
					if (e.key.code == sf::Mouse::Left)
						if (sButtonClassic.getGlobalBounds().contains(posMouse.x, posMouse.y)) 
						{
							sButtonClassic.setTexture(tButtonClassic1);
							buttonState = 1;
							setMode = false;
						}
						else
							if (sButtonWild.getGlobalBounds().contains(posMouse.x, posMouse.y)) 
							{
								sButtonWild.setTexture(tButtonWild1);
								buttonState = 1;
								setMode = true;
							}
							else
								if (sHelpButton.getGlobalBounds().contains(posMouse.x, posMouse.y)) 
								{
									sHelpButton.setTexture(tHelpButton1);
									helpMode = true;
								}
								else
									if (sMusicButton.getGlobalBounds().contains(posMouse.x, posMouse.y) && !musicIsPaused)
									{
										sMusicButton.setTexture(tMusicOn1);
										musicState = true;
									}
									else
										if (sMusicButton.getGlobalBounds().contains(posMouse.x, posMouse.y) && musicIsPaused)
										{
											sMusicButton.setTexture(tMusicOff1);
											musicState = true;
										}

				if (e.type == sf::Event::MouseButtonReleased)
					if (e.key.code == sf::Mouse::Left)
						if (buttonState)
						{
							gameStatus = 1;
							sButtonClassic.setTexture(tButtonClassic);
							sButtonWild.setTexture(tButtonWild);
							buttonState = 0;
						}
						else
							if (helpMode)
							{
								gameStatus = 4;
								sHelpButton.setTexture(tHelpButton);
								helpMode = false;
							}
							else
								if (musicState && !musicIsPaused)
								{
									sMusicButton.setTexture(tMusicOff);
									musicState = false;
									MenuMusic.pause();
									musicIsPaused = true;
								}
								else
									if (musicState && musicIsPaused)
									{
										sMusicButton.setTexture(tMusicOn);
										musicState = false;
										MenuMusic.play();
										musicIsPaused = false;
									}
						
			}
			// ROOM
			if (gameStatus == 1)
			{
				drawSclassic = false;
				drawShelp = false;
				drawSwild = false;
				if (e.type == sf::Event::MouseButtonPressed)
					if (e.key.code == sf::Mouse::Left)
						if (sAdd.getGlobalBounds().contains(posMouse.x, posMouse.y)) 
						{
							sAdd.setTexture(tAdd1);
							addWindow = true;
						}
						else
							if (sRemove.getGlobalBounds().contains(posMouse.x, posMouse.y)) 
							{
								sRemove.setTexture(tRemove1);
								removeWindow = true;
							}
							else
								if (sBack.getGlobalBounds().contains(posMouse.x, posMouse.y)) 
								{
									sBack.setTexture(tBack1);
									goBack = true;
								}
								else
									if (sStart.getGlobalBounds().contains(posMouse.x, posMouse.y)) 
									{
										sStart.setTexture(tStart1);
										goStart = true;
									}
				if (e.type == sf::Event::MouseButtonPressed)
					if (e.key.code == sf::Mouse::Left)
						for (int i = 0; i < nrOfPlayers; i++) {
							if (sNext[i].getGlobalBounds().contains(posMouse.x, posMouse.y)) {
								next[i] = true;
								sNext[i].setTexture(tNext1);
							}
							if (sPrevious[i].getGlobalBounds().contains(posMouse.x, posMouse.y)) {
								previous[i] = true;
								sPrevious[i].setTexture(tPrevious1);
							}
						}
				if (e.type == sf::Event::MouseButtonPressed)
					if (e.key.code == sf::Mouse::Left)
						for (int i = 0; i < nrOfPlayers; i++) {
							if (sNext[i + 4].getGlobalBounds().contains(posMouse.x, posMouse.y)) 
							{
								nextSkin[i] = true;
								sNext[i + 4].setTexture(tNext1);
							}
							if (sPrevious[i + 4].getGlobalBounds().contains(posMouse.x, posMouse.y)) 
							{
								previousSkin[i] = true;
								sPrevious[i + 4].setTexture(tPrevious1);
							}
						}
				if (e.type == sf::Event::MouseButtonReleased)
					if (e.key.code == sf::Mouse::Left)
						for (int i = 0; i < nrOfPlayers; i++)
						{
							if (next[i])
							{
								playerType[i] += 1;
								if (playerType[i] > 2)
									playerType[i] = 2;
								next[i] = false;
								sNext[i].setTexture(tNext);
							}
							if (previous[i])
							{
								playerType[i] -= 1;
								if (playerType[i] < 0)
									playerType[i] = 0;
								previous[i] = false;
								sPrevious[i].setTexture(tPrevious);
							}
						}
				if (e.type == sf::Event::MouseButtonReleased)
					if (e.key.code == sf::Mouse::Left)
						for (int i = 0; i < nrOfPlayers; i++)
						{
							if (nextSkin[i])
							{
								playerSkin[i] += 1;
								if (playerSkin[i] > 10)
									playerSkin[i] = 10;
								nextSkin[i] = false;
								sNext[i + 4].setTexture(tNext);
							}
							if (previousSkin[i])
							{
								playerSkin[i] -= 1;
								if (playerSkin[i] < 0)
									playerSkin[i] = 0;
								previousSkin[i] = false;
								sPrevious[i + 4].setTexture(tPrevious);
							}
						}
				if (e.type == sf::Event::MouseButtonReleased)
					if (e.key.code == sf::Mouse::Left)
						if (addWindow)
						{
							nrOfPlayers += 1;
							if (nrOfPlayers > 4)
								nrOfPlayers = 4;
							sAdd.setTexture(tAdd);
							addWindow = false;
						}
						else
							if (removeWindow)
							{
								nrOfPlayers -= 1;
								if (nrOfPlayers < 2)
									nrOfPlayers = 2;
								sRemove.setTexture(tRemove);
								removeWindow = false;
							}
							else
								if (goBack)
								{
									sBack.setTexture(tBack);
									goBack = false;
									bool setMode = false;
									gameStatus = 0;
								}
								else
									if (goStart)
									{
										sStart.setTexture(tStart);
										goStart = false;
										gameStatus = 2;
									}

			}

			///// Game Help Menu
			if (gameStatus == 4) {
				drawSclassic = false;
				drawShelp = false;
				drawSwild = false;
				
				if (e.type == sf::Event::MouseButtonPressed)
					if (e.key.code == sf::Mouse::Left)
						if (sPrev.getGlobalBounds().contains(posMouse.x, posMouse.y))
						{
							goPrev = true;
							sPrev.setTexture(tPrev1);
						}
						else if (sUrm.getGlobalBounds().contains(posMouse.x, posMouse.y))
						{
							goUrm = true;
							sUrm.setTexture(tUrm1);
						}
						else if (go_back.getGlobalBounds().contains(posMouse.x, posMouse.y))
						{
							leaveHelp = true;
							go_back.setTexture(tgo_back1);
						}
				if (e.type == sf::Event::MouseButtonReleased)
					if (e.key.code == sf::Mouse::Left)
						if (goPrev) 
						{
							helpStatus--;
							goPrev = false;
							sPrev.setTexture(tPrev);
							if(helpStatus >= 1)
								sHelpBackground.setTexture(tHB[helpStatus-1]);
						}
						else if (goUrm) 
						{
							helpStatus++;
							goUrm = false;
							sUrm.setTexture(tUrm);
							if(helpStatus <= 8)
								sHelpBackground.setTexture(tHB[helpStatus-1]);
						}
						else if (leaveHelp)
						{
							leaveHelp = false;
							go_back.setTexture(tgo_back);
							gameStatus = 0;
						}
				if (helpStatus > 8)
					helpStatus = 8;
				if (helpStatus == 0)
				{		
					helpStatus = 1;
					gameStatus = 0;
				}
			}

			///// Game is Started !
			if (gameStatus == 3)
			{
				if (waitingforAI == 0)
				{
					////// NEED POWER UPS ???
					

					if (e.type == sf::Event::MouseButtonPressed)
						if (e.key.code == sf::Mouse::Left)
							if (sExit1.getGlobalBounds().contains(posMouse.x, posMouse.y))
							{
								sExit1.setTexture(tExit2);
								isExit = true;
							}
							else if (sBack.getGlobalBounds().contains(posMouse.x, posMouse.y))
							{
								sBack.setTexture(tBack1);
								goBack = true;
							}
					if (e.type == sf::Event::MouseButtonReleased)
						if (e.key.code == sf::Mouse::Left && isExit)
							window.close();

					if (e.type == sf::Event::MouseButtonReleased)
						if (e.key.code == sf::Mouse::Left)
							if (goBack)
							{
								sBack.setTexture(tBack);
								goBack = false;
								bool setMode = false;
								gameStatus = 0;
								sBack.setPosition(462, 570);
								turnUI.setPosition(-100, -100);
							}

					if (winner == 0)
					{
						if (e.type == sf::Event::MouseButtonPressed)
							if (e.key.code == sf::Mouse::Left)
								if (sPawn[turn].getGlobalBounds().contains(posMouse.x, posMouse.y))
									isMove = true;

						if (e.type == sf::Event::MouseButtonReleased)
							if (e.key.code == sf::Mouse::Left)
								sButtonWall1.setTexture(tButtonWall1);

						////////// PLACE THE WALL ////////  
						posWall.x = (posMouse.x - leftMarginForPlacingWalls) / wallActiveZone;
						posWall.y = (posMouse.y - topMarginForPlacingWalls) / wallActiveZone;
						//Clamp
						if (posWall.x < 0)
							posWall.x = 0;
						else if (posWall.x > 7)
							posWall.x = 7;
						if (posWall.y < 0)
							posWall.y = 0;
						else if (posWall.y > 7)
							posWall.y = 7;

						fixedPosWall.x = 219 + wallActiveZone*posWall.x;
						fixedPosWall.y = 81 + wallActiveZone*posWall.y;
						if (e.type == sf::Event::MouseButtonPressed)
							if (e.key.code == sf::Mouse::Left)
								if (JustOneWall == true && funWallCanBePlaced(wallRotation, posWall))
								{
									//Mark walls in matrix
									if (wallRotation == 0)
									{
										wallMatrix[posWall.y * 2][posWall.x] = 1;
										wallMatrix[posWall.y * 2 + 2][posWall.x] = 1;
									}
									else
									{
										wallMatrix[posWall.y * 2 + 1][posWall.x] = 1;
										wallMatrix[posWall.y * 2 + 1][posWall.x + 1] = 1;
									}


									if (!funBlocksPlayer(0))
									{

										crossWalls[posWall.x][posWall.y] = 1;
										sWalls[nrOfPlacedWalls + 1].setTexture(tWall);
										JustOneWall = false;
										maxWallsPerPlayer[turn] -= 1;
										canWallBePlaced = true;
										nextTurn();
										checkRotation[nrOfPlacedWalls - 1] = wallRotation;
										//std::cout << "This wall is rotate ? " << checkRotation[nrOfPlacedWalls - 1] << " his number is: " << nrOfPlacedWalls - 1 << std::endl;
									}
									else
									{
										if (wallRotation == 0)
										{
											wallMatrix[posWall.y * 2][posWall.x] = 0;
											wallMatrix[posWall.y * 2 + 2][posWall.x] = 0;
										}
										else
										{
											wallMatrix[posWall.y * 2 + 1][posWall.x] = 0;
											wallMatrix[posWall.y * 2 + 1][posWall.x + 1] = 0;
										}
									}
									//debug walls
									for (int i = 0; i <= 16; i++)
									{
										if (i % 2 == 0)
											std::cout << " ";
										for (int j = 0; j <= 7 + i % 2; j++)
											std::cout << wallMatrix[i][j] << " ";
										std::cout << std::endl;
									}
									std::cout << std::endl;

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
									JustOneWall = true;

									wallRotation = 0;
									sWalls[nrOfPlacedWalls - 1].setRotation(90 * wallRotation);

								}

						if (e.type == sf::Event::MouseButtonReleased)
							if (e.key.code == sf::Mouse::Right && isMove == false && !isWallPlaceable && JustOneWall == true)
							{
								nrOfPlacedWalls -= 1;
								JustOneWall = false;
							}
						if (e.type == sf::Event::MouseWheelMoved || (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::R))
							if (JustOneWall)
							{
								wallRotation = (wallRotation + 1) % 2;
								sWalls[nrOfPlacedWalls - 1].setRotation(90 * wallRotation);
							}


						//Move a pawn
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

								sf::Vector2i deltaPawn = sf::Vector2i(pawn[turn].x - nextPawn[turn].x, pawn[turn].y - nextPawn[turn].y);
								sf::Vector2i dirPawn = sf::Vector2i(((deltaPawn.x > 0) - (deltaPawn.x < 0)), ((deltaPawn.y > 0) - (deltaPawn.y < 0)));
								sf::Vector2i adjacentPawn = sf::Vector2i(pawn[turn].x - dirPawn.x, pawn[turn].y - dirPawn.y);

								//placed above another player
								if (isOccupiedByPawn(nextPawn[turn]))
									nextPawn[turn] = pawn[turn];

								//hop above a pawn
								//vertically
								else if (deltaPawn.y == 0 && abs(deltaPawn.x) == 2 && wallMatrix[2 * adjacentPawn.x - dirPawn.x][pawn[turn].y] == 0)
								{
									if (!isOccupiedByPawn(sf::Vector2i(adjacentPawn.x, pawn[turn].y)))
										nextPawn[turn] = pawn[turn];
									//check for wall
									else if (funCheckforWall(pawn[turn], sf::Vector2i(adjacentPawn.x, pawn[turn].y)))
										nextPawn[turn] = pawn[turn];
								}
								//diagonal
								else if (abs(deltaPawn.y) == 1 && abs(deltaPawn.x) == 1)
								{
									if (!((!funCheckforWall(pawn[turn], sf::Vector2i(adjacentPawn.x, pawn[turn].y)) && !funCheckforWall(nextPawn[turn], sf::Vector2i(adjacentPawn.x, pawn[turn].y)) && isOccupiedByPawn(sf::Vector2i(adjacentPawn.x, pawn[turn].y)) && wallMatrix[2 * adjacentPawn.x - dirPawn.x][pawn[turn].y] != 0) || (!funCheckforWall(pawn[turn], sf::Vector2i(pawn[turn].x, adjacentPawn.y)) && !funCheckforWall(nextPawn[turn], sf::Vector2i(pawn[turn].x, adjacentPawn.y)) && isOccupiedByPawn(sf::Vector2i(pawn[turn].x, adjacentPawn.y)) && wallMatrix[2 * pawn[turn].x][adjacentPawn.y - (dirPawn.y > 0)] != 0)))
										nextPawn[turn] = pawn[turn];
								}
								//horizontally
								else if (deltaPawn.x == 0 && abs(deltaPawn.y) == 2 && wallMatrix[2 * pawn[turn].x][adjacentPawn.y - (dirPawn.y > 0)] == 0)
								{
									if (!isOccupiedByPawn(sf::Vector2i(pawn[turn].x, adjacentPawn.y)))
										nextPawn[turn] = pawn[turn];
									//check for wall
									else if (funCheckforWall(pawn[turn], sf::Vector2i(pawn[turn].x, adjacentPawn.y)))
										nextPawn[turn] = pawn[turn];
								}


								//more than one position
								else if (abs(deltaPawn.x) > 1 || abs(deltaPawn.y) > 1)
									nextPawn[turn] = pawn[turn];
								//diagonal move
								else if (abs(abs(deltaPawn.x) - abs(deltaPawn.y)) != 1)
									nextPawn[turn] = pawn[turn];

								//check for wall
								else if (nextPawn[turn] != pawn[turn])
									if (funCheckforWall(pawn[turn], nextPawn[turn]))
										nextPawn[turn] = pawn[turn];

								if (pawn[turn] != nextPawn[turn])
								{
									sf::Vector2i tempPawn = pawn[turn];
									pawn[turn] = nextPawn[turn];

									//blocks off other players
									/*
									if (funBlocksPlayer(1))
									{
										pawn[turn] = tempPawn;
										posPawn[turn] = sf::Vector2i(marginWidth + sizeTotal*pawn[turn].y, widthWall + sizeTotal*pawn[turn].x);
										sPawn[turn].setPosition(posPawn[turn].x, posPawn[turn].y);
									}
									else
									{*/
									posPawn[turn] = sf::Vector2i(marginWidth + sizeTotal*pawn[turn].y, widthWall + sizeTotal*pawn[turn].x);
									sPawn[turn].setPosition(posPawn[turn].x, posPawn[turn].y);
									if (threeMoves > 0 && playerType[turn] == 0)
										threeMoves -= 1;

									if (setMode)
									{
										if (posPawn[turn].x == powerUpPos.x && posPawn[turn].y == powerUpPos.y)
										{
											randPos.y = (powerUpPos.x - marginWidth) / sizeTotal;
											randPos.x = (powerUpPos.y - widthWall) / sizeTotal;
											powerMatrix[randPos.x - 3][randPos.y - 3] = 0;
											threeMoves = 3;
											countDown = 3;
											powerUpPos.x = -100;
											powerUpPos.y = -100;
											spowerUp.setPosition(powerUpPos.x, powerUpPos.y);
										}
										if (posPawn[turn].x == powerUpPos2.x && posPawn[turn].y == powerUpPos2.y)
										{
											randPos.y = (powerUpPos2.x - marginWidth) / sizeTotal;
											randPos.x = (powerUpPos2.y - widthWall) / sizeTotal;
											powerMatrix[randPos.x - 3][randPos.y - 3] = 0;
											maxWallsPerPlayer[turn] = maxNumber;
											powerUpPos2.x = -100;
											powerUpPos2.y = -100;
											spowerUp2.setPosition(powerUpPos2.x, powerUpPos2.y);
											countDown2 = 0;
										}
										if (posPawn[turn].x == powerUpPos3.x && posPawn[turn].y == powerUpPos3.y)
										{
											randPos.y = (powerUpPos3.x - marginWidth) / sizeTotal;
											randPos.x = (powerUpPos3.y - widthWall) / sizeTotal;
											powerMatrix[randPos.x - 3][randPos.y - 3] = 0;
											funDeleteWalls();
											powerUpPos3.x = -100;
											powerUpPos3.y = -100;
											spowerUp3.setPosition(powerUpPos3.x, powerUpPos3.y);
											countDown3 = 0;
										}
										for (int owo = 0; owo < 3; owo++) {
											for (int wow = 0; wow < 3; wow++) {
												std::cout << powerMatrix[owo][wow] << " ";
											}
											std::cout << std::endl;
										}
									}

									funCheckWinCondition();

									if(winner == 0 && threeMoves == 0)
										nextTurn();
								//}
									
								}
								else
								{
									posPawn[turn] = sf::Vector2i(marginWidth + sizeTotal*pawn[turn].y, widthWall + sizeTotal*pawn[turn].x);
									sPawn[turn].setPosition(posPawn[turn].x, posPawn[turn].y);
								}

							}
					}


				}
				else if (waitingforAI > 0)
				{
					if (e.type == sf::Event::MouseButtonPressed)
						if (e.key.code == sf::Mouse::Left)
							if (sExit1.getGlobalBounds().contains(posMouse.x, posMouse.y))
							{
								sExit1.setTexture(tExit2);
								isExit = true;
							}
							else if (sBack.getGlobalBounds().contains(posMouse.x, posMouse.y))
							{
								sBack.setTexture(tBack1);
								goBack = true;
							}
					if (e.type == sf::Event::MouseButtonReleased)
						if (e.key.code == sf::Mouse::Left && isExit)
							window.close();

					if (e.type == sf::Event::MouseButtonReleased)
						if (e.key.code == sf::Mouse::Left)
							if (goBack)
							{
								sBack.setTexture(tBack);
								goBack = false;
								bool setMode = false;
								gameStatus = 0;
								sBack.setPosition(462, 570);
								turnUI.setPosition(-100, -100);
							}
				}
			}

		}
		if (gameStatus == 3)
		{
			if (waitingforAI > 1)
				waitingforAI -= 1;
			if (waitingforAI == 1)
				nextTurn();
				
			if (JustOneWall)
			{
				sWalls[nrOfPlacedWalls - 1].setPosition(fixedPosWall.x, fixedPosWall.y);
			}
			if (isMove && playerType[turn] == 0)
				sPawn[turn].setPosition(posMouse.x - sPawn[turn].getTextureRect().width / 2, posMouse.y - sPawn[turn].getTextureRect().height / 2);

			window.clear(sf::Color::White);
			window.draw(sBoard);
			window.draw(sTurnDisplay);
			window.draw(sPlayerWalls[maxWallsPerPlayer[turn]]);
			if (setMode)
			{
				window.draw(spowerUp);
				window.draw(spowerUp2);
				window.draw(spowerUp3);
			}
			if (nrOfPlacedWalls > WallsPlaceableLimit)
				nrOfPlacedWalls = WallsPlaceableLimit;
			for (int i = 0; i <= nrOfPlacedWalls - 1; i++)
				window.draw(sWalls[i]);

			window.draw(sButtonWall1);
			window.draw(sExit1);
			window.draw(sBack);
			for (int i = 0; i <= nrOfPlayers - 1; i++)
				window.draw(sPawn[i]);

			if (winner == 1)
				window.draw(sWin);
		}
		else
			if (gameStatus == 0) {
				window.clear(sf::Color::White);
				window.draw(sMenuBackground);
				window.draw(sButtonClassic);
				window.draw(sButtonWild);
				window.draw(sButtonQuit);
				window.draw(sHelpButton);
				window.draw(sTitle);
				window.draw(sMusicButton);
				if (drawSclassic)
					window.draw(sStateClassic);
				if (drawSwild)
					window.draw(sStateWild);
				if (drawShelp)
					window.draw(sStateHelp);
				window.draw(textRef2);
				window.draw(textRef3);
				window.draw(textRef);
			}
			else
				if (gameStatus == 1) 
				{
					window.clear(sf::Color::White);
					window.draw(sMenuBackground);
					for (int i = 0; i < nrOfPlayers; i++)
						window.draw(sSetPlayerBackground[i]);
					window.draw(sAdd);
					window.draw(sRemove);
					window.draw(sBack);
					window.draw(sStart);
					for (int i = 0; i < nrOfPlayers; i++)
						window.draw(sState[i][playerType[i]]);
					for (int i = 0; i < nrOfPlayers; i++)
						window.draw(sSkin[i][playerSkin[i]]);
					for (int i = 0; i < nrOfPlayers; i++)
					{
						window.draw(sNext[i]);
						window.draw(sPrevious[i]);
					}
					for (int i = 4; i < 4 + nrOfPlayers; i++)
					{
						window.draw(sNext[i]);
						window.draw(sPrevious[i]);
					}
				}
				else
					if (gameStatus == 4) {
						window.draw(sHelpBackground);
						window.draw(sPrev);
						window.draw(sUrm);
						window.draw(go_back);
					}
		window.draw(turnUI);
		window.display();
	}
	return 0;
}