#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "../../Commun/interface.h"
#include "../../Strategie/src/Strategy.h"

class Game {

public:
	Game() = default;

	~Game() = default;

	/* verification si coup est valide*/
	bool coup_valide(SMap* map, STurn* turn, int id_att);

	/* duel avec tirages d�s et nb de d�s de chaque territoires*/
	/* retourne true si l'attaquant gagne, false sinon */
	bool duel(SGameState* state, STurn* turn);

	/* mise � jour des territoires */
	void update_territory(SGameState* state, SMap* map, STurn* turn, bool duel);

	/* check s'il reste des territoires � l'adversaire*/
	bool check_territory(SGameState* state, int playerId);

	/* Calcul de la taille de la plus grande CC du joueur courant */
	int calculComposanteConnexe(SGameState* state, SMap* map, unsigned int currenntOwner);

	/* distribution des d�s sur les territoires � la fin du gameturn*/
	void dealDices(SGameState* state, SMap* map, std::vector<std::tuple<unsigned int, unsigned int>>& territories, unsigned int composante, int owner);

	/* M�thode qui stocke les idCell et leur nombre de d�s */
	void getIdCellAndNbDices(unsigned int currentOwner, const SGameState* state, std::vector<std::tuple<unsigned int, unsigned int>>& idCellsAndNbDices);

	/*check si id est dans le vector tab*/
	bool check_id(std::vector<int> tab, int id);
};

