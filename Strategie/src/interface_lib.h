#pragma once
// v 1.0

#include "../../Commun/interface.h"
#include "../../Commun/library.h"

extern "C"
{
	// Fonction à appeler au début de chaque partie
	// La stratégie doit compléter la structure SPlayerInfo
	//	* Ne pas oublier pour l'arbitre de dupliquer toute la structure map pour chaque appel !
	// valeur de retour : contexte de la stratégie (à envoyer dans chaque appel de PlayTurn)
	API_EXPORT void* InitGame(unsigned int id, unsigned int nbPlayer, const SMap *map, SPlayerInfo *info);

	// Fonction à appeler à chaque tour sur la stratégie et tant que le retour de fonction est vrai et qu'il n'y a pas d'erreur.
	//	* En cas d'erreur, rétablir la carte dans l'état initial avant le premier tour du joueur.
	// valeur de retour : booléen : 0 coups terminés, 1 structure turn complétée avec un nouveau coup à jouer.
	// gameTurn : tour de jeu (permet à la stratégie de savoir si elle a échouée au tour précédant)
	// ctx : contexte de la stratégie
	API_EXPORT int PlayTurn(unsigned int gameTurn, void *ctx, const SGameState *state, STurn *turn);

	// Fonction à appeler à la fin de chaque partie
	// ctx : contexte de la stratégie
	API_EXPORT void EndGame(void *ctx, unsigned int idWinner);
}
