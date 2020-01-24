
#include "Helpers.h"
// Helpers ----------------------------------------------------------------------------

void afficheCC(std::map<int, std::tuple<unsigned int, std::vector<unsigned int>>> compConnexeMap) {

	// Affichage de la map
	for (std::map<int, std::tuple<unsigned int, std::vector<unsigned int>>>::iterator it = compConnexeMap.begin(); it != compConnexeMap.end(); it++)
	{
		std::cout << "Id composante connexe : " << it->first << std::endl;
		std::cout << "Owner : " << std::get<0>(it->second) << std::endl;
		std::cout << "Composante connexe : " << "[";
		for (std::vector<unsigned int>::iterator itr = std::get<1>(it->second).begin(); itr != std::get<1>(it->second).end(); itr++)
		{
			std::cout << " " << *itr;
		}
		std::cout << " ]" << std::endl;
	}
}

void getListIdPlayers(const SGameState* state, std::vector<unsigned int>& vectorOwners)
{
	SCellInfo* currentCell = state->cells;
	unsigned int countCells = 0;

	// Parcours de state
	while (countCells < state->nbCells)
	{
		// Test pour chaque idCell
		// Regarder si le owner est dans le vecteur ou non
		std::vector<unsigned int>::iterator it = vectorOwners.begin();
		while (it != vectorOwners.end())
		{
			// Si dedans, on passe au suivant
			if (currentCell->owner == *it)
			{
				break;
			}
			it++;
		}

		// Si pas dedans, on l'ajoute dans le vecteur
		if (it == vectorOwners.end())
		{
			vectorOwners.push_back(currentCell->owner);
		}

		currentCell++;
		countCells++;
	}
}

void getListIdCellPerPlayer(const SGameState* state, std::vector<unsigned int>& vectorOwners, std::map<unsigned int, std::vector<unsigned int>>& mapIdCellsPerPlayer)
{
	unsigned int countCells = 0;

	// Pour chaque joueur diffrent
	for (std::vector<unsigned int>::iterator itr = vectorOwners.begin(); itr != vectorOwners.end(); itr++)
	{
		SCellInfo* currentCell = state->cells;
		unsigned int currentOwner = *itr;
		std::vector<unsigned int> idCellsOwner;

		// Pour chacun des territoires de state
		for (countCells = 0; countCells < state->nbCells; countCells++)
		{
			// Si le owner courant et le owner du territoire correspondent
			if (currentCell->owner == currentOwner)
			{
				// Ajout de l'idCell dans la map
				mapIdCellsPerPlayer[currentOwner].push_back(currentCell->id);
			}
			currentCell++;
		}

	}
}


int getIdCellsCurrentPlayer(const SGameState* state, void* ctx, std::vector<unsigned int>& idCellsVector, bool attack)
{
	Contexte* contexte = static_cast<Contexte*>(ctx);

	// Recherche de tous ses territoires dans state
	SCellInfo* chosenCell = state->cells;
	unsigned int countCells = 0;
	unsigned int nbOwnedCells = 0;
	unsigned int nbInterestingCells = 0;

	// Pour chacun des territoires de state
	for (countCells = 0; countCells < state->nbCells; countCells++)
	{
		// Si le territoire nous appartient 
		if (chosenCell->owner == contexte->getPlayerId())
		{
			if (attack)
			{
				// Si son nombre de ds est > 1 (on ne peut pas attaquer si nombre de ds = 1)
				if (chosenCell->nbDices > 1)
				{
					// On stocke l'id du territoire
					idCellsVector.push_back(chosenCell->id);
					nbInterestingCells++;
				}

			}

			else
			{
				// On stocke l'id du territoire
				idCellsVector.push_back(chosenCell->id);
			}

			nbOwnedCells++;

		}
		chosenCell++;
	}

	// Si le joueur possde tous les territoires, aucun ou ne peut pas attaquer il renvoie 0
	if (nbOwnedCells == state->nbCells)
	{
		std::cout << "Je possede tous les territoires de la map " << std::endl;
		return 0;
	}

	if (nbOwnedCells == 0)
	{
		std::cout << "Je ne possede aucun territoire " << std::endl;
		return 0;
	}

	if (attack)
	{
		if (nbInterestingCells == 0 && nbOwnedCells != 0)
		{
			std::cout << "Je ne peux pas attaquer " << std::endl;
			return 0;
		}

		else
		{
			return 1;
		}
	}

	else
	{
		return 2;
	}
}

// remplit std::tuple<SCell, SCell> attack avec l'attaque la moins risque. tuple : <SCell from, Scell To>
// en fonction du nombre de ds sur 2 les trritoires
// retourne 1 si on joue, 0 si on passe son tour
// retourne 0 si dans aucune attaque on a plus de ds que l'ennemie
// dominance : entre 0 et 100 : le score du joueur. -1 si on s'en ballec
int lessRiskyAttack(std::vector<std::tuple<SCell, SCell>> attaks, std::tuple<SCell, SCell> attack, int dominance)
{
	// dicesDiffMap : cl : diffrence de ds, valeur : tuple SCell From, SCell To
	std::map<int, std::vector<std::tuple<SCell, SCell>>> dicesDiffMap;

	std::tuple<SCell, SCell> attackDesEgal;

	// parcours de toutes les attaques possibles
	for (std::vector<std::tuple<SCell, SCell>>::iterator tupleAttack = attaks.begin(); tupleAttack != attaks.end(); tupleAttack++) {
		// Calcul de la difference de des
		int dicesDiff = std::get<0>(*tupleAttack).infos.nbDices - std::get<1>(*tupleAttack).infos.nbDices;

		// si on a plus de des que l'adversaire (ca permet deja d'eliminer les attaques ou on a moins de des que l'adversaire)
		if (dicesDiff >= 1) {
			// on recherche dans la map si la cle [difference de des] existe deja
			//auto i = dicesDiffMap.find(dicesDiff);
			std::map<int, std::vector<std::tuple<SCell, SCell>>>::iterator i = dicesDiffMap.find(dicesDiff);
			// si on a rien trouve
			if (i == dicesDiffMap.end()) {
				// on creer le vector et on l'ajoute a la map
				std::vector<std::tuple<SCell, SCell>> newTupleVector;
				newTupleVector.insert(newTupleVector.end(), *tupleAttack);
				dicesDiffMap[dicesDiff] = newTupleVector;
			}
			else {
				// on insert le tuple attak dans le vector map correspondant  la difference de des
				i->second.insert(i->second.end(), *tupleAttack);
			}
		}
		else if (dicesDiff == 0) {
			// on a autant de dés que l'ennemie
			std::get<0>(attackDesEgal) = std::get<0>(*tupleAttack);
			std::get<1>(attackDesEgal) = std::get<1>(*tupleAttack);
		}
	}

	// ----------- on a maintenant la map dicesDiffMap remplit

	// si il a que des attaques a nombre de des egal
	if (dicesDiffMap.size() == 0 && dominance > 60) {
		std::get<0>(attack) = std::get<0>(attackDesEgal);
		std::get<1>(attack) = std::get<1>(attackDesEgal);
		return 1;
	}
	if (dicesDiffMap.size() == 0) {
		return 0;
	}

	// on prend le dernier elt de la map
	std::vector<std::tuple<SCell, SCell>> meilleurVector = dicesDiffMap.rbegin()->second;

	// on remplit le resultat si il y a qu'un seul choix
	if (meilleurVector.size() == 1) {
		std::get<0>(attack) = std::get<0>(meilleurVector[0]);
		std::get<1>(attack) = std::get<1>(meilleurVector[0]);
	}
	else {
		// s'il y a plusieurs choix :
		SCell CellFrom;
		SCell CellTo;
		int minDices = std::get<0>(meilleurVector[0]).infos.nbDices;
		// On regarde le nombre de des des territoires d'attaque
		for (std::vector<std::tuple<SCell, SCell>>::iterator i = meilleurVector.begin(); i != meilleurVector.end(); i++)
		{
			// On choisi le territoire d'attaque qui a le moins de des
			if (std::get<0>(*i).infos.nbDices <= minDices)
			{
				minDices = std::get<1>(*i).infos.nbDices;
				CellFrom = std::get<0>(*i);
				CellTo = std::get<1>(*i);
			}
		}

		std::get<0>(attack) = CellFrom;
		std::get<1>(attack) = CellTo;
	}
	return 1;
}


void calculComposanteConnexe(const SGameState* state, void* ctx, std::map<int, std::tuple<unsigned int, std::vector<unsigned int>>>& compConnexeMap)
{
	Contexte* contexte = static_cast<Contexte*>(ctx);

	// Recuperation des differents owner qui n'ont pas encore perdu
	// Creation d'un vecteur pour stocker ces idOwners
	std::vector<unsigned int> vectorOwners;
	getListIdPlayers(state, vectorOwners);

	// Rcupration des territoires de chaque joueur
	std::map<unsigned int, std::vector<unsigned int>> mapIdCellsPerPlayer;
	getListIdCellPerPlayer(state, vectorOwners, mapIdCellsPerPlayer);

	// Id qui permet de distinguer les differentes composantes connexes des joueurs ( = cle de la map en dessous)
	unsigned int idCompConnexe = 0;

	// Map compConnexeMap : cl : id composante connexe ; valeur : tuple(idOwner, vecteur d'id de la composante connexe)

	// Pour chaque groupement de territoires qu'un joueur possede
	for (std::map<unsigned int, std::vector<unsigned int>>::iterator it = mapIdCellsPerPlayer.begin(); it != mapIdCellsPerPlayer.end(); it++)
	{
		// On recupere le vecteur d'idCell et l'id du owner
		std::vector<unsigned int> idCellsVector;
		unsigned int idOwner = it->first;
		idCellsVector = it->second;

		// Creation d'un vecteur qui va permettre de faire defiler les idCells d'une meme composante connexe
		std::vector<int> compConnexeVector;

		int countDico = 0;
		int countId = 0;
		bool idTrouve;

		// Iterateur temporaire pour stocker la derniere cle de la map
		std::map<int, std::tuple<unsigned int, std::vector<unsigned int>>>::iterator itTemp;

		// Tant que le nombre d'insertion d'id dans la map compConnexeMap ne depasse pas la taille du vecteur des id
		while (countDico < idCellsVector.size())
		{
			idTrouve = 0;

			// Parcours de la map
			std::map<int, std::tuple<unsigned int, std::vector<unsigned int>>>::iterator iter = compConnexeMap.begin();
			while (iter != compConnexeMap.end())
			{
				if (!idTrouve)
				{
					// On recupere le vecteur de la cle correspondante
					std::vector<unsigned int>::iterator itr = std::get<1>(iter->second).begin();

					// Tant qu'on n'a pas parcouru tout le vecteur
					while (itr != std::get<1>(iter->second).end())
					{
						// Si l'idCell est deja dedans
						if (*itr == idCellsVector[countId])
						{
							// On passe a l'idCell suivant
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
				std::get<1>(compConnexeMap[idCompConnexe]).push_back(idCellsVector[countId]);
				// Insertion de l'owner
				std::get<0>(compConnexeMap[idCompConnexe]) = idOwner;
				countDico++;
			}

			// Si idCell pas deja dedans
			else if (!idTrouve && compConnexeMap.size() != 0)
			{
				// Nouvelle cle : idCompConnexe = cle du dernier element de la map + 1
				idCompConnexe = itTemp->first + 1;

				// Insertion de l'idCell dans la map
				std::get<1>(compConnexeMap[idCompConnexe]).push_back(idCellsVector[countId]);
				// Insertion de l'owner
				std::get<0>(compConnexeMap[idCompConnexe]) = idOwner;
				countDico++;
			}

			if (!idTrouve)
			{
				// On insere cet id dans le vecteur de "defilement des voisins"
				compConnexeVector.push_back(idCellsVector[countId]);

				unsigned int currentIdCell;
				// Tant que la taille de ce vecteur n'est pas nulle
				while (compConnexeVector.size() != 0)
				{
					currentIdCell = *compConnexeVector.begin();
					int i = 0;

					// Recherche du territoire dans la map pour ensuite trouver ses voisins
					while (contexte->getMap()->cells[i].infos.id != currentIdCell)
					{
						i++;
					}

					// On a trouve le territoire dans la map
					SCell cell = contexte->getMap()->cells[i];

					// Pour chacun de ses voisins
					for (int indexVoisin = 0; indexVoisin < cell.nbNeighbors; indexVoisin++)
					{
						// Si c'est un des territoires de l'idOwner courant
						if (cell.neighbors[indexVoisin]->infos.owner == idOwner)
						{
							// Parcours du vecteur concerne pour voir s'il n'est pas deja dedans
							std::vector<unsigned int>::iterator iterator = std::get<1>(compConnexeMap[idCompConnexe]).begin();
							while (iterator != std::get<1>(compConnexeMap[idCompConnexe]).end())
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
							if (iterator == std::get<1>(compConnexeMap[idCompConnexe]).end())
							{
								// On ajoute l'idCell voisin  la map
								std::get<1>(compConnexeMap[idCompConnexe]).push_back(cell.neighbors[indexVoisin]->infos.id);
								countDico++;

								// On insere cet id dans le vecteur de "defilement des voisins"
								compConnexeVector.push_back(cell.neighbors[indexVoisin]->infos.id);

							}

						}
					}
					// On a "traite" cet idCell, on peut l'enlever de lu vecteur de defilement des voisins
					compConnexeVector.erase(compConnexeVector.begin());
				}
			}
			// Passage  l'id suivant
			countId++;
		}
	}


}

// map: cle: id composante connexe; valeur: tuple(idOwner, vecteur des idCells de la CC)
void chooseCompConnexe(void* ctx, std::map<int, std::tuple<unsigned int, std::vector<unsigned int>>>& compConnexeMap, std::vector<unsigned int>& biggestCompConnexe, bool extend)
{
	Contexte* contexte = static_cast<Contexte*>(ctx);

	// Remplissage d'un vecteur par les valeurs de la map
	std::vector< std::tuple<unsigned int, std::vector<unsigned int>>> sortedVector;
	for (std::map<int, std::tuple<unsigned int, std::vector<unsigned int>>>::iterator it = compConnexeMap.begin(); it != compConnexeMap.end(); it++)
	{
		sortedVector.push_back(it->second);
	}

	// Tri du vecteur par taille des composantes connexes
	std::sort(sortedVector.begin(), sortedVector.end(), [](std::tuple<unsigned int, std::vector<unsigned int>> t1, std::tuple<unsigned int, std::vector<unsigned int>> t2) {

		return std::get<1>(t1).size() > std::get<1>(t2).size(); });

	// Parcours du vecteur pour trouver la plus grande composante connexe du joueur (la premiere trouve)
	std::vector< std::tuple<unsigned int, std::vector<unsigned int>>>::iterator itr = sortedVector.begin();


	while (itr != sortedVector.end())
	{
		if (extend)
		{
			// Si il trouve son id
			if (std::get<0>(*itr) == contexte->getPlayerId())
			{
				break;
			}
			itr++;
		}

		else
		{
			// Si il trouve l'id d'un joueur adverse
			if (std::get<0>(*itr) != contexte->getPlayerId())
			{
				break;
			}
			itr++;
		}
	}

	// Si tout le vecteur a ete parcouru
	if (itr == sortedVector.end())
	{
		std::cout << "Pas trouve mon id" << std::endl;
	}

	// Sinon le joueur a trouve sa plus grande composante connexe
	else
	{
		if (extend)
		{
			std::cout << "Trouve ma plus grande composante connexe : ";
		}

		else
		{
			std::cout << "Trouve la plus grande composante connexe ennemie : ";
		}

		biggestCompConnexe = std::get<1>(*itr);
		// Affichage composante connexe
		std::cout << "[";
		for (auto& idCell : biggestCompConnexe)
		{
			std::cout << " " << idCell;
		}
		std::cout << " ]" << std::endl;
	}

	// On va la relier a la strategie de difference de des
}


bool selectCompConnexeCorner(void* ctx, std::map<int, std::tuple<unsigned int, std::vector<unsigned int>>>& compConnexeMap, std::vector<unsigned int>& chosenCompConnexe)
{
	Contexte* contexte = static_cast<Contexte*>(ctx);

	// Parcours de la map
	int minNbNeighboors;
	unsigned int idCellMin;
	bool firstCell = 0;

	int nbCells = contexte->getMap()->nbCells;
	for (int i = 0; i < contexte->getMap()->nbCells; i++)
	{
		// Si c'est un des territoires de joueur courant
		if (contexte->getMap()->cells[i].infos.owner == contexte->getPlayerId())
		{
			if (!firstCell)
			{
				minNbNeighboors = contexte->getMap()->cells[i].nbNeighbors;
				idCellMin = contexte->getMap()->cells[i].infos.id;
				firstCell = 1;
			}
			// Si son nombre de voisins est inferieur au min
			if (contexte->getMap()->cells[i].nbNeighbors < minNbNeighboors)
			{
				minNbNeighboors = contexte->getMap()->cells[i].nbNeighbors;
				idCellMin = contexte->getMap()->cells[i].infos.id;
			}
		}
	}

	// On considere que si le nombre de voisins minimum est > 2, cela ne vaut pas la peine de s'interesser a sa CC
	if (!firstCell || minNbNeighboors > 2)
	{
		return 0;
	}

	else
	{
		// Parcours de compConnexeMap pour trouver la composante connexe de l'idCell choisie
		// map: cl: id composante connexe; valeur: tuple(idOwner, vecteur des idCells de la CC)
		for (auto& idCC : compConnexeMap)
		{
			// Si c'est une CC du joueur courant
			if (std::get<0>(idCC.second) == contexte->getPlayerId())
			{
				std::vector<unsigned int>::const_iterator idCell;
				// Tant qu'on n'a pas trouve l'idCellMin
				for (idCell = std::get<1>(idCC.second).begin(); idCell != std::get<1>(idCC.second).end(); idCell++)
				{
					if (*idCell != idCellMin)
					{
						// Copie de la CC
						chosenCompConnexe = std::get<1>(idCC.second);
						break;
					}
				}
			}
		}

		return 1;
	}
}



// Surcouche :/
// En réalité on peut s'en passer si on utilise correctement state dans les stratégies
void updateMap(SMap * map, const SGameState * state)
{
	// Pas de garantie que l'arbitre donne dans le même ordre dans state et map.
	for (int i = 0; i < map->nbCells; i++)
	{
		for (int j = 0; j < state->nbCells; j++)
		{
			// S'il s'agit des mêmes cellules
			if (map->cells[i].infos.id == state->cells[j].id)
			{
				map->cells[i].infos.nbDices = state->cells[j].nbDices;
				map->cells[i].infos.owner = state->cells[j].owner;
			}
		}
	}
}