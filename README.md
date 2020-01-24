# Dicewars

Ce projet contient le programme permettant de jouer au jeu Dicewars.

## Les règles du jeu

```
L'objectif final du jeu du Dicewars est d'avoir tous les territoires du jeu.
Pour cela, vous devrez attaquer, quand ce sera votre tour, les territoires adverses.
Chaque territoire possède un nombre de dés aléatoire au début du jeu.
Il augmente en fonction de la taille de votre plus grand continent.
Pour envahir un territoire, il faut gagner un duel :
un duel se fait entre le nombre de dés que vous avez sur un territoire et le nombre de dés  
du territoire que vous voulez attaquer.
Exemple : Votre territoire a 5 dés, et le territoire adverse a 4 dés. Vous décidez d'attaquer.
Le jeu va tirer aléatoirement vos 5 dés (leur valeur peut être entre 1 et 6) et les 4 dés de l'adversaire.
Le gagnant du duel est celui qui a le plus gros score en faisant la somme des valeurs de chaque dés.
Si la somme de vos 5 dés vaut 15, et que celle de votre adversaire vaut 14, vous gagnez son territoire.
Si les deux scores sont égaux, c'est le défenseur qui gagne.
Ainsi de suite jusqu'à ce qu'un joueur remporte tous les territoires.

```

* [Dicewars](http://www.794point8.com/blog/dice-wars) - Les règles officielles



## Auteurs

* **Julien Le Roux**
* **Alan Le Verge**
*  **Thomas Vuillemin**
*  **Nathan Seva**


## Pré-requis

Pour tester ce projet, vous devez avoir installé Visual Studio 2017

* [Visual Studio](https://visualstudio.microsoft.com/fr/downloads/) - Lien de téléchargement

## Tester le projet

Pour tester le projet et lancer l'interface graphique, vous devez :

```
1. Télécharger le projet
2. L'ouvrir dans Visual Studio 2017
3. Le compiler
4. Ouvrir en parallèle dans Google chrome la page localhost:5678
```

## Différentes parties du projet


### L'Arbitre


Les fichiers [Game.cpp](https://gitlab.univ-nantes.fr/projets6_dicewars/dicewars/blob/develop/Arbitre/src/Game.cpp) et [Game.h](https://gitlab.univ-nantes.fr/projets6_dicewars/dicewars/blob/develop/Arbitre/src/Game.h) regroupent toutes les méthodes de la classe Game permettant de :

```
- Vérifier si un coup est valide.
- Réaliser un duel en tirant au sort les dés en fonction du nombre de dés du territoire qui attaque  
et du territoire qui est attaqué.  
- Copier la carte et l’état du jeu avant que le joueur joue, comme ça s’il fait un coup illicite,  
on peut revenir à l’état du jeu avant qu’il ait fait tous ses coups.  
- Mettre à jour les territoires après qu’un coup soit joué, c’est-à-dire modifier le propriétaire et  
le nombre de dés du territoire.
- Calculer la plus grande composante connexe du joueur, ceci permet de distribuer ses dés lorsqu’il  
a fini de jouer.
- Distribution des dés à la fin du tour du joueur.

Il y a aussi deux méthodes annexes permettant de récupérer le nombre de dés pour chaque cellule  
dans un vecteur, et une autre qui regarde si une cellule est présente dans un vecteur.

```

Les fichiers [MapGen.cpp](https://gitlab.univ-nantes.fr/projets6_dicewars/dicewars/blob/develop/Arbitre/src/MapGen.cpp) et [MapGen.h](https://gitlab.univ-nantes.fr/projets6_dicewars/dicewars/blob/develop/Arbitre/src/MapGen.h) “””à expliquer”””

Les fichiers [MapGenerator.cpp](https://gitlab.univ-nantes.fr/projets6_dicewars/dicewars/blob/develop/Arbitre/src/MapGen.cpp) et [MapGenerator.h](https://gitlab.univ-nantes.fr/projets6_dicewars/dicewars/blob/develop/Arbitre/src/MapGen.h) “””à expliquer”””

Le fichier [Player.h](https://gitlab.univ-nantes.fr/projets6_dicewars/dicewars/blob/develop/Arbitre/src/Player.h) comprend la structure du joueur avec ses informations tels que son noms et son identifiant.

Le fichier [main.cpp](https://gitlab.univ-nantes.fr/projets6_dicewars/dicewars/blob/develop/Arbitre/src/main.cpp) permet de faire tourner le jeu et lancer l’interface graphique.


### La stratégie

Les fichiers [Helpers.cpp](https://gitlab.univ-nantes.fr/projets6_dicewars/dicewars/blob/develop/Strategie/src/Helpers.cpp) et [Helpers.h](https://gitlab.univ-nantes.fr/projets6_dicewars/dicewars/blob/develop/Strategie/src/Helpers.h) possèdent les fonctions qui aident aux stratégies, comme le calcul de la plus grande composante connexe d’un joueur,  
une fonction qui permettant d’avoir toutes les cellules d’un joueur dans un vecteur,...

Il s’y trouve notamment une fonction lessRiskyAttack qui choisit l’attaque la moins risquée parmi une liste d’attaques passée en paramètre, ainsi qu'une fonction qui donne un degré de dominance de la partie entre 0 et 100.

Les fichiers [Strategy.cpp](https://gitlab.univ-nantes.fr/projets6_dicewars/dicewars/blob/develop/Strategie/src/Strategy.cpp) et [Strategy.h](https://gitlab.univ-nantes.fr/projets6_dicewars/dicewars/blob/develop/Strategie/src/Strategy.h) contiennent nos différentes stratégies :

```
“dicesDifferenceAttack”
Attaquer le territoire voisin où on a le plus de chance de gagner le duel.  

“attackFromBiggestCompConnexe”
L’idée pour la stratégie est de récupérer sa plus grande composante connexe, puis d’attaquer un territoire adverse.

“attack1_2”
Comme son nom l’indique, c’est un mélange de la première et de la deuxième stratégie.

Stratégie non implémentée
Si on a deux composantes connexes assez grandes (supérieures à trois territoires) on essaye de les rejoindre pour créer une grande composante connexe.

“cassageCCAttack”
Le but de cette stratégie est d’attaquer le plus grande composante connexe ennemie, et si possible, de la casser en deux composantes connexes plus petite.

“safeWayAttack”
Dans cette stratégie, il s’agit de chercher le plus long chemin d’attaque possible sans prendre trop de risque.

“domination” (idéale, partiellement implémenté)
On créé une stratégie où, à chaque tour, on est capable d’analyser le jeu du joueur courant (savoir s’il domine ou non la partie), et en fonction du résultat on lance la stratégie optimale (l’une des stratégies détaillées ci-dessous).
```

## Notre rapport de projet

Pour avoir notre rapport de projet plus détaillé : [Rapport](https://docs.google.com/document/d/189ypjKlqyxrxtVD1_HTdlzUvnNvy0t8Gpy9gTokVIAU/edit?usp=sharing)

## Langage utilisé

* C++
