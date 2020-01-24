#pragma once
#include "../../Commun/interface.h"

class Contexte
{
public :
	Contexte(unsigned int playerId, unsigned int nbPlayer, const SMap *map);
	~Contexte();
	unsigned int getPlayerId();
	SMap* getMap();

	SMap *map;

private :
	unsigned int playerId;
	unsigned int nbPlayer;
	SMap* createCopy(const SMap*);
};