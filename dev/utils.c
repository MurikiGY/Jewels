#include <stdio.h>

#include "utils.h"


//Teste de inicialização
void must_init(bool test, const char *description){
   if(test) return;
   printf("couldn't initialize %s\n", description);
   exit(1);
}

//Inteiros aleatorios
int between(int lo, int hi){
  return lo + (rand() % (hi - lo));
}
  
//Float aleatorios
float between_f(float lo, float hi){
  return lo + ((float)rand() / (float)RAND_MAX) * (hi - lo);
}
