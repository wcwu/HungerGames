#include "Board.h"
#include "general.h"
#include "FilePlayer.h"

Board::Board()
{
	text=new char*[HEIGHT];
	for(int i=0;i<HEIGHT;++i)
		text[i]=new char[WIDTH];
	numOfComputerPlayersOnBoard=0;
	numOfHumanPlayersOnBoard=0;
	numOfFilePlayersOnBoard=0;
	scoreBoardPlace.setPlace(0,0);
	giftMap=new map<int,vector<char>*>;
	(*giftMap).empty();
	human=NULL;
}

void Board::readFile(char** files)
{
	ifstream boardFile;
	char ch;
	bool OWasFound=false;
	boardFile.open(files[BoardFile],ifstream::in);
	for(int i=0;i<HEIGHT;++i)
	{
		for(int j=0;j<WIDTH;++j)
		{
			ch=(char)(boardFile.get());
			switch(ch)
			{
			case 'W':
				text[i][j]=WALL;
				break;
			case 'C':
				if((numOfFilePlayersOnBoard<MAX_NUM_OF_FILE_PLAYERS)&&
					(getNumOfPlayerOnBoard()<MAX_NUM_OF_PLAYERS_ON_BOARD)&&(files[(FileType)(numOfFilePlayersOnBoard+1)]!=NULL))
				{
						numOfFilePlayersOnBoard++;
						text[i][j]=PLAYER;
						pList->add(i,j,getNumOfPlayerOnBoard(),File_Player);
						FilePlayer* fp=dynamic_cast<FilePlayer*>(pList->getHead()->getPlayer());
						if(fp!=NULL)
						{
							mapFile(fp->getEventMap(),files[(FileType)(numOfFilePlayersOnBoard)]);
						}
						fp->getLocation()->setBoard(this);
				}
				else text[i][j]=' ';
				break;
			case 'H':
				if((numOfHumanPlayersOnBoard<MAX_NUM_OF_HUMAN_PLAYERS)&&(getNumOfPlayerOnBoard()<MAX_NUM_OF_PLAYERS_ON_BOARD))
				{
					numOfHumanPlayersOnBoard++;
					text[i][j]=PLAYER;
					pList->add(i,j,HUMAN_PLAYER,Human_Player);
					pList->getHead()->getPlayer()->getLocation()->setBoard(this);
					setHumanPlayer(pList->getHead()->getPlayer());
				}
				else text[i][j]=' ';
				break;
			case PLAYER:
				if((numOfComputerPlayersOnBoard<MAX_NUM_OF_COMPUTER_PLAYERS)&&(getNumOfPlayerOnBoard()<MAX_NUM_OF_PLAYERS_ON_BOARD))
				{
					numOfComputerPlayersOnBoard++;
					text[i][j]=PLAYER;
					pList->add(i,j,getNumOfPlayerOnBoard(),Computer_Player);
					pList->getHead()->getPlayer()->getLocation()->setBoard(this);
				}
				else text[i][j]=' ';
				break;
			case 'O':
				if(!OWasFound)
				{
					OWasFound=true;
					scoreBoardPlace.setPlace(i,j);
				}
				text[i][j]=' ';
				break;
			default: text[i][j]=' ';
			}
		}
		boardFile.get();
	}
	boardFile.close();
}

void Board::mapFile(map<int,vector<char>*>* eventMap,const char* file)
{
	int num=0;
	char ch;
	ifstream inFile;
	inFile.open(file,ifstream::in);
	while (!inFile.eof())
	{
		inFile >> num;
		(*eventMap)[num]=new vector<char>;
		(*(*eventMap)[num]).empty();
		ch=inFile.get();
		while((ch!='\n')&&(!inFile.eof()))
		{
			if(ch!=' ')
			{
				(*((*eventMap)[num])).push_back(ch);
			}
			ch=inFile.get();
		}
	}
	inFile.close();
}

bool Board::checkBoard()
{
	int x,y;
	bool validBoard=true,validPlace=true;
	Point playerPlace,nextPlace;
	PlayerItem* curr=pList->getHead();
	scoreBoardPlace.getPlace(x,y);
	if((x<MIN_VALID_BOARD_PLACE_X)||(x>MAX_VALID_BOARD_PLACE_X)||(y<MIN_VALID_BOARD_PLACE_Y)||(y>MAX_VALID_BOARD_PLACE_Y))
	{
		validBoard=false;
		cout << "Illegal Text file" << endl;
	}
	if(validBoard)
	{
		while(curr!=NULL)
		{
			curr->getPlayer()->getLocation()->getPlace(x,y);
			playerPlace.setPlace(x,y);
			if((isPointInScoreBoard(playerPlace)))
			{
				setContent(playerPlace,' '); //remove player from board
				validPlace=randomLocation(playerPlace);
				if(!validPlace)
				{
					cout << "Valid place wasn't found" << endl;
					validBoard=false;
				}
				else
				{
					playerPlace.getPlace(x,y);
					curr->getPlayer()->getLocation()->setPlace(x,y);
					setContent(playerPlace,PLAYER);
				}
			}
			curr=curr->getNext();
		}
	}
	if(validBoard)
	{
		while(getNumOfPlayerOnBoard()<MIN_NUM_OF_PLAYERS_ON_BOARD)
		{
			validPlace=randomLocation(playerPlace);
			if(!validPlace)
			{
				cout << "Valid place didn't found" << endl;
				validBoard=false;
			}
			else
			{
				numOfComputerPlayersOnBoard++;
				playerPlace.getPlace(x,y);
				pList->add(x,y,numOfComputerPlayersOnBoard,Computer_Player);
				pList->getHead()->getPlayer()->getLocation()->setBoard(this);
				setContent(playerPlace,PLAYER);
			}
		}
	}
	if(validBoard)
	{
		scoreBoardPlace.getPlace(x,y);
		y--;
		x--;
		for(int i=y;i<y+SCORE_BOARD_WIDTH;++i)
		{
			text[x][i]=WALL;
			text[x+(SCORE_BOARD_HEIGHT-1)][i]=WALL;
		}
		for(int j=x+1;j<x+SCORE_BOARD_HEIGHT;++j)
		{
			text[j][y]=WALL;
			text[j][y+(SCORE_BOARD_WIDTH-1)]=WALL;
		}
	}
	return validBoard;
}

void Board::printText()const
{
	for(int i=0;i<HEIGHT;++i)
	{
		for(int j=0;j<WIDTH;++j)
		{
			if((text[i][j]==PLAYER)||(text[i][j]==ARROW))
			{
				cout << ' ';
			}
			else
			{
				cout << text[i][j];
			}
		}
		cout << endl;
	}
	pList->print();
	aList->print();
	printScoreBoard(0);
}

char Board::getContent(const Point& p)const
{
	int x,y;
	p.getPlace(x,y);
	return text[x][y];
}

void Board::setContent(const Point& p,char ch)const
{
	int x,y;
	p.getPlace(x,y);
	text[x][y]=ch;
}

bool Board::randomLocation(Point& p)const
{
	Point newLocation;
	char newLocationContent;
	bool newLocationWasFound=false;
	int x,y,timeSearchedForLocation=0;
	while((!newLocationWasFound)&&(timeSearchedForLocation<=NUM_OF_TRIES_TO_FIND_NEW_LOCATION))
	{
		x=rand()%HEIGHT;
		y=rand()%WIDTH;
		newLocation.setPlace(x,y);
		newLocationContent=getContent(newLocation);
		if((newLocationContent==' ')&&(!isPointInScoreBoard(newLocation)))
		{
			newLocationWasFound=true;
			p.setPlace(x,y);
		}
		timeSearchedForLocation++;
	}
	if(!newLocationWasFound)
	{
		for(x=0;x<HEIGHT;++x)
		{
			for(y=0;y<WIDTH;++y)
			{
				newLocation.setPlace(x,y);
				newLocationContent=getContent(newLocation);
				if((newLocationContent==' ')&&(!isPointInScoreBoard(newLocation)))
				{
					newLocationWasFound=true;
					p.setPlace(x,y);
				}
			}
		}
	}
	return newLocationWasFound;
}

bool Board::isPointNearAPlayer(const Point& p)const
{
	PlayerItem* curr=pList->getHead();
	int x,y,playerX,playerY;
	p.getPlace(x,y);
	bool isPointNearThePlayer=false;
	while((curr!=NULL)&&(!isPointNearThePlayer))
	{
		curr->getPlayer()->getLocation()->getPlace(playerX,playerY);
		playerX-=DISTANCE_BETWEEN_ITEM_TO_PLAYER;
		playerY-=DISTANCE_BETWEEN_ITEM_TO_PLAYER;
		if((x-playerX<=DISTANCE_BETWEEN_ITEM_TO_PLAYER+2)&&(y-playerY<=DISTANCE_BETWEEN_ITEM_TO_PLAYER+2)
			&&(x-playerX>=DISTANCE_BETWEEN_ITEM_TO_PLAYER-2)&&(y-playerY>=DISTANCE_BETWEEN_ITEM_TO_PLAYER-2))
		{
			isPointNearThePlayer=true;
		}
		curr=curr->getNext();
	}
	return isPointNearThePlayer;
}

void Board::throwGifts(int playCounter)
{
	if((*giftMap).size())
	{
		throwMapGifts(playCounter);
	}
	else
	{
		throwAutoGifts();
	}
}

void Board::throwAutoGifts()const
{
	int chance=(rand()%GIFT_CHANCE)+1;//1-20
	//Bomb 0.05
	if(chance%int((1/BOMB_CHANCE))==0)//20
	{
		tryToThrowAGift(BOMB_GIFT);
	}
	//Arrow 0.1
	if(chance%int((1/ARROW_CHANCE))==0)//10,20
	{
		tryToThrowAGift(ARROW_GIFT);
	}
	//Food 0.2
	if(chance%int((1/FOOD_CHANCE))==0)//5,10,15,20
	{
		tryToThrowAGift(FOOD_GIFT);
	}
}

void Board::throwMapGifts(int playCounter)
{
	char ch;
	if((*giftMap)[playCounter])
	{
		vector<char> giftVector=*((*giftMap)[playCounter]);
		vector<char>::iterator itr=giftVector.begin();
		vector<char>::iterator itrEnd=giftVector.end();
		while(itr!=itrEnd)
		{
			ch=*itr;
			switch(ch)
			{
			case 'f':
			case 'F':
				tryToThrowAGift(FOOD_GIFT);
				break;
			case 'q':
			case 'Q':
				tryToThrowAGift(ARROW_GIFT);
				break;
			case 'b':
			case 'B':
				tryToThrowAGift(BOMB_GIFT);
				break;
			}
			itr++;
		}
	}
}

void Board::tryToThrowAGift(const char ch)const
{
	Point giftLocation;
	if(randomLocation(giftLocation))
	{
		if(!isPointNearAPlayer(giftLocation))
		{
			setContent(giftLocation,ch);
			giftLocation.draw(ch);
		}
	}
}

bool Board::isPointInScoreBoard(const Point& p)const
{
	int SBx,SBy,x,y;
	scoreBoardPlace.getPlace(SBx,SBy);
	SBx--;
	SBy--;
	p.getPlace(x,y);
	if((x-SBx<SCORE_BOARD_HEIGHT)&&(y-SBy<SCORE_BOARD_WIDTH)&&(x-SBx>=0)&&(y-SBy>=0))
	{
		return true;
	}
	else return false;
}

void Board::printScoreBoard(int playCounter)const
{
	int SBx,SBy,x,y,score,numOfBombingArrows,numOfPassingArrows,numOfRegularArrows;
	char ch;
	Point p;
	PlayerItem* curr=pList->getHead();
	scoreBoardPlace.getPlace(SBx,SBy);
	while(curr!=NULL)
	{
		x=SBx;
		y=SBy;
		scoreBoardPlace.getPlace(x,y);
		curr->getPlayer()->getArrows(numOfBombingArrows,numOfPassingArrows,numOfRegularArrows);
		score=curr->getPlayer()->getScore();
		ch=curr->getPlayer()->getSymbol();
		x+=(ch-1)*2;
		p.setPlace(x,y);
		p.draw(ch);//Draw the player icon
		if(numOfBombingArrows>9)
		{
			cout << '*';
		}
		else
		{
			cout << numOfBombingArrows;
		}
		if(numOfPassingArrows>9)
		{
			cout << '*';
		}
		else
		{
			cout << numOfPassingArrows;
		}
		if(numOfRegularArrows>9)
		{
			cout << '*';
		}
		else
		{
			cout << numOfRegularArrows;
		}
		cout << " ";
		if(score>9999)
		{
			cout << score;
		}
		else if(score>999)
		{
			cout << " " << score;
		}
		else if(score>99)
		{
			cout << "  " << score;
		}
		else if(score>9)
		{
			cout << "   " << score;
		}
		else if(score>0)
		{
			cout << "    " << score;
		}
		else
		{
			cout << "    0";
		}
		curr=curr->getNext();
	}
}

void Board::playerFight(const Point& p)const
{
	int x,y,playerX,playerY,numOfMaxPlayers=0,max=0,score;
	PlayerItem* curr=pList->getHead();
	p.getPlace(x,y);
	p.draw(BELL);//Ring a bell when players fight
	while(curr!=NULL)
	{
		curr->getPlayer()->getLocation()->getPlace(playerX,playerY);
		if((x==playerX)&&(y==playerY))
		{
			if(curr->getPlayer()->getScore()>max)
			{
				max=curr->getPlayer()->getScore();
				numOfMaxPlayers=1;
			}
			else if(curr->getPlayer()->getScore()==max)
			{
				numOfMaxPlayers++;
			}
		}
		curr=curr->getNext();
	}
	curr=pList->getHead();
	while(curr!=NULL)
	{
		curr->getPlayer()->getLocation()->getPlace(playerX,playerY);
		if((x==playerX)&&(y==playerY))
		{
			score=curr->getPlayer()->getScore();
			if(curr->getPlayer()->getScore()<max)
			{
				curr->getPlayer()->setScore(score-LOW_POWER_PLAYER);
			}
			else if((curr->getPlayer()->getScore()==max)&&(numOfMaxPlayers==NUM_OF_PLAYERS_WITH_MAX_SCORE))
			{
				curr->getPlayer()->setScore(score-HIGH_POWER_PLAYER);
			}
			else if((curr->getPlayer()->getScore()==max)&&(numOfMaxPlayers>NUM_OF_PLAYERS_WITH_MAX_SCORE))
			{
				curr->getPlayer()->setScore(score-EQUAL_POWER_PLAYER);
			}
		}
		curr=curr->getNext();
	}
}

void Board::arrowHitsPlayer(const Point& p)const
{
	PlayerItem* pCurr=pList->getHead();
	ArrowItem* aCurr=aList->getHead();
	int x,y,playerX,playerY,arrowX,arrowY,score;
	p.getPlace(x,y);
	p.draw(BELL);//Ring a bell when arrow hits player
	while(pCurr!=NULL)
	{
		pCurr->getPlayer()->getLocation()->getPlace(playerX,playerY);
		if((x==playerX)&&(y==playerY))
		{
			score=pCurr->getPlayer()->getScore();
			pCurr->getPlayer()->setScore(score-ARROW_HIT_PLAYER);
		}
		pCurr=pCurr->getNext();
	}
	while(aCurr!=NULL)
	{
		aCurr->getArrow()->getLocation()->getPlace(arrowX,arrowY);
		if((x==arrowX)&&(y==arrowY))
		{
			aCurr->getArrow()->setKillArrow();
		}
		aCurr=aCurr->getNext();
	}
}
void Board::freeMap(map<int,vector<char>*>* eventMap)
{
	map<int,vector<char>*>::iterator itr=(*eventMap).begin();
	map<int,vector<char>*>::iterator itrEnd=(*eventMap).end();
	while(itr!=itrEnd)
	{
		delete itr->second;
		itr++;
	}
}

Board::~Board()
{
	freeMap(giftMap);
	delete giftMap;
	for(int i=0;i<HEIGHT;++i)
	{
		delete text[i];
	}
	delete []text;
}