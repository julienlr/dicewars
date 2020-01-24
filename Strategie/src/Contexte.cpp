#include "Contexte.h"

// Constructeur prenant le numéro du joueur, ainsi que l'original de la carte.
// Ce constructeur s'occupera de créer une copie de la carte
Contexte::Contexte(unsigned int playerId, unsigned int nbPlayer, const SMap * map)
{
	this->playerId = playerId;
	this->nbPlayer = nbPlayer;
	this->map = createCopy(map);
}

Contexte::~Contexte()
{
	delete this->map;
}

unsigned int Contexte::getPlayerId()
{
	return this->playerId;
}

SMap * Contexte::getMap()
{
	return this->map;
}

SMap * Contexte::createCopy(const SMap * map)
{
	SMap* copy = new SMap();
	copy->cells = new SCell[map->nbCells];

	SCell cell;

	for (int i = 0; i < map->nbCells; i++)
	{
		cell = SCell();
		cell.infos = SCellInfo();
		/*
		cell.infos.id = map->cells[i].infos.id;
		cell.infos.nbDices = map->cells[i].infos.nbDices;
		cell.infos.owner = map->cells[i].infos.owner;

		cell.nbNeighbors = map->cells[i].nbNeighbors;
		cell.neighbors = new SCell*[cell.nbNeighbors];

		std::cout << "Création cellule " << i << std::endl;

		for (int j = 0; j < map->nbCells; j++)
		{
			cell.neighbors[j] = map->cells[i].neighbors[j];
		}
		*/
		copy->cells[i] = map->cells[i];
	}

	return(copy);
}


