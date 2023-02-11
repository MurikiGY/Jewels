#include "boardOperators.h"
#include "libGame.h"
#include "utils.h"


//Gera um novo tabuleiro
void gen_new_board(GAME_ENGINE *game_set){
  for (int i=0; i<BOARD_N+1 ;i++)
    for (int j=0; j<BOARD_N ;j++)
      game_set->board[i][j].type = between(0, JEWEL_TYPE_N);

  //Inicia board consistente
  bool aux = false;
  int ok = jewel_fall(game_set, 0, &aux);
  while ( ok && !aux)
    ok = jewel_fall(game_set, 0, &aux);

  game_set->score->local_score = 0;
  game_set->mission->quant = 0;
  game_set->mission->level = 1;
  game_set->mission->type = between(0, JEWEL_TYPE_N);
}


//Retorna se tem sequencia de três
int board_check(JEWEL **board){
  //Verifica horizontal
  for (int i=1; i<BOARD_N+1 ;i++)
    for (int j=0; j<BOARD_N-2 ;j++){
      int tipo = board[i][j].type;
      if ( (board[i][j+1].type == tipo                     || 
            abs(tipo-board[i][j+1].type) == JEWEL_TYPE_N   ||
            abs(tipo-board[i][j+1].type) == 2*JEWEL_TYPE_N ) && //Testa joias a direita
          (board[i][j+2].type == tipo                      ||
           abs(tipo-board[i][j+2].type) == JEWEL_TYPE_N    ||
           abs(tipo-board[i][j+2].type) == 2*JEWEL_TYPE_N  ) )
        return 1;
    }
  //Verifica vertical
  for (int i=1; i<BOARD_N-1 ;i++)
    for (int j=0; j<BOARD_N ;j++){
      int tipo = board[i][j].type;
      if ( (board[i+1][j].type == tipo                     || 
            abs(tipo-board[i+1][j].type) == JEWEL_TYPE_N   ||
            abs(tipo-board[i+1][j].type) == 2*JEWEL_TYPE_N ) && //Testa joias a baixo
          (board[i+2][j].type == tipo                      || 
           abs(tipo-board[i+2][j].type) == JEWEL_TYPE_N    ||
           abs(tipo-board[i+2][j].type) == 2*JEWEL_TYPE_N  ) )
        return 1;
    }
  return 0; //Não foi marcado ponto
}


//Troca joias de posicao logicamente
void switch_jewel_position(JEWEL **board, int x, int y, int z, int w){
  int tipo = board[x][y].type;
  board[x][y].type = board[z][w].type;
  board[z][w].type = tipo;
}


//Renderiza troca de joias
void switch_movement(JEWEL **board, int i_clk, int j_clk, int i_rls, int j_rls, int direction){
  int horizontal, vertical;
  if ( direction > 0 ){             //Movimenta indo
    horizontal = j_rls - j_clk;
    vertical = i_rls - i_clk;
  } else {                          //Movimenta voltando
    horizontal = j_clk - j_rls;
    vertical = i_clk - i_rls;
  }

  if ( horizontal > 0 ){            
    board[i_clk][j_clk].x += FALL_SPEED;       //Movimenta click pra direita
    board[i_rls][j_rls].x -= FALL_SPEED;       //Movimenta release para esquerda
  } else if ( horizontal < 0 ){
    board[i_clk][j_clk].x -= FALL_SPEED;     //Movimenta click para esquerda
    board[i_rls][j_rls].x += FALL_SPEED;     //Movimenta release para direita
  } else if ( vertical > 0 ){
    board[i_clk][j_clk].y += FALL_SPEED;     //Movimenta click para baixo
    board[i_rls][j_rls].y -= FALL_SPEED;     //Movimenta release para cima
  } else if ( vertical < 0 ){
    board[i_clk][j_clk].y -= FALL_SPEED;     //Movimenta click para cima
    board[i_rls][j_rls].y += FALL_SPEED;     //Movimenta release para baixo
  }
}


//testa se ainda existem possiveis jogadas
//Retorna 1 em caso de game over
int board_game_over(JEWEL **board){
  //Testa submatriz do board
  for (int i=1; i<BOARD_N; i++)        //i vai de 1 a 7
    for (int j=0; j<BOARD_N-1 ;j++){   //j vai de 0 a 6
      //Troca horizontal
      switch_jewel_position(board, i, j, i, j+1);
      if ( board_check(board) ){ switch_jewel_position(board, i, j, i, j+1); return 0; }
      switch_jewel_position(board, i, j, i, j+1);

      //Troca vertical
      switch_jewel_position(board, i, j, i+1, j);
      if ( board_check(board) ){ switch_jewel_position(board, i, j, i+1, j); return 0; }
      switch_jewel_position(board, i, j, i+1, j);
    }
  //Testa coluna final
  for (int i=1; i<BOARD_N ;i++){
    switch_jewel_position(board, i, 7, i+1, 7);
    if ( board_check(board) ){ switch_jewel_position(board, i, 7, i+1, 7); return 0; }
    switch_jewel_position(board, i, 7, i+1, 7);
  }
  //Testa linha final
  for (int j=0; j<BOARD_N-1 ;j++){
    switch_jewel_position(board, 8, j, 8, j+1);
    if ( board_check(board) ){ switch_jewel_position(board, 8, j, 8, j+1); return 0; }
    switch_jewel_position(board, 8, j, 8, j+1);
  }
  return 1;
}


//Seta draw para 0 a posicao de uma peça especial
void hide_special_explosion(GAME_ENGINE *game_set, int i, int j){
  JEWEL **board = game_set->board;
  //Se explosao circular
  if ( board[i][j].type >= JEWEL_TYPE_N && board[i][j].type < 2*JEWEL_TYPE_N ){	//Se tipo > 5 e < 12
    //Marca pontuação
    game_set->score->local_score += 800;
    //Remove tipo especial para não backtraking infinito
    board[i][j].type -= JEWEL_TYPE_N;
    int a_end = i+2,  b_start = j-1,   b_end = j+2;
    if ( j == 0 )               b_start = j;
    else if ( j == BOARD_N-1 )  b_end = j+1;
    if ( i == BOARD_N )         a_end = i+1;

    for (int a=i-1; a<a_end ;a++)
      for (int b=b_start; b<b_end  ;b++){
        board[a][b].draw = 0;
        if ( game_set->mission->type == board[a][b].type )
          game_set->mission->quant += 1;
        if ( board[a][b].type >= JEWEL_TYPE_N )  //Se tipo especial
          hide_special_explosion(game_set, a, b); }
    al_play_sample(game_set->audio->special1_snd_effect, 2.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

  } else if ( board[i][j].type >= 2*JEWEL_TYPE_N ) {  //Explosao em cruz
    //Marca pontuação
    game_set->score->local_score += 1500;
    //remove tipo especial
    board[i][j].type -= 2*JEWEL_TYPE_N;
    //Esconde horizontal
    for (int aux=0; aux<BOARD_N ;aux++){
      board[i][aux].draw = 0;
      if ( game_set->mission->type == board[i][aux].type )
        game_set->mission->quant += 1;
      if ( board[i][aux].type >= JEWEL_TYPE_N )
        hide_special_explosion(game_set, i, aux); }

    //Esconde vertical
    for (int aux=1; aux<BOARD_N+1 ;aux++){
      board[aux][j].draw = 0;
      if ( game_set->mission->type == board[aux][j].type )
        game_set->mission->quant += 1;
      if ( board[aux][j].type >= JEWEL_TYPE_N )
        hide_special_explosion(game_set, aux, j); }
    al_play_sample(game_set->audio->special2_snd_effect, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
  }
}


//Verifica mathpoint
int matchpoint_verify (JEWEL **board, int tipo, int i, int j, int match_type){
  switch ( match_type ){
    case 1:                                                                         //Caso simples
      if (board[i][j].type   ==  tipo || abs(tipo-board[i][j].type)  == JEWEL_TYPE_N  || abs(tipo-board[i][j].type)  == 2*JEWEL_TYPE_N)
        return 1;
      break;

    case 2:                                                                         //Caso horizontal
      if ((board[i][j+1].type == tipo || abs(tipo-board[i][j+1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+1].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j+2].type == tipo || abs(tipo-board[i][j+2].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+2].type) == 2*JEWEL_TYPE_N))
        return 1;
      break;

    case 3:                                                                         //Caso vertical
      if ((board[i+1][j].type == tipo || abs(tipo-board[i+1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+1][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i+2][j].type == tipo || abs(tipo-board[i+2][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+2][j].type) == 2*JEWEL_TYPE_N)) 
        return 1;
      break;

    case 4:   //Caso L
      if ((board[i-1][j].type == tipo || abs(tipo-board[i-1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i-1][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i-2][j].type == tipo || abs(tipo-board[i-2][j].type) == JEWEL_TYPE_N || abs(tipo-board[i-2][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j+1].type == tipo || abs(tipo-board[i][j+1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+1].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j+2].type == tipo || abs(tipo-board[i][j+2].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+2].type) == 2*JEWEL_TYPE_N))
        return 1;
      break;

    case 5:   //Caso L contrario
      if ((board[i-1][j].type == tipo || abs(tipo-board[i-1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i-1][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i-2][j].type == tipo || abs(tipo-board[i-2][j].type) == JEWEL_TYPE_N || abs(tipo-board[i-2][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j-1].type == tipo || abs(tipo-board[i][j-1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j-1].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j-2].type == tipo || abs(tipo-board[i][j-2].type) == JEWEL_TYPE_N || abs(tipo-board[i][j-2].type) == 2*JEWEL_TYPE_N))
        return 1;
      break;

    case 6:   //Caso L de ponta-cabeca
      if ((board[i+1][j].type == tipo || abs(tipo-board[i+1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+1][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i+2][j].type == tipo || abs(tipo-board[i+2][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+2][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j+1].type == tipo || abs(tipo-board[i][j+1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+1].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j+2].type == tipo || abs(tipo-board[i][j+2].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+2].type) == 2*JEWEL_TYPE_N))
        return 1;
      break;

    case 7:   //Caso L contrario de ponta-cabeca
      if ((board[i+1][j].type == tipo || abs(tipo-board[i+1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+1][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i+2][j].type == tipo || abs(tipo-board[i+2][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+2][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j-1].type == tipo || abs(tipo-board[i][j-1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j-1].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j-2].type == tipo || abs(tipo-board[i][j-2].type) == JEWEL_TYPE_N || abs(tipo-board[i][j-2].type) == 2*JEWEL_TYPE_N))
        return 1;
      break;

    case 8:   //Caso T
      if ((board[i][j-1].type == tipo || abs(tipo-board[i][j-1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j-1].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j+1].type == tipo || abs(tipo-board[i][j+1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+1].type) == 2*JEWEL_TYPE_N) &&
          (board[i+1][j].type == tipo || abs(tipo-board[i+1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+1][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i+2][j].type == tipo || abs(tipo-board[i+2][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+2][j].type) == 2*JEWEL_TYPE_N))
        return 1;
      break;

    case 9:   //Caso T de ponta-cabeca
      if ((board[i][j-1].type == tipo || abs(tipo-board[i][j-1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j-1].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j+1].type == tipo || abs(tipo-board[i][j+1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+1].type) == 2*JEWEL_TYPE_N) &&
          (board[i-1][j].type == tipo || abs(tipo-board[i-1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i-1][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i-2][j].type == tipo || abs(tipo-board[i-2][j].type) == JEWEL_TYPE_N || abs(tipo-board[i-2][j].type) == 2*JEWEL_TYPE_N))
        return 1;
      break;

    case 10:  //Caso T deitado esquerda
      if ((board[i][j+1].type == tipo || abs(tipo-board[i][j+1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+1].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j+2].type == tipo || abs(tipo-board[i][j+2].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+2].type) == 2*JEWEL_TYPE_N) &&
          (board[i-1][j].type == tipo || abs(tipo-board[i-1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i-1][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i+1][j].type == tipo || abs(tipo-board[i+1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+1][j].type) == 2*JEWEL_TYPE_N))
        return 1;
      break;

    case 11:  //Caso T deitado Direita
      if ((board[i][j-1].type == tipo || abs(tipo-board[i][j-1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j-1].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j-2].type == tipo || abs(tipo-board[i][j-2].type) == JEWEL_TYPE_N || abs(tipo-board[i][j-2].type) == 2*JEWEL_TYPE_N) &&
          (board[i-1][j].type == tipo || abs(tipo-board[i-1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i-1][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i+1][j].type == tipo || abs(tipo-board[i+1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+1][j].type) == 2*JEWEL_TYPE_N))
        return 1;
      break;
  }
  return 0;
}


//Seta draw para 0
//Testa se há peças especiais e se não foram recem geradas
void hide_pieces(GAME_ENGINE *game_set, int tipo, int i, int j, int match_type){
  JEWEL **board = game_set->board;

  //Se for do tipo especial e estiver com a flag setada em zero
  switch ( match_type ){
    case 1:   //Caso em L
      board[i-1][j].draw = 0; board[i][j+1].draw = 0;
      board[i-2][j].draw = 0; board[i][j+2].draw = 0;
      if      ( board[i-1][j].type >= JEWEL_TYPE_N && board[i-1][j].special_gen_flag == 0 ) hide_special_explosion(game_set, i-1, j);
      else if ( board[i-2][j].type >= JEWEL_TYPE_N && board[i-2][j].special_gen_flag == 0 ) hide_special_explosion(game_set, i-2, j);
      else if ( board[i][j+1].type >= JEWEL_TYPE_N && board[i][j+1].special_gen_flag == 0 ) hide_special_explosion(game_set, i, j+1);
      else if ( board[i][j+2].type >= JEWEL_TYPE_N && board[i][j+2].special_gen_flag == 0 ) hide_special_explosion(game_set, i, j+2);
      break;

    case 2:   //Caso de L invertido
      board[i-1][j].draw = 0; board[i][j-1].draw = 0;
      board[i-2][j].draw = 0; board[i][j-2].draw = 0;
      if      ( board[i-1][j].type >= JEWEL_TYPE_N && board[i-1][j].special_gen_flag == 0 ) hide_special_explosion(game_set, i-1, j);      
      else if ( board[i-2][j].type >= JEWEL_TYPE_N && board[i-2][j].special_gen_flag == 0 ) hide_special_explosion(game_set, i-2, j);
      else if ( board[i][j-1].type >= JEWEL_TYPE_N && board[i][j-1].special_gen_flag == 0 ) hide_special_explosion(game_set, i, j-1);
      else if ( board[i][j-2].type >= JEWEL_TYPE_N && board[i][j-2].special_gen_flag == 0 ) hide_special_explosion(game_set, i, j-2);
      break;

    case 3:   //Caso de L de ponta-cabeca 
      board[i+1][j].draw = 0; board[i][j+1].draw = 0;
      board[i+2][j].draw = 0; board[i][j+2].draw = 0;
      if      ( board[i+1][j].type >= JEWEL_TYPE_N && board[i+1][j].special_gen_flag == 0 ) hide_special_explosion(game_set, i+1, j);
      else if ( board[i+2][j].type >= JEWEL_TYPE_N && board[i+2][j].special_gen_flag == 0 ) hide_special_explosion(game_set, i+2, j);
      else if ( board[i][j+1].type >= JEWEL_TYPE_N && board[i][j+1].special_gen_flag == 0 ) hide_special_explosion(game_set, i, j+1);
      else if ( board[i][j+2].type >= JEWEL_TYPE_N && board[i][j+2].special_gen_flag == 0 ) hide_special_explosion(game_set, i, j+2);
      break;

    case 4:   //Caso de L invertido de ponta-cabeca
      board[i+1][j].draw = 0; board[i][j-1].draw = 0;
      board[i+2][j].draw = 0; board[i][j-2].draw = 0;
      if      ( board[i+1][j].type >= JEWEL_TYPE_N && board[i+1][j].special_gen_flag == 0 ) hide_special_explosion(game_set, i+1, j);      
      else if ( board[i+2][j].type >= JEWEL_TYPE_N && board[i+2][j].special_gen_flag == 0 ) hide_special_explosion(game_set, i+2, j);
      else if ( board[i][j-1].type >= JEWEL_TYPE_N && board[i][j-1].special_gen_flag == 0 ) hide_special_explosion(game_set, i, j-1);
      else if ( board[i][j-2].type >= JEWEL_TYPE_N && board[i][j-2].special_gen_flag == 0 ) hide_special_explosion(game_set, i, j-2);
      break;

    case 5:   //Caso de T em pé
      board[i][j-1].draw = 0; board[i+1][j].draw = 0;
      board[i][j+1].draw = 0; board[i+2][j].draw = 0;
      if      ( board[i][j-1].type >= JEWEL_TYPE_N && board[i][j-1].special_gen_flag == 0 ) hide_special_explosion(game_set, i, j-1);      
      else if ( board[i][j+1].type >= JEWEL_TYPE_N && board[i][j+1].special_gen_flag == 0 ) hide_special_explosion(game_set, i, j+1);
      else if ( board[i+1][j].type >= JEWEL_TYPE_N && board[i+1][j].special_gen_flag == 0 ) hide_special_explosion(game_set, i+1, j);
      else if ( board[i+2][j].type >= JEWEL_TYPE_N && board[i+2][j].special_gen_flag == 0 ) hide_special_explosion(game_set, i+2, j);
      break;

    case 6:   //Caso de T de ponta-cabeca
      board[i][j-1].draw = 0; board[i-1][j].draw = 0;
      board[i][j+1].draw = 0; board[i-2][j].draw = 0;
      if      ( board[i][j-1].type >= JEWEL_TYPE_N && board[i][j-1].special_gen_flag == 0 ) hide_special_explosion(game_set, i, j-1);      
      else if ( board[i][j+1].type >= JEWEL_TYPE_N && board[i][j+1].special_gen_flag == 0 ) hide_special_explosion(game_set, i, j+1);
      else if ( board[i-1][j].type >= JEWEL_TYPE_N && board[i-1][j].special_gen_flag == 0 ) hide_special_explosion(game_set, i-1, j);
      else if ( board[i-2][j].type >= JEWEL_TYPE_N && board[i-2][j].special_gen_flag == 0 ) hide_special_explosion(game_set, i-2, j);
      break;

    case 7:   //Caso de T deitado para esquerda
      board[i][j+1].draw = 0; board[i-1][j].draw = 0;
      board[i][j+2].draw = 0; board[i+1][j].draw = 0;
      if      ( board[i][j+1].type >= JEWEL_TYPE_N && board[i][j+1].special_gen_flag == 0 ) hide_special_explosion(game_set, i, j+1);      
      else if ( board[i][j+2].type >= JEWEL_TYPE_N && board[i][j+2].special_gen_flag == 0 ) hide_special_explosion(game_set, i, j+2);
      else if ( board[i-1][j].type >= JEWEL_TYPE_N && board[i-1][j].special_gen_flag == 0 ) hide_special_explosion(game_set, i-1, j);
      else if ( board[i+1][j].type >= JEWEL_TYPE_N && board[i+1][j].special_gen_flag == 0 ) hide_special_explosion(game_set, i+1, j);
      break;

    case 8:   //Caso de T deitado para direita
      board[i][j-1].draw = 0; board[i-1][j].draw = 0;
      board[i][j-2].draw = 0; board[i+1][j].draw = 0;
      if      ( board[i][j-1].type >= JEWEL_TYPE_N && board[i][j-1].special_gen_flag == 0 ) hide_special_explosion(game_set, i, j-1);      
      else if ( board[i][j-2].type >= JEWEL_TYPE_N && board[i][j-2].special_gen_flag == 0 ) hide_special_explosion(game_set, i, j-2);
      else if ( board[i-1][j].type >= JEWEL_TYPE_N && board[i-1][j].special_gen_flag == 0 ) hide_special_explosion(game_set, i-1, j);
      else if ( board[i+1][j].type >= JEWEL_TYPE_N && board[i+1][j].special_gen_flag == 0 ) hide_special_explosion(game_set, i+1, j);
      break;
  }
  return;
}


//Verifica matchpoint de L em pe
//Retorna quantas joias foram destruidas
int L_test(GAME_ENGINE *game_set, int sound_flag){
  JEWEL **board = game_set->board;
  int quant = 0;
  //Verifica l normal
  for (int i=3; i<BOARD_N+1 ;i++)                                     //Vai de 3 a 8
    for (int j=0; j<BOARD_N-2 ;j++){                                  //Vai de 0 a 5
      int tipo = board[i][j].type;
      if ( matchpoint_verify(board, tipo, i, j, 4) ){                 //Se marcou ponto
        if ( board[i][j].type < JEWEL_TYPE_N){                        //Se não for especial
          board[i][j].type += JEWEL_TYPE_N;                           //Transforma em especial
          board[i][j].special_gen_flag = 1;                           //Seta flag de recem gerado
        }
        hide_pieces(game_set, tipo, i, j, 1);                            //Escode joias a descer
        quant += 5;
        game_set->score->local_score += 500*game_set->mission->level;
        if ( sound_flag )
          al_play_sample(game_set->audio->fall_snd_effect, 2.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        if ( game_set->mission->type == tipo )
          game_set->mission->quant += 5; } }
  //Verifica l invertido
  for (int i=3; i<BOARD_N+1 ;i++)                                     //Vai de 3 a 8
    for (int j=2; j<BOARD_N ;j++){                                    //Vai de 2 a 7
      int tipo = board[i][j].type;
      if ( matchpoint_verify(board, tipo, i, j, 5) ){                 //Se marcou ponto
        if ( board[i][j].type < JEWEL_TYPE_N){                        //Se não for especial
          board[i][j].type += JEWEL_TYPE_N;                           //Transforma em especial
          board[i][j].special_gen_flag = 1;                           //Seta flag de recem gerado
        }
        hide_pieces(game_set, tipo, i, j, 2);                            //Esconde joias a descer
        quant += 5;
        game_set->score->local_score += 500*game_set->mission->level;
        if ( sound_flag )
          al_play_sample(game_set->audio->fall_snd_effect, 2.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        if ( game_set->mission->type == tipo )
          game_set->mission->quant += 5; } }
  //Verifica l de ponta-cabeça
  for (int i=1; i<BOARD_N-1 ;i++)                                     //Vai de 1 a 6
    for (int j=0; j<BOARD_N-2 ;j++){                                  //Vai de 0 a 5
      int tipo = board[i][j].type;
      if ( matchpoint_verify(board, tipo, i, j, 6) ){                 //Se marcou ponto
        if ( board[i][j].type < JEWEL_TYPE_N){                        //Se não for especial
          board[i][j].type += JEWEL_TYPE_N;                           //Transforma em especial
          board[i][j].special_gen_flag = 1;                           //Seta flag de recem gerado
        }
        hide_pieces(game_set, tipo, i, j, 3);                            //Esconde joias a descer
        quant += 5;
        game_set->score->local_score += 500*game_set->mission->level;
        if ( sound_flag )
          al_play_sample(game_set->audio->fall_snd_effect, 2.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        if ( game_set->mission->type == tipo )
          game_set->mission->quant += 5; } }
  //Verifica l invertido e de ponta-cabeça
  for (int i=1; i<BOARD_N-1 ;i++)                                     //Vai de 1 a 6
    for (int j=2; j<BOARD_N ;j++){                                    //Vai de 2 a 7
      int tipo = board[i][j].type;
      if ( matchpoint_verify(board, tipo, i, j, 7) ){                 //Se marcou ponto
        if ( board[i][j].type < JEWEL_TYPE_N){                        //Se não for especial
          board[i][j].type += JEWEL_TYPE_N;                           //Transforma em especial
          board[i][j].special_gen_flag = 1;                           //Seta flag de recem gerado
        }
        hide_pieces(game_set, tipo, i, j, 4);                            //Esconde joias a descer
        quant += 5;
        game_set->score->local_score += 500*game_set->mission->level;
        if ( sound_flag )
          al_play_sample(game_set->audio->fall_snd_effect, 2.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        if ( game_set->mission->type == tipo )
          game_set->mission->quant += 5; } }
  return quant;
}

//Verifica matchpoint em T
//Retorna numero de joias marcadas
int T_test(GAME_ENGINE *game_set, int sound_flag){
  JEWEL **board = game_set->board;
  int quant = 0;
  //Verifica T em pe
  for (int i=1; i<BOARD_N-1 ;i++)
    for (int j=1; j<BOARD_N-1; j++){
      int tipo = board[i][j].type;
      if ( matchpoint_verify(board, tipo, i, j, 8) ){                 //Se marcou ponto
        if ( board[i][j].type < JEWEL_TYPE_N ){                       //Se nao for especial  
          board[i][j].type += JEWEL_TYPE_N;                           //Transforma em especial
          board[i][j].special_gen_flag = 1;                           //Seta flag de recem gerado
        }
        hide_pieces(game_set, tipo, i, j, 5);                            //Esconde joias
        quant += 5;
        game_set->score->local_score += 500*game_set->mission->level;
        if ( sound_flag )
          al_play_sample(game_set->audio->fall_snd_effect, 2.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        if ( game_set->mission->type == tipo )
          game_set->mission->quant += 5; } }
  //Verifica T de ponta cabeca
  for (int i=3; i<BOARD_N+1 ;i++)
    for (int j=1; j<BOARD_N-1; j++){
      int tipo = board[i][j].type;
      if ( matchpoint_verify(board, tipo, i, j, 9) ){
        if ( board[i][j].type < JEWEL_TYPE_N ){
          board[i][j].type += JEWEL_TYPE_N;
          board[i][j].special_gen_flag = 1;
        }
        hide_pieces(game_set, tipo, i, j, 6);
        quant += 5;
        game_set->score->local_score += 500*game_set->mission->level;
        if ( sound_flag )
          al_play_sample(game_set->audio->fall_snd_effect, 2.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        if ( game_set->mission->type == tipo )
          game_set->mission->quant += 5; } }
  //Verifica T deitado para esquerda
  for (int i=2; i<BOARD_N ;i++)
    for (int j=0; j<BOARD_N-2; j++){
      int tipo = board[i][j].type;
      if ( matchpoint_verify(board, tipo, i, j, 10) ){
        if ( board[i][j].type < JEWEL_TYPE_N ){
          board[i][j].type += JEWEL_TYPE_N;
          board[i][j].special_gen_flag = 1;
        }
        hide_pieces(game_set, tipo, i, j, 7);
        quant += 5;
        game_set->score->local_score += 500*game_set->mission->level;
        if ( sound_flag )
          al_play_sample(game_set->audio->fall_snd_effect, 2.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        if ( game_set->mission->type == tipo )
          game_set->mission->quant += 5; } }
  //Verifica T deitado para direita
  for (int i=2; i<BOARD_N ;i++)
    for (int j=2; j<BOARD_N; j++){
      int tipo = board[i][j].type;
      if ( matchpoint_verify(board, tipo, i, j, 11) ){
        if ( board[i][j].type < JEWEL_TYPE_N ){
          board[i][j].type += JEWEL_TYPE_N;
          board[i][j].special_gen_flag = 1;
        }
        hide_pieces(game_set, tipo, i, j, 8);
        quant += 5;
        game_set->score->local_score += 500*game_set->mission->level;
        if ( sound_flag )
          al_play_sample(game_set->audio->fall_snd_effect, 2.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        if ( game_set->mission->type == tipo )
          game_set->mission->quant += 5; } }
  return quant;
}


//Verifica matchpoint na horizontal
//Retorna numero de peças destruidas
int horizontal_test(GAME_ENGINE *game_set, int sound_flag){
  JEWEL **board = game_set->board;
  int quant = 0;
  for (int i=1; i<BOARD_N+1 ;i++)
    for (int j=0; j<BOARD_N-2 ;j++){
      if ( board[i][j].draw ){

        int tipo = board[i][j].type;
        if ( matchpoint_verify(board, tipo, i, j, 2) ){                                 //Se marcou ponto
          int k = j+3;
          while ( k < BOARD_N && matchpoint_verify(board, tipo, i, k, 1) ) k++;         //Pega joias sequenciadas
          for (int aux=j; aux<k ;aux++)                                                 //Esconde doces sequenciados
            if ( board[i][aux].type < JEWEL_TYPE_N )
              board[i][aux].draw = 0;
            else
              if ( board[i][aux].special_gen_flag == 0 )
                hide_special_explosion(game_set, i, aux);
          if ( sound_flag )
            al_play_sample(game_set->audio->fall_snd_effect, 2.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
          if ( game_set->mission->type == tipo )
            game_set->mission->quant += k-j;
          game_set->score->local_score += 100*(k-j)*game_set->mission->level;

          //Gera joia especial
          if ( k-j == 5 ){
            board[i][j+2].type += 2*JEWEL_TYPE_N;
            board[i][j+2].draw = 1;
            board[i][j+2].special_gen_flag = 1; }
          quant += k-j;
        }}}

  return quant;
}

//Verifica matchpoint na vertical
//Retorna numero de peças destruidas
int vertical_test(GAME_ENGINE *game_set, int sound_flag){
  JEWEL **board = game_set->board;
  int quant = 0;
  for (int i=1; i<BOARD_N-1 ;i++)
    for (int j=0; j<BOARD_N ;j++){
      if ( board[i][j].draw ){

        int tipo = board[i][j].type;
        if ( matchpoint_verify(board, tipo, i, j, 3) ){
          int k = i+3;
          while ( k < BOARD_N+1 && matchpoint_verify(board, tipo, k, j, 1) ) k++;       //Pega joias sequenciadas
          for (int aux=i; aux<k ;aux++)                                                 //Esconde doces sequenciados
            if ( board[aux][j].type < JEWEL_TYPE_N )
              board[aux][j].draw = 0;
            else
              if ( board[aux][j].special_gen_flag == 0 )
                hide_special_explosion(game_set, aux, j);
          if ( sound_flag )
            al_play_sample(game_set->audio->fall_snd_effect, 2.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
          if ( game_set->mission->type == tipo )
            game_set->mission->quant += k-i;
          game_set->score->local_score += 100*(k-i)*game_set->mission->level;

          //Gera joia especial
          if ( k-i == 5 ){
            board[i+2][j].type += 2*JEWEL_TYPE_N;
            board[i+2][j].draw = 1;
            board[i+2][j].special_gen_flag = 1; }
          quant += k-i;
        }}}

  return quant;
}


// Renderiza joias caindo
// Retorna 1 se tiver joia para cair
// Retorna 0 do contrario
int jewel_fall(GAME_ENGINE *game_set,int sound_flag, bool *game_over){
  JEWEL **board = game_set->board;
  STATES *global_state = &(game_set->global_state);
  int *i_fall = &(global_state->i_jewel_fall);
  int *fall_flag = &(global_state->fall_flag);
  int jewel_quant = 0;

  switch ( global_state->fall_state ){
    case TEST_FALL:
      //Bateria de testes
      jewel_quant += T_test(game_set, sound_flag);
      jewel_quant += L_test(game_set, sound_flag);
      jewel_quant += horizontal_test(game_set, sound_flag);
      jewel_quant += vertical_test(game_set, sound_flag);

      //Atualiza pontuacao
      if ( sound_flag )
        if ( game_set->score->local_score >= game_set->score->global_score )
          game_set->score->global_score = game_set->score->local_score;

      //Atualiza missão
      if ( game_set->mission->quant >= 10 ){
            al_play_sample(game_set->audio->level_up_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        game_set->mission->quant = 0;
        game_set->mission->level += 1;
        game_set->mission->type = between(0, JEWEL_TYPE_N);
      }

      //Se marcou pontuacao, muda pra fall_board
      if ( jewel_quant ){
        global_state->fall_state = RENDER_FALL;
        *fall_flag = 1;
        *i_fall = 1;
        return 1;
      } else
        if ( board_game_over(board) ){
          printf("Game Over ein, ruim pra caramba\n");
          *game_over = true;
        } else 
          global_state->board_state = BOARD_NEW_PLAY;
      return 0;

      //Renderiza joias caindo na linha *i_fall
    case RENDER_FALL:
      for (int j=0; j<BOARD_N ;j++){
        //Se encontrou joia para renderizar
        if ( board[*i_fall][j].draw == 0 ){
          //Marca como achou e marca que não terminou de renderizar
          *fall_flag = 0;
          if ( board[*i_fall-1][j].y == board[*i_fall][j].y ){     //Se joia tiver terminado de cair

            //restaura y
            for (int i=*i_fall-1; i>-1 ;i--)
              board[i][j].y -= JEWEL_SIZE;
            //Troca tipos
            for (int i=*i_fall; i>0 ;i--)
              board[i][j].type = board[i-1][j].type;
            board[0][j].type = between(0, JEWEL_TYPE_N);
            //Permite renderizar e apaga linha zero
            board[*i_fall][j].draw = 1;
            board[0][j].draw = 0;
            //Passa para proxima linha se tiver renderizado tudo
            *fall_flag = 1;

          } else {
            //Renderiza
            for (int i=*i_fall-1; i>-1 ;i--){
              board[i][j].y += FALL_SPEED; if ( board[0][j].y > 100 ) board[0][j].draw = 1;
            } } } }

      //Se não houve joia para renderizar, ou terminou de renderizar
      if ( *fall_flag == 1 ){
        (*i_fall)++;
        if ( *i_fall > BOARD_N ){
          if ( sound_flag )
            al_play_sample(game_set->audio->fall_snd_effect, 2.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
          global_state->fall_state = TEST_FALL; } }
      break;
  }
  return 1;
}


//Renderiza troca de joias e testa se deve desfazer
void switch_jewels(JEWEL **board, STATES *global_state, MOUSE *mouse){
  int *i_clk = &(mouse->i_clk), *j_clk = &(mouse->j_clk);   //Coordenadas board click
  int *i_rls = &(mouse->i_rls), *j_rls = &(mouse->j_rls);   //Coordenadas board release
  int *x_jewel_clk = &(global_state->x_jewel_clk), *y_jewel_clk = &(global_state->y_jewel_clk);   //Coordenada de trocas 
  int *x_jewel_rls = &(global_state->x_jewel_rls), *y_jewel_rls = &(global_state->y_jewel_rls);   //Coordenada de trocas

  switch ( global_state->jewel_state ){
    case JEWEL_GO:
      //Testa de terminou de movimentar
      if ( board[*i_clk][*j_clk].x == *x_jewel_rls && board[*i_clk][*j_clk].y == *y_jewel_rls &&
          board[*i_rls][*j_rls].x == *x_jewel_clk && board[*i_rls][*j_rls].y == *y_jewel_clk ){
        switch_jewel_position(board, *i_clk, *j_clk, *i_rls, *j_rls);                       //Troca peças na logica
        if ( board_check(board) ){                                                          //Se marcou ponto
          global_state->board_state = BOARD_JEWEL_FALL;                                     //Busca nova jogada
          board[*i_clk][*j_clk].x = *x_jewel_clk; board[*i_clk][*j_clk].y = *y_jewel_clk;   //Restaura coordenadas
          board[*i_rls][*j_rls].x = *x_jewel_rls; board[*i_rls][*j_rls].y = *y_jewel_rls;   //Restaura coordenadas
          *x_jewel_clk = -1; *y_jewel_clk = -1; *x_jewel_rls = -1; *y_jewel_rls = -1;
        } else {                                                                            //Se não marcou ponto
          switch_jewel_position(board, *i_clk, *j_clk, *i_rls, *j_rls);                     //Destroca na logica
          global_state->jewel_state = JEWEL_BACK;                                           //Retrocede doce
        }
      } else
        //Renderiza joia indo
        switch_movement(board, *i_clk, *j_clk, *i_rls, *j_rls, 1);
      break;

    case JEWEL_BACK:
      //Se terminou de voltar a posição original, busca nova jogada
      if ( board[*i_clk][*j_clk].x == *x_jewel_clk && board[*i_clk][*j_clk].y == *y_jewel_clk &&
          board[*i_rls][*j_rls].x == *x_jewel_rls && board[*i_rls][*j_rls].y == *y_jewel_rls ){
        global_state->jewel_state = JEWEL_GO;                                               //Estado da joia indo
        global_state->board_state = BOARD_NEW_PLAY;                                         //Carrega nova jogada
      }
      else
        switch_movement(board, *i_clk, *j_clk, *i_rls, *j_rls, -1);                         //Renderiza movimento
      break;
  }
}


//Busca nova jogada do mouse
void get_new_play(JEWEL **board, STATES *global_state, MOUSE *mouse){
  int *i_clk = &(mouse->i_clk), *j_clk = &(mouse->j_clk);   //Coordenadas board click
  int *i_rls = &(mouse->i_rls), *j_rls = &(mouse->j_rls);   //Coordenadas board release
  int *x_jewel_clk = &(global_state->x_jewel_clk), *y_jewel_clk = &(global_state->y_jewel_clk);
  int *x_jewel_rls = &(global_state->x_jewel_rls), *y_jewel_rls = &(global_state->y_jewel_rls);

  //Se o click foi no tabuleiro
  if ( (mouse->x_clk > X_OFFSET && mouse->x_clk < DISP_W-X_OFFSET) &&
      (mouse->y_clk > Y_OFFSET && mouse->y_clk < DISP_H-20) )
    //Se o release foi no tabuleiro
    if ( (mouse->x_rls > X_OFFSET && mouse->x_rls < DISP_W-X_OFFSET) &&
        (mouse->y_rls > Y_OFFSET && mouse->y_rls < DISP_H-20) )
      //testa se houve movimento
      if ( mouse->x_clk != mouse->x_rls || mouse->y_clk != mouse->y_rls ) {
        int h_delta     = abs(mouse->x_clk-mouse->x_rls);
        int v_delta     = abs(mouse->y_clk-mouse->y_rls);
        int horizontal  = mouse->x_rls - mouse->x_clk;
        int vertical    = mouse->y_rls - mouse->y_clk;

        //Testa movimentacao minima
        if ( h_delta >= JEWEL_SIZE-18 || v_delta >= JEWEL_SIZE-18 ) {
          //Calcula coordenadas do doce clicado e solto na matriz
          *i_clk = (mouse->y_clk - Y_OFFSET + JEWEL_SIZE)/JEWEL_SIZE;  //Linha da joia clicada
          *j_clk = (mouse->x_clk - X_OFFSET)/JEWEL_SIZE;               //Coluna da joia clicada
          *i_rls = (mouse->y_rls - Y_OFFSET + JEWEL_SIZE)/JEWEL_SIZE;  //Linha da joia solta
          *j_rls = (mouse->x_rls - X_OFFSET)/JEWEL_SIZE;               //Coluna da joia solta

          //Se variação horizontal maior que vertical
          if ( h_delta > v_delta ){  *i_rls = *i_clk;
            if ( horizontal > 0 )    *j_rls = *j_clk + 1;              //Movimenta direita
            else                     *j_rls = *j_clk - 1;              //Movimenta esquerda
          } else {                   *j_rls = *j_clk;
            if ( vertical > 0 )      *i_rls = *i_clk + 1;              //Movimenta baixo
            else                     *i_rls = *i_clk - 1;              //Movimenta cima
          }

          //Correção de bug
          if ( *i_rls < BOARD_N+1 && *j_rls < BOARD_N ) {
            *x_jewel_clk = board[*i_clk][*j_clk].x;                    //Salva as coordenadas da troca
            *y_jewel_clk = board[*i_clk][*j_clk].y;                    //Salva as coordenadas da troca
            *x_jewel_rls = board[*i_rls][*j_rls].x;                    //Salva as coordenadas da troca
            *y_jewel_rls = board[*i_rls][*j_rls].y;                    //Salva as coordenadas da troca
            global_state->board_state = BOARD_SWITCH_JEWEL;
          } } }
  return;
}


//Atualiza board
void board_update(GAME_STATE *game_status, ALLEGRO_ENGINE *al_engine, GAME_ENGINE *game_set, bool *game_over){
  switch ( game_set->global_state.board_state ){
    case BOARD_NEW_PLAY:    //Carrega nova jogada
      get_new_play(game_set->board, &game_set->global_state, al_engine->mouse);
      break;

    case BOARD_SWITCH_JEWEL:                                  //Troca joias de posição
      switch_jewels(game_set->board, &game_set->global_state, al_engine->mouse);
      break;

    case BOARD_JEWEL_FALL:                                    //Desce joias do matchpoint
      jewel_fall(game_set, 1, game_over);
      break;
  }
}
