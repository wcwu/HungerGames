#ifndef _BOARD_H_
#define _BOARD_H_

#include "PlayerList.h"
#include "ArrowList.h"
#include "Player.h"
#include "Point.h"

class Board
{
	char** text;
	int numOfComputerPlayersOnBoard;
	int numOfHumanPlayersOnBoard;
	int numOfFilePlayersOnBoard;
	Point scoreBoardPlace;
	PlayerList* pList;
	ArrowList* aList;
	Player* human;
	Board(const Board&);
public:
	Board();
	bool readFile(char** files);
	void setPList(PlayerList& lst){pList=&lst;}
	void setAList(ArrowList& lst){aList=&lst;}
	void printText()const;
	char getContent(const Point& p)const;
	void setContent(const Point& p,char ch)const;
	bool checkBoard();
	bool randomLocation(Point& p)const;
	void throwGifts()const;
	bool isPointInScoreBoard(const Point& p)const;
	bool isPointNearAPlayer(const Point& p)const;
	void playerFight(const Point& p)const;
	void arrowHitsPlayer(const Point& p)const;
	void printScoreBoard()const;
	void setHumanPlayer(Player* p){human=p;}
	Player* getHumanPlayer()const{return human;}
	~Board();
};

#endif