#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <list>
#include <map>
#include <tuple>
#include <algorithm>
#include "../../Commun/interface.h"
#include "../../Strategie/src/Strategy.h"
#include "Game.h"



/* verification si coup est valide*/
bool Game::coup_valide(SMap* map, STurn* turn, int id_att) {

	SCell territory_att, territory_def;

	//on recupere les infos sur les cellules att et def
	for (int j = 0; j < map->nbCells; j++)
	{
		if (map->cells[j].infos.id == turn->cellFrom)
		{
			territory_att = map->cells[j];
			territory_att.infos.owner = map->cells[j].infos.owner;
			territory_att.infos.nbDices = map->cells[j].infos.nbDices;
		}
		if (map->cells[j].infos.id == turn->cellTo)
		{
			territory_def = map->cells[j];
			territory_def.infos.owner = map->cells[j].infos.owner;
		}
	}

	int neigh = 0;

	//check si le territoire attaque est bien un voisin
	for (int i = 0; i < territory_att.nbNeighbors; i++)
		if (territory_att.neighbors[i]->infos.id == territory_def.infos.id) neigh = 1;

	// check si le nb de des est au moins de deux
	// check si le territoire attaque est un territoire adverse
	//check si le territoire qui attaque appartient bien à l'attaquant
	return(territory_att.infos.owner == id_att && territory_att.infos.nbDices > 1 && territory_att.infos.owner != territory_def.infos.owner && neigh == 1);

}


/* duel avec tirages des et nb de des de chaque territoires*/
/* retourne true si l'attaquant gagne, false sinon */
bool Game::duel(SGameState * state, STurn * turn) {

	int nb_dices_att = 0, nb_dices_def = 0;

	//on rcupre les infos sur les cellules att et def
	for (int j = 0; j < state->nbCells; j++)
	{
		if (state->cells[j].id == turn->cellFrom) nb_dices_att = state->cells[j].nbDices;
		if (state->cells[j].id == turn->cellTo) nb_dices_def = state->cells[j].nbDices;
	}

	srand((int)time(NULL));

	int tot1 = 0, tot2 = 0;

	for (int i = 0; i < nb_dices_att; i++) tot1 += rand() % 7 + 1;
	for (int j = 0; j < nb_dices_def; j++) tot2 += rand() % 7 + 1;

	return(tot1 > tot2);
}

/* mise a jour des infos des territoires attaque et attaquant */
void Game::update_territory(SGameState * state, SMap * map, STurn * turn, bool duel) {

	int nbdes, proprietaire;
	int i = 0;
	int j = 0;
	// Recherche des indices des territoires cellFrom et cellTo
	while (map->cells[i].infos.id != turn->cellFrom)
	{
		i++;
	}
	while (map->cells[j].infos.id != turn->cellTo)
	{
		j++;
	}

	if (duel == true) {

		nbdes = state->cells[i].nbDices;
		state->cells[i].nbDices = 1;
		map->cells[i].infos.nbDices = 1;
		proprietaire = state->cells[i].owner;

		state->cells[j].nbDices = nbdes - 1;
		map->cells[j].infos.nbDices = nbdes - 1;
		state->cells[j].owner = proprietaire;
		map->cells[j].infos.owner = proprietaire;

	}
	else {
		state->cells[i].nbDices = 1;
		map->cells[i].infos.nbDices = 1;
	}

}

/* check s'il reste des territoires au joueur courant */
bool Game::check_territory(SGameState * state, int playerId) {

	for (int j = 0; j < state->nbCells; j++)
		if (state->cells[j].owner == playerId) return true;

	return false;
}


/* fonction calcul la plus grande composante connexe d'un joueur*/
int Game::calculComposanteConnexe(SGameState * state, SMap * map, unsigned int currentOwner)
{
	std::vector<std::tuple<unsigned int, unsigned int>> idCellsAndNbDices;
	// Recupération des idCells et leur nombre de des dans ce vecteur
	this->getIdCellAndNbDices(currentOwner, state, idCellsAndNbDices);

	// Id qui permet de distinguer les differentes composantes connexes des joueurs ( = cle de la map en dessous)
	unsigned int idCompConnexe = 0;

	// Map compConnexeMap : cle : id composante connexe ; valeur : vecteur d'id de la composante connexe
	std::map<int, std::vector<unsigned int>> compConnexeMap;

	// Creation d'un vecteur qui va permettre de faire defiler les idCells d'une meme composante connexe
	std::vector<int> compConnexeVector;

	int countDico = 0;
	int countId = 0;
	bool idTrouve;

	// Iterateur temporaire pour stocker la derniere cle de la map
	std::map<int, std::vector<unsigned int>>::iterator itTemp;

	// Tant que le nombre d'insertion d'id dans la map compConnexeMap ne depasse pas la taille du vecteur des id
	while (countDico < idCellsAndNbDices.size())
	{
		idTrouve = 0;

		// Parcours de la map
		std::map<int, std::vector<unsigned int>>::iterator iter = compConnexeMap.begin();
		while (iter != compConnexeMap.end())
		{
			if (!idTrouve)
			{
				// On recupere le vecteur de la cle correspondante
				std::vector<unsigned int>::iterator itr = begin(iter->second);

				// Tant qu'on n'a pas parcouru tout le vecteur
				while (itr != end(iter->second))
				{
					// Si l'idCell est deja dedans
					if (*itr == std::get<0>(idCellsAndNbDices[countId]))
					{
						idTrouve = 1;
						break;
					}
					itr++;
				}

				itTemp = iter;
				iter++;
			}

			else
			{
				break;
			}

		}

		if (!idTrouve && compConnexeMap.size() == 0)
		{
			// Nouvelle cle
			idCompConnexe = 0;

			// Insertion de l'idCell dans la map
			compConnexeMap[idCompConnexe].push_back(std::get<0>(idCellsAndNbDices[countId]));
			countDico++;
		}

		// Si idCell pas deja dedans
		else if (!idTrouve && compConnexeMap.size() != 0)
		{
			// Nouvelle cle : idCompConnexe = cle du dernier element de la map + 1
			idCompConnexe = itTemp->first + 1;

			// Insertion de l'idCell dans la map
			compConnexeMap[idCompConnexe].push_back(std::get<0>(idCellsAndNbDices[countId]));
			countDico++;
		}

		if (!idTrouve)
		{
			// On insere cet id dans le vecteur de "defilement des voisins"
			compConnexeVector.push_back(std::get<0>(idCellsAndNbDices[countId]));

			unsigned int currentIdCell;
			// Tant que la taille de ce vecteur n'est pas nulle
			while (compConnexeVector.size() != 0)
			{
				currentIdCell = *begin(compConnexeVector);
				int i = 0;

				// Recherche du territoire dans la map pour ensuite trouver ses voisins
				while (map->cells[i].infos.id != currentIdCell)
				{
					i++;
				}

				// On a trouve le territoire dans la map
				SCell cell = map->cells[i];

				// Pour chacun de ses voisins
				for (int indexVoisin = 0; indexVoisin < cell.nbNeighbors; indexVoisin++)
				{
					// Si c'est un des territoires de l'idOwner courant
					if (cell.neighbors[indexVoisin]->infos.owner == currentOwner)
					{
						// Parcours du vecteur concerne pour voir s'il n'est pas deja dedans
						std::vector<unsigned int>::iterator iterator = begin(compConnexeMap[idCompConnexe]);
						while (iterator != end(compConnexeMap[idCompConnexe]))
						{
							// Si l'idCell est deja dedans
							if (cell.neighbors[indexVoisin]->infos.id == *iterator)
							{
								// On passe au voisin suivant
								break;
							}
							iterator++;
						}

						// Si il n'est pas deja dedans
						if (iterator == end(compConnexeMap[idCompConnexe]))
						{
							// On ajoute l'idCell voisin  la map
							compConnexeMap[idCompConnexe].push_back(cell.neighbors[indexVoisin]->infos.id);
							countDico++;

							// On insere cet id dans le vecteur de "defilement des voisins"
							compConnexeVector.push_back(cell.neighbors[indexVoisin]->infos.id);
						}
					}
				}
				// On a "traite" cet idCell, on peut l'enlever du vecteur de defilement des voisins
				compConnexeVector.erase(compConnexeVector.begin());
			}
		}
		// On passe a l'idCell suivant
		countId++;
	}

	unsigned int biggestSize;
	// On verifie si la map est vide ou non
	if (!compConnexeMap.empty())
	{
		// Parcours de compConnexeMap pour trouver la taille max de la CC
		biggestSize = static_cast<unsigned int>(size(begin(compConnexeMap)->second));
		for (auto& elt : compConnexeMap)
		{
			if (size(elt.second) > biggestSize)
			{
				biggestSize = static_cast<unsigned int>(size(elt.second));
			}
		}
	}

	else
	{
		biggestSize = 0;
	}

	state->points[currentOwner] = biggestSize;
	return biggestSize;

}

/* distribution des des sur les territoires a la fin du gameturn*/
void Game::dealDices(SGameState * state, SMap * map, std::vector<std::tuple<unsigned int, unsigned int>> & territories, unsigned int composante, int owner)
{
	srand((int)time(NULL));
	std::vector<int> tab;

	composante += state->diceStock[owner];
	while (composante > 0) {
		//si tous les territoires du joueur ont 8 des
		if (size(tab) == size(territories)) {
			//on stock les des supplementaires
			state->diceStock[owner] = composante;
			composante = 0;
		}
		else {
			int r;
			// on choisit au hasard un des territoires du joueur
			r = rand() % size(territories);
			//si le territoire choisi n'a pas 8 des
			if (std::get<1>(territories[r]) < 8) {
				int x;

				// si le nb de des qu'on peut ajouter au territoire est <= aux des qu'il reste a distribuer
				if ((8 - std::get<1>(territories[r])) < composante) {
					// on fait un random sur le nombre de des que l'on peut ajouter au territoire
					x = rand() % (8 - std::get<1>(territories[r])) + 1;
				}
				//sinon
				else {
					// on fait un random sur le nombre de des que l'on peut ajouter
					x = rand() % composante + 1;
				}
				//on enleve le nb de des distribues
				composante -= x;

				//on maj state et map
				int i = 0;
				while (state->cells[i].id != std::get<0>(territories[r])) i++;
				state->cells[i].nbDices += x;
				map->cells[i].infos.nbDices += x;
				std::get<1>(territories[r]) += x;
				//si le territoire a 8 des, on l'ajoute au vecteur
				if (state->cells[i].nbDices == 8) tab.push_back(r);

			}
			else {
				if (!check_id(tab, r)) tab.push_back(r);
			}

		}
	}
}


/* fonction permettant de recuperer l'id et le nb de des pour chaque territoire d'un joueur */

void Game::getIdCellAndNbDices(unsigned int currentOwner, const SGameState * state, std::vector<std::tuple<unsigned int, unsigned int>> & idCellsAndNbDices)
{
	// Recuperation des idCell du joueur courant et de leur nombre de des
	SCellInfo* currentCell = state->cells;
	unsigned int countCells;

	// Pour chacun des territoires de state
	for (countCells = 0; countCells < state->nbCells; countCells++)
	{
		// Si le owner courant et le owner du territoire correspondent
		if (currentCell->owner == currentOwner)
		{
			// Creation d'une paire
			std::tuple<unsigned int, unsigned int> paire;
			paire = std::make_tuple(currentCell->id, currentCell->nbDices);
			// Ajout de l'idCell dans la map
			idCellsAndNbDices.push_back(paire);
		}
		currentCell++;
	}
}


/* recherche la place de la cellule dans le vecteur*/
bool Game::check_id(std::vector<int> tab, int id) {

	for (int i = 0; i < size(tab); i++) if (tab[i] == id) return true;

	return false;
}



