#ifndef _BOARDOPERATORS_
#define _BOARDOPERATORS_

#include "utils.h"
#include "states.h"
#include "allegroEngine.h"
#include "libGame.h"

// Renderiza joias caindo
// Retorna 1 se tiver joia para cair
// Retorna 0 do contrario
int jewel_fall(GAME_ENGINE *game_set);

//Renderiza troca de joias e testa se deve desfazer
void switch_jewels(JEWEL **board, STATES *global_state, MOUSE *mouse);

//Busca nova jogada do mouse
void get_new_play(JEWEL **board, STATES *global_state, MOUSE *mouse);

//Atualiza board
void board_update(GAME_STATE *game_status, ALLEGRO_ENGINE *al_engine, GAME_ENGINE *game_set);


#endif
