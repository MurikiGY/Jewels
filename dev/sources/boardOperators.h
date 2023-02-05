#ifndef _BOARDOPERATORS_
#define _BOARDOPERATORS_

#include "utils.h"
#include "allegroEngine.h"
#include "libGame.h"

//Retorna se tem sequencia de três
int board_check(JEWEL **board);

// Renderiza joias caindo
// Retorna 1 se tiver joia para cair
// Retorna 0 do contrario
int jewel_fall(JEWEL **board, STATES *global_state, SCORE *game_score);

//Renderiza troca de joias e testa se deve desfazer
void switch_jewels(JEWEL **board, STATES *global_state, MOUSE *mouse);

//Busca nova jogada do mouse
void get_new_play(JEWEL **board, STATES *global_state, MOUSE *mouse);

//Atualiza board
void board_update(JEWEL **board, STATES *global_state, MOUSE *mouse, SCORE *game_score);


#endif
