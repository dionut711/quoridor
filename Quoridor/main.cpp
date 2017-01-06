#include <SFML\Graphics.hpp>
#include <time.h>
#include <cmath>
#include <iostream>

int gameStatus = 0;
//0 - menu
//1 - game room
//2 - initializing game
//3 - classic game

sf::Vector2i pawn[4];
sf::Sprite sPawn[4];

int turn;
int playerType[4];
int playerSkin[11];
int wallMatrix[17][9];
int nrOfPlayers = 2;
//// DON'T LET WALLS TO COLIDE EACHOTHER ////
int crossWalls[8][8];
/////// DISPLAY PLAYER'S TURN IMAGES ////////
sf::Texture tTurn;
sf::Sprite sTurn;
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

//////// Set The Mode : Classic is 0 , Wild is 1
bool setMode = false;

bool isOccupiedByPawn(sf::Vector2i pos)
{
	for (int i = 0; i <= nrOfPlayers - 1; i++)
		if (pos.x == pawn[i].x && pos.y == pawn[i].y)
			return 1;
	return 0;
}
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

bool funWallCanBePlaced(int wallRotation,sf::Vector2i posWall)
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
			board[pawn[i].x][pawn[i].y] = 1;


		while (prim <= ultim && !foundPath)
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
	int dl[4] = { -1, 0, 1, 0 };
	int dc[4] = { 0, 1, 0, -1 };
	if (board[pos.x][pos.y] == 2)
		return pos;
	sf::Vector2i mn(pos.x, pos.y);
	for (int i = 0; i <= 3; i++)
	{
		if(pos.x + dl[i] >= 0 && pos.x + dl[i] <= 8 && pos.y + dc[i] >= 0 && pos.y + dc[i] <= 8)
			if(!funCheckforWall(sf::Vector2i(pos.x, pos.y),sf::Vector2i(pos.x + dl[i], pos.y + dc[i])))
				if (board[mn.x][mn.y] > board[pos.x + dl[i]][pos.y + dc[i]] && board[pos.x + dl[i]][pos.y + dc[i]] > 1)
				{
					mn.x = pos.x + dl[i];
					mn.y = pos.y + dc[i];
				}
	}
	return funReverseLee(board, mn);
}
sf::Vector2i funAImove(int turn)
{
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
					sf::Vector2i tempPawn = pawn[turn];
					pawn[turn] = sf::Vector2i(v[0],v[1]);
					if (funBlocksPlayer(1))
					{
						pawn[turn] = sf::Vector2i(tempPawn.x, tempPawn.y);
					}
					else
					{
						board[v[0]][v[1]] = board[p[0]][p[1]] + 1;

						ultim++;
						c[ultim][0] = v[0];
						c[ultim][1] = v[1];
						if (turn == 0 || turn == 1)
							if (v[0] == (1 - turn) * 8)
							{
								return funReverseLee(board, sf::Vector2i(v[0], v[1]));
							}

						if (turn == 2 || turn == 3)
							if (v[1] == (3 - turn) * 8)
								return funReverseLee(board, sf::Vector2i(v[0], v[1]));
					}

					
				}
			}

	}
	
}
void nextTurn(int &currentTurn)
{
	currentTurn = (currentTurn + 1) % nrOfPlayers;
	//Check if next turn is AI
	if (playerType[currentTurn] == 1)
	{
		sf::Vector2i nextPawn = funAImove(currentTurn);
		pawn[currentTurn].x = nextPawn.x;
		pawn[currentTurn].y = nextPawn.y;


		sf::Vector2i posPawn = sf::Vector2i(155 + 76*pawn[currentTurn].y, 15 + 76*pawn[currentTurn].x);
		sPawn[currentTurn].setPosition(posPawn.x, posPawn.y);
		nextTurn(currentTurn);
	}
}

int main()
{
	for (int i = 0;i <= 3;i++)
		playerSkin[i] = i;
	sf::RenderWindow window(sf::VideoMode(969, 696), "Game");
	sf::Texture tBoard;
	sf::Sprite sBoard;


	sf::Texture tPawn[11];
	sf::Texture tWall;
	sf::Sprite sWalls[42];

	///// Menu
	sf::Texture tMenuBackground,tButtonClassic,tButtonWild,tButtonQuit,tButtonClassic1,tButtonWild1,tButtonQuit1;
	tMenuBackground.loadFromFile("images/MenuBackground.png");
	tButtonClassic.loadFromFile("images/Classic1.png");
	tButtonWild.loadFromFile("images/Wild1.png");
	tButtonQuit.loadFromFile("images/Quit1.png");
	tButtonClassic1.loadFromFile("images/Classic2.png");
	tButtonWild1.loadFromFile("images/Wild2.png");
	tButtonQuit1.loadFromFile("images/Quit2.png");

	sf::Sprite sMenuBackground, sButtonClassic, sButtonWild, sButtonQuit;
	sMenuBackground.setTexture(tMenuBackground);
	sButtonClassic.setTexture(tButtonClassic);
	sButtonQuit.setTexture(tButtonQuit);
	sButtonWild.setTexture(tButtonWild);
	sButtonClassic.setPosition(410, 300);
	sButtonWild.setPosition(410, 360);
	sButtonQuit.setPosition(410, 420);

	///// SECOND MENU
	sf::Sprite sSetPlayerBackground[4], sAdd, sRemove, sBack, sStart, sNext[8], sPrevious[8], sState[4][3], sSkin[4][11];
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
	
	for (int i = 0;i <= 3;i++)
		for (int j = 0;j <= 10;j++)
		{
			sSkin[i][j].setTexture(tPawn[j]);
			sSkin[i][j].setPosition(696, 72 + i * 130);
		}

	for (int i = 0;i <= 3;i++)
		for (int j = 0;j <= 2;j++)
		{
			sState[i][j].setTexture(tState[j]);
			sState[i][j].setPosition(389, 50 + i * 130);
		}
	for (int i = 0;i <= 3;i++)
	{
		sNext[i].setTexture(tNext);
		sPrevious[i].setTexture(tPrevious);
		sNext[i].setPosition(583, 86 + i * 130);
		sPrevious[i].setPosition(395, 86 + i * 130);
	}
	for (int i = 4;i <= 7;i++)
	{
		sNext[i].setTexture(tNext);
		sPrevious[i].setTexture(tPrevious);
		sNext[i].setPosition(806, 86 + (i-4) * 130);
		sPrevious[i].setPosition(620, 86 + (i-4) * 130);
	}

	////// [images]Display number of walls for each player /////
	sf::Texture tPlayerWalls;
	tPlayerWalls.loadFromFile("images/Walls.png");
	sf::Sprite sPlayerWalls[11];
	int nrOfPlacedWalls, WallsPlaceableLimit;
	sf::Vector2i posWall;
	sf::Vector2i fixedPosWall;
	int leftMarginForPlacingWalls, topMarginForPlacingWalls, wallActiveZone;
	int maxWallsPerPlayer[4];
	int wallRotation = 0;
	bool canWallBePlaced = false;

	int sizeSquare = 61, widthWall = 15, marginWidth = 155;
	int sizeTotal = sizeSquare + widthWall;

	//buttons
	sf::Texture tButtonWall1, tButtonWall2;
	sf::Sprite sButtonWall1;
	sf::Texture tExit1, tExit2;
	sf::Sprite sExit1;

	sf::Vector2i nextPawn[4];
	sf::Vector2i posPawn[4];


	////////// WINNER ///////////
	sf::Texture twin1, twin2, twin3, twin4;
	twin1.loadFromFile("images/BluePawnWin1.png");
	twin2.loadFromFile("images/GreenPawnWin1.png");
	twin3.loadFromFile("images/YellowPawnWin1.png");
	twin4.loadFromFile("images/RedPawnWin1.png");
	sf::Sprite sWin;

	bool isExit = false;
	bool isMove = false;
	bool isWallPlaceable = false;
	bool wallOrientation = false;
	bool JustOneWall = false;

		tBoard.loadFromFile("images/QuoridorBoard1.png");
		sBoard.setTexture(tBoard);

		tWall.loadFromFile("images/QuoridorWall.png");

		nrOfPlacedWalls = 0;
		WallsPlaceableLimit = 40;

		for (int i = 0; i <= 10; i++) {
			sPlayerWalls[i].setTexture(tPlayerWalls);
			sPlayerWalls[i].setTextureRect(sf::IntRect(i * 99, 0, 99, 83));
			sPlayerWalls[i].setPosition(20, 305);
		}

		//////// PLACE THE WALL AT A PERFECT POSITION /////////
		leftMarginForPlacingWalls = 181;
		topMarginForPlacingWalls = 43;
		wallActiveZone = 76;

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

		tTurn.loadFromFile("images/turnMark.png");
		sTurn.setTexture(tTurn);

		isMove = false;

//////////////////// ACTION WHEN WINDOW IS OPEN //////////////////////////
	while (window.isOpen())
	{
			sf::Vector2i posMouse = sf::Mouse::getPosition(window);
			sf::Event e;
			while (window.pollEvent(e))
			{	
				///// PREPARING THE GAME
				if (gameStatus == 2) 
				{
					for (int i = 0;i < nrOfPlayers;i++)
						sPawn[i].setTexture(tPawn[playerSkin[i]]);
					turn = -1;
					
					//playerType[1] = 1;
					//playerType[2] = 1;
					//playerType[3] = 1;
					for (int i = 0;i < 17;i++)
						for (int j = 0;j < 9;j++)
							wallMatrix[i][j] = 0;
					for (int i = 0; i < 8;i++)
						for (int j = 0;j < 8;j++)
							crossWalls[i][j] = 0;
					winner = 0;
					isMove = false;
					isWallPlaceable = false;
					wallOrientation = false;
					JustOneWall = false;
					nrOfPlacedWalls = 0;
					WallsPlaceableLimit = 40;


					pawn[0] = sf::Vector2i(0, 4);//blue
					pawn[1] = sf::Vector2i(8, 4);//green
					pawn[2] = sf::Vector2i(4, 0);//yellow
					pawn[3] = sf::Vector2i(4, 8);//red
					for (int i = 0; i <= nrOfPlayers - 1; i++)
						sPawn[i].setPosition(marginWidth + sizeTotal*pawn[i].y, widthWall + sizeTotal*pawn[i].x);
					sTurn.setPosition(marginWidth + sizeTotal * 4, widthWall);
					//// SET WALLS FOR PLAYERS ////
					for (int i = 0; i < nrOfPlayers; i++) 
					{
						int maxNumber = 10;
						if (nrOfPlayers == 4)
							maxNumber = 10;
						maxWallsPerPlayer[i] = maxNumber;
					}
					nextTurn(turn);
					gameStatus = 3;
					sBack.setPosition(872, 90);
				}

				/////////// CLOSE THE WINDOW /////////////
				if (e.type == sf::Event::Closed)
					window.close();
				if (gameStatus == 0) {
						///// CLOSE MENU
						if (e.type == sf::Event::MouseButtonPressed)
							if (e.key.code == sf::Mouse::Left)
								if (sButtonQuit.getGlobalBounds().contains(posMouse.x, posMouse.y)) {
									sButtonQuit.setTexture(tButtonQuit1);
									isExit = true;
								}
						if (e.type == sf::Event::MouseButtonReleased)
							if (e.key.code == sf::Mouse::Left && isExit)
								window.close();
						//// GO TO NEXT WINDOW FROM MENU
						if (e.type == sf::Event::MouseButtonPressed)
							if (e.key.code == sf::Mouse::Left)
								if (sButtonClassic.getGlobalBounds().contains(posMouse.x, posMouse.y)) {
									sButtonClassic.setTexture(tButtonClassic1);
									buttonState = 1;
									setMode = false;
								}
								else
									if (sButtonWild.getGlobalBounds().contains(posMouse.x, posMouse.y)) {
										sButtonWild.setTexture(tButtonWild1);
										buttonState = 1;
										setMode = true;
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
						}
					////////// SET GAME PLAYERS AND OTHERS
				if (gameStatus == 1)
				{
					if (e.type == sf::Event::MouseButtonPressed)
						if (e.key.code == sf::Mouse::Left)
							if (sAdd.getGlobalBounds().contains(posMouse.x, posMouse.y)) {
								sAdd.setTexture(tAdd1);
								addWindow = true;
							}
							else
								if (sRemove.getGlobalBounds().contains(posMouse.x, posMouse.y)) {
									sRemove.setTexture(tRemove1);
									removeWindow = true;
								}
								else
									if (sBack.getGlobalBounds().contains(posMouse.x, posMouse.y)) {
										sBack.setTexture(tBack1);
										goBack = true;
									}
									else
										if (sStart.getGlobalBounds().contains(posMouse.x, posMouse.y)) {
											sStart.setTexture(tStart1);
											goStart = true;
										}
					if (e.type == sf::Event::MouseButtonPressed)
						if (e.key.code == sf::Mouse::Left)
							for (int i = 0;i < nrOfPlayers;i++) {
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
							for (int i = 0;i < nrOfPlayers;i++) {
								if (sNext[i + 4].getGlobalBounds().contains(posMouse.x, posMouse.y)) {
									nextSkin[i] = true;
									sNext[i + 4].setTexture(tNext1);
								}
								if (sPrevious[i + 4].getGlobalBounds().contains(posMouse.x, posMouse.y)) {
									previousSkin[i] = true;
									sPrevious[i + 4].setTexture(tPrevious1);
								}
							}
					if (e.type == sf::Event::MouseButtonReleased)
						if (e.key.code == sf::Mouse::Left)
							for (int i = 0;i < nrOfPlayers;i++) {
								if (next[i]) {
									playerType[i] += 1;
									if (playerType[i] > 2)
										playerType[i] = 2;
									std::cout << "playerType[" << i << "]= " << playerType[i] << std::endl;
									next[i] = false;
									sNext[i].setTexture(tNext);
								}
								if (previous[i]) {
									playerType[i] -= 1;
									if (playerType[i] < 0)
										playerType[i] = 0;
									std::cout << "playerType[" << i << "]= " << playerType[i] << std::endl;
									previous[i] = false;
									sPrevious[i].setTexture(tPrevious);
								}
							}
					if (e.type == sf::Event::MouseButtonReleased)
						if (e.key.code == sf::Mouse::Left)
							for (int i = 0;i < nrOfPlayers;i++) {
								if (nextSkin[i]) {
									playerSkin[i] += 1;
									if (playerSkin[i] > 10)
										playerSkin[i] = 10;
									nextSkin[i] = false;
									sNext[i + 4].setTexture(tNext);
								}
								if (previousSkin[i]) {
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

					///// Game is Started !
					if (gameStatus == 3)
					{
						sTurn.setPosition(marginWidth + sizeTotal*pawn[turn].y, widthWall + sizeTotal*pawn[turn].x);
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
									gameStatus = 0;
									sBack.setPosition(462, 570);
								}
						/////////////// CHECK FOR THE WIN
						if (pawn[0].x != 8 && pawn[1].x != 0 && pawn[2].y != 8 && pawn[3].y != 0)
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
											nextTurn(turn);
											canWallBePlaced = true;
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
											for (int j = 0; j <= 7 + i%2; j++)
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
										sWalls[nrOfPlacedWalls-1].setRotation(90 * wallRotation);
										
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
										if (funBlocksPlayer(1))
										{
											pawn[turn] = tempPawn;
											posPawn[turn] = sf::Vector2i(marginWidth + sizeTotal*pawn[turn].y, widthWall + sizeTotal*pawn[turn].x);
											sPawn[turn].setPosition(posPawn[turn].x, posPawn[turn].y);
										}
										else
										{
											posPawn[turn] = sf::Vector2i(marginWidth + sizeTotal*pawn[turn].y, widthWall + sizeTotal*pawn[turn].x);
											sPawn[turn].setPosition(posPawn[turn].x, posPawn[turn].y);
											nextTurn(turn);
										}
									}
									else
									{
										posPawn[turn] = sf::Vector2i(marginWidth + sizeTotal*pawn[turn].y, widthWall + sizeTotal*pawn[turn].x);
										sPawn[turn].setPosition(posPawn[turn].x, posPawn[turn].y);
									}

								}
						}
						else
							winner = 1;
					}
				}
			if (gameStatus == 3)
			{
				if (JustOneWall)
					sWalls[nrOfPlacedWalls - 1].setPosition(fixedPosWall.x, fixedPosWall.y);

				if (isMove)
					sPawn[turn].setPosition(posMouse.x - sPawn[turn].getTextureRect().width / 2, posMouse.y - sPawn[turn].getTextureRect().height / 2);

				window.clear(sf::Color::White);
				window.draw(sBoard);
				window.draw(sPlayerWalls[maxWallsPerPlayer[turn]]);

				if (nrOfPlacedWalls > WallsPlaceableLimit)
					nrOfPlacedWalls = WallsPlaceableLimit;
				for (int i = 0; i <= nrOfPlacedWalls - 1; i++)
					window.draw(sWalls[i]);

				//Draw Buttons
				window.draw(sButtonWall1);
				window.draw(sExit1);
				window.draw(sBack);
				//draw players and win display
				for (int i = 0; i <= nrOfPlayers - 1; i++)
					window.draw(sPawn[i]);
				window.draw(sTurn);
				if (winner == 1) {
					if (pawn[0].x == 8)
						sWin.setTexture(twin1);
					else if (pawn[1].x == 0)
						sWin.setTexture(twin2);
					else if (pawn[2].y == 8)
						sWin.setTexture(twin3);
					else if (pawn[3].y == 0)
						sWin.setTexture(twin4);
					winner = 2;
				}
				if (winner == 2)
					window.draw(sWin);
			}
			else
			if (gameStatus == 0) {
				window.clear(sf::Color::White);
				window.draw(sMenuBackground);
				window.draw(sButtonClassic);
				window.draw(sButtonWild);
				window.draw(sButtonQuit);
			}else
				if (gameStatus == 1) {
					window.clear(sf::Color::White);
					window.draw(sMenuBackground);
					for (int i = 0;i < nrOfPlayers;i++)
						window.draw(sSetPlayerBackground[i]);
					window.draw(sAdd);
					window.draw(sRemove);
					window.draw(sBack);
					window.draw(sStart);
					for (int i = 0;i < nrOfPlayers;i++)
						window.draw(sState[i][playerType[i]]);
					for (int i = 0; i < nrOfPlayers;i++)
						window.draw(sSkin[i][playerSkin[i]]);
					for (int i = 0;i < nrOfPlayers;i++)
					{
						window.draw(sNext[i]);
						window.draw(sPrevious[i]);
					}
					for (int i = 4;i < 4 + nrOfPlayers;i++)
					{
						window.draw(sNext[i]);
						window.draw(sPrevious[i]);
					}
				}
				window.display();
	}
	return 0;
	/////// IAR FACE FIGURI GIT HUB-UL
}