#include "Strategy.h"

bool dicesDifferenceAttack(const SGameState* state, STurn* turn, void* ctx, std::vector<unsigned int>& givenVector)
{

	Contexte* contexte = static_cast<Contexte*>(ctx);

	//MAJ de de la map du contexte dont se sert le joueur
	updateMap(contexte->getMap(), state);

	std::map<int, std::tuple<unsigned int, std::vector<unsigned int>>> compConnexeMap;
	calculComposanteConnexe(state, ctx, compConnexeMap);
	afficheCC(compConnexeMap);

	// Vecteur qui va contenir les idCells 
	std::vector<unsigned int> idCellsVector;

	// Si la taille de la plus grande composante connexe est insufisante
	if (givenVector.size() < 3)
	{
		// Récupération des idCell du joueur courant
		if (!getIdCellsCurrentPlayer(state, ctx, idCellsVector, 1))
		{
			return 0;
		}
	}

	else
	{
		idCellsVector = givenVector;
	}

	// Création d'une map : clé : différence de dés (entre territoire d'attaque et territoire attaqué) ; valeur : liste de tuples (cellFrom, cellTo)
	std::map<int, std::vector<std::tuple<SCell, SCell>>> dicesDiffMap;

	// Pour chaque territoire que le joueur possède
	for (std::vector<unsigned int>::iterator id = idCellsVector.begin(); id != idCellsVector.end(); id++)
	{
		int i = 0;
		// Recherche du territoire dans la map pour ensuite trouver ses voisins
		while (contexte->getMap()->cells[i].infos.id != *id)
		{
			i++;
		}

		// On a trouvé notre territoire à nous
		SCell cell = contexte->getMap()->cells[i];

		// On stocke le nombre de dés de notre territoire 
		int ownDices = cell.infos.nbDices;

		// Pour chacun de ses voisins
		for (int indexVoisin = 0; indexVoisin < cell.nbNeighbors; indexVoisin++)
		{
			// Si c'est un territoire ennemi
			if (cell.neighbors[indexVoisin]->infos.owner != contexte->getPlayerId())
			{
				// Création du tuple de type (cellFrom, cellTo)
				std::tuple<SCell, SCell> tupleAttack = std::make_tuple(cell, *(cell.neighbors[indexVoisin]));

				// Calcul de la diférence de dés
				int dicesDiff = ownDices - std::get<1>(tupleAttack).infos.nbDices;

				std::map<int, std::vector<std::tuple<SCell, SCell>>>::iterator it = dicesDiffMap.begin();

				// Parcours des clés de la map pour voir si on a besoin de créer un nouveau vecteur ou pas
				while (it != dicesDiffMap.end())
				{
					// Si on trouve une clé qui correspond, on insère directement le tuple
					if (it->first == dicesDiff)
					{
						dicesDiffMap[it->first].insert(dicesDiffMap[it->first].end(), tupleAttack);
						break;
					}

					it++;
				}

				// Si on trouve pas de clé == dicesDiff
				if (it == dicesDiffMap.end())
				{
					// On crée un nouveau vecteur, on insère le tuple dans la liste de la map
					std::vector<std::tuple<SCell, SCell>> newTupleVector;
					newTupleVector.insert(newTupleVector.end(), tupleAttack);
					dicesDiffMap[dicesDiff] = newTupleVector;
				}
			}
		}
	}


	if (dicesDiffMap.empty())
	{
		std::cout << "Pas de voisins" << std::endl;
		return 0;
	}

	// On prend le dernier vecteur de la map (dernière clé = différence de dés la plus grande), on regarde le nombre d'éléments du vecteur
	std::map<int, std::vector<std::tuple<SCell, SCell>>>::iterator iter = dicesDiffMap.begin();
	// Valeur de la derniere clé
	int lastKeyValue;

	std::vector<std::tuple<SCell, SCell>> lastVector = dicesDiffMap[iter->first];
	while (iter != dicesDiffMap.end())
	{
		lastVector = dicesDiffMap[iter->first];
		lastKeyValue = iter->first;
		iter++;
	}

	// Si la dernière cle est > 0 (c'est intéressant pour le joueur d'attaquer)
	if (lastKeyValue > 0)
	{
		// Si il n'y a qu'un tuple dans le vecteur, on peut remplir turn
		if (lastVector.size() == 1)
		{
			turn->cellFrom = std::get<0>(lastVector[0]).infos.id;
			turn->cellTo = std::get<1>(lastVector[0]).infos.id;
		}

		// Si plusieurs possibilités
		else
		{
			unsigned int idCellFrom;
			unsigned int idCellTo;
			int minDices = std::get<0>(lastVector[0]).infos.nbDices;

			// On regarde le nombre de dés des territoires d'attaque
			for (std::vector<std::tuple<SCell, SCell>>::iterator i = lastVector.begin(); i != lastVector.end(); i++)
			{
				// On choisi le territoire d'attaque qui a le moins de dés
				if (std::get<0>(*i).infos.nbDices <= minDices)
				{
					minDices = std::get<0>(*i).infos.nbDices;
					idCellFrom = std::get<0>(*i).infos.id;
					idCellTo = std::get<1>(*i).infos.id;
				}
			}

			turn->cellFrom = idCellFrom;
			turn->cellTo = idCellTo;
		}

		return 1;
	}

	else
	{
		// Si le joueur courant vient de recevoir des des de reserve
		if (state->diceStock[contexte->getPlayerId()] != 0)
		{
			// Si il n'y a qu'un tuple dans le vecteur, on peut remplir turn
			if (lastVector.size() == 1)
			{
				turn->cellFrom = std::get<0>(lastVector[0]).infos.id;
				turn->cellTo = std::get<1>(lastVector[0]).infos.id;
			}

			// Si plusieurs possibilités
			else
			{
				unsigned int idCellFrom;
				unsigned int idCellTo;
				int minDices = std::get<1>(lastVector[0]).infos.nbDices;
				// On regarde le nombre de dés des territoires d'attaque
				for (std::vector<std::tuple<SCell, SCell>>::iterator i = lastVector.begin(); i != lastVector.end(); i++)
				{
					// On choisi le territoire d'attaque qui a le moins de dés
					if (std::get<1>(*i).infos.nbDices <= minDices)
					{
						minDices = std::get<0>(*i).infos.nbDices;
						idCellFrom = std::get<0>(*i).infos.id;
						idCellTo = std::get<1>(*i).infos.id;
					}
				}

				turn->cellFrom = idCellFrom;
				turn->cellTo = idCellTo;
			}

			return 1;
		}
		else
		{
			return 0;
		}
	}
}

bool attackFromBiggestCompConnexe(const SGameState* state, STurn* turn, void* ctx)
{
	Contexte* contexte = static_cast<Contexte*>(ctx);

	updateMap(contexte->getMap(), state);

	std::map<int, std::tuple<unsigned int, std::vector<unsigned int>>> compConnexeMap;
	calculComposanteConnexe(state, ctx, compConnexeMap);
	afficheCC(compConnexeMap);

	std::vector<unsigned int> myBiggestCompConnexe;
	chooseCompConnexe(ctx, compConnexeMap, myBiggestCompConnexe, 1);

	unsigned int idCellFrom;
	unsigned int idCellTo;
	bool attack = 0;
	// Pour chaque territoire que le joueur possède
	for (std::vector<unsigned int>::iterator id = myBiggestCompConnexe.begin(); id != myBiggestCompConnexe.end(); id++)
	{
		int i = 0;
		// Recherche du territoire dans la map pour ensuite trouver ses voisins
		while (contexte->getMap()->cells[i].infos.id != *id)
		{
			i++;
		}

		// On a trouvé notre territoire à nous
		SCell cell = contexte->getMap()->cells[i];

		// On stocke le nombre de dés de notre territoire 
		int ownDices = cell.infos.nbDices;

		// Pour chacun de ses voisins
		for (int indexVoisin = 0; indexVoisin < cell.nbNeighbors; indexVoisin++)
		{
			// Si c'est un territoire ennemi
			if (cell.neighbors[indexVoisin]->infos.owner != contexte->getPlayerId())
			{
				// Si son nombre de dés est inférieur au nôtre
				if (ownDices > cell.neighbors[indexVoisin]->infos.nbDices)
				{
					idCellFrom = cell.infos.id;
					idCellTo = cell.neighbors[indexVoisin]->infos.id;
					attack = 1;
				}

				else
				{
					// Pour pas que ça tourne à l'infini
					if (state->diceStock[contexte->getPlayerId()] != 0)
					{
						idCellFrom = cell.infos.id;
						idCellTo = cell.neighbors[indexVoisin]->infos.id;
						attack = 1;
					}
				}
			}
		}
	}

	if (attack)
	{
		turn->cellFrom = idCellFrom;
		turn->cellTo = idCellTo;
		return 1;
	}

	else
	{
		return 0;
	}
}

bool attack1_2(const SGameState* state, STurn* turn, void* ctx)
{
	Contexte* contexte = static_cast<Contexte*>(ctx);

	// MAJ map du contexte
	updateMap(contexte->getMap(), state);

	std::map<int, std::tuple<unsigned int, std::vector<unsigned int>>> compConnexeMap;
	calculComposanteConnexe(state, ctx, compConnexeMap);
	afficheCC(compConnexeMap);

	// Appel de chosenCompConnexe
	std::vector<unsigned int> chosenCompConnexe;
	chooseCompConnexe(ctx, compConnexeMap, chosenCompConnexe, 1);

	// Vecteur qui va contenir les idCells du joueur ou de sa plus grande composante connexe
	std::vector<unsigned int> idCellsVector;
	bool attack;
	if (size(chosenCompConnexe) >= 3)
	{
		idCellsVector = chosenCompConnexe;
		attack = dicesDifferenceAttack(state, turn, ctx, idCellsVector);
	}

	else
	{
		attack = dicesDifferenceAttack(state, turn, ctx, idCellsVector);
	}

	return attack;
}

// stratégie qui s'attaque aux composantes connexe ennemies
bool cassageCCAttack(const SGameState* state, STurn* turn, void* ctx)
{
	Contexte* contexte = static_cast<Contexte*>(ctx);

	updateMap(contexte->getMap(), state);

	// compConnexeMap : clé : id composante connexe ; valeur : tuple (idOwner, vecteur des idCells de la CC)
	std::map<int, std::tuple<unsigned int, std::vector<unsigned int>>> compConnexeMap;
	calculComposanteConnexe(state, ctx, compConnexeMap);
	afficheCC(compConnexeMap);

	// meilleur attaque si on ne peut pas casser 
	std::tuple<SCell, SCell> MeilleurAttaqueGlobal;
	int nbCellPlusGrandeCCEnnemie = 0;
	std::vector<std::tuple<SCell, SCell>> attaquesPossiblesCassageGlobal; // CellFrom, CellTo des attaques possibles, que l'on passera à lessRiskyAttack
	std::vector<std::tuple<SCell, SCell>> attaquesPossiblesNONCassageGlobal; // CellFrom, CellTo des attaques possibles, que l'on passera à lessRiskyAttack

	for (auto& it : compConnexeMap) { // on boucle sur les CC
		// si la CC est pas a nous (on ne fait rien si elle est à nous
		if (std::get<0>(it.second) != contexte->getPlayerId())
		{
			// 2 listes d'attaques pour chaque CC ennemie
			std::vector<std::tuple<SCell, SCell>> attaquesPossiblesCassage; // CellFrom, CellTo des attaques possibles, que l'on passera à lessRiskyAttack
			std::vector<std::tuple<SCell, SCell>> attaquesPossiblesNONCassage; // CellFrom, CellTo des attaques possibles, que l'on passera à lessRiskyAttack

			// pour chaque cell ennemie de la CC ennemie :
			for (std::vector<unsigned int>::iterator it_CC = std::get<1>(it.second).begin(); it_CC != std::get<1>(it.second).end(); ++it_CC) {

				// on récupère la SCell ennemie
				unsigned int i = 0;
				while (contexte->getMap()->cells[i].infos.id != *it_CC) i++;
				SCell cellEnnemie = contexte->getMap()->cells[i];


				// On regarde si une cell de la CC ennemie a un voisin à nous et un voisin à quelqu'un d'autre

				bool unVoisinANous = false;
				bool UnVoisinAQuelqun = false;
				SCell CellVoisineANous;

				// Pour chacun de ses voisins
				for (int indexVoisin = 0; indexVoisin < cellEnnemie.nbNeighbors; indexVoisin++) {
					// id de la cell voisine à la cell ennemie
					int idNotreCell = cellEnnemie.neighbors[indexVoisin]->infos.id;

					// Cell voisine à la cell ennemie de la CC ennemie
					SCell CellVoisine = contexte->getMap()->cells[idNotreCell];

					bool unVoisinANous = false; // true si la cell voisine est a nous
					bool UnVoisinALui = false; // true si la cell voisine est à quelqu'un d'autre (pas nous ni à la CC)

					// Si c'est un territoire à nous et s'il y a plus d'un dé
					if (cellEnnemie.neighbors[indexVoisin]->infos.owner == contexte->getPlayerId() &&
						cellEnnemie.neighbors[indexVoisin]->infos.nbDices > 1)
					{
						unVoisinANous = true;
						CellVoisineANous = CellVoisine;
					}
					// Si c'est un territoire à nous et qu'il n'est pas à la CC et s'il y a plus d'un dé
					if (cellEnnemie.neighbors[indexVoisin]->infos.owner != contexte->getPlayerId() &&
						cellEnnemie.neighbors[indexVoisin]->infos.owner != cellEnnemie.neighbors[indexVoisin]->infos.owner &&
						cellEnnemie.neighbors[indexVoisin]->infos.nbDices > 1)
					{
						UnVoisinAQuelqun = true;
					}

				} // fin de la boucle sur les voisins de la cell ennemie de la CC ennemie

				if (unVoisinANous && UnVoisinAQuelqun) {
					// on l'ajoute dans la liste des attaques possible : le voisin à nous vers la cell ennemie (de la boucle)
					attaquesPossiblesCassage.push_back(std::make_tuple(CellVoisineANous, cellEnnemie));
				}
				else if (unVoisinANous) {
					attaquesPossiblesNONCassage.push_back(std::make_tuple(CellVoisineANous, cellEnnemie));
				}

			} // fin de la boucle sur les cell de la CC ennemie

			// si on ne peut pas casser la CC ennemie : jamais trouvé de (unVoisinANous && UnVoisinAQuelqun)
			if (attaquesPossiblesCassage.size() == 0) {
				// on ajoute la meilleur attaque qui casse la CC ennemie
				std::tuple<SCell, SCell> attack;
				if (lessRiskyAttack(attaquesPossiblesNONCassage, attack, -1) == 1) {
					attaquesPossiblesNONCassageGlobal.push_back(attack);
				}
			}
			else { // si on peut casser la CC ennemie
				// on ajoute la meilleur attaque qui casse la CC ennemie
				std::tuple<SCell, SCell> attack;
				if (lessRiskyAttack(attaquesPossiblesCassage, attack, -1) == 1) {
					attaquesPossiblesCassageGlobal.push_back(attack);
				}
			}


		} // end if si la CC est pas à nous
	} // fin de la boucle sur les CC

	if (attaquesPossiblesCassageGlobal.size() != 0) {
		// on peut casser une CC ennemie

		// on selectione la moins risquée
		std::tuple<SCell, SCell> attack;
		if (lessRiskyAttack(attaquesPossiblesCassageGlobal, attack, -1) == 1) {
			// on remplit STurn avec attacks
			turn->cellFrom = std::get<0>(attack).infos.id;
			turn->cellTo = std::get<1>(attack).infos.id;
			return 1;
		}
	}

	if (attaquesPossiblesNONCassageGlobal.size() != 0) {
		// on peut attaquer une CC ennemie

		// on selectione la moins risquée
		std::tuple<SCell, SCell> attack;
		if (lessRiskyAttack(attaquesPossiblesNONCassageGlobal, attack, -1) == 1) {
			// on remplit STurn avec attacks
			turn->cellFrom = std::get<0>(attack).infos.id;
			turn->cellTo = std::get<1>(attack).infos.id;
			return 1;
		}
	}

	// si on ne peut ni caser ni attaquer une CC ennemie, alors on ne fait rien
	return 0;
}

bool safeWayAttack(const SGameState* state, STurn* turn, void* ctx)
{
	Contexte* contexte = static_cast<Contexte*>(ctx);

	updateMap(contexte->getMap(), state);


	std::vector<unsigned int> idCellsVector;

	// Récupération des territoires du joueur courant
	if (!getIdCellsCurrentPlayer(state, ctx, idCellsVector, 1))
	{
		return 0;
	}

	// Création vecteur de longueur des différents chemins (idCellFrom, idCellTO, nbTourMax)
	std::vector<std::tuple<unsigned int, unsigned int, unsigned int>> lengthWaysVector;

	// Pour chacun des territoires du joueur courant avec un nombre de dés > 1
	for (auto& idCell : idCellsVector)
	{
		int i = 0;
		// Recherche du territoire dans la map pour ensuite trouver ses voisins
		while (contexte->getMap()->cells[i].infos.id != idCell)
		{
			i++;
		}

		// On a trouvé notre territoire à nous
		SCell cell = contexte->getMap()->cells[i];
		unsigned int nbTours = 0;
		std::tuple<SCell, SCell, unsigned int> pivot;
		// J'insere 2 fois cell par défaut
		pivot = std::make_tuple(cell, cell, 0);

		// Création vecteur de défilement
		std::vector<std::tuple<SCell, SCell, unsigned int>> defilementVector;
		defilementVector.push_back(pivot);
		// Crétion vecteur de traitement
		std::vector<std::tuple<SCell, SCell, unsigned int>> treatmentVector;
		// Création vecteur de cellTo
		std::vector<unsigned int> cellToVector;

		unsigned int max;
		unsigned int idCellFrom;
		unsigned int idCellTo;

		// Tant que le vecteur de défilement des territoires n'est pas vide
		while (defilementVector.size() != 0)
		{
			// Pour chacun de ses voisins
			for (int indexVoisin = 0; indexVoisin < std::get<1>(defilementVector[0]).nbNeighbors; indexVoisin++)
			{
				// Si c'est un territoire ennemi
				if (std::get<1>(defilementVector[0]).neighbors[indexVoisin]->infos.owner != contexte->getPlayerId())
				{
					if (std::get<1>(defilementVector[0]).infos.nbDices > std::get<1>(defilementVector[0]).neighbors[indexVoisin]->infos.nbDices)
					{
						// Parcours du vecteur de traitement
						std::vector<std::tuple<SCell, SCell, unsigned int>>::iterator tuple;
						for (tuple = treatmentVector.begin(); tuple != treatmentVector.end(); tuple++)
						{
							// Si on est déja passé par ce territoire
							if (std::get<1>(*tuple).infos.id == std::get<1>(defilementVector[0]).neighbors[indexVoisin]->infos.id)
							{
								break;
							}
						}

						// Si on est pas passé par ce territoire
						if (tuple == treatmentVector.end())
						{
							nbTours = std::get<2>(defilementVector[0]) + 1;

							if (nbTours == 1)
							{
								std::get<1>(defilementVector[0]).neighbors[indexVoisin]->infos.nbDices = std::get<1>(defilementVector[0]).infos.nbDices - 1;
								defilementVector.push_back(std::make_tuple(*std::get<1>(defilementVector[0]).neighbors[indexVoisin], *std::get<1>(defilementVector[0]).neighbors[indexVoisin], nbTours));
							}
							std::get<1>(defilementVector[0]).neighbors[indexVoisin]->infos.nbDices = std::get<1>(defilementVector[0]).infos.nbDices - 1;
							defilementVector.push_back(std::make_tuple(std::get<0>(defilementVector[0]), *std::get<1>(defilementVector[0]).neighbors[indexVoisin], nbTours));
						}

						else
						{
							break;
						}
					}
				}
			}
			// Ajout du territoire traité au vecteur correspondant
			treatmentVector.push_back(*defilementVector.begin());
			// Suppression de ce territoire du vecteur de défilement des territoires
			defilementVector.erase(defilementVector.begin());

		}
		// Parcours du vecteur de traitement pour trouver le nombre de tours max (chemin le plus long à partir du pivot correspondant)
		max = std::get<2>(treatmentVector[0]);
		idCellFrom = std::get<0>(pivot).infos.id;
		idCellTo;
		for (auto& tuple : treatmentVector)
		{
			if (std::get<2>(tuple) > max)
			{
				max = std::get<2>(tuple);
				idCellTo = std::get<0>(tuple).infos.id;
			}
		}

		if (max != 0)
		{
			lengthWaysVector.push_back(std::make_tuple(idCellFrom, idCellTo, max));
		}
	}

	if (!lengthWaysVector.empty())
	{
		// Parcours du vecteur de longueur des chemins
		unsigned int maxNbTours = std::get<2>(lengthWaysVector[0]);
		unsigned int chosenCellFrom = std::get<0>(lengthWaysVector[0]);
		unsigned int chosenCellTo = std::get<1>(lengthWaysVector[0]);
		for (auto& tuple : lengthWaysVector)
		{
			if (std::get<2>(tuple) > maxNbTours)
			{
				maxNbTours = std::get<2>(tuple);
				chosenCellFrom = std::get<0>(tuple);
				chosenCellTo = std::get<1>(tuple);
			}
		}

		std::cout << "Chemin de longueur : " << maxNbTours << std::endl;
		turn->cellFrom = chosenCellFrom;
		turn->cellTo = chosenCellTo;
		return 1;

	}

	else
	{
		bool alternative;
		// Vecteur qui va contenir les idCells 
		std::vector<unsigned int> idCellsVector;
		alternative = dicesDifferenceAttack(state, turn, ctx, idCellsVector);

		if (alternative)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}
