#include<map>
#include<tuple>
#include<vector>
#include<list>
#include<set>

#include "dominance.h"
#include "helpers.h"
#include "contexte.h"

// retourne le score entre 0 et 100
int dominance(const SGameState *state, Contexte * ctx)
{
	Contexte* contexte = static_cast<Contexte*>(ctx);

	// compConnexeMap : clé : id composante connexe ; valeur : tuple (idOwner, vecteur des idCells de la CC)
	std::map<int, std::tuple<unsigned int, std::vector<unsigned int>>> compConnexeMap;
	calculComposanteConnexe(state, ctx, compConnexeMap);

	// initialisation des variables ---------------------------------------------------------------------------------------------
	// Nombre de dés total
	int nbDesEnnemie = 0;
	int nbDesANous = 0;

	int nbCellANous = 0;
	int nbCellEnnemie = 0;
	
	// id des joueurs ennemies, pour le nombre de joueur
	std::set<int> idEnnemies;

	// nb CC
	int nbCCEnnemie = 0;
	int nbCCANous = 0;

	// Parcours des cells ---------------------------------------------------------------------------------------------
	// Boucle sur les Cell de la carte
	SCellInfo* currentCell = state->cells;
	unsigned int countCells = 0;
	while (countCells < state->nbCells)
	{

		if (currentCell->owner == contexte->getPlayerId()) {
			// Nombre de dés par cell
			nbDesANous += currentCell->nbDices;
			nbCellANous++;
		}
		else {
			// Nombre de dés par cell
			nbDesEnnemie += currentCell->nbDices;
			nbCellEnnemie++;

			// id des joueurs ennemies, pour le nombre de joueur
			idEnnemies.insert(currentCell->owner);
		}

		// incrementation des "itérateurs" de la boucle
		currentCell++;
		countCells++;
	}

	const int nbEnnemie = *idEnnemies.rbegin();

	if (nbCellANous == 0) {
		return 0;
	}
	if (nbCellEnnemie == 0 || nbEnnemie == 0) {
		return 100;
	}

	// Moyenne de dés par cell
	const double MoyNbDesANous = nbDesANous / nbCellANous;
	double MoyNbDesEnnemie = nbDesEnnemie / nbCellEnnemie;

	const double scoreMoyNbDes = MoyNbDesANous / (MoyNbDesEnnemie + MoyNbDesANous) * 25;

	// Moyenne de cell par joueur
	const double NbMoyCellParEnnemie = nbCellEnnemie / nbEnnemie;
	const double scoreNbMoyCell = nbCellANous / (NbMoyCellParEnnemie + nbCellANous) * 25;

	// Parcours des CC ---------------------------------------------------------------------------------------------
	for (auto &it : compConnexeMap) {
		if (std::get<0>(it.second) == contexte->getPlayerId()) {
			// la CC est a nous
			nbCCANous++;
		}
		else {
			nbCCEnnemie++;
		}
	}

	// taille des CC
	const double moyTailleCCANous = nbCellANous / nbCCANous;
	const double moyTailleCCEnnemie = nbCellEnnemie / nbCCEnnemie;
	const double scoreTailleCC = moyTailleCCANous / (moyTailleCCEnnemie + moyTailleCCANous) * 25;

	// Nombre de dés par CC
	const double moyDesParCCANous = nbDesANous / nbCCANous;
	const double moyDesParCCEnnemie = nbDesEnnemie / nbCCEnnemie;
	const double scoreDesParCC = moyDesParCCANous / (moyDesParCCEnnemie + moyDesParCCANous) * 25;

	// calcul du score ---------------------------------------------------------------------------------------------
	int score = static_cast<int>(scoreMoyNbDes + scoreNbMoyCell + scoreTailleCC + scoreDesParCC);
	return score;
}