#include "Game.h"
#include "Board.h"
#include "general.h"
#include "Point.h"
#include <Windows.h>
#include <conio.h>
#include <process.h>

const int ESC=27;

void Game::play(char* fileName)
{
	Board b;
	bool answerPressed=false,stopGame=false,validBoard;
	char answer;
	int playCounter=0;
	PlayerList pList;
	ArrowList aList;
	b.setPList(pList);
	b.setAList(aList);
	b.readFile(fileName);
	validBoard=b.checkBoard();
	if (validBoard)
	{
		b.printText();
		while((!stopGame)&&(!isThereAWinner(pList)))
		{
			playCounter++;
			moveArrows(aList);
			b.throwGifts();
			if(playCounter%2==0)
			{
				movePlayers(pList);
			}
			if(playCounter%6==0)
			{
				shootArrows(pList,aList);
			}
			b.printScoreBoard();
			if(_kbhit()&&_getch()==ESC)
			{
				system("cls");
				cout << "Do you want to stop the game (y/n)? ";
				while(!answerPressed)
				{
					if(_kbhit())
					{
						answer=_getch();
						if(answer=='y')
						{
							answerPressed=true;
							stopGame=true;
						}
						else if(answer=='n')
						{
							answerPressed=true;
							system("cls");
							b.printText();
						}
					}
				}
				answerPressed=false;
			}
			Sleep(50);
		}
		if(stopGame==true)
		{
			cout << "\nThank you for playing our hunger game" << endl;
		}
		else
		{
			system("cls");
			if((pList.getHead()!=0)&&(pList.getHead()->getPlayer()->getScore()>0))
			{
				cout << "\n" << pList.getHead()->getPlayer()->getChar() << " is the winner!!!" << endl;
			}
			else
			{	
				cout << "\nGame over. Thank you for playing our hunger game" << endl;
			}
		}
	}
	else 
	{
		cout << "The text file isn't valid" << endl;
	}
}

bool Game::isThereAWinner(PlayerList& pList)
{
	int score;
	bool winnerFound=false;
	PlayerItem* curr=pList.getHead(),*next;
	while((curr!=0)&&(!winnerFound))
	{
		next=curr->getNext();
		score=curr->getPlayer()->getScore();
		if(score<=0)
		{
			curr->getPlayer()->getLocation()->draw(' ');
			pList.Remove(*(curr->getPlayer()));
		}
		curr=next;
		if(pList.getHead()->getNext()==0)
		{
			winnerFound=true;
		}
	}
	return winnerFound;
}

void Game::movePlayers(PlayerList& pList)
{
	PlayerItem* curr=pList.getHead();
	while(curr!=0)
	{
		curr->getPlayer()->move();
		curr=curr->getNext();
	}
}

void Game::moveArrows(ArrowList& aList)
{
	ArrowItem* curr=aList.getHead(),*next;
	char giftSteppedOn;
	while(curr!=0)
	{
		if(curr->getArrow()->getIAmAlive()==true)
		{
			curr->getArrow()->move();
			curr=curr->getNext();
		}
		else
		{
			next=curr->getNext();
			giftSteppedOn=curr->getArrow()->getGiftSteppedOn();
			curr->getArrow()->getLocation()->getBoard()->setContent(*(curr->getArrow()->getLocation()),giftSteppedOn);
			aList.Remove(*(curr->getArrow()));
			curr=next;
		}
	}
}

void Game::shootArrows(PlayerList& pList,ArrowList& aList)
{
	PlayerItem* curr=pList.getHead();
	Point p;
	char nextPlace;
	int x,y,numOfArrows;
	Board* b;
	Direction direct;
	while(curr!=0)
	{
		numOfArrows=curr->getPlayer()->getArrows();
		if(numOfArrows>0)
		{
			b=curr->getPlayer()->getLocation()->getBoard();
			direct=curr->getPlayer()->getDirect();
			curr->getPlayer()->getLocation()->getNextMove(direct,p);
			nextPlace=b->getContent(p);
			if((nextPlace!=WALL)&&(nextPlace!=ARROW))
			{
				if(nextPlace==PLAYER)
				{
					b->arrowHitsPlayer(p);
				}
				else
				{
					p.getPlace(x,y);
					aList.Add(x,y,direct);
					aList.getHead()->getArrow()->getLocation()->setBoard(b);
					curr->getPlayer()->setArrows(numOfArrows-1);
					aList.getHead()->getArrow()->setGiftSteppedOn(nextPlace);
					b->setContent(p,ARROW);
				}
			}
		}
		curr=curr->getNext();
	}
}