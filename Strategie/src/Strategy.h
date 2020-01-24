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

// Stratégie 1 : Méthode qui regarde pour chacun des voisins le(s) territoire(s) pour lesquels la différence de dés 
// entre le territoire d'attaque et le territoire ennemi est la plus importante. 
// Dans le cas ou on a plusieurs couples (cellFrom, cellTo) avec la même différence de dés (et >= 1), on attaque avec avec le territoire qui a le moins de dés.
bool dicesDifferenceAttack(const SGameState* state, STurn* turn, void* ctx, std::vector<unsigned int>& idCellsVector);

// Stratégie 2 : Méthode qui ne fait qu'attaquer à partir de la plus grande composante connexe en choisissant un de ses territoires 
// qui a strictement plus de dés qu'un de ses voisins. Sinon elle passe son tour.
bool attackFromBiggestCompConnexe(const SGameState* state, STurn* turn, void* ctx);

// Stratégie 3 : Mixe entre stratégie 1 et stratégie 2
bool attack1_2(const SGameState* state, STurn* turn, void* ctx);

// Stratégie 4 : S'attaque aux CC ennemies
bool cassageCCAttack(const SGameState* state, STurn* turn, void* ctx);

// Stratégie 5 : Méthode qui attaque à partir du territoire qui fait parti du plus long chemin d'attaque "sur"
bool safeWayAttack(const SGameState* state, STurn* turn, void* ctx);
