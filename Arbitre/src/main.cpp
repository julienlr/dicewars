#include <iostream>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <string>
#include <vector>
#include "../../Commun/interface_gui.h"
#include "../../Commun/library.h"

#include "Game.h"
#include "MapGenerator.h"
#include "Player.h"

#define GETFUNCTION(handler, name) \
	if ((name = (p##name)GETPROC(handler, #name)) == nullptr)\
	{\
		std::cerr << "Impossible de trouver la fonction '"#name"' dans la dll" << std::endl;\
		return(-1);\
	}

int main(int argc, char* argv[])
{
	// Initialisation des structures
	SMap map = SMap();
	SGameState state;
	STurn turn;
	void* ctxGUI;

	// Pointeurs de fonctions pour les bibliothèques
	pInitGame InitGame;
	pPlayTurn PlayTurn;
	pEndGame EndGame;

	// appel classe game
	Game game;
	// Pour l'aléatoire côté génération de carte
	srand((unsigned int)time(NULL));

	// Définition du nombre de joueurs en fonction du nombre d'arguments
	int nbPlayers = argc - 1;

	for (int i = 1; i < argc; i++) {
		std::cout << "Argument de la commande : '" << argv[i] << "'" << std::endl;
	}

	// Chargement des bibliothèques
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " libfile" << std::endl;
		return(-1);
	}

	// Générateur de carte
	MapGenerator generator = MapGenerator(32, 28, 30);

	// Récupération des données de la carte
	map = generator.getSMap(nbPlayers);
	state = generator.getGameState();

	// Initialisation de l'interface graphique
	ctxGUI = InitGUI(nbPlayers, generator.getRegions());
	SetGameState(ctxGUI, 0, &state);

	generator.display();

	HLIB* hLibs = new HLIB[nbPlayers];

	std::vector<SPlayer> players(nbPlayers);
	std::vector<SPlayer> playersLeft(nbPlayers);

	for (int i = 0; i < nbPlayers; i++)
	{
		// Chargement de chaque bibliothque
		if ((hLibs[i] = LOADLIB(argv[i + 1])) == nullptr)
		{
			std::cerr << "Impossible de charger la librairie '" << argv[i + 1] << "'" << std::endl;
			return(-1);
		}

		// Récupération des fonctions de chaque bibliothèque
		GETFUNCTION(hLibs[i], InitGame);
		GETFUNCTION(hLibs[i], PlayTurn);
		GETFUNCTION(hLibs[i], EndGame);

		// Affectation des fonctions au joueur
		players[i].InitGame = InitGame;
		players[i].PlayTurn = PlayTurn;
		players[i].EndGame = EndGame;

		players[i].info = SPlayerInfo();
		players[i].id = i;
		players[i].ctx = InitGame(players[i].id, nbPlayers, &map, &players[i].info);

		players[i].dernier_coup_valide = 1;

		//players.push_back(players[i]);
		// playersLeft.push_back(players[i]); // pourquoi pas faire un nouveau joueur, le remplir et le push_back ?

		std::cout << "[INIT] Player" << i + 1 << std::endl;
	}

	// Initialisation du nombre de points
	for (auto player = players.begin(); player < players.end(); player++)
	{
		state.points[(*player).id] = game.calculComposanteConnexe(&state, &map, (*player).id);
	}

	// Attente 4s pour permettre à utilisateur de charger la page
	std::cout << "Appuyez sur une touche pour commencer la partie." << std::endl;
	int i;
	std::cin >> i;
	Sleep(4000);

	unsigned int idWinner = 0;

	// Nombre de joueurs restants
	int nbPlayersLeft = nbPlayers;
	playersLeft = players;

	while (nbPlayersLeft >= 2) {

		std::vector<SPlayer>::iterator player = playersLeft.begin();
		while (player != playersLeft.end())
		{
			bool coupValide;
			bool remainCell = true;
			bool replay = true;
			// Copie de state et de map
			SMap* ptrMap = &map;
			SMap copyMap = (*ptrMap);
			SGameState* ptrState = &state;
			SGameState copyGame = (*ptrState);

			while (replay)
			{
				replay = (*player).PlayTurn((*player).dernier_coup_valide, (*player).ctx, &state, &turn);
				if (replay == 0) {
					// le joueur ne veut pas jouer
					break;
				}

				// Rcupration auprs du jeu l'indicateur de validit
				coupValide = game.coup_valide(&map, &turn, player->id);

				if (coupValide)
				{
					bool duel = game.duel(&state, &turn);
					if (duel)
					{
						std::cout << "Attaquant a gagne" << std::endl;
					}
					else
					{
						std::cout << "Attaquant a perdu" << std::endl;
					}
					//  MAJ map de l'arbitre et state
					game.update_territory(&state, &map, &turn, duel);
					//// MAJ map donne au joueur
					//(*player).ctx->map = &map;

				}
				else
				{
					std::cout << "Coup invalide !\n";
					map = copyMap;
					state = copyGame;
					(*player).dernier_coup_valide++;
					replay = 0;
				}

				Sleep(300);
				// idTurn jsp par contre :/
				SetGameState(ctxGUI, 0, &state);
			}

			remainCell = game.check_territory(&state, (*player).id);

			// Fin du tour d'un joueur
			// Distribution des ds si coup valide

			if (remainCell)
			{
				std::cout << "Taille plus grande CC : " << game.calculComposanteConnexe(&state, &map, (*player).id) << std::endl;
				std::vector<std::tuple<unsigned int, unsigned int>> idCellsAndNbDices;
				game.getIdCellAndNbDices((*player).id, &state, idCellsAndNbDices);
				game.dealDices(&state, &map, idCellsAndNbDices, state.points[(*player).id], (*player).id);
				std::cout << "FIN tour" << std::endl;
			}
			else
			{
				// Enlever joueur
				std::cout << "[PLAYER " << (*player).id << "] va etre supprime" << std::endl;
				nbPlayersLeft--;
				replay = 0;
				player = playersLeft.erase(player);
			}

			// idTurn jsp par contre :/
			SetGameState(ctxGUI, 0, &state);

			// Copie de l'id du vainqueur si plus qu'un joueur
			if (nbPlayersLeft == 1)
			{
				idWinner = playersLeft[0].id;
			}

			// Passage au joueur suivant
			if (player != playersLeft.end() && remainCell)
			{
				player++;
			}
		}
	}

	std::cout << "Le gagnant est le joueur #" << idWinner+1 << std::endl; // le numéro ici correspond à l'interface graphique

	// EndGame pour chaque joueur
	for (int i = 0; i < nbPlayers; i++)
	{
		players[i].EndGame(players[i].ctx, idWinner);
		CLOSELIB(hLibs[i]);
	}

	return(0);
}
