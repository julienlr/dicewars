#pragma once
#include <iostream>
#include <random>
#include "../../Commun/interface.h"
#include "../../Commun/interface_gui.h"

class MapGenerator
{
public:
	MapGenerator() = delete;
	MapGenerator(int width, int height, int nbRegions);
	MapGenerator(const MapGenerator&) = delete;
	~MapGenerator();
    
	SMap getSMap(unsigned int nbPlayers) const;
	SGameState& getGameState() const;
	SRegions* getRegions() const;

	void setRegions(std::vector<std::vector<std::pair<unsigned int, unsigned int>>>);

	void display() const;


private:
	SMap map;
	std::vector<std::pair<unsigned int, unsigned int>>* regions;

	int height;
	int width;

	// Grille des cases de la carte
	int** grid;
	// Grille des cases de la carte à l'étape d'avant
	// Améliore l'équité entre les régions pour s'étendre
	int** gridOldStep;

	// Matrice d'adjacence de voisinage des régions
	bool** links;
	int* connexComp;

	// Initialise la grille donnée
	void initGrid(int** grid);
	// Initialise les régions
	void initRegions();

	// Etend les régions pour remplir la carte
	void growRegions();
	// Applique growCell() sur l'ensemble des voisins de (x,y)
	void growNeighs(int x, int y, int value);
	// Etend la cellule vers ses cases voisines
	void growCell(int x, int y, int value);

	// Indique si le graphe possède une unique composante connexe
	bool graphIsOneConnex();
};