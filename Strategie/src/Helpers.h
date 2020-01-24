#pragma once
#include<map>
#include<tuple>
#include<vector>

#include "../../Commun/interface.h"
#include "Strategy.h"
#include "Contexte.h"

void afficheCC(std::map<int, std::tuple<unsigned int, std::vector<unsigned int>>> compConnexeMap);

// Methode qui remplit un vecteur qui va contenir les id des différents joueurs encore en jeu
void getListIdPlayers(const SGameState* state, std::vector<unsigned int>& vectorOwners);

// Methode qui remplit une map par des vecteurs des idCells de chaque joueur : cle : owner ; valeur : vecteur d'idCells
void getListIdCellPerPlayer(const SGameState* state, std::vector<unsigned int>& vectorOwners, std::map<unsigned int, std::vector<unsigned int>>& mapIdCellsPerPlayer);

// Méthode qui remplit un vecteur par tous les territoires qu'il possédent. 
// Renvoie 2 si on n'a pas l'intention d'attaquer, 1 pour indiquer que la stratégie peut attaquer et 0 si elle ne peut pas.
// attack = 1 --> on ne sélectionne pas les territoires avec un nb de dés égal à 1 / attack = 0 --> si on n'a pas l'intention d'attaquer à partir de ces territoires
int getIdCellsCurrentPlayer(const SGameState* state, void* ctx, std::vector<unsigned int>& idCellsVector, bool attack);

// fonction qui renvoie l'attaque la moins risquée entre celles données en paramètre
int lessRiskyAttack(std::vector<std::tuple<SCell, SCell>> attaks, std::tuple<SCell, SCell> attack, int dominance);

// Méthode qui remplit une map : clé : id composante connexe ; valeur : tuple (idOwner, vecteur des idCells de la CC)
void calculComposanteConnexe(const SGameState* state, void* ctx, std::map<int, std::tuple<unsigned int, std::vector<unsigned int>>>& compConnexeMap);

// Méthode qui cherche à agrandir la taille de la plus grande composante connexe; booleen extend : 1 -> la stratégie veut choisir sa plus grande CC 0 -> la stratégie veut choisir
void chooseCompConnexe(void* ctx, std::map<int, std::tuple<unsigned int, std::vector<unsigned int>>>& compConnexeMap, std::vector<unsigned int>& myBiggestCompConnexe, bool extend);

// Méthode qui sélectionne la composante connexe qui contient un coin de la map ( retourne 1 si elle a trouvé une CC, 0 sinon
bool selectCompConnexeCorner(void* ctx, std::map<int, std::tuple<unsigned int, std::vector<unsigned int>>>& compConnexeMap, std::vector<unsigned int>& myBiggestCompConnexe);

// A virer quadn stratégies modifiées
void updateMap(SMap* map, const SGameState* state);