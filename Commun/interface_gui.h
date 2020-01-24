#pragma once
// v 1.0

#include "interface.h"

typedef struct
{
	unsigned int idPlayer;
	STurn turn;
	unsigned int dices[2][8];
} SGameTurn;

struct SRegionCell
{
	unsigned int x;
	unsigned int y;
};

struct SRegion
{
	size_t nbCells;
	SRegionCell *cells;
};

struct SRegions
{
	size_t nbRegions;
	SRegion *region;
};

#ifdef GUI_DLL

// Types prédéfinis pour simplifier la récupération des fonctions dans la librairie
using pInitGUI = void* (*)(unsigned int nbPlayer, const SRegions *map);
using pSetPlayerInfo = int(*)(void *ctx, unsigned int id, const SPlayerInfo *info);
using pSetGameState = void (*)(void *ctx, unsigned int idTurn, const SGameState *state);
using pUpdateGameState = void(*)(void *ctx, unsigned int idTurn, const SGameTurn *turn, const SGameState *finalState);
using pUninitGUI = void(*)(void *ctx);

#else

#ifdef GUIDLL_EXPORTS
#include "library.h"
#define GUI_FUNC API_EXPORT
#else
#define GUI_FUNC
#endif

extern "C"
{
	GUI_FUNC void* InitGUI(unsigned int nbPlayer, const SRegions *map);
	GUI_FUNC void SetPlayerInfo(void *ctx, unsigned int id, const SPlayerInfo *info);
	GUI_FUNC void SetGameState(void *ctx, unsigned int idTurn, const SGameState *state);
	GUI_FUNC void UpdateGameState(void *ctx, unsigned int idTurn, const SGameTurn *turn, const SGameState *finalState);
	GUI_FUNC void UninitGUI(void *ctx);
}

#endif
