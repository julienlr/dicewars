#include "interface_lib.h"
#include <cstring>
#include <iostream>

#include "Strategy.h"
#include "dominance.h"
#include "Contexte.h"

#ifdef _MSC_VER
#pragma warning(disable:4996)	// disable _CRT_SECURE_NO_WARNINGS
#endif


API_EXPORT void* InitGame(unsigned int id, unsigned int nbPlayer, const SMap *map, SPlayerInfo *info)
{
	std::cout << "[PLAYER " << id << "] Initialisation " << std::endl;

	// Nom de la stratégie
	strcpy_s(info->name, "strategie" + id);

	Contexte* ctx = new Contexte(id, nbPlayer, map);

	// Membres du groupe
	strcpy_s(info->members[0], "Julien");
	strcpy_s(info->members[1], "Alan");
	strcpy_s(info->members[2], "Thomas");
	strcpy_s(info->members[3], "Nathan");
	strcpy_s(info->members[4], "");

	// Récupération des informations de la carte

	// DEBUG: Indique le nombre de cases que l'on a
	/*
	int nbOwnedCells = 0;
	for (int i = 0; i < map->nbCells; i++)
	{
		if (map->cells[i].infos.owner == id) nbOwnedCells++;
	}

	std::cout << "[PLAYER " << id << "] Possede " << nbOwnedCells << "/" << map->nbCells << " cases" << std::endl;
	*/
	std::cout << std::endl;

	return(ctx);
}

API_EXPORT int PlayTurn(unsigned int gameTurn, void *ctx, const SGameState *state, STurn *turn)
{
	Contexte* contexte = static_cast<Contexte*>(ctx);
	std::cout << std::endl;
	std::cout << "[PLAYER " << contexte->getPlayerId() << "] PlayTurn" << std::endl;
	// Vecteur qui va contenir les idCells (strategie dicesDifferenceAttack)
	std::vector<unsigned int> idCellsVector;

	bool strategyChoice;
	switch (contexte->getPlayerId()) {
	case 1:
		strategyChoice = dicesDifferenceAttack(state, turn, ctx, idCellsVector);
		//strategyChoice = attackFromBiggestCompConnexe(state, turn, ctx);
		//strategyChoice = attack1_2(state, turn, ctx);
		//strategyChoice = safeWayAttack(state, turn, ctx);
		//strategyChoice = cassageCCAttack(state, turn, ctx);
		break;
	case 2:
		//strategyChoice = dicesDifferenceAttack(state, turn, ctx, idCellsVector);
		strategyChoice = attackFromBiggestCompConnexe(state, turn, ctx);
		//strategyChoice = attack1_2(state, turn, ctx);
		//strategyChoice = safeWayAttack(state, turn, ctx);
		//strategyChoice = cassageCCAttack(state, turn, ctx);
		break;

	case 3:
		//strategyChoice = dicesDifferenceAttack(state, turn, ctx, idCellsVector);
		//strategyChoice = attackFromBiggestCompConnexe(state, turn, ctx);
		strategyChoice = attack1_2(state, turn, ctx);
		//strategyChoice = safeWayAttack(state, turn, ctx);
		//strategyChoice = cassageCCAttack(state, turn, ctx);
		break;

	case 4:
		//strategyChoice = dicesDifferenceAttack(state, turn, ctx, idCellsVector);
		//strategyChoice = attackFromBiggestCompConnexe(state, turn, ctx);
		//strategyChoice = attack1_2(state, turn, ctx);
		strategyChoice = safeWayAttack(state, turn, ctx);
		//strategyChoice = cassageCCAttack(state, turn, ctx);
		break;

	default:
		strategyChoice = dicesDifferenceAttack(state, turn, ctx, idCellsVector);
		break;
	}

	if (strategyChoice)
	{
		std::cout << "Le joueur " << contexte->getPlayerId() << " attaque a partir du territoire : " << turn->cellFrom << std::endl;
		std::cout << "Et veut attaquer le territoire : " << turn->cellTo << std::endl;
		std::cout << "Je veux continuer d'attaquer ! " << std::endl;
		return 1;
	}

	else
	{
		std::cout << "Je passe mon tour ! " << std::endl;
		return 0;
	}
}

API_EXPORT void EndGame(void *ctx, unsigned int idWinner)
{
	Contexte* contexte = static_cast<Contexte*>(ctx);

	std::cout << "[Player " << contexte->getPlayerId() << "] EndGame" << std::endl;

	delete ctx;
}

