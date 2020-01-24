#pragma once
#include <vector>
#include <map>
#include <iostream>
#include <vector>
#include <list>
#include <tuple>
#include <algorithm>

#include "../../Commun/interface.h"
#include "Helpers.h"
#include "Contexte.h"

// Strat�gie 1 : M�thode qui regarde pour chacun des voisins le(s) territoire(s) pour lesquels la diff�rence de d�s 
// entre le territoire d'attaque et le territoire ennemi est la plus importante. 
// Dans le cas ou on a plusieurs couples (cellFrom, cellTo) avec la m�me diff�rence de d�s (et >= 1), on attaque avec avec le territoire qui a le moins de d�s.
bool dicesDifferenceAttack(const SGameState* state, STurn* turn, void* ctx, std::vector<unsigned int>& idCellsVector);

// Strat�gie 2 : M�thode qui ne fait qu'attaquer � partir de la plus grande composante connexe en choisissant un de ses territoires 
// qui a strictement plus de d�s qu'un de ses voisins. Sinon elle passe son tour.
bool attackFromBiggestCompConnexe(const SGameState* state, STurn* turn, void* ctx);

// Strat�gie 3 : Mixe entre strat�gie 1 et strat�gie 2
bool attack1_2(const SGameState* state, STurn* turn, void* ctx);

// Strat�gie 4 : S'attaque aux CC ennemies
bool cassageCCAttack(const SGameState* state, STurn* turn, void* ctx);

// Strat�gie 5 : M�thode qui attaque � partir du territoire qui fait parti du plus long chemin d'attaque "sur"
bool safeWayAttack(const SGameState* state, STurn* turn, void* ctx);
