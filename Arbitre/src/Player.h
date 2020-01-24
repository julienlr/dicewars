#pragma once
#include "../../Commun/interface.h"

typedef struct SPlayer
{
	SPlayerInfo info;
	SMap map;

	pInitGame InitGame;
	pPlayTurn PlayTurn;
	pEndGame EndGame;
	void* ctx;
	unsigned int id;
	unsigned int dernier_coup_valide; // 1 veut dire que le coup est valide;
} SPlayer;
