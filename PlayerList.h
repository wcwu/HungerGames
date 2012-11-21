#ifndef _PLAYERLIST_H
#define _PLAYERLIST_H

#include "PlayerItem.h"

class PlayerList
{
	PlayerItem* head;
	char numOfPlayers;
	PlayerList(const PlayerList&);
public:
	PlayerList():head(0),numOfPlayers(0){}
	void Add(int x,int y);
	void Remove(const Player& player);
	PlayerItem* getHead(){return head;}
	void Print();
	~PlayerList(){delete head;}
};

#endif