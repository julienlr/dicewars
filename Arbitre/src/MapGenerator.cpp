#include "MapGenerator.h"

MapGenerator::MapGenerator(int width, int height, int nbRegions)
{
	this->map = SMap();
	this->map.nbCells = nbRegions;

	this->map.cells = new SCell[nbRegions];
	this->regions = new std::vector<std::pair<unsigned int, unsigned int>>[nbRegions];

	this->width = width;
	this->height = height;

	this->grid = new int* [height];
	this->initGrid(this->grid);

	this->gridOldStep = new int* [height];
	this->initGrid(this->gridOldStep);

	this->links = new bool* [nbRegions];
	this->connexComp = new int[nbRegions];

	// Initialisation de la matrice d'adjacence et du tableau des régions
	for (int i = 0; i < nbRegions; i++)
	{
		// Initialisation de la rangée i de la matrice d'adjacence
		this->links[i] = new bool[nbRegions];

		// Initialisation de la région i
		this->regions[i] = std::vector<std::pair<unsigned int, unsigned int>>();

		// Chaque région crée une composante au départ
		this->connexComp[i] = i + 1;

		// Mise à zéro des liens d'adjacence
		for (int j = 0; j < nbRegions; j++)
		{
			this->links[i][j] = 0;
		}
	}

	this->initRegions();
	this->growRegions();
}

MapGenerator::~MapGenerator()
{
}

SMap MapGenerator::getSMap(unsigned int nbPlayers) const
{

	// Indicateur du nombre de dés restants à donner aux joueurs
	int* dicesLeft = new int[nbPlayers];


	switch (nbPlayers) {
	case 2:
		for (unsigned int i = 0; i < nbPlayers; i++) dicesLeft[i] = map.nbCells*1.4;
	case 3:
		for (unsigned int i = 0; i < nbPlayers; i++) dicesLeft[i] = map.nbCells*1.2;
	case 4:
		for (unsigned int i = 0; i < nbPlayers; i++) dicesLeft[i] = map.nbCells / 1.2;
	case 5:
		for (unsigned int i = 0; i < nbPlayers; i++) dicesLeft[i] = map.nbCells / 1.4;
	case 6:
		for (unsigned int i = 0; i < nbPlayers; i++) dicesLeft[i] = map.nbCells / 1.6;
	case 7:
		for (unsigned int i = 0; i < nbPlayers; i++) dicesLeft[i] = map.nbCells / 1.8;
	case 8:
		for (unsigned int i = 0; i < nbPlayers; i++) dicesLeft[i] = map.nbCells / 2.0;

	}


	int* territoryLeft = new int[nbPlayers];
	int nbterritory = map.nbCells;
	int nb = nbterritory / nbPlayers;
	for (unsigned int i = 0; i < nbPlayers; i++) {
		territoryLeft[i] = nb;
		nbterritory -= nb;
	}
	std::vector<int> tab;
	std::vector<int>::iterator it;
	unsigned int t = 0;
	while (nbterritory > 0) {
		int i = rand() % nbPlayers;
		if (t < nbPlayers) {
			it = find(tab.begin(), tab.end(), i);

			if (it == tab.end()) {
				territoryLeft[i] += 1;
				nbterritory--;
				tab.push_back(i);
				t++;
			}

		}
		else {
			territoryLeft[i] += 1;
			nbterritory--;
		}

	}

	// Création des régions
	for (int i = 0; i < this->map.nbCells; i++)
	{
		SCellInfo infos;
		infos.id = i + 1;
		infos.nbDices = 1;
		int x = rand() % nbPlayers;
		while (territoryLeft[x] == 0) x = rand() % nbPlayers;

		infos.owner = x + 1;
		dicesLeft[x]--;
		territoryLeft[x]--;

		SCell cell;
		cell.infos = infos;
		map.cells[i] = cell;
	}

	for (unsigned int i = 0; i < nbPlayers; i++)
	{
		while (dicesLeft[i] > 0) {

			int j = rand() % map.nbCells;
			while (map.cells[j].infos.owner != i + 1) j = rand() % map.nbCells;
			if (map.cells[j].infos.nbDices < 8) {

				int x = 0;
				if (dicesLeft[i] >= (4 - map.cells[j].infos.nbDices)) {
					if ((4 - map.cells[j].infos.nbDices) <= 0) {
						x = 1;
					}
					else {
						x = rand() % (4 - map.cells[j].infos.nbDices) + 1;
					}

				}
				else {
					x = rand() % dicesLeft[i] + 1;
				}
				map.cells[j].infos.nbDices += x;
				dicesLeft[i] -= x;
			}

		}

	}



	// Ajout des voisinages
	for (int i = 0; i < this->map.nbCells; i++)
	{
		map.cells[i].nbNeighbors = 0;
		int neighIndex = 0;

		// Parcours des régions
		for (int j = 0; j < this->map.nbCells; j++)
		{
			// S'il y a un lien sur la matrice d'adjacence
			if (this->links[i][j] == 1)
			{
				map.cells[i].nbNeighbors++;
			}
		}

		std::cout << "[SMAP] Cellule " << i << " id: " << map.cells[i].infos.id << " voisins: " << map.cells[i].nbNeighbors << std::endl;

		map.cells[i].neighbors = new SCell *[map.cells[i].nbNeighbors];

		// Parcours des régions
		for (int j = 0; j < this->map.nbCells; j++)
		{
			// S'il y a un lien sur la matrice d'adjacence
			if (this->links[i][j] == 1)
			{
				map.cells[i].neighbors[neighIndex] = &map.cells[j];
				neighIndex++;
			}
		}
	}

	return(map);
}

SGameState& MapGenerator::getGameState() const
{
	SGameState* state = new SGameState();

	state->nbCells = this->map.nbCells;

	state->cells = new SCellInfo[this->map.nbCells];

	for (int i = 0; i < this->map.nbCells; i++)
	{
		state->cells[i] = this->map.cells[i].infos;
	}

	return(*state);
}

void MapGenerator::setRegions(std::vector<std::vector<std::pair<unsigned int, unsigned int>>> regions)
{
	int i = 0;
	for (auto region = regions.begin(); region < regions.end(); region++)
	{
		std::cout << "Region " << i << std::endl;
		i++;
	}
}


SRegions* MapGenerator::getRegions() const
{
	SRegions* reg = new SRegions;

	reg->nbRegions = this->map.nbCells;
	reg->region = new SRegion[reg->nbRegions];

	for (unsigned int i = 0; i < reg->nbRegions; ++i)
	{
		reg->region[i].nbCells = this->regions[i].size();
		reg->region[i].cells = new SRegionCell[reg->region[i].nbCells];
		for (unsigned int j = 0; j < reg->region[i].nbCells; ++j)
		{
			reg->region[i].cells[j].y = this->regions[i][j].first;
			reg->region[i].cells[j].x = this->regions[i][j].second;
		}
	}

	return(reg);
}

// Initialise la grille donnée
void MapGenerator::initGrid(int** grid)
{
	// Initialisation des bordures horizontales
	grid[0] = new int[width];
	grid[height - 1] = new int[width];

	// Remplissage des bordures horizontales
	for (int i = 0; i < width; i++)
	{
		grid[0][i] = -1;
		grid[height - 1][i] = -1;
	}

	// Remplissage centre
	for (int i = 1; i < height - 1; i++)
	{
		grid[i] = new int[width];

		for (int j = 1; j < width - 1; j++)
		{
			grid[i][j] = 0;
		}
	}

	// Remlissage des bordures verticales
	for (int i = 0; i < height; i++)
	{
		grid[i][0] = -1;
		grid[i][width - 1] = -1;
	}
}

// Initialise les régions
void MapGenerator::initRegions()
{
	if (this->width * this->height < this->map.nbCells)
	{
		std::cerr << "[ERR] Impossible de créer les " << this->map.nbCells
			<< " pour une grille " << this->width << "x" << this->height << std::endl;
	}
	else
	{
		// Coordonnées des points ciblés
		int x, y;

		// Pour chaque région à ajouter
		for (int i = 1; i < this->map.nbCells + 1; i++)
		{
			// Tirage de coordonnées d'une case interne
			do
			{
				x = (rand() % (height - 2)) + 1;
				y = (rand() % (width - 2)) + 1;

			} while (this->grid[x][y] != 0);

			// Affectation de la nouvelle région à la position (x,y)
			this->grid[x][y] = i;
			this->gridOldStep[x][y] = i;

			// On permet aux régions d'avoir un minimum de chance de s'étendre
			this->growNeighs(x, y, i);

			// Ajout de la case à la région
			// -1 car on ne veut pas afficher les bordures sur la carte
			this->regions[i - 1].push_back(std::pair<unsigned int, unsigned int>(x - 1, y - 1));
		}
	}
}

// Etend les régions pour remplir la carte
void MapGenerator::growRegions()
{
	int val;

	while (!this->graphIsOneConnex())
	{
		std::cout << "res: " << this->graphIsOneConnex() << std::endl;

		for (int i = 1; i < this->height - 1; i++)
		{
			for (int j = 1; j < this->width - 1; j++)
			{
				// On regarde l'état de la case à l'étape d'avant
				val = this->gridOldStep[i][j];

				// Si la cellule n'est pas vide, on regarde ses voisins
				if (val != 0)
				{
					this->growNeighs(i, j, val);
				}
			}
		}

		for (int x = 0; x < this->height; x++)
		{
			for (int y = 0; y < this->width; y++)
			{
				this->gridOldStep[x][y] = this->grid[x][y];
			}
		}
	}
}

void MapGenerator::growNeighs(int i, int j, int value)
{
	// Case du dessus
	this->growCell(i - 1, j, value);
	// Case à gauche
	this->growCell(i, j - 1, value);
	// Case à droite
	this->growCell(i, j + 1, value);
	// Case du dessous
	this->growCell(i + 1, j, value);

	// Ligne paire
	if (i % 2 == 0)
	{
		// Case du dessus + 1
		this->growCell(i - 1, j + 1, value);
		// Case du dessous + 1
		this->growCell(i + 1, j + 1, value);
	}
	else
	{
		// Case du dessus - 1
		this->growCell(i - 1, j - 1, value);
		// Case du dessous - 1
		this->growCell(i + 1, j - 1, value);
	}

}

// Etend la région de valeur value vers la case (x,y)
void MapGenerator::growCell(int x, int y, int value)
{
	int* target = &this->grid[x][y];

	// Si la case visée n'est pas dans la même région
	if (*target != value)
	{
		// Si la case est disponible
		if (this->grid[x][y] == 0)
		{
			if (rand() % 2 == 0)
			{
				*target = value;

				// Ajout de la case dans la région
				this->regions[value - 1].push_back(std::pair<unsigned int, unsigned int>(x - 1, y - 1));
			}
		}
		else
		{
			// Pas de prise en compte des bords
			if (this->grid[x][y] != -1)
			{
				// Ajout du lien sur la matrice d'adjacence
				this->links[*target - 1][value - 1] = true;
				this->links[value - 1][*target - 1] = true;

				if (*target < value)
				{
					this->connexComp[value - 1] = *target;
				}
				else
				{
					this->connexComp[*target - 1] = value;
				}
			}
		}
	}
}

// Indique si le graphe possède une unique composante connexe
bool MapGenerator::graphIsOneConnex()
{
	// Met à jour les indices de composantes connexes à partir de la matrice d'adjacence
	for (int x = 0; x < this->map.nbCells; x++)
	{
		for (int y = x; y < this->map.nbCells; y++)
		{
			if (this->links[x][y])
			{
				this->connexComp[x] < this->connexComp[y] ?
					this->connexComp[y] = this->connexComp[x] : this->connexComp[x] = this->connexComp[y];
			}
		}
	}

	int previous;

	for (int i = 1; i < this->map.nbCells; i++)
	{
		previous = this->connexComp[i - 1];

		if (previous != this->connexComp[i])
		{
			return false;
		}
	}

	return true;
}


void MapGenerator::display() const
{
	std::cout << "Carte : (" << this->map.nbCells << " regions)" << std::endl;

	for (int i = 0; i < this->height; i++)
	{
		for (int j = 0; j < this->width; j++)
		{
			if (this->grid[i][j] == 0)
			{
				std::cout << "[ ]";
			}
			else if (this->grid[i][j] == -1)
			{
				std::cout << "[x]";
			}
			else
			{
				std::cout << "[" << (char)(this->grid[i][j] + 64) << "]";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl << std::endl;
}
